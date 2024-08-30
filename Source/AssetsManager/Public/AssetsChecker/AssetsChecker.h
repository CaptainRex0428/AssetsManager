// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "AssetsManagerConfig.h"

#include "AssetsChecker.generated.h"

/*
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsChecker : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:

#pragma region PublicFuntions

#pragma region Common Tools

	static bool bIsPowerOfTwo(const int num);
	static bool bIsPowerOfTwo(const uint32 num);
	static bool bIsPowerOfTwo(const double num);
	static bool bIsPowerOfTwo(const float num);

	static void ECopyAssetsPtrList(
		TArray<TSharedPtr<FAssetData>>& ListToCopy, 
		TArray<TSharedPtr<FAssetData>>& ListToOutput);

#pragma endregion

#pragma region Standard Check Tools
	
	int EDuplicateAssets(
		const TArray<FAssetData>& AssetsDataSelected, 
		int NumOfDupicates, 
		bool forced);
	
#pragma region FixPrefix

	static bool EConfirmPrefixes(
		TArray< TSharedPtr<FAssetData>>& AssetsSelected,
		TArray< TSharedPtr<FAssetData>>& ReadyToFixAssets);

	static void EAddPrefixes(
		const TArray<UObject*>& AssetsSelected);
	
	static void EAddPrefixes(
		const TArray<FAssetData>& AssetsSelected);

#pragma endregion

#pragma region GetReferences

	static TArray<FString> EGetAssetReferencesPath(
		const FString& AssetPath);
	static TArray<FString> EGetAssetReferencesPath(
		const FAssetData& AssetData);
	static TArray<FString> EGetAssetReferencesPath(
		const TSharedPtr<FAssetData>& AssetData);

#pragma endregion

#pragma region HandleRedirectors

	static void EFixUpRedirectors(const FString& Path = "/Game");
	static void EFixUpRedirectors(const TArray<FString>& Path);

#pragma endregion

#pragma region HandleTexture

	static FVector2D EGetTextureAssetSourceSize(
		const FAssetData& AssetData);

	static FVector2D EGetTextureAssetMaxInGameSize(
		const FAssetData& AssetData);
	
	static TSharedPtr<TextureCompressionSettings> EGetTextureAssetCompressionSettings(
		const FAssetData& AssetData);

	static TSharedPtr<TextureGroup> EGetTextureAssetTextureGroup(
		const FAssetData& AssetData);

	static bool ESetTextureAssetCompressionSettings(
		const FAssetData& AssetData,
		const TEnumAsByte<TextureCompressionSettings> & CompressionSetting);

	static TSharedPtr<bool> EGetTextureAssetSRGBSettings(
		const FAssetData& AssetData);

	static bool ESetTextureSRGBSettings(
		const FAssetData& AssetData,
		const bool & sRGB);

	static bool EFixTextureMaxSizeInGame(
		FAssetData& ClickedAssetData, 
		double maxSize, 
		bool forced = false);

	static bool ESetTextureSize(
		FAssetData& ClickedAssetData, 
		double maxSize);

	static bool ESetTextureStandardSettings(
		FAssetData& ClickedAssetData);

	static TSharedPtr<TextureGroup> EGetTextureLODGroup(
		const FAssetData& AssetData);

	static TSharedPtr<TextureGroup> EGetTextureLODStandardGroup(
		const FAssetData& AssetData,
		bool bIsCategoryStrict = false);

	static bool ESetTextureLODGroup(
		FAssetData& AssetData,
		TextureGroup InTextureGroup);

#pragma endregion

#pragma region DeleteAssets

	static uint32 EDeleteAssets(
		const TArray<FAssetData>& AssetsData);
	static uint32 EDeleteAsset(
		const FAssetData& AssetData);

#pragma endregion

#pragma region HandleUnusedAssets

	void ERemoveUnusedAssets(
		const TArray<FAssetData>& AssetsDataSelected);
	static void ERemoveUnusedAssets(
		const TArray<FString>& FolderPathSelected);

	static void ERemoveEmptyFolder(
		const FString FolderPathSelected = "/Game");
	static void ERemoveEmptyFolder(
		const TArray<FString>& FolderPathSelected);

#pragma endregion

#pragma region HandleAssetsName

	int EReplaceName(
		const TArray<UObject*>& AssetsSelected, 
		const FString& OriginStr, 
		const FString& ReplaceStr);
	
	static bool ERenameAsset(
		TSharedPtr<FAssetData> & AssetData, 
		FString NewName);

	TSharedPtr<FString> EGetAssetNameSubfix(
		const FAssetData & AssetSelected);

#pragma endregion

#pragma region ListAssets

	static TArray<TSharedPtr<FAssetData>> EListAssetsDataPtrUnderSelectedFolder(
		const FString& FolderPathSelected);

	static TArray<TSharedPtr<FAssetData>> EListAssetsDataPtrUnderSelectedFolder(
		const TArray<FString>& FolderPathSelected);

	static void EListUnusedAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListPrefixErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListSameNameErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListMaxInGameSizeErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListSourceSizeErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListTextureSubfixErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListTextureSettingsErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList,
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void EListTextureLODGroupErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList,
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

#pragma endregion

	// 留着为了测试 没别的
	static void ECheckerCheck(const FAssetData & AssetData);

	UFUNCTION(CallInEditor)
	void CheckCheck();

#pragma endregion

#pragma region Call In Editor

	UFUNCTION(CallInEditor)
	void DuplicateAssets(
		int NumOfDupicates, 
		bool forced = true);
	
	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();

	UFUNCTION(CallInEditor)
	void ReplaceName(
		const FString& OriginStr, 
		const FString & ReplaceStr);

#pragma endregion

#pragma endregion

protected:
	static const TMap<UClass*, FString>& EGetPrefixMap();
};
