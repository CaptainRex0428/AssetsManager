// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardSkeletalMeshData.h"

#include "ManagerLogger.h"


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

USkeletalMesh* FCustomStandardSkeletalMeshData::GetSkeletalMesh()
{
	if (!bSkeletalMesh) 
	{
		return nullptr;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	return SKAsset;
}

bool FCustomStandardSkeletalMeshData::HasLODMeshDescription(int32 LODIdx)
{
	if (!this->bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIdx))
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
	int32 LODIndex)
{
	if (!this->bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIndex))
	{
		return 0;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	const FSkeletalMeshLODModel & SKMesh 
		= SKAsset->GetImportedModel()->LODModels[LODIndex];

	int LODTriNum = 0;

	for (int32 SectionIndex = 0; SectionIndex < SKMesh.Sections.Num(); ++SectionIndex)
	{
		const FSkelMeshSection& Section = SKMesh.Sections[SectionIndex];

		LODTriNum += Section.NumTriangles;
	}

	return LODTriNum;
}

int32 FCustomStandardSkeletalMeshData::GetLODVertexNum(int32 LODIndex)
{
	if (!this->bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIndex))
	{
		return 0;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	const FSkeletalMeshLODModel& SKMesh
		= SKAsset->GetImportedModel()->LODModels[LODIndex];

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
	if (!this->bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIndex))
	{
		return false;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

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

TArray<FSkeletalMaterial> FCustomStandardSkeletalMeshData::GetMaterialSlots()
{
	if (!this->bSkeletalMesh)
	{
		return TArray<FSkeletalMaterial>();
	}

	USkeletalMesh* SkelMesh = Cast<USkeletalMesh>(this->GetAsset());

	return SkelMesh->GetMaterials();
}

UMaterialInterface* FCustomStandardSkeletalMeshData::GetSlotMaterialInterface(int32 SlotIndex)
{
	if (!bSkeletalMesh || SlotIndex >= GetMaterialSlots().Num())
	{
		return nullptr;
	}

	return GetMaterialSlots()[SlotIndex].MaterialInterface;
	
}

UMaterialInterface* FCustomStandardSkeletalMeshData::GetSlotMaterialInterface(FName SlotName)
{
	if (!bSkeletalMesh)
	{
		return nullptr;
	}

	for (FSkeletalMaterial mat : GetMaterialSlots())
	{
		if (mat.MaterialSlotName == SlotName)
		{
			return mat.MaterialInterface;
		}
	}

	return nullptr;
}

FName FCustomStandardSkeletalMeshData::GetSlotName(int32 SlotIndex)
{
	if (!bSkeletalMesh || SlotIndex >= GetMaterialSlots().Num())
	{
		return FName("");
	}

	return GetMaterialSlots()[SlotIndex].MaterialSlotName;
}

int32 FCustomStandardSkeletalMeshData::GetSlotIndex(FName SlotName)
{
	if (!bSkeletalMesh)
	{
		return -1;
	}

	TArray<FSkeletalMaterial> Slots = this->GetMaterialSlots();

	for (int idx = 0; idx < Slots.Num(); ++idx)
	{
		if (Slots[idx].MaterialSlotName == SlotName)
		{
			return idx;
		}
	}

	return -1;
}

void FCustomStandardSkeletalMeshData::GetEditorOnlyLODSections(
	int32 LODIndex,
	TArray<FSkelMeshSection>& SectionsEx)
{
	SectionsEx.Empty();

	if(!bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIndex))
	{
		return;
	}
	
#if WITH_EDITOR
	SectionsEx = GetSkeletalMesh()->GetImportedModel()->LODModels[LODIndex].Sections;
#endif
}

void FCustomStandardSkeletalMeshData::ResetLODSectionsMaterial()
{
	for (int32 idx = 0; idx < GetLODNum(); ++idx)
	{
		SetLODMaterialMap(idx, TArray<int32>());
	};
}

TArray<int32> FCustomStandardSkeletalMeshData::GetLODMaterialMap(int32 LODIndex)
{
	if (!bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIndex))
	{
		return TArray<int32>();
	}

	return TArray<int32>();
}

bool FCustomStandardSkeletalMeshData::SetLODMaterialMap(
	int32 LODIndex, 
	TArray<int32> LODMaterialMap)
{
	if (!bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(LODIndex))
	{
		return false;
	}

#if WITH_EDITOR
	TArray<FSkelMeshSection> SectionEx;
	GetEditorOnlyLODSections(LODIndex, SectionEx);
	if (LODMaterialMap.Num()<SectionEx.Num())
	{
		for (int32 idx = 0; idx < SectionEx.Num()-LODMaterialMap.Num();++idx)
		{
			LODMaterialMap.Add(-1);
		}
	}
#else
	if (LODMaterialMap.Num()<GetMaterialSlots().Num())
	{
		for (int32 idx = 0; idx < GetMaterialSlots().Num() - LODMaterialMap.Num(); ++idx)
		{
			LODMaterialMap.Add(-1);
		}
	}
#endif

	GetSkeletalMesh()->GetLODInfo(LODIndex)->LODMaterialMap = LODMaterialMap;

	return UEditorAssetLibrary::SaveAsset(this->GetObjectPathString());
}

bool FCustomStandardSkeletalMeshData::SetAllowCPUAccess(
	int32 SourceLODIndex, 
	bool CPUAccessState)
{
	if (!this->bSkeletalMesh || !GetSkeletalMesh()->IsValidLODIndex(SourceLODIndex))
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
