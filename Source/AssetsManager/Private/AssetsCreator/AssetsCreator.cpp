// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetsCreator/AssetsCreator.h"

#include "ManagerLogger.h"

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

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "LevelEditorSubsystem.h"
#include "LevelEditor.h"

#include "AssetTools.h"

UMaterialInstanceConstant* UAssetsCreator::CreateMaterialInstanceConstant(
	FString PackageFolder, 
	FString AssetName, 
	UMaterialInterface* MaterialParent)
{
	PackageFolder.RemoveFromStart("/All");

	FString PackageName = FPaths::Combine(PackageFolder, AssetName);
	UPackage* Package = CreatePackage(*PackageName);

	auto MaterialFactory = NewObject<UMaterialInstanceConstantFactoryNew>();

	// Create asset
	UMaterialInstanceConstant * Instance = 
		(UMaterialInstanceConstant*)MaterialFactory->FactoryCreateNew(
			UMaterialInstanceConstant::StaticClass(), 
			Package, *AssetName, 
			RF_Standalone | RF_Public, NULL, GWarn);
	
	FAssetRegistryModule::AssetCreated(Instance);
	
	//Load package
	Package->FullyLoad();

	//Set value
	Instance->SetParentEditorOnly(MaterialParent);

	Instance->MarkPackageDirty();
	Instance->PreEditChange(nullptr);
	Instance->PostEditChange();

	//Save package
	TArray<UPackage*> PackageToSave;
	PackageToSave.Add(Package);
	UEditorLoadingAndSavingUtils::SavePackages(PackageToSave, true);

	return Instance;
}

bool UAssetsCreator::ToggleMaterialInstanceConstantStaticSwitch(
	UMaterialInstanceConstant* MaterialInstance, 
	FString ParameterName,
	EMaterialParameterAssociation Association)
{
	UPackage * Pkg = MaterialInstance->GetPackage();
	Pkg->FullyLoad();

	MaterialInstance->MarkPackageDirty();
	MaterialInstance->PreEditChange(nullptr);

	MaterialInstance->SetStaticSwitchParameterValueEditorOnly(
		*ParameterName,
		GetMaterialInstanceConstantStaticSwitch(MaterialInstance, ParameterName, Association) ? false : true);
	
	MaterialInstance->PostEditChange();

	MaterialInstance->CacheShaders(EMaterialShaderPrecompileMode::Background);

	TArray<UPackage*> PackageToSave;
	PackageToSave.Add(Pkg);
	UEditorLoadingAndSavingUtils::SavePackages(PackageToSave, true);

	return GetMaterialInstanceConstantStaticSwitch(MaterialInstance, ParameterName, Association);
}

bool UAssetsCreator::GetMaterialInstanceConstantStaticSwitch(
	UMaterialInstanceConstant* MaterialInstance, 
	FString ParameterName,
	EMaterialParameterAssociation Association)
{
	bool value;
	FGuid SwitchGuid;

	if (MaterialInstance->GetStaticSwitchParameterValue(
		FMaterialParameterInfo(*ParameterName, Association, Association == LayerParameter ? 0 : INDEX_NONE),
		value,
		SwitchGuid))
	{
		return value;
	};

	return false;
	
}

bool UAssetsCreator::CreateLevel(const FString& AssetPath, const FString& TemplateAssetPath)
{
	ULevelEditorSubsystem* LevelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>();

	bool Success = false;

	if (!LevelEditorSubsystem)
	{
		return false;
	}

	if (TemplateAssetPath.IsEmpty())
	{
		Success = LevelEditorSubsystem->NewLevel(AssetPath);
	}
	else
	{
		Success = LevelEditorSubsystem->NewLevelFromTemplate(AssetPath, TemplateAssetPath);
	}

	if (!Success)
	{
		Success = LevelEditorSubsystem->LoadLevel(AssetPath);
	}

	return Success;
}
