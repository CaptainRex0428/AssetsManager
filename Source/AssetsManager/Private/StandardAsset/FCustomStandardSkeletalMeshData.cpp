// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardSkeletalMeshData.h"
#include "Rendering/SkeletalMeshModel.h"

FCustomStandardSkeletalMeshData::FCustomStandardSkeletalMeshData(FAssetData& AssetData)
	:FCustomStandardAssetData(AssetData)
{
	this-> bSkeletalMesh = this->GetAsset()->IsA<USkeletalMesh>();

	if (this->bSkeletalMesh)
	{

	}
}

FCustomStandardSkeletalMeshData::~FCustomStandardSkeletalMeshData()
{
}

int32 FCustomStandardSkeletalMeshData::GetLODNum()
{
	if (!this->bSkeletalMesh)
	{
		return 0;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	if (SKAsset) 
	{
		return SKAsset->GetLODNum();
	};

	return 0;
}

int32 FCustomStandardSkeletalMeshData::GetLODTrianglesNum(
	int32 Index)
{
	if (!this->bSkeletalMesh)
	{
		return 0;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	const FSkeletalMeshLODModel & SKMesh 
		= SKAsset->GetImportedModel()->LODModels[Index];

	int LODTriNum = 0;

	for (int32 SectionIndex = 0; SectionIndex < SKMesh.Sections.Num(); ++SectionIndex)
	{
		const FSkelMeshSection& Section = SKMesh.Sections[SectionIndex];

		LODTriNum += Section.NumTriangles;
	}

	return LODTriNum;
}

int32 FCustomStandardSkeletalMeshData::GetLODVertexNum(int32 Index)
{
	if (!this->bSkeletalMesh)
	{
		return 0;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	const FSkeletalMeshLODModel& SKMesh
		= SKAsset->GetImportedModel()->LODModels[Index];

	int LODVertexNum = 0;

	for (int32 SectionIndex = 0; SectionIndex < SKMesh.Sections.Num(); ++SectionIndex)
	{
		const FSkelMeshSection& Section = SKMesh.Sections[SectionIndex];

		LODVertexNum += Section.NumVertices < 0? 0 : Section.NumVertices;
	}

	return LODVertexNum;
}
