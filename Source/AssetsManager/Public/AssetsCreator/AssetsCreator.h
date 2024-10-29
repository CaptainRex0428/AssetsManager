// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "ConfigManager.h"

/*
 * 
 */
class ASSETSMANAGER_API UAssetsCreator : public UAssetActionUtility
{

public:
	static UMaterialInstanceConstant* CreateMaterialInstanceConstant(
		FString PackageFolder,
		FString AssetName,
		UMaterialInterface* MaterialParent);

	static bool ToggleMaterialInstanceConstantStaticSwitch(
		UMaterialInstanceConstant* MaterialInstance,
		FString ParameterName,
		EMaterialParameterAssociation Association = EMaterialParameterAssociation::GlobalParameter
	);

	static bool GetMaterialInstanceConstantStaticSwitch(
		UMaterialInstanceConstant* MaterialInstance,
		FString ParameterName,
		EMaterialParameterAssociation Association = EMaterialParameterAssociation::GlobalParameter
	);

	static bool CreateLevel();
};
