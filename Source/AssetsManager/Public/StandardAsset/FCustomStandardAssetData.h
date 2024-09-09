// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfigManager.h"

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

#define VNAME_STRUCT(value) VNAME(value),value

/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardAssetData : public FAssetData
{
public:
	enum Category
	{
		Undefined = 0,
		Character, Effect, Scene, UI, Hair,
		LastCatergory
	};

	FCustomStandardAssetData(const FAssetData & AssetData, bool StricCheckMode = false);
	virtual ~FCustomStandardAssetData();

	TSharedPtr<FString> GetAssetNameInfoByIndex(
		const int32 & index,
		bool bContainsInfoStartIndex = false);

	const TSharedPtr<FString> GetAssetPrefix() const;
	const TSharedPtr<FString> GetAssetSuffix();
	const uint32 GetAssetNameInfoCount() const;
	
	bool IsPrefixStandarized() const;
	const TSharedPtr<FString> GetAssetStandardPrefix();

	const FCustomStandardAssetData::Category& GetCommonAssetCategory();
	const FCustomStandardAssetData::Category& GetStrictAssetCategory();
	const FCustomStandardAssetData::Category GetConfirmAssetCategory();
	bool IsCatogryStandarized();

private:
	TArray<FString> SplitStringRecursive(
		const FString& InStr,
		const FString& SpliteTag);

	TArray<FString> GetValidCategoryTag(
		Category Cate);

protected:
	bool bStrictCheckMode;
	
	TArray<FString> m_AssetNameInfoList;

	uint32 m_AssetNameInfoStartIndex;

	FCustomStandardAssetData::Category m_CommonAssetCategory;
	TSharedPtr<FString> m_CommonAssetCategoryTag;

	FCustomStandardAssetData::Category m_StrictAssetCategory;
	TSharedPtr<FString> m_StrictAssetCategoryTag;

	TSharedPtr<FString> AssetConfigGlobalSection;

	bool bHasStandardPrefix;
};

static const TMap<UClass*, FString> UClassNameMap =
{
	{UBlueprint::StaticClass(),UBlueprint::StaticClass()->GetName()},
	{UStaticMesh::StaticClass(),UStaticMesh::StaticClass()->GetName()},
	{UMaterial::StaticClass(),UMaterial::StaticClass()->GetName()},
	{UMaterialInstanceConstant::StaticClass(),UMaterialInstanceConstant::StaticClass()->GetName()},
	{UMaterialFunctionInterface::StaticClass(),UMaterialFunctionInterface::StaticClass()->GetName()},
	{UParticleSystem::StaticClass(),UParticleSystem::StaticClass()->GetName()},
	{USoundCue::StaticClass(),USoundCue::StaticClass()->GetName()},
	{USoundWave::StaticClass(),USoundWave::StaticClass()->GetName()},
	{UTexture2D::StaticClass(),UTexture2D::StaticClass()->GetName()},
	{UTexture2DArray::StaticClass(),UTexture2DArray::StaticClass()->GetName()},
	{UUserWidget::StaticClass(),UUserWidget::StaticClass()->GetName()},
	{USkeletalMesh::StaticClass(),USkeletalMesh::StaticClass()->GetName()},
	{UNiagaraSystem::StaticClass(),UNiagaraSystem::StaticClass()->GetName()},
	{UNiagaraEmitter::StaticClass(),UNiagaraEmitter::StaticClass()->GetName()},
	{UAnimSequence::StaticClass(),UAnimSequence::StaticClass()->GetName()},
	{UAnimMontage::StaticClass(),UAnimMontage::StaticClass()->GetName()},
	{UPhysicsAsset::StaticClass(),UPhysicsAsset::StaticClass()->GetName()}

};