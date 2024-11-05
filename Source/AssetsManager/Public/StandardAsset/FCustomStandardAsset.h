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
#include "WidgetBlueprint.h"

#include "ConfigManager.h"

#define VNAME_STRUCT(value) VNAME(value),value
#define VCLASSNAME_STRUCT(value) value,value->GetName()

UENUM(BlueprintType)
enum class AssetCategory : uint8
{
	Undefined = 0 UMETA(DisplayName = "Undefined", ToolTip="Tags are defined in AssetsManger.ini"),
	Character UMETA(DisplayName = "Character", ToolTip = "Tags are defined in AssetsManger.ini"),
	Effect UMETA(DisplayName = "Effect", ToolTip = "Tags are defined in AssetsManger.ini"),
	Scene UMETA(DisplayName = "Scene", ToolTip = "Tags are defined in AssetsManger.ini"),
	UI UMETA(DisplayName = "UI", ToolTip = "Tags are defined in AssetsManger.ini"),
	Hair UMETA(DisplayName = "Hair", ToolTip = "Tags are defined in AssetsManger.ini"),
	LastCatergory
};

class ASSETSMANAGER_API UCustomStandardObject : public UObject
{
	
};

/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardAssetData : public FAssetData
{
public:
	

	FCustomStandardAssetData(const FAssetData & AssetData, bool StricCheckMode = false);
	virtual ~FCustomStandardAssetData();

	TSharedPtr<FString> GetAssetNameInfoByIndex(
		const int32 & index,
		bool bContainsInfoStartIndex = false);

	const TSharedPtr<FString> GetAssetPrefix() const;
	const TSharedPtr<FString> GetAssetSuffix();
	const uint32 GetAssetNameInfoCount() const;
	FString GetAssetNameWithoutPrefix() const;
	
	bool IsPrefixStandarized() const;
	const TSharedPtr<FString> GetAssetStandardPrefix();

	const AssetCategory& GetCommonAssetCategory();
	const AssetCategory& GetStrictAssetCategory();
	const AssetCategory GetConfirmAssetCategory();
	bool IsCatogryStandarized();

	virtual int64 GetMemorySize(bool bEstimatedTotal = true);
	virtual int64 GetMemorySize(AssetsInfoDisplayLevel& DisplayLevel,bool bEstimatedTotal = true);
	virtual int64 GetDiskSize();
	virtual int64 GetDiskSize(AssetsInfoDisplayLevel& DisplayLevel);

private:

	TArray<FString> GetValidCategoryTag(
		AssetCategory Cate);

protected:
	bool bStrictCheckMode;
	
	TArray<FString> m_AssetNameInfoList;

	uint32 m_AssetNameInfoStartIndex;

	AssetCategory m_CommonAssetCategory;
	TSharedPtr<FString> m_CommonAssetCategoryTag;

	AssetCategory m_StrictAssetCategory;
	TSharedPtr<FString> m_StrictAssetCategoryTag;

	TSharedPtr<FString> AssetConfigGlobalSection;

	bool bHasStandardPrefix;
};

static const TMap<UClass*, FString> UClassNameMap =
{
	{VCLASSNAME_STRUCT(UBlueprint::StaticClass())},
	{VCLASSNAME_STRUCT(UStaticMesh::StaticClass())},
	{VCLASSNAME_STRUCT(UMaterial::StaticClass())},
	{VCLASSNAME_STRUCT(UMaterialInstanceConstant::StaticClass())},
	{VCLASSNAME_STRUCT(UMaterialFunctionInterface::StaticClass())},
	{VCLASSNAME_STRUCT(UMaterialFunction::StaticClass())},
	{VCLASSNAME_STRUCT(UParticleSystem::StaticClass())},
	{VCLASSNAME_STRUCT(USoundCue::StaticClass())},
	{VCLASSNAME_STRUCT(USoundWave::StaticClass())},
	{VCLASSNAME_STRUCT(UTexture2D::StaticClass())},
	{VCLASSNAME_STRUCT(UTexture2DArray::StaticClass())},
	{VCLASSNAME_STRUCT(UUserWidget::StaticClass())},
	{VCLASSNAME_STRUCT(UWidgetBlueprint::StaticClass())},
	{VCLASSNAME_STRUCT(USkeletalMesh::StaticClass())},
	{VCLASSNAME_STRUCT(UNiagaraSystem::StaticClass())},
	{VCLASSNAME_STRUCT(UNiagaraEmitter::StaticClass())},
	{VCLASSNAME_STRUCT(UAnimSequence::StaticClass())},
	{VCLASSNAME_STRUCT(UAnimMontage::StaticClass())},
	{VCLASSNAME_STRUCT(UPhysicsAsset::StaticClass())}

};