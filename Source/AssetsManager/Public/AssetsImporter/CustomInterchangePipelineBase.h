// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InterchangePipelineBase.h"

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
class ASSETSMANAGER_API UCustomInterchangePipelineBase : public UInterchangePipelineBase
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
