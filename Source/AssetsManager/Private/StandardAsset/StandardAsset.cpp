// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/StandardAsset.h"
#include "AssetsChecker/AssetsChecker.h"

StandardAsset::StandardAsset(const FAssetData& AssetData)
	:FAssetData(AssetData)
{
	FString AssetSelectedName = AssetData.AssetName.ToString();

	m_AssetNameInfoList.Empty();
	m_AssetNameInfoList = SplitStringRecursive(AssetSelectedName, "_");

	const FString * StandardPrefixPtr 
		= PrefixMap.Find(this->GetAsset()->GetClass());

	if (StandardPrefixPtr && *StandardPrefixPtr == m_AssetNameInfoList[0])
	{
		bHasStandardPrefix = true;
		m_AssetNameInfoStartIndex = 1;
	}
	else
	{
		bHasStandardPrefix = false;
		m_AssetNameInfoStartIndex = 0;
	}

}

StandardAsset::~StandardAsset()
{
}

TSharedPtr<FString> StandardAsset::GetAssetNameInfoByIndex(
	const int32 & index, 
	bool bContainsInfoStartIndex)
{
	if (!this->IsValid())
	{
		return nullptr;
	}

	int32 SearchIndex = index;

	if (SearchIndex >= 0 && bContainsInfoStartIndex)
	{
		SearchIndex += m_AssetNameInfoStartIndex;
	}

	if (SearchIndex < 0)
	{
		SearchIndex += m_AssetNameInfoList.Num();
	}

	if (SearchIndex >= m_AssetNameInfoList.Num() || SearchIndex < 0)
	{
		return nullptr;
	}

	return MakeShared<FString>(m_AssetNameInfoList[SearchIndex]);
}

const TSharedPtr<FString> StandardAsset::GetAssetStandardPrefix() const
{
	if(bHasStandardPrefix)
	{
		return MakeShared<FString>(m_AssetNameInfoList[0]);
	}

	return nullptr;
}

const TSharedPtr<FString> StandardAsset::GetAssetSubfix()
{
	if (GetAssetNameInfoCount() <= 1)
	{
		return nullptr;
	}

	return GetAssetNameInfoByIndex(-1);
}

const uint32 StandardAsset::GetAssetNameInfoCount() const
{
	return m_AssetNameInfoList.Num();
}

TArray<FString> StandardAsset::SplitStringRecursive(
	const FString& InStr, 
	const FString& SpliteTag)
{
	TArray<FString> OutList;
	OutList.Empty();

	FString LStr;
	FString RStr;

	if (InStr.Split(SpliteTag,&LStr,&RStr))
	{
		OutList.Add(LStr);

		while (RStr.Split(SpliteTag, &LStr, &RStr))
		{
			OutList.Add(LStr);
		}

		OutList.Add(RStr);
	}
	else
	{
		OutList.Add(InStr);
	}	

	return OutList;
}


