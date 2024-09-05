// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfigManager.h"

/**
 * 
 */
class ASSETSMANAGER_API UAssetsConfigManager
{
public:
	static UAssetsConfigManager& Get();
	UAssetsConfigManager(UAssetsConfigManager&) = delete;

	TArray<FString> GetCategoryValidTag(FString& Categoty);
	
private:
	UAssetsConfigManager();
	~UAssetsConfigManager() = default;
};
