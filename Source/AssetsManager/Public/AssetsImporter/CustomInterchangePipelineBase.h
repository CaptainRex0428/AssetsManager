// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InterchangePipelineBase.h"
#include "InterchangeFactoryBase.h"
#include "InterchangeTranslatorBase.h"
#include "InterchangeWriterBase.h"
#include "InterchangeSourceData.h"

#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/StaticMesh.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxAssetImportData.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxMeshImportData.h"
#include "Factories/FbxSkeletalMeshImportData.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "Factories/FbxTextureImportData.h"
#include "InterchangeAssetImportData.h"
// #include "Fbx/InterchangeFbxTranslator.h"
#include "InterchangeAnimSequenceFactoryNode.h"
#include "InterchangeGenericAnimationPipeline.h"
#include "InterchangeGenericAssetsPipeline.h"
#include "InterchangeGenericAssetsPipelineSharedSettings.h"
#include "InterchangeGenericMaterialPipeline.h"
#include "InterchangeGenericMeshPipeline.h"
#include "InterchangeGenericTexturePipeline.h"
#include "InterchangeSkeletalMeshFactoryNode.h"
#include "InterchangeStaticMeshFactoryNode.h"
#include "InterchangeGenericScenesPipeline.h"
#include "InterchangeManager.h"
#include "Nodes/InterchangeBaseNodeContainer.h"

#include "Engine/StaticMesh.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"       
#include "PhysicsEngine/PhysicsAsset.h" 

#include "CustomInterchangePipelineBase.generated.h"

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UCustomInterchangePipeline_Base : public UInterchangePipelineBase
{
	GENERATED_BODY()
	
public:
	virtual void ExecutePostImportPipeline(
		const UInterchangeBaseNodeContainer* BaseNodeContainer, 
		const FString& NodeKey,
		UObject* CreatedAsset,
		bool bIsAReimport) override;

private:
	bool IsAssetQualified(UObject* SkeletalMesh);
};

UCLASS()
class ASSETSMANAGER_API UCustomAssetsGenericInterchangePipeline : public UInterchangeGenericAssetsPipeline
{
		GENERATED_BODY()

public:
	virtual void ExecutePipeline(
		UInterchangeBaseNodeContainer* InBaseNodeContainer, 
		const TArray<UInterchangeSourceData*>& InSourceDatas, 
		const FString& ContentBasePath) override;
	
	virtual void ExecutePostFactoryPipeline(
		const UInterchangeBaseNodeContainer* BaseNodeContainer, 
		const FString& NodeKey, 
		UObject* CreatedAsset, 
		bool bIsAReimport) override;
	
	virtual void ExecutePostImportPipeline(
		const UInterchangeBaseNodeContainer* BaseNodeContainer,
		const FString& NodeKey,
		UObject* CreatedAsset,
		bool bIsAReimport) override;

private:
	bool ValidateNode(UInterchangeBaseNode* Node);

private:
	bool bShouldDeleteImportedAssets = false;
};
