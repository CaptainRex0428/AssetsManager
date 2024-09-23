// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetsCreator/AssetsCreator.h"

/* Material includes*/
#include "Materials/Material.h"
#include "Materials/MaterialinstanceConstant.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "FileHelpers.h"

UMaterialInstanceConstant* UAssetsCreator::CreateMaterialConstant(
	FString PackageFolder, 
	FString AssetName, 
	UMaterialInterface* MaterialParent)
{
	PackageFolder.RemoveFromStart("/All");

	FString PackageName = FPaths::Combine(PackageFolder, AssetName);
	UPackage* Package = CreatePackage(*PackageName);

	auto MaterialFactory = NewObject<UMaterialInstanceConstantFactoryNew>();

	UMaterialInstanceConstant * Instance = 
		(UMaterialInstanceConstant*)MaterialFactory->FactoryCreateNew(
			UMaterialInstanceConstant::StaticClass(), 
			Package, *AssetName, 
			RF_Standalone | RF_Public, NULL, GWarn);
	
	FAssetRegistryModule::AssetCreated(Instance);
	Package->FullyLoad();

	Instance->SetParentEditorOnly(MaterialParent);
	Instance->MarkPackageDirty();
	Instance->PreEditChange(nullptr);
	FAssetRegistryModule::AssetCreated(Instance);
	Instance->PostEditChange();

	TArray<UPackage*> PackageToSave;
	PackageToSave.Add(Package);
	UEditorLoadingAndSavingUtils::SavePackages(PackageToSave, true);

	return nullptr;
}
