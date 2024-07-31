// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetsManager.h"
#include "ManagerLogger.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetsChecker/AssetsChecker.h"

#include "SlateWidgets/ManagerSlate.h"

#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FAssetsManagerModule"

void FAssetsManagerModule::StartupModule()
{
	InitCBMenuExtension();
	
	RegisterCustomEditorTab();
}

void FAssetsManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtend

// Third bind. The actual function to excute.
void FAssetsManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	UAssetsChecker::ERemoveUnusedAssets(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked()
{
	UAssetsChecker::ERemoveEmptyFolder(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnAssetsCheckerButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDERMANAGERTABNAME));
}

void FAssetsManagerModule::OnTestButtonClicked()
{
	UAssetsChecker::ECheckerCheck(SelectedContentFolderPaths);
}

// Second bind. Define the details for the menu entry.
void FAssetsManagerModule::AddEntryCBMenuExtension(FMenuBuilder& MenuBuilder)
{
	// Third bind. 
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete assets never used or referenced.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnDeleteUnusedAssetButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Folders")),
		FText::FromString(TEXT("Safely delete folders never used or referenced.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT(CONTENTFOLDERMANAGERTABNAME)),
		FText::FromString(TEXT("A tab window to check the assets inside the seleted folder.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsCheckerButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("CheckerCheck")),
		FText::FromString(TEXT("Check Checker.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnTestButtonClicked)
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
		// tag
		FName(CONTENTFOLDERMANAGERTABNAME),
		FOnSpawnTab::CreateRaw(this, &FAssetsManagerModule::OnSpawnAssetsCheckerTab))
		.SetDisplayName(FText::FromString(TEXT(CONTENTFOLDERMANAGERTABNAME)));
}

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnAssetsCheckerTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SAssetsCheckerTab)
				.TitleText(CONTENTFOLDERMANAGERTABNAME)
				.StoredAssetsData(UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(SelectedContentFolderPaths))
		];
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetsManagerModule, AssetsManager)