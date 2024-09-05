// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardAssetData.h"
#include "AssetsChecker/AssetsChecker.h"
#include "AssetsManagerConfig.h"
#include "ConfigManager.h"

FCustomStandardAssetData::FCustomStandardAssetData(const FAssetData& AssetData)
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

	m_CommonAssetCategory = Undefined;

	TArray<FString> ValidCategoryInFString;
	ValidCategoryInFString.Empty();

	for (TPair<FString, FCustomStandardAssetData::Category> Pair :AssetCategoryMap)
	{
		ValidCategoryInFString.Add(Pair.Key);
	}

	
	for (int InfoIndex = 0; InfoIndex < m_AssetNameInfoList.Num(); ++InfoIndex)
	{
		bool ShouldBreak = false;

		FString JudgeInfo = m_AssetNameInfoList[InfoIndex];

		for (FString key : ValidCategoryInFString)
		{
			if (JudgeInfo.StartsWith(key))
			{
				m_CommonAssetCategory = *AssetCategoryMap.Find(key);

				break;
			}
		}

		if (ShouldBreak) 
		{
			break;
		}
	}

	m_StrictAssetCategory = Undefined;

	if (bHasStandardPrefix)
	{
		const FCustomStandardAssetData::Category* CategoryFound =
			AssetCategoryMap.Find(m_AssetNameInfoList[1]);

		if (CategoryFound)
		{
			m_StrictAssetCategory = *CategoryFound;
		}
	}
	else
	{
		const FCustomStandardAssetData::Category* CategoryFoundLoc0 =
			AssetCategoryMap.Find(m_AssetNameInfoList[0]);

		if (CategoryFoundLoc0)
		{
			m_StrictAssetCategory = *CategoryFoundLoc0;
		}
		
		if (m_AssetNameInfoList.Num() > 1) 
		{
			const FCustomStandardAssetData::Category* CategoryFoundLoc1 =
				AssetCategoryMap.Find(m_AssetNameInfoList[1]);

			if (CategoryFoundLoc1)
			{
				m_StrictAssetCategory = *CategoryFoundLoc1;
			}
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

const TSharedPtr<FString> FCustomStandardAssetData::GetAssetStandardPrefix() const
{
	if(bHasStandardPrefix)
	{
		return MakeShared<FString>(m_AssetNameInfoList[0]);
	}

	return nullptr;
}

const TSharedPtr<FString> FCustomStandardAssetData::GetAssetSubfix()
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

bool FCustomStandardAssetData::IsStandardCatogry()
{
	return !(m_StrictAssetCategory == FCustomStandardAssetData::Undefined);
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

FString FCustomStandardAssetData::GetAssetConfigSection(
	const FString& UsageCategory,
	bool bStrictMode)
{
	FString ConfigCategory;

	switch (bStrictMode ? GetConfirmAssetCategory():GetCommonAssetCategory())
	{
	case FCustomStandardAssetData::Undefined:
	{
		ConfigCategory = "Global";
		break;
	}
	case FCustomStandardAssetData::Character:
	{
		ConfigCategory = "Character";
		break;
	}
	case FCustomStandardAssetData::Effect:
	{
		ConfigCategory = "Effect";
		break;
	}
	case FCustomStandardAssetData::Scene:
	{
		ConfigCategory = "Scene";
		break;
	}
	case FCustomStandardAssetData::UI:
	{
		ConfigCategory = "UI";
		break;
	}
	case FCustomStandardAssetData::Hair:
	{
		ConfigCategory = "Hair";
		break;
	}
	default:
	{
		ConfigCategory = "Global";
		break;
	}
	}

	FString GlobalTextureConfigSectionName = 
		"AssetsManager." + ConfigCategory + "." + UsageCategory;

	const FConfigSection * GlobalMaxSizeConfig =
		FConfigManager::Get().GetSection(*GlobalTextureConfigSectionName);

	if(GlobalMaxSizeConfig)
	{
		return GlobalTextureConfigSectionName;
	}

	return "";
}

FString FCustomStandardAssetData::GetAssetConfigGlobalSection(
	const FString& UsageCategory)
{
	FString DGlobalTextureConfigSectionName;

	if(UsageCategory.IsEmpty())
	{
		DGlobalTextureConfigSectionName = FPaths::Combine(ModuleConfigMaster,TEXT("Global"));
	}
	else
	{
		DGlobalTextureConfigSectionName = FPaths::Combine(ModuleConfigMaster, TEXT("Global"),UsageCategory);
	}
	
	if (FConfigManager::Get().GetSection(*DGlobalTextureConfigSectionName))
	{
		return DGlobalTextureConfigSectionName;
	}

	return FPaths::Combine(ModuleConfigMaster, TEXT("Global"));
}


