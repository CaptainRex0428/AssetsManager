// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "ConfigManager.h"


class FToolBarBuilder;
class FMenuBuilder;

class FAssetsManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

#pragma region ContentBrowserMenuExtend

	void AddEntryCBMenuExtension(class FMenuBuilder& MenuBuilder);
	TSharedRef<FExtender> CoordCBMenuExtension(const TArray<FString>& SelectedPaths);

	void InitCBMenuExtension();

	TArray<FString> SelectedContentFolderPaths;
	
#pragma endregion 

#pragma region CustomEditorTab
	TSharedRef<SDockTab> OnSpawnManagerSlateTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> OnSpawnMaterialCreatorSlateTab(const FSpawnTabArgs& SpawnTabArgs);
	
#pragma endregion

private:

	void OnDeleteUnusedAssetButtonClicked();
	void OnDeleteEmptyFolderButtonClicked();
	void OnAssetsManagerButtonClicked();
	void OnMaterialCreatButtonClicked();
	void OnLookDevButtonClicked();

	void RegisterMenus();

	TSharedPtr<class FUICommandList> PluginCommands_DeleteEmptyFolders;
	TSharedPtr<class FUICommandList> PluginCommands_AssetsManager;
	TSharedPtr<class FUICommandList> PluginCommands_LookDev;
};
