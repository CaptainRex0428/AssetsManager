// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardSkeletalMeshData.h"

#include "ManagerLogger.h"

#include "LODUtilities.h"

#include "Engine/RendererSettings.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/SkinnedAssetAsyncCompileUtils.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "Factories/FbxSkeletalMeshImportData.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

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

bool FCustomStandardSkeletalMeshData::IsSkeletalMesh() const
{
	return bSkeletalMesh;
}

bool FCustomStandardSkeletalMeshData::HasLODMeshDescription(int32 LODIdx)
{
	if(!bSkeletalMesh)
	{
		return false;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	return SKAsset->HasMeshDescription(LODIdx);
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

bool FCustomStandardSkeletalMeshData::GetAllowCPUAccess(int32 LODIndex)
{
	if (!this->bSkeletalMesh)
	{
		return false;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	if(!SKAsset->IsValidLODIndex(LODIndex))
	{
		return false;
	}

	return SKAsset->GetLODInfo(LODIndex)->bAllowCPUAccess;
}

void FCustomStandardSkeletalMeshData::SetLODsAllowCPUAccess(bool CPUAccess)
{
	if (!this->bSkeletalMesh)
	{
		return;
	}

	USkeletalMesh* SkelMesh = Cast<USkeletalMesh>(this->GetAsset());

	for (int idx =0; idx < SkelMesh->GetLODNum(); ++idx)
	{
		if (GetAllowCPUAccess(idx) != CPUAccess)
		{
			SkelMesh->Modify();

			SkelMesh->GetLODInfo(idx)->bAllowCPUAccess = CPUAccess;
		}
	}

	FLODUtilities::RegenerateLOD(
		SkelMesh,
		GetTargetPlatformManagerRef().GetRunningTargetPlatform(),
		SkelMesh->GetLODNum()
	);

	UEditorAssetLibrary::SaveAsset(this->GetObjectPathString());

	return;
}

bool FCustomStandardSkeletalMeshData::SetAllowCPUAccess(int32 SourceLODIndex, bool CPUAccessState)
{
	if (!this->bSkeletalMesh)
	{
		return false;
	}

	USkeletalMesh* SkelMesh = Cast<USkeletalMesh>(this->GetAsset());

	if (SkelMesh->IsValidLODIndex(SourceLODIndex))
	{
		
		if (GetAllowCPUAccess(SourceLODIndex) != CPUAccessState)
		{
			SkelMesh->Modify();

			SkelMesh->GetLODInfo(SourceLODIndex)->bAllowCPUAccess = CPUAccessState;

			FLODUtilities::RegenerateLOD(
				SkelMesh,
				GetTargetPlatformManagerRef().GetRunningTargetPlatform(),
				SkelMesh->GetLODNum()
				);
		};
	}

	return UEditorAssetLibrary::SaveAsset(this->GetObjectPathString());
}
