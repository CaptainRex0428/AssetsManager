// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "ConfigManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "AssetsCreator/AssetsCreator.h"
#include "DSAssetsHandler.generated.h"

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UDSAssetsHandler : public UAssetActionUtility
{
	GENERATED_BODY()
	
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "DSAssetsHandler")
	void CreateIDCheckerMaterial(
		TextureChannel Channel,
		int32 UVs);
	
};
