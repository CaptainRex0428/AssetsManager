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

	// �ж� CreatedAsset �����ͣ�ʶ�� Skeletal Mesh ��������ļ�
	if (CreatedAsset->IsA(USkeletalMesh::StaticClass()))
	{
		SkeletalMeshAsset = CreatedAsset; // ��¼ Skeletal Mesh
	}
	else if (CreatedAsset->IsA(UPhysicsAsset::StaticClass()) || CreatedAsset->IsA(USkeleton::StaticClass()))
	{
		RelatedAssets.Add(CreatedAsset); // �����صĹ����������ʲ�
	}

	// �ж� Skeletal Mesh �Ƿ�ϸ�
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
