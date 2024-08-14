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
#include "PhysicsEngine/PhysicsAsset.h"

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
	{USkeletalMesh::StaticClass(), TEXT("SK")},
	{UNiagaraSystem::StaticClass(),TEXT("NS")},
	{UNiagaraEmitter::StaticClass(), TEXT("NE")},
	{UAnimSequence::StaticClass(), TEXT("A")},
	{UAnimMontage::StaticClass(), TEXT("AM")},
	{UPhysicsAsset::StaticClass(), TEXT("PHYS")}

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
	{TEXT("SkeletalMesh"),USkeletalMesh::StaticClass()},
	{TEXT("NiagaraSystem"),UNiagaraSystem::StaticClass()},
	{TEXT("NiagaraEmitter"),UNiagaraEmitter::StaticClass()},
	{TEXT("Animation"),UAnimSequence::StaticClass()},
	{TEXT("AnimationMontage"),UAnimMontage::StaticClass()},
	{TEXT("PhysicsAsset"),UPhysicsAsset::StaticClass()}

};

const TMap<TextureCompressionSettings, FString> TextureCompressionMap =
{
	{TC_Default ,"Default (BC1 or BC3 with A)"},
	{TC_Normalmap,"NormalMap (BC5)"},
	{TC_Masks,"Masks (no sRGB)"},
	{TC_Grayscale,"Grayscale (G8/16, RGB8 sRGB)"},
	{TC_Displacementmap,"DisplacementMap (G8/16)"},
	{TC_VectorDisplacementmap,"VectorDisplacementmap (RGBA8)"},
	{TC_HDR,"HDR (RGBA16F, no sRGB)"},
	{TC_EditorIcon,"UserInterface2D (RGBA8)"},
	{TC_Alpha,"Alpha (no sRGB, BC4)"},
	{TC_DistanceFieldFont,"DistanceFieldFont (G8)"},
	{TC_HDR_Compressed,"HDR Compressed (RGB, BC6H)"},
	{TC_BC7,"BC7 (RGBA)"},
	{TC_HalfFloat,"Half Float (R16F)"},
	{TC_LQ,"Low Quality (BGR565/BGR555A1)"},
	{TC_EncodedReflectionCapture,"EncodedReflectionCapture"},
	{TC_SingleFloat,"Single Float (R32F)"},
	{TC_HDR_F32,"HDR High Precision (RGBA32F)"},
	{TC_MAX,"MAX"}
};

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UAssetsChecker : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:    
#pragma region PublicFuntions

#pragma region Common Tools
	static bool bIsPowerOfTwo(const int num);
	static bool bIsPowerOfTwo(const uint32 num);
	static bool bIsPowerOfTwo(const double num);
	static bool bIsPowerOfTwo(const float num);

	static void ECopyAssetsPtrList(const TArray<TSharedPtr<FAssetData>>& ListToCopy, TArray<TSharedPtr<FAssetData>>& ListToOutput);

#pragma endregion

#pragma region Standard Check Tools
	
	int EDuplicateAssets(
		const TArray<FAssetData>& AssetsDataSelected, 
		int NumOfDupicates, 
		bool forced);
	
#pragma region FixPrefix
	static bool EConfirmPrefixes(
		TArray< TSharedPtr<FAssetData>>& AssetsSelected,
		TArray< TSharedPtr<FAssetData>>& ReadyToFixAssets);

	static void EAddPrefixes(
		const TArray<UObject*>& AssetsSelected);
	
	static void EAddPrefixes(
		const TArray<FAssetData>& AssetsSelected);
#pragma endregion

#pragma region GetReferences
	static TArray<FString> EGetAssetReferencesPath(const FString& AssetPath);
	static TArray<FString> EGetAssetReferencesPath(const FAssetData& AssetData);
	static TArray<FString> EGetAssetReferencesPath(const TSharedPtr<FAssetData>& AssetData);
#pragma endregion

#pragma region HandleRedirectors
	static void EFixUpRedirectors(const FString& Path = "/Game");
	static void EFixUpRedirectors(const TArray<FString>& Path);
#pragma endregion

#pragma region HandleTexture
	static FVector2D EGetTextureAssetSourceSize(
		const FAssetData& AssetData);

	static FVector2D EGetTextureAssetMaxInGameSize(
		const FAssetData& AssetData);
	
	static TSharedPtr<TEnumAsByte<TextureCompressionSettings>> EGetTextureAssetCompressionSettings(
		const FAssetData& AssetData);

	static bool ESetTextureAssetCompressionSettings(
		const FAssetData& AssetData,
		const TEnumAsByte<TextureCompressionSettings> & CompressionSetting);

	static bool EFixTextureMaxSizeInGame(FAssetData& ClickedAssetData, double maxSize, bool forced = false);
	static bool ESetTextureSize(FAssetData& ClickedAssetData, double maxSize);
#pragma endregion

#pragma region DeleteAssets
	static uint32 EDeleteAssets(const TArray<FAssetData>& AssetsData);
	static uint32 EDeleteAsset(const FAssetData& AssetData);
#pragma endregion

#pragma region HandleUnusedAssets
	void ERemoveUnusedAssets(const TArray<FAssetData>& AssetsDataSelected);
	static void ERemoveUnusedAssets(const TArray<FString>& FolderPathSelected);

	static void ERemoveEmptyFolder(const FString FolderPathSelected = "/Game");
	static void ERemoveEmptyFolder(const TArray<FString>& FolderPathSelected);
#pragma endregion

#pragma region HandleAssetsName
	int EReplaceName(const TArray<UObject*>& AssetsSelected, const FString& OriginStr, const FString& ReplaceStr);
#pragma endregion

#pragma region ListAssets
	static TArray<TSharedPtr<FAssetData>> EListAssetsDataPtrUnderSelectedFolder(const FString& FolderPathSelected);
	static TArray<TSharedPtr<FAssetData>> EListAssetsDataPtrUnderSelectedFolder(const TArray<FString>& FolderPathSelected);

	static void EListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
	static void EListPrefixErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
	static void EListSameNameErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
	static void EListMaxInGameSizeErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
	static void EListSourceSizeErrorAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& FindInList, TArray<TSharedPtr<FAssetData>>& OutList);
#pragma endregion

	// 留着为了测试 真没别的
	static void ECheckerCheck(const FAssetData & AssetData);

	UFUNCTION(CallInEditor)
	void CheckCheck(TEnumAsByte<TextureCompressionSettings> Compression);
#pragma endregion

#pragma region Call In Editor
	UFUNCTION(CallInEditor)
	void DuplicateAssets(int NumOfDupicates, bool forced = true);
	
	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();

	UFUNCTION(CallInEditor)
	void ReplaceName(const FString& OriginStr, const FString & ReplaceStr);
#pragma endregion

#pragma endregion

protected:
	static const TMap<UClass*, FString>& EGetPrefixMap();
};
