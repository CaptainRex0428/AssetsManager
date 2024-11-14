// Fill out your copyright notice in the Description page of Project Settings.

#pragma warning(disable: 5038)

#include "StandardAsset/FCustomStandardAsset.h"
#include "AssetsChecker/AssetsChecker.h"

#include "ManagerLogger.h"
#include "HAL/FileManager.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"



FCustomStandardAssetData::FCustomStandardAssetData(const FAssetData& AssetData, bool StrictCheckMode)
	:FAssetData(AssetData), 
	bStrictCheckMode(StrictCheckMode),
	Object(this->GetAsset(),StrictCheckMode)
{
}

FCustomStandardAssetData::~FCustomStandardAssetData()
{
}

UCustomStandardObject& FCustomStandardAssetData::Get()
{
	return this->Object;
}


int64 FCustomStandardAssetData::GetDiskSize()
{
	return this->GetPackage()->GetFileSize();
}

int64 FCustomStandardAssetData::GetDiskSize(
	AssetsInfoDisplayLevel& DisplayLevel)
{
	int64 DiskSize = GetDiskSize();

	FString AssetGlobalSection = "/AssetsManager/Global";

	TArray<FString> ValidLevels = UManagerConfig::Get().GenerateStructKeyValueArray(
		*AssetGlobalSection,
		L"AssetDiskSizeLevelDivide",
		L"Level");

	int32 LevelOut = 0;

	for (int32 levelIdx = 0; levelIdx < ValidLevels.Num(); ++levelIdx)
	{
		TSharedPtr<FString> LevelValue = UManagerConfig::Get().FindInSectionStructArray(
			*AssetGlobalSection,
			L"AssetDiskSizeLevelDivide",
			L"Level",
			FString::FromInt(levelIdx),
			L"Value");

		if (!LevelValue.IsValid())
		{
			continue;
		}

		int BorderSize = UManagerConfig::Get().SToI(*LevelValue);

		LevelOut = levelIdx;

		if (DiskSize < BorderSize * 1024 * 1024)
		{
			break;
		}
	}

	DisplayLevel = UAssetsChecker::IntToDisplayLevel(LevelOut);

	return DiskSize;
}

UCustomStandardObject::UCustomStandardObject(UObject* InObj, bool StricCheckMode)
	:bStrictCheckMode(StricCheckMode),
	 AssetConfigGlobalSection(nullptr)
{
	if (InObj)
	{
		this->Object = TWeakObjectPtr<UObject>(InObj);
	}
	else
	{
		this->Object = nullptr;
	}

	FString GlobalSection = FPaths::Combine(ModuleConfigMaster, TEXT("Global"));

	if (UManagerConfig::Get().GetSection(*GlobalSection))
	{
		this -> AssetConfigGlobalSection = MakeShareable(new FString(GlobalSection));
	}

	/*
	* Split asset's name into TArray<FString>
	*/

	this->AssetNameInfoList.Empty();
	this->AssetNameInfoList = UAssetsChecker::SplitStringRecursive(this->GetClassValidObjectName(), "_");

	
}

UCustomStandardObject::~UCustomStandardObject()
{
}

TWeakObjectPtr<UObject> UCustomStandardObject::Get()
{
	return this->Object;
}

FString UCustomStandardObject::GetClassValidObjectName()
{
	if (!this->Get().IsValid())
	{
		return L"";
	}

	if (this->Get()->IsA<UPackage>())
	{
		return FPaths::GetBaseFilename(this->Get()->GetFName().ToString());
	}

	return this->Get()->GetFName().ToString();
}

TSharedPtr<FString> UCustomStandardObject::GetAssetNameInfoByIndex(
	const int32& index)
{
	if (!this->Get().IsValid())
	{
		return nullptr;
	}

	int32 SearchIndex = index;

	if (SearchIndex >= this->AssetNameInfoList.Num() || SearchIndex < -1 * this->AssetNameInfoList.Num())
	{
		return nullptr;
	}

	if (SearchIndex < 0)
	{
		SearchIndex += this->AssetNameInfoList.Num();
	}

	return MakeShared<FString>(this->AssetNameInfoList[SearchIndex]);
}

const TSharedPtr<FString> UCustomStandardObject::GetAssetPrefix()
{
	if (!this->IsPrefixNonstandarized())
	{
		return MakeShared<FString>(this->AssetNameInfoList[0]);
	}

	return nullptr;
}

const TSharedPtr<FString> UCustomStandardObject::GetAssetSuffix()
{
	if (this->GetAssetNameInfoCount() <= 1)
	{
		return nullptr;
	}

	return this->GetAssetNameInfoByIndex(-1);
}

const uint32 UCustomStandardObject::GetAssetNameInfoCount() const
{
	return this->AssetNameInfoList.Num();
}

FString UCustomStandardObject::GetAssetNameWithoutPrefix()
{
	TArray<FString> SubNameInfoList;
	SubNameInfoList.Empty();

	for (int32 idx = this->IsPrefixNonstandarized() ? 0 : 1; idx < this->AssetNameInfoList.Num(); ++idx)
	{
		SubNameInfoList.Add(this->AssetNameInfoList[idx]);
	}

	return FString::Join(SubNameInfoList, TEXT("_"));
}

bool UCustomStandardObject::IsPrefixNonstandarized()
{
	/*
	* Judge if the asset has standard prefix
	*/
	if (this->Get().IsValid())
	{
		const FString* StandardClassName
			= UClassNameMap.Find(this->Get()->GetClass());

		if (StandardClassName)
		{
			TSharedPtr<FString> StandardPrefixPtr =
				UManagerConfig::Get().FindInSectionStructArray(
					**AssetConfigGlobalSection,
					"UClassPrefix",
					"UClassName",
					*StandardClassName,
					"Prefix");

			if (StandardPrefixPtr.IsValid())
			{
				if (*StandardPrefixPtr == this->AssetNameInfoList[0])
				{
					

					return false;
				}

				return true;
			}
			else
			{
				if (!this->bStrictCheckMode)
				{
					

					return false;
				}

				return true;
			}

		}

		return false;
	}

	return false;
}

const TSharedPtr<FString> UCustomStandardObject::GetAssetStandardPrefix()
{
	if (!this->IsPrefixNonstandarized())
	{
		return this->GetAssetPrefix();
	}

	const FString* ClassFullName = UClassNameMap.Find(this->Get()->GetClass());

	if (!ClassFullName)
	{
		return nullptr;
	}

	TSharedPtr<FString> PrefixFound =
		UManagerConfig::Get().FindInSectionStructArray(
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

int64 UCustomStandardObject::GetMemorySize(bool bEstimatedTotal)
{
	return this->Get()->GetResourceSizeBytes(bEstimatedTotal ? EResourceSizeMode::EstimatedTotal : EResourceSizeMode::Exclusive);;
}

int64 UCustomStandardObject::GetMemorySize(AssetsInfoDisplayLevel& DisplayLevel, bool bEstimatedTotal)
{
	return int64();
}

AssetCategory UCustomStandardObject::GetCommonAssetCategory()
{
	AssetCategory AssetValidCategory;

	for (AssetValidCategory = (AssetCategory)1;
		AssetValidCategory < AssetCategory::LastCatergory;
		AssetValidCategory = (AssetCategory)((uint8)AssetValidCategory + 1))
	{
		TArray<FString> ValidCategoryTags = UAssetsChecker::GetValidCategoryTag(AssetValidCategory,*AssetConfigGlobalSection);

		for (int InfoIndex = 0; InfoIndex < this->AssetNameInfoList.Num(); ++InfoIndex)
		{
			FString JudgeInfo = this->AssetNameInfoList[InfoIndex];

			for (FString& Tag : ValidCategoryTags)
			{
				if (JudgeInfo.StartsWith(Tag))
				{
					return AssetValidCategory;
				}
			}
		}
	}

	return AssetCategory::Undefined;
}

AssetCategory UCustomStandardObject::GetStrictAssetCategory()
{
	AssetCategory AssetValidCategory;

	bool ShouldBreak = false;

	for (AssetValidCategory = (AssetCategory)1;
		AssetValidCategory < AssetCategory::LastCatergory;
		AssetValidCategory = (AssetCategory)((uint8)AssetValidCategory + 1))
	{
		TArray<FString> ValidCategoryTags = UAssetsChecker::GetValidCategoryTag(AssetValidCategory, *AssetConfigGlobalSection);

		for (int InfoIndex = 0; InfoIndex < (this->AssetNameInfoList.Num() < 2 ? 1 : 2); ++InfoIndex)
		{
			if(!this->IsPrefixNonstandarized() && InfoIndex == 0)
			{
				continue;
			}

			FString JudgeInfo = this->AssetNameInfoList[InfoIndex];

			for (FString& Tag : ValidCategoryTags)
			{
				if (JudgeInfo.StartsWith(Tag))
				{
					return AssetValidCategory;
				}
			}
		}
	}

	return AssetCategory::Undefined;
}

bool UCustomStandardObject::IsCatogryStandarized()
{
	return !(this->GetStrictAssetCategory() == AssetCategory::Undefined);
}
