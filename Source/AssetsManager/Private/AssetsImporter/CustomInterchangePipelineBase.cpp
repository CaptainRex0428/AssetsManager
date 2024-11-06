// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsImporter/CustomInterchangePipelineBase.h"

#include "ManagerLogger.h"

void UCustomInterchangePipelineBase::ExecutePostImportPipeline(
	const UInterchangeBaseNodeContainer* BaseNodeContainer, 
	const FString& NodeKey, 
	UObject* CreatedAsset, 
	bool bIsAReimport)
{
	TArray<UObject*> RelatedAssets;
	UObject* SkeletalMeshAsset = nullptr;

	// 判断 CreatedAsset 的类型，识别 Skeletal Mesh 和其相关文件
	if (CreatedAsset->IsA(USkeletalMesh::StaticClass()))
	{
		SkeletalMeshAsset = CreatedAsset; // 记录 Skeletal Mesh
	}
	else if (CreatedAsset->IsA(UPhysicsAsset::StaticClass()) || CreatedAsset->IsA(USkeleton::StaticClass()))
	{
		RelatedAssets.Add(CreatedAsset); // 添加相关的骨骼或物理资产
	}

	// 判断 Skeletal Mesh 是否合格
	if (SkeletalMeshAsset && !IsAssetQualified(SkeletalMeshAsset))
	{
		
	}
	else
	{
		MsgLog(TEXT("Skeletal Mesh is qualified."));
	}

	MsgLog(TEXT("GetHere"));
}

bool UCustomInterchangePipelineBase::IsAssetQualified(UObject* SkeletalMesh)
{
	return true;
}
