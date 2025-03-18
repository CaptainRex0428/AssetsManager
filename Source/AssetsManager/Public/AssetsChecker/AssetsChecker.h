// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "ConfigManager.h"
#include "StandardAsset/FCustomStandardAsset.h"

#include "AssetManagerEditorModule.h"


/*
 * 
 */
class ASSETSMANAGER_API UAssetsChecker : public UAssetActionUtility
{
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

	static bool JudgeSort(FVector2D& A, FVector2D& B, bool bReverse = false);
	static bool JudgeSort(FString& A, FString& B, bool bReverse = false);

	static TArray<FString> SplitStringRecursive(
		const FString& InStr,
		const FString& SpliteTag);

	/*
	* Pattern logic:
	* pattern between "&" is sub-pattern
	* all sub-pattern should be matched at the same time
	* sub-pattern will be split by space " "
	* only one condition in sub-pattern matched will be judged as sub-pattern matched
	*/
	static bool StringMatchPattern(
		const FString& Pattern, 
		FString& StringToMatch);

	static bool StringMatchSubPattern(
		const FString& Pattern,
		FString& StringToMatch);

#pragma endregion

#pragma region FileSize
	static double ByteConversion(
		uint64 ByteSize,
		AssetSizeDisplayUnit Target = AssetSizeDisplayUnit::AssetSizeDisplayUnit_Max);

	static double ByteConversion(
		uint64 ByteSize,
		FString& UnitOut,
		bool OnlyUnit = true,
		AssetSizeDisplayUnit Target = AssetSizeDisplayUnit::AssetSizeDisplayUnit_Max);

	static AssetsInfoDisplayLevel IntToDisplayLevel(int32 Idx);
	static FColor DisplayLevelToColor(AssetsInfoDisplayLevel & DisplayLevel);

	static FString IntStrAddColumn(FString SourceStr);
#pragma endregion

#pragma region DesktopOperator

	static bool OpenLocalFolder(const FString& FolderPath);

#pragma endregion

#pragma region EditorOperator

	static bool OpenSizeMapUI(TArray<TSharedPtr<FAssetData>> AssetsIn);
	static bool OpenSizeMapUI(TSharedPtr<FAssetData> AssetIn);

	static bool OpenEditorUI(TSharedPtr<FAssetData> AssetIn);

	static TArray<FString> GetCurrentContentBrowserSelectedPaths();
	static FString GetCurrentContentBrowserPath();

	static bool SaveAsset(const FString & AssetToSave, bool bOnlyIfIsDirty = true);
	static bool SaveAsset(UObject * ObjectToSave);

#pragma endregion

#pragma region Standard Check Tools
	
	static int DuplicateAssets(
		const TArray<FAssetData>& AssetsDataSelected, 
		int NumOfDupicates, 
		bool forced);

	static TArray<FString> GetValidCategoryTag(
		AssetCategory Category,
		FString ConfigSection);

	static FString GetCategoryTag(AssetCategory Category);
	
#pragma region FixPrefix

	static bool ConfirmPrefixes(
		TArray< TSharedPtr<FAssetData>>& AssetsSelected,
		TArray< TSharedPtr<FAssetData>>& ReadyToFixAssets);

	static void AddPrefixes(
		const TArray<UObject*>& AssetsSelected);
	
	static void AddPrefixes(
		const TArray<FAssetData>& AssetsSelected);

#pragma endregion

#pragma region GetReferences

	static TArray<FString> GetAssetReferencesPath(
		const FString& AssetPath,
		bool bLoadToCheck = false);

	static TArray<FString> GetAssetReferencesPath(
		const FAssetData& AssetData);

	static TArray<FString> GetAssetReferencesPath(
		const TSharedPtr<FAssetData>& AssetData);

#pragma endregion

#pragma region HandleRedirectors

	static void FixUpRedirectors(const FString& Path = "/Game");

	static void FixUpRedirectors(const TArray<FString>& Path);

#pragma endregion

#pragma region DeleteAssets

	static uint32 DeleteAssets(
		const TArray<FAssetData>& AssetsData);

	static uint32 DeleteAsset(
		const FAssetData& AssetData); 
	
	static uint32 DeleteObject(
		UObject * OBJ);

#pragma endregion

#pragma region HandleUnusedAssets

	static void RemoveUnusedAssets(
		const TArray<FAssetData>& AssetsDataSelected);

	static void RemoveUnusedAssets(
		const TArray<FString>& FolderPathSelected);

	static void RemoveEmptyFolder(
		const FString FolderPathSelected = "/Game");

	static void RemoveEmptyFolder(
		const TArray<FString>& FolderPathSelected);

#pragma endregion

#pragma region HandleAssetsName

	static int ReplaceName(
		const TArray<UObject*>& AssetsSelected, 
		const FString& OriginStr, 
		const FString& ReplaceStr);
	
	static bool RenameAsset(
		TSharedPtr<FAssetData> & AssetData, 
		FString NewName);

	static TSharedPtr<FString> GetAssetNameSubfix(
		const FAssetData & AssetSelected);

#pragma endregion

#pragma region ListAssets

	static TArray<TSharedPtr<FAssetData>> ListAssetsDataPtrUnderSelectedFolder(
		const FString& FolderPathSelected);

	static TArray<TSharedPtr<FAssetData>> ListAssetsDataPtrUnderSelectedFolder(
		const TArray<FString>& FolderPathSelected);

	static void FilterAssetsWithTagFromAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList,
		TArray<TSharedPtr<FAssetData>>& OutList,
		FName TagName);

	static void FilterUnusedAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		TArray<FString> StoredFolderPaths = TArray<FString>{},
		bool bRecursive = false,
		bool isAdditiveMode = false);

	static void FilterPrefixErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void FilterSameNameErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void FilterMaxInGameSizeErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList,
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool bStrictMode = false,
		bool isAdditiveMode = false);

	static void FilterSourceSizeErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool bStrictMode = false,
		bool isAdditiveMode = false);

	static void FilterTextureSubfixErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList, 
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void FilterTextureSettingsErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList,
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

	static void FilterTextureLODGroupErrorAssetsForAssetList(
		const TArray<TSharedPtr<FAssetData>>& FindInList,
		TArray<TSharedPtr<FAssetData>>& OutList,
		bool isAdditiveMode = false);

#pragma endregion
	
};
