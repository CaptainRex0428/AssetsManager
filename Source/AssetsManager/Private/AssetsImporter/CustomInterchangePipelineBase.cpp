// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsImporter/CustomInterchangePipelineBase.h"
#include "AssetsChecker/AssetsChecker.h"
#include "StandardAsset/CustomStandardAsset.h"
#include "StandardAsset/CustomStandardSkeletalMesh.h"
#include "ManagerLogger.h"

void UCustomInterchangePipeline_Base::ExecutePostImportPipeline(
	const UInterchangeBaseNodeContainer* BaseNodeContainer, 
	const FString& NodeKey, 
	UObject* CreatedAsset, 
	bool bIsAReimport)
{
}

bool UCustomInterchangePipeline_Base::IsAssetQualified(UObject* SkeletalMesh)
{
	return true;
}

void UCustomAssetsGenericInterchangePipeline::ExecutePipeline(
	UInterchangeBaseNodeContainer* InBaseNodeContainer, 
	const TArray<UInterchangeSourceData*>& InSourceDatas, 
	const FString& ContentBasePath)
{
	this->bShouldDeleteImportedAssets =false;

	MsgLog(FString::Printf(L"ExePipeline:%s",*ContentBasePath));

	TArray<UInterchangeBaseNode*> NodesToRemove;
	TArray<UInterchangeBaseNode*> NodesOfClass;
	InBaseNodeContainer->IterateNodes(
		[this](const FString & NodeName,UInterchangeBaseNode * BaseNode)
		{
			if (!this->ValidateNode(BaseNode))
			{
				this->bShouldDeleteImportedAssets = true;
			}
		});

	Super::ExecutePipeline(InBaseNodeContainer, InSourceDatas, ContentBasePath);
}

void UCustomAssetsGenericInterchangePipeline::ExecutePostFactoryPipeline(
	const UInterchangeBaseNodeContainer* BaseNodeContainer, 
	const FString& NodeKey, 
	UObject* CreatedAsset, 
	bool bIsAReimport)
{
	MsgLog(FString::Printf(L"ExePostFactoryPipeline:%s", *CreatedAsset->GetFName().ToString()));

	// Judge SkeletalMesh 

	UCustomStandardSkeletalMeshObject StandardSK(CreatedAsset);
	
	if (StandardSK.IsSkeletalMesh())
	{
		FString ErrorInfo = "";

		if(StandardSK.IsPrefixNonstandarized())
		{
			this->bShouldDeleteImportedAssets = true;
			ErrorInfo += "- Prefix Error\n";
		}
		
		if(StandardSK.GetCommonAssetCategory() == AssetCategory::Character && StandardSK.GetLODTrianglesNum(0) > 500000)
		{
			this->bShouldDeleteImportedAssets = true;
			ErrorInfo += "- Triangle count exceeds the standard limit.\n";
		}

		if (this->bShouldDeleteImportedAssets)
		{
			DlgMsgLog(EAppMsgType::Ok,FString::Printf(L"%s:\n%s",*StandardSK.GetClassValidObjectName(),*ErrorInfo));
			return;
		}
	}

	Super::ExecutePostFactoryPipeline(BaseNodeContainer, NodeKey, CreatedAsset, bIsAReimport);
}

void UCustomAssetsGenericInterchangePipeline::ExecutePostImportPipeline(
	const UInterchangeBaseNodeContainer* BaseNodeContainer, 
	const FString& NodeKey, 
	UObject* CreatedAsset, 
	bool bIsAReimport)
{
	MsgLog(FString::Printf(L"ExePostImportPipeline:%s", *CreatedAsset->GetFName().ToString()));
	
	if (this->bShouldDeleteImportedAssets)
	{
		MsgLog(FString::Printf(L"%s Skipping import of related assets (MaterialInstance, Physics, Skeleton) due to SkeletalMesh validation failure.", *CreatedAsset->GetFName().ToString()));
		UAssetsChecker::DeleteObject(CreatedAsset);
	}

	Super::ExecutePostImportPipeline(BaseNodeContainer, NodeKey, CreatedAsset, bIsAReimport);
}

bool UCustomAssetsGenericInterchangePipeline::ValidateNode(UInterchangeBaseNode* Node)
{
	FString NodeName = Node->GetDisplayLabel();
	return true;
}
