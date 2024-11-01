// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetsManagerCommand.h"
#include "ManagerLogger.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetsChecker/AssetsChecker.h"
#include "AssetsChecker/AssetsTypeActions.h"
#include "AssetsCreator/AssetsCreator.h"
#include "SlateWidgets/ManagerSlate.h"
#include "SlateWidgets/BatchRenameSlate.h"
#include "SlateWidgets/Material/MaterialCreatorSlate.h"
#include "AssetToolsModule.h"
#include "AssetsManagerStyle.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FAssetsManagerModule"

void FAssetsManagerModule::StartupModule()
{

	FAssetsMangerStyle::Initialize();
	FAssetsMangerStyle::ReloadTextures();

	FAssetsManagerCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_LookDev,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnLookDevButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithCurrentPath,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsManagerWithCurrentPathButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithSelectedPath,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsManagerWithSelectedPathButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_DeleteEmptyFolders,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetsManagerModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName(CONTENTFOLDER_MANAGERTAB_NAME),
		FOnSpawnTab::CreateRaw(this, &FAssetsManagerModule::OnSpawnManagerSlateTab))
		.SetDisplayName(FText::FromString(TEXT(CONTENTFOLDER_MANAGERTAB_NAME)))
		.SetIcon(FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	// 创建并注册自定义的 AssetTypeActions
	TSharedRef<UTextureAssetTypeActions> CustomAssetTypeAction = MakeShareable(new UTextureAssetTypeActions());
	AssetTools.RegisterAssetTypeActions(CustomAssetTypeAction);
}

void FAssetsManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPreImport.RemoveAll(this);
	//GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.RemoveAll(this);

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

void FAssetsManagerModule::OnAssetsManagerWithSelectedPathButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MANAGERTAB_NAME));
}

void FAssetsManagerModule::OnAssetsManagerWithCurrentPathButtonClicked()
{
	SelectedContentFolderPaths.Empty();
	SelectedContentFolderPaths.Add(UAssetsChecker::GetCurrentContentBrowserPath());

	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MANAGERTAB_NAME));
}

void FAssetsManagerModule::OnMaterialCreatButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MATERIALCREATORTAB_NAME));
}

void FAssetsManagerModule::OnLookDevButtonClicked()
{
	UAssetsCreator::CreateLevel(L"/Script/Engine.World'/Game/Level_LookDevTemp.Level_LookDevTemp'", L"/Script/Engine.World'/AssetsManager/LookDev/LookDev.LookDev'");
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
				PluginCommands,
				TAttribute<FText>(),
				TAttribute<FText>(),
				FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.ManagerLookDev")
				);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("AssetsManager");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetsManagerCommands::Get().PluginAction_LookDev));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.ManagerLookDev");
			}
		}
	}


	// Content Browser
	
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		
		// register ContentBrowser Paths Selected Menu
		{
			ContentBrowserModule.GetAllPathViewContextMenuExtenders().Add(
				FContentBrowserMenuExtender_SelectedPaths::CreateLambda([this](const TArray<FString>& SelectedPaths)
				{
						TSharedRef<FExtender> MenuExtender(new FExtender());

						SelectedContentFolderPaths = SelectedPaths;

						if (SelectedPaths.Num() > 0)
						{
							MenuExtender->AddMenuExtension(
								FName("PathViewFolderOptions"),
								EExtensionHook::After,
								PluginCommands,
								FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
									{
										MenuBuilder.AddMenuEntry(
											FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithSelectedPath,
											NAME_None,
											LOCTEXT("AssetsManager_Label", "AssetsManager"),
											TAttribute<FText>(),
											FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager"));

										MenuBuilder.AddMenuEntry(
											FAssetsManagerCommands::Get().PluginAction_DeleteEmptyFolders,
											NAME_None,
											LOCTEXT("DeleteEmptyFolder_Label", "DeleteEmptyFolder"),
											TAttribute<FText>(),
											FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedFolders"));
									}));
						}

						return MenuExtender;
				}));
		}

		// register ContentBrowser Current Path Menu
		{
			UToolMenu* ContentBrowserToolBar = UToolMenus::Get()->ExtendMenu("ContentBrowser.ToolBar");
			{
				FToolMenuSection& Section = ContentBrowserToolBar->AddSection(
					"AssetsManager",
					LOCTEXT("AssetsManager_Label", "AssetsManager"),
					FToolMenuInsert(FName("Save"), EToolMenuInsertType::After)
				);

				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithCurrentPath));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager");
			}
		}
		
		
	}
}

#pragma endregion

#pragma region CustomEditorTab

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnManagerSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{

	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SManagerSlateTab)
				//.TitleText(CONTENTFOLDER_MANAGERTAB_NAME)
				.TitleText("Assets Manager")
				.SelectedFolderPaths(SelectedContentFolderPaths)
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