// Fill out your copyright notice in the Description page of Project Settings.

#pragma warning(disable: 5038)

#include "StandardAsset/FCustomStandardAssetData.h"
#include "AssetsChecker/AssetsChecker.h"

#include "ManagerLogger.h"
#include "HAL/FileManager.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"

FCustomStandardAssetData::FCustomStandardAssetData(const FAssetData& AssetData, bool StrictCheckMode)
	:FAssetData(AssetData), 
	bStrictCheckMode(StrictCheckMode),
	m_CommonAssetCategoryTag(nullptr),
	m_StrictAssetCategoryTag(nullptr),
	AssetConfigGlobalSection(nullptr),
	bHasStandardPrefix(false),
	m_AssetNameInfoStartIndex(0)
{
	/*
	* Get AssetsManager global settings section name
	*/
	FString GlobalSection = FPaths::Combine(ModuleConfigMaster, TEXT("Global"));

	if(FConfigManager::Get().GetSection(*GlobalSection))
	{
		AssetConfigGlobalSection = MakeShareable(new FString(GlobalSection));
	}

	/*
	* Split asset's name into TArray<FString>
	*/

	FString AssetSelectedName = AssetData.AssetName.ToString();

	m_AssetNameInfoList.Empty();
	m_AssetNameInfoList = SplitStringRecursive(AssetSelectedName, "_");

	/*
	* Judge if the asset has standard prefix
	*/

	const FString * StandardClassName
		= UClassNameMap.Find(this->GetAsset()->GetClass());
	if (StandardClassName)
	{
		TSharedPtr<FString> StandardPrefixPtr =
			FConfigManager::Get().FindInSectionStructArray(
				**AssetConfigGlobalSection, 
				"UClassPrefix",
				"UClassName",
				*StandardClassName,
				"Prefix");

		if(StandardPrefixPtr.IsValid())
		{
			if(*StandardPrefixPtr == m_AssetNameInfoList[0])
			{
				bHasStandardPrefix = true;
				m_AssetNameInfoStartIndex = 1;
			}
		}
		else
		{
			if(!this->bStrictCheckMode)
			{
				bHasStandardPrefix = true;
				m_AssetNameInfoStartIndex = 1;
			}
		}
		
	}

	/*
	* Judge if the asset has common standard category tag\
	* This judgment rule is loose
	* Any part of the name matches any member in an array will be treated as a category tag
	*/

	/*
	* Judge if the asset has strict standard category tag
	* This judgment rule is strict
	* Only the first or second part of the name matches any member in an array will be treated as a category tag
	*/

	m_CommonAssetCategory = Undefined;
	m_StrictAssetCategory = Undefined;

	Category AssetValidCategory;

	bool ShouldBreak = false;

	for (AssetValidCategory = Undefined; 
		AssetValidCategory < LastCatergory; 
		AssetValidCategory = (Category)(AssetValidCategory+1))
	{
		TArray<FString> ValidCategoryTags = GetValidCategoryTag(AssetValidCategory);

		for (int InfoIndex = 0; InfoIndex < m_AssetNameInfoList.Num(); ++InfoIndex)
		{
			FString JudgeInfo = m_AssetNameInfoList[InfoIndex];

			for (FString & Tag: ValidCategoryTags)
			{
				if(JudgeInfo.StartsWith(Tag))
				{
					m_CommonAssetCategory = AssetValidCategory;

					switch (m_CommonAssetCategory)
					{
					case FCustomStandardAssetData::Character:
						m_CommonAssetCategoryTag = MakeShared<FString>("Character");
						break;
					case FCustomStandardAssetData::Effect:
						m_CommonAssetCategoryTag = MakeShared<FString>("Effect");
						break;
					case FCustomStandardAssetData::Scene:
						m_CommonAssetCategoryTag = MakeShared<FString>("Scene");
						break;
					case FCustomStandardAssetData::UI:
						m_CommonAssetCategoryTag = MakeShared<FString>("UI");
						break;
					case FCustomStandardAssetData::Hair:
						m_CommonAssetCategoryTag = MakeShared<FString>("Hair");
						break;
					case FCustomStandardAssetData::Undefined:
					case FCustomStandardAssetData::LastCatergory:
					default:
						break;
					}

					if (bHasStandardPrefix ? (InfoIndex == 1) : (InfoIndex<=1))
					{
						m_StrictAssetCategory = AssetValidCategory;

						switch (m_StrictAssetCategory)
						{
						case FCustomStandardAssetData::Character:
							m_StrictAssetCategoryTag = MakeShared<FString>("Character");
							break;
						case FCustomStandardAssetData::Effect:
							m_StrictAssetCategoryTag = MakeShared<FString>("Effect");
							break;
						case FCustomStandardAssetData::Scene:
							m_StrictAssetCategoryTag = MakeShared<FString>("Scene");
							break;
						case FCustomStandardAssetData::UI:
							m_StrictAssetCategoryTag = MakeShared<FString>("UI");
							break;
						case FCustomStandardAssetData::Hair:
							m_StrictAssetCategoryTag = MakeShared<FString>("Hair");
							break;
						case FCustomStandardAssetData::Undefined:
						case FCustomStandardAssetData::LastCatergory:
						default:
							break;
						}

						ShouldBreak = true;
					}
				}
			}
		}

		if (ShouldBreak)
		{
			break;
		}
	}

}

FCustomStandardAssetData::~FCustomStandardAssetData()
{
}

TSharedPtr<FString> FCustomStandardAssetData::GetAssetNameInfoByIndex(
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

const TSharedPtr<FString> FCustomStandardAssetData::GetAssetPrefix() const
{
	if(bHasStandardPrefix)
	{
		return MakeShared<FString>(m_AssetNameInfoList[0]);
	}

	return nullptr;
}

const TSharedPtr<FString> FCustomStandardAssetData::GetAssetSuffix()
{
	if (GetAssetNameInfoCount() <= 1)
	{
		return nullptr;
	}

	return GetAssetNameInfoByIndex(-1);
}

const uint32 FCustomStandardAssetData::GetAssetNameInfoCount() const
{
	return m_AssetNameInfoList.Num();
}

FString FCustomStandardAssetData::GetAssetNameWithoutPrefix() const
{
	TArray<FString> SubNameInfoList;
	SubNameInfoList.Empty();

	for (int32 idx = m_AssetNameInfoStartIndex; idx < m_AssetNameInfoList.Num(); ++idx)
	{
		SubNameInfoList.Add(m_AssetNameInfoList[idx]);
	}

	return FString::Join(SubNameInfoList, TEXT("_"));
}

bool FCustomStandardAssetData::IsPrefixStandarized() const
{
	return bHasStandardPrefix;
}

const TSharedPtr<FString> FCustomStandardAssetData::GetAssetStandardPrefix()
{
	if (bHasStandardPrefix)
	{
		return GetAssetPrefix();
	}

	const FString * ClassFullName = UClassNameMap.Find(this->GetAsset()->GetClass());

	if (!ClassFullName)
	{
		return nullptr;
	}

	TSharedPtr<FString> PrefixFound = 
		FConfigManager::Get().FindInSectionStructArray(
		**AssetConfigGlobalSection,
		"UClassPrefix",
		"UClassName",
		*ClassFullName,
		"Prefix");

	if (!PrefixFound)
	{
		return nullptr;
	}

	return PrefixFound;
}

const FCustomStandardAssetData::Category& FCustomStandardAssetData::GetCommonAssetCategory()
{
	return m_CommonAssetCategory;
}

const FCustomStandardAssetData::Category& FCustomStandardAssetData::GetStrictAssetCategory()
{
	return m_StrictAssetCategory;
}

const FCustomStandardAssetData::Category FCustomStandardAssetData::GetConfirmAssetCategory()
{
	if (m_CommonAssetCategory == m_StrictAssetCategory)
	{
		return m_StrictAssetCategory;
	}

	return FCustomStandardAssetData::Undefined;
}

bool FCustomStandardAssetData::IsCatogryStandarized()
{
	return !(m_StrictAssetCategory == FCustomStandardAssetData::Undefined);
}

int64 FCustomStandardAssetData::GetLoadedSize(
	bool bEstimatedTotal)
{
	FResourceSizeEx size(bEstimatedTotal ? EResourceSizeMode::EstimatedTotal : EResourceSizeMode::Exclusive);
	this->GetAsset()->GetResourceSizeEx(size);

	int32 Size = size.GetTotalMemoryBytes();

	return Size;
}

int64 FCustomStandardAssetData::GetDiskSize()
{
	return this->GetPackage()->GetFileSize();
}



TArray<FString> FCustomStandardAssetData::SplitStringRecursive(
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

TArray<FString> FCustomStandardAssetData::GetValidCategoryTag(
	Category Cate)
{
	if (AssetConfigGlobalSection) 
	{
		TArray<FConfigValue> TagValue;
		TagValue.Empty();

		switch (Cate)
		{
		
		case FCustomStandardAssetData::Character:
		{
			TagValue = FConfigManager::Get().GetSectionValuesArray(
				**AssetConfigGlobalSection,
				"CharacterCategoryTag");
			break;
		}
		case FCustomStandardAssetData::Effect:
		{
			TagValue=FConfigManager::Get().GetSectionValuesArray(
				**AssetConfigGlobalSection,
				"EffectCategoryTag");
			break;
		}
		case FCustomStandardAssetData::Scene:
		{
			TagValue = FConfigManager::Get().GetSectionValuesArray(
				**AssetConfigGlobalSection,
				"SceneCategoryTag");
			break;
		}
		case FCustomStandardAssetData::UI:
		{
			TagValue = FConfigManager::Get().GetSectionValuesArray(
				**AssetConfigGlobalSection,
				"UICategoryTag");
			break;
		}
		case FCustomStandardAssetData::Hair:
		{
			TagValue = FConfigManager::Get().GetSectionValuesArray(
				**AssetConfigGlobalSection,
				"HairCategoryTag");
			break;
		}
		case FCustomStandardAssetData::Undefined:
		case FCustomStandardAssetData::LastCatergory:
		default:
		{
			break;
		}
		}

		TArray<FString> Tags;
		Tags.Empty();

		for(FConfigValue & value : TagValue)
		{
			Tags.Add(value.GetValue());
		}

		return Tags;
	}

	return TArray<FString>();	
}


