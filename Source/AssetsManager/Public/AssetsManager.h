// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


#define ASSETSMANAGER_LOGFOLDER FPaths::ProjectDir() + "/Saved/AssetsManager/"

class FAssetsManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

#pragma region ContentBrowserMenuExtend

	void OnDeleteUnusedAssetButtonClicked();
	void OnDeleteEmptyFolderButtonClicked();
	void OnAssetsManagerButtonClicked();
	void OnTestButtonClicked();

	void AddEntryCBMenuExtension(class FMenuBuilder& MenuBuilder);
	TSharedRef<FExtender> CoordCBMenuExtension(const TArray<FString>& SelectedPaths);

	void InitCBMenuExtension();

	TArray<FString> SelectedContentFolderPaths;
	
#pragma endregion 

#pragma region CustomEditorTab
	void RegisterCustomEditorTab();

	TSharedRef<SDockTab> OnSpawnAssetsCheckerTab(const FSpawnTabArgs& SpawnTabArgs);
#pragma endregion
};
