// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "ConfigManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "AssetsCreator/AssetsCreator.h"

#include "AssetsHandler.generated.h"

UENUM(BlueprintType)
enum class TextureChannel : uint8
{
	Red = 0 UMETA(DisplayName = "Red"),
	Green = 1 UMETA(DisplayName = "Green"),
	Blue = 2 UMETA(DisplayName = "Blue"),
	Alpha = 3 UMETA(DisplayName = "Alpha")
};

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsHandler : public UAssetActionUtility
{
	GENERATED_BODY()
	
#pragma region CallInEditor

	UFUNCTION(CallInEditor, Category = "AssetsHandler")
	void CreateIDCheckerMaterial(
		TextureChannel Channel,
		int32 UVs,
		bool CheckNum = false);

	UFUNCTION(CallInEditor, Category = "AssetsHandler")
	void DuplicateAssets(
		int NumOfDupicates,
		bool forced = true);

	UFUNCTION(CallInEditor, Category = "AssetsHandler")
	void RemoveUnusedAssets();

	UFUNCTION(CallInEditor, Category = "AssetsHandler")
	void ReplaceName(
		const FString& OriginStr,
		const FString& ReplaceStr);

	UFUNCTION(CallInEditor, Category = "AssetsHandler")
	void Check();

#pragma endregion

#pragma region CallInEditor

#pragma endregion

};
