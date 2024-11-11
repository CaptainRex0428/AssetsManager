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
	void OnInterchangeAssetPostImport(UObject * OBJ);
	
	void OnEditorDelegatePreImport(UFactory* Factory, UClass* InClass, UObject* Outer, const FName& Name, const TCHAR* Type);

	void OnEditorDelegatePostImport(UFactory* Factory, UObject* CreatedObject);

	void RegisterMenus();

	void OnDeleteUnusedAssetButtonClicked();
	void OnDeleteEmptyFolderButtonClicked();
	void OnAssetsManagerWithSelectedPathButtonClicked();
	void OnAssetsManagerWithCurrentPathButtonClicked();
	void OnMaterialCreatButtonClicked();
	void OnLookDevButtonClicked();
	void OnCharacterLookDevButtonClicked();

	
	TArray<FString> SelectedContentFolderPaths;

	TSharedPtr<class FUICommandList> PluginCommands;

#pragma region CustomEditorTab
	TSharedRef<SDockTab> OnSpawnManagerSlateTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> OnSpawnMaterialCreatorSlateTab(const FSpawnTabArgs& SpawnTabArgs);
#pragma endregion
};
