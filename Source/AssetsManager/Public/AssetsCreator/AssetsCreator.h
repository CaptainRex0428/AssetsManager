// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "ConfigManager.h"

#include "AssetsCreator.generated.h"

UENUM(BlueprintType)
enum class TextureChannel : uint8
{
	Red=0 UMETA(DisplayName="Red"),
	Green=1 UMETA(DisplayName = "Green"),
	Blue=2 UMETA(DisplayName = "Blue"),
	Alpha=3 UMETA(DisplayName = "Alpha")
};

/*
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsCreator : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,Category = "AssetsCreator|Material")
	static UMaterialInstanceConstant* CreateMaterialInstanceConstant(
		FString PackageFolder,
		FString AssetName,
		UMaterialInterface* MaterialParent);

	UFUNCTION(BlueprintCallable, Category = "AssetsCreator|Material")
	static bool ToggleMaterialInstanceConstantStaticSwitch(
		UMaterialInstanceConstant* MaterialInstance,
		FString ParameterName,
		EMaterialParameterAssociation Association = EMaterialParameterAssociation::GlobalParameter
	);

	UFUNCTION(BlueprintCallable, Category = "AssetsCreator|Material")
	static bool GetMaterialInstanceConstantStaticSwitch(
		UMaterialInstanceConstant* MaterialInstance,
		FString ParameterName,
		EMaterialParameterAssociation Association = EMaterialParameterAssociation::GlobalParameter
	);
};
