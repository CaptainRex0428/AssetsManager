// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetsManager.h"
#include "ManagerLogger.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetsChecker/AssetsChecker.h"
#include "SlateWidgets/ManagerSlate.h"
#include "SlateWidgets/MaterialCreatorSlate.h"
#include "AssetsManagerStyle.h"

#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FAssetsManagerModule"

void FAssetsManagerModule::StartupModule()
{
	InitCBMenuExtension();
	FAssetsMangerStyle::InitializeIcons();
	RegisterCustomEditorTab();
}

void FAssetsManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(CONTENTFOLDERMANAGERTABNAME));
	FAssetsMangerStyle::ShutDown();
}

#pragma region ContentBrowserMenuExtend

// Third bind. The actual function to execute.
void FAssetsManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	UAssetsChecker::ERemoveUnusedAssets(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked()
{
	UAssetsChecker::ERemoveEmptyFolder(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnAssetsManagerButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDERMANAGERTABNAME));
}

void FAssetsManagerModule::OnMaterialCreatButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDERMATERIALCREATORTABNAME));
}

// Second bind. Define the details for the menu entry.
void FAssetsManagerModule::AddEntryCBMenuExtension(FMenuBuilder& MenuBuilder)
{
	// Third bind. 

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Folders")),
		FText::FromString(TEXT("Safely delete folders never used or referenced.")),
		FSlateIcon(FAssetsMangerStyle::GetStyleName(),"ContentBrowser.DeleteUnusedFolders"),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT(CONTENTFOLDERMANAGERTABNAME)),
		FText::FromString(TEXT("A tab window to check the assets inside the seleted folder.")),
		FSlateIcon(FAssetsMangerStyle::GetStyleName(),"ContentBrowser.AssetsManager"),
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

void FAssetsManagerModule::RegisterCustomEditorTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName(CONTENTFOLDERMANAGERTABNAME),
		FOnSpawnTab::CreateRaw(this, &FAssetsManagerModule::OnSpawnManagerSlateTab))
		.SetDisplayName(FText::FromString(TEXT(CONTENTFOLDERMANAGERTABNAME)))
		.SetIcon(FSlateIcon(FAssetsMangerStyle::GetStyleName(),"ContentBrowser.AssetsManager"));
}

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnManagerSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SManagerSlateTab)
				//.TitleText(CONTENTFOLDERMANAGERTABNAME)
				.TitleText("Assets Manager")
				.SelectedFolderPaths(SelectedContentFolderPaths)
				.StoredAssetsData(UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(SelectedContentFolderPaths))
		];
}

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnMaterialCreatorSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
		SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SMaterialCreatorSlate)
				//.TitleText(CONTENTFOLDERMANAGERTABNAME)
				.TitleText("Material Creator")
		];
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetsManagerModule, AssetsManager)