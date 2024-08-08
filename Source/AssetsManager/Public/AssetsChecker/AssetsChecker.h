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
#include "Engine/Texture2DArray.h"
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
	{UTexture2D::StaticClass(),TEXT("T")},
	{UTexture2DArray::StaticClass(),TEXT("TArray")},
	{UUserWidget::StaticClass(), TEXT("WBP")},
	{USkeletalMeshComponent::StaticClass(), TEXT("SK")},
	{UNiagaraSystem::StaticClass(),TEXT("NS")},
	{UNiagaraEmitter::StaticClass(), TEXT("NE")}
};

const TMap<FString,UClass*> AssetFullNameMap =
{
	{TEXT("Blueprint"),UBlueprint::StaticClass()},
	{TEXT("StaticMesh"),UStaticMesh::StaticClass()},
	{TEXT("Material"),UMaterial::StaticClass()},
	{TEXT("MaterialInstance"),UMaterialInstanceConstant::StaticClass()},
	{TEXT("MaterialFunction"),UMaterialFunctionInterface::StaticClass()},
	{TEXT("ParticleSystem"),UParticleSystem::StaticClass()},
	{TEXT("SoundCue"),USoundCue::StaticClass()},
	{TEXT("SoundWave"),USoundWave::StaticClass()},
	{TEXT("Texture"),UTexture2D::StaticClass()},
	{TEXT("TextureArray"),UTexture2DArray::StaticClass()},
	{TEXT("UserWidget"),UUserWidget::StaticClass()},
	{TEXT("SkeletalMesh"),USkeletalMeshComponent::StaticClass()},
	{TEXT("NiagaraSystem"),UNiagaraSystem::StaticClass()},
	{TEXT("NiagaraEmitter"),UNiagaraEmitter::StaticClass()}
};

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsChecker : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:

	static bool bIsPowerOfTwo(const int num);
	static bool bIsPowerOfTwo(const uint32 num);
	static bool bIsPowerOfTwo(const double num);
	static bool bIsPowerOfTwo(const float num);

	int EDuplicateAssets(const TArray<FAssetData>& AssetsDataSelected, int NumOfDupicates, bool forced);
	UFUNCTION(CallInEditor)
	void DuplicateAssets(int NumOfDupicates, bool forced = true);

	void EAddPrefixes(const TArray<UObject*> & AssetsSelected);
	UFUNCTION(CallInEditor)
	void AddPrefixes();

	static TArray<FString> EGetAssetReferencesPath(const FString & AssetPath);
	static TArray<FString> EGetAssetReferencesPath(const FAssetData & AssetData);
	static TArray<FString> EGetAssetReferencesPath(const TSharedPtr<FAssetData> & AssetData);

	static FVector2D EGetTextureAssetSourceSize(const FAssetData& AssetData);
	static FVector2D EGetTextureAssetMaxInGameSize(const FAssetData& AssetData);

	static bool EFixTextureMaxSizeInGame(FAssetData& ClickedAssetData, double maxSize, bool forced = false);
	static bool ESetTextureSize(FAssetData& ClickedAssetData, double maxSize);

	static void EListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>> & OutList);
	static void EListPrefixErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
	static void EListMaxInGameSizeErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
	static void EListSourceSizeErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);

	static uint32 EDeleteAssets(const TArray<FAssetData> & AssetsData);
	static uint32 EDeleteAsset(const FAssetData & AssetData);

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

	static void ECopyAssetsPtrList(const TArray<TSharedPtr<FAssetData>>& ListToCopy, TArray<TSharedPtr<FAssetData>>& ListToOutput);
	
	static void ECheckerCheck(const TArray<FString>& Path);

protected:
	static const TMap<UClass*, FString>& EGetPrefixMap();
};
