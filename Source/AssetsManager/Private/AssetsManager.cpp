// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetsManagerCommand.h"
#include "ManagerLogger.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetsChecker/AssetsChecker.h"
#include "SlateWidgets/ManagerSlate.h"
#include "SlateWidgets/BatchRenameSlate.h"
#include "SlateWidgets/Material/MaterialCreatorSlate.h"
#include "AssetsManagerStyle.h"

#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FAssetsManagerModule"

void FAssetsManagerModule::StartupModule()
{
	InitCBMenuExtension();

	FAssetsMangerStyle::Initialize();
	FAssetsMangerStyle::ReloadTextures();

	FAssetsManagerCommands::Register();

	PluginCommands_LookDev = MakeShareable(new FUICommandList);
	PluginCommands_LookDev->MapAction(
		FAssetsManagerCommands::Get().PluginAction_LookDev,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnLookDevButtonClicked),
		FCanExecuteAction());

	PluginCommands_AssetsManager = MakeShareable(new FUICommandList);
	PluginCommands_AssetsManager->MapAction(
		FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindow,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsManagerButtonClicked),
		FCanExecuteAction()
	);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetsManagerModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName(CONTENTFOLDER_MANAGERTAB_NAME),
		FOnSpawnTab::CreateRaw(this, &FAssetsManagerModule::OnSpawnManagerSlateTab))
		.SetDisplayName(FText::FromString(TEXT(CONTENTFOLDER_MANAGERTAB_NAME)))
		.SetIcon(FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FAssetsManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAssetsMangerStyle::ShutDown();

	FAssetsManagerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(CONTENTFOLDER_MANAGERTAB_NAME));
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(CONTENTFOLDER_MATERIALCREATORTAB_NAME));
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(TABNAME_BATCHRENAME));
	FAssetsMangerStyle::ShutDown();
}

#pragma region ContentBrowserMenuExtend

// Third bind. The actual function to execute.
void FAssetsManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	UAssetsChecker::RemoveUnusedAssets(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked()
{
	UAssetsChecker::RemoveEmptyFolder(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnAssetsManagerButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MANAGERTAB_NAME));
}

void FAssetsManagerModule::OnMaterialCreatButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MATERIALCREATORTAB_NAME));
}

void FAssetsManagerModule::OnLookDevButtonClicked()
{
	NtfyMsgLog("LookDev Clicked.");
}

void FAssetsManagerModule::RegisterMenus()
{
	// register LookDev Button

	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(
				FAssetsManagerCommands::Get().PluginAction_LookDev, 
				PluginCommands_LookDev,
				TAttribute<FText>(),
				TAttribute<FText>(),
				FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.ManagerLookDev")
				);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetsManagerCommands::Get().PluginAction_LookDev));
				Entry.SetCommandList(PluginCommands_LookDev);
				Entry.Icon = FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.ManagerLookDev");
			}
		}
	}


	// register AssetsManager Button

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(
				FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindow, 
				PluginCommands_AssetsManager,
				TAttribute<FText>(),
				TAttribute<FText>(),
				FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManagerTitle"));
		}
	}
}

// Second bind. Define the details for the menu entry.
void FAssetsManagerModule::AddEntryCBMenuExtension(FMenuBuilder& MenuBuilder)
{
	// Third bind. 

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Folders")),
		FText::FromString(TEXT("Safely delete folders never used or referenced.")),
		FSlateIcon(FAssetsMangerStyle::GetStyleSetName(),"ContentBrowser.DeleteUnusedFolders"),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT(CONTENTFOLDER_MANAGERTAB_NAME)),
		FText::FromString(TEXT("A tab window to check the assets inside the seleted folder.")),
		FSlateIcon(FAssetsMangerStyle::GetStyleSetName(),"ContentBrowser.AssetsManager"),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsManagerButtonClicked)
	);
}

// First bind. Define the position for inserting menu entry.
TSharedRef<FExtender> FAssetsManagerModule::CoordCBMenuExtension(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		// Second bind.
		MenuExtender->AddMenuExtension(
			FName("PathViewFolderOptions"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FAssetsManagerModule::AddEntryCBMenuExtension)
		);

		SelectedContentFolderPaths = SelectedPaths;
	}

	return MenuExtender;
}

void FAssetsManagerModule::InitCBMenuExtension()
{
	// load menu
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	// delegate
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;

	// First bind.
	CustomCBMenuDelegate.BindRaw(this, &FAssetsManagerModule::CoordCBMenuExtension);

	// registry menu
	CBMenuExtenders.Add(CustomCBMenuDelegate);
}



#pragma endregion

#pragma region CustomEditorTab

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnManagerSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TArray<TSharedPtr<FAssetData>> assets = UAssetsChecker::ListAssetsDataPtrUnderSelectedFolder(SelectedContentFolderPaths);

	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SManagerSlateTab)
				//.TitleText(CONTENTFOLDER_MANAGERTAB_NAME)
				.TitleText("Assets Manager")
				.SelectedFolderPaths(SelectedContentFolderPaths)
				.StoredAssetsData(&assets)
		];
}

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnMaterialCreatorSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
		SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SMaterialCreatorSlate)
				//.TitleText(CONTENTFOLDER_MANAGERTAB_NAME)
				.TitleText("Material Creator")
		];
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetsManagerModule, AssetsManager)