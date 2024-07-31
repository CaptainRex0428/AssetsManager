// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialinstanceConstant.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Engine/Texture.h"
#include "Blueprint/Userwidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"

#include "AssetsChecker.generated.h"

const TMap<UClass*, FString> PrefixMap =
{
	{UBlueprint::StaticClass(),TEXT("BP")},
	{UStaticMesh::StaticClass(),TEXT("SM")},
	{UMaterial::StaticClass(),TEXT("M") },
	{UMaterialInstanceConstant::StaticClass(),TEXT("MI")},
	{UMaterialFunctionInterface::StaticClass(), TEXT("MF")},
	{UParticleSystem::StaticClass(),TEXT("PS")},
	{USoundCue::StaticClass(),TEXT("SC")},
	{USoundWave::StaticClass(), TEXT("SW")},
	{UTexture::StaticClass(),TEXT("T")},
	{UTexture2D::StaticClass(),TEXT("TArray")},
	{UUserWidget::StaticClass(), TEXT("WBP")},
	{USkeletalMeshComponent::StaticClass(), TEXT("SK")},
	{UNiagaraSystem::StaticClass(),TEXT("NS")},
	{UNiagaraEmitter::StaticClass(), TEXT("NE")}
};

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsChecker : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:

	int EDuplicateAssets(const TArray<FAssetData>& AssetsDataSelected, int NumOfDupicates, bool forced);
	UFUNCTION(CallInEditor)
	void DuplicateAssets(int NumOfDupicates, bool forced = true);

	void EAddPrefixes(const TArray<UObject*> & AssetsSelected);
	UFUNCTION(CallInEditor)
	void AddPrefixes();

	void ERemoveUnusedAssets(const TArray<FAssetData>& AssetsDataSelected);
	static void ERemoveUnusedAssets(const TArray<FString> & FolderPathSelected);
	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();

	static void ERemoveEmptyFolder(const FString FolderPathSelected = "/Game");
	static void ERemoveEmptyFolder(const TArray<FString>& FolderPathSelected);

	static TArray<TSharedPtr<FAssetData>> EListAssetsDataPtrUnderSelectedFolder(const FString& FolderPathSelected);
	static TArray<TSharedPtr<FAssetData>> EListAssetsDataPtrUnderSelectedFolder(const TArray<FString>& FolderPathSelected);

	int EReplaceName(const TArray<UObject*>& AssetsSelected, const FString& OriginStr, const FString& ReplaceStr);
	UFUNCTION(CallInEditor)
	void ReplaceName(const FString& OriginStr, const FString & ReplaceStr);

	static void EFixUpRedirectors(const FString& Path = "/Game");
	static void EFixUpRedirectors(const TArray<FString> & Path);

	static void ECheckerCheck(const TArray<FString>& Path);

protected:
	static const TMap<UClass*, FString>& EGetPrefixMap();
};
