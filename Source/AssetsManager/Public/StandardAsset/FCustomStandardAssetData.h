// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfigManager.h"
/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardAssetData : public FAssetData
{
public:
	enum Category
	{
		Undefined = 0,
		Character, Effect, Scene, UI, Hair,
		LastCatergory
	};

	FCustomStandardAssetData(const FAssetData & AssetData);
	virtual ~FCustomStandardAssetData();

	TSharedPtr<FString> GetAssetNameInfoByIndex(
		const int32 & index,
		bool bContainsInfoStartIndex = false);

	const TSharedPtr<FString> GetAssetStandardPrefix() const;
	const TSharedPtr<FString> GetAssetSuffix();
	const uint32 GetAssetNameInfoCount() const;

	const FCustomStandardAssetData::Category& GetCommonAssetCategory();
	const FCustomStandardAssetData::Category& GetStrictAssetCategory();
	const FCustomStandardAssetData::Category GetConfirmAssetCategory();
	bool IsStandardCatogry();

protected:
	TArray<FString> SplitStringRecursive(
		const FString& InStr,
		const FString& SpliteTag);

	TArray<FString> GetValidCategoryTag(
		Category Cate);

protected:
	TArray<FString> m_AssetNameInfoList;
	bool bHasStandardPrefix;

	uint32 m_AssetNameInfoStartIndex;

	FCustomStandardAssetData::Category m_CommonAssetCategory;
	TSharedPtr<FString> m_CommonAssetCategoryTag;

	FCustomStandardAssetData::Category m_StrictAssetCategory;
	TSharedPtr<FString> m_StrictAssetCategoryTag;

	TSharedPtr<FString> AssetConfigGlobalSection;
	
};
