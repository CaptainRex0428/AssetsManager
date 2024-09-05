#include "AssetsConfigManager.h"

UAssetsConfigManager& UAssetsConfigManager::Get()
{
	static UAssetsConfigManager instance;
	return instance;
}

TArray<FString> UAssetsConfigManager::GetCategoryValidTag(FString& Categoty)
{
	FString SectionName = "AssetsManager/Global";
	FString KeyName = "+" + Categoty + "CategoryTag";
	
	TArray<FConfigValue> ValueArray 
		= UConfigManager::Get().GetSectionValuesArray(*SectionName, *KeyName);

	TArray<FString> ValidTag;
	ValidTag.Empty();

	if(ValueArray.Num() ==0)
	{
		return ValidTag;
	}

	for (FConfigValue& ConfigValue : ValueArray)
	{
		ValidTag.Add(ConfigValue.GetValue());
	}

	return ValidTag;
}