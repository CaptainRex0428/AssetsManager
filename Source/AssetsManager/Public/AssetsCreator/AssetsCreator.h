// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetsChecker/AssetsChecker.h"
#include "AssetsCreator.generated.h"

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsCreator : public UAssetsChecker
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,Category = "AssetsCreator|Material")
	UMaterialInstanceConstant* CreateMaterialConstant(
		FString PackageFolder,
		FString AssetName,
		UMaterialInterface* MaterialParent);
};
