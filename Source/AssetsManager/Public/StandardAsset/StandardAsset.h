// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ASSETSMANAGER_API StandardAsset : public FAssetData
{
public:
	StandardAsset(const FAssetData & AssetData);
	virtual ~StandardAsset();

	TSharedPtr<FString> GetAssetNameInfoByIndex(
		const int32 & index,
		bool bContainsInfoStartIndex = false);

	const TSharedPtr<FString> GetAssetStandardPrefix() const;
	const TSharedPtr<FString> GetAssetSubfix();
	const uint32 GetAssetNameInfoCount() const;

protected:
	TArray<FString> SplitStringRecursive(
		const FString& InStr,
		const FString& SpliteTag);

private:
	TArray<FString> m_AssetNameInfoList;
	bool bHasStandardPrefix;
	uint32 m_AssetNameInfoStartIndex;
};
