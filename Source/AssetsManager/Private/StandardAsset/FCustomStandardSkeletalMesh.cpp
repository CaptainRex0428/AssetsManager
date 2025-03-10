// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardSkeletalMesh.h"
#include "LODUtilities.h"
#include "ManagerLogger.h"
#include "AssetsChecker/AssetsChecker.h"


FCustomStandardSkeletalMeshData::FCustomStandardSkeletalMeshData(FAssetData& AssetData, bool StrictMode)
	:FCustomStandardAssetData(AssetData, StrictMode),
	StandardSkeletalMeshObject(AssetData.GetAsset(),StrictMode)
{
}

FCustomStandardSkeletalMeshData::~FCustomStandardSkeletalMeshData()
{
}

UCustomStandardSkeletalMeshObject& FCustomStandardSkeletalMeshData::Get()
{
	return StandardSkeletalMeshObject;
}

USkeletalMesh* FCustomStandardSkeletalMeshData::GetSkeletalMesh()
{
	if (!this->Get().IsSkeletalMesh()) 
	{
		return nullptr;
	}

	USkeletalMesh* SKAsset = Cast<USkeletalMesh>(this->GetAsset());

	return SKAsset;
}

UCustomStandardSkeletalMeshObject::UCustomStandardSkeletalMeshObject(
	UObject* InObj, 
	bool StricCheckMode)
	:UCustomStandardObject(InObj,StricCheckMode)
{
}

UCustomStandardSkeletalMeshObject::UCustomStandardSkeletalMeshObject(
	USkeletalMesh* InSkeletalMesh, 
	bool StricCheckMode)
	:UCustomStandardObject(InSkeletalMesh, StricCheckMode)
{
}

UCustomStandardSkeletalMeshObject::~UCustomStandardSkeletalMeshObject()
{
}

TWeakObjectPtr<USkeletalMesh> UCustomStandardSkeletalMeshObject::Get()
{
	return Cast<USkeletalMesh>(UCustomStandardObject::Get());
}

bool UCustomStandardSkeletalMeshObject::IsSkeletalMesh()
{
	return this->Get().IsValid();
}

bool UCustomStandardSkeletalMeshObject::HasLODMeshDescription(int32 LODIdx)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIdx))
	{
		return false;
	}

	return this->Get()->HasMeshDescription(LODIdx);
}

int32 UCustomStandardSkeletalMeshObject::GetLODNum()
{
	if (!this->IsSkeletalMesh())
	{
		return 0;
	}
	
	return this->Get()->GetLODNum();
}

int32 UCustomStandardSkeletalMeshObject::GetLODTrianglesNum(int32 LODIndex)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return 0;
	}


	const FSkeletalMeshLODModel& SKMesh
		= this->Get()->GetImportedModel()->LODModels[LODIndex];

	int LODTriNum = 0;

	for (int32 SectionIndex = 0; SectionIndex < SKMesh.Sections.Num() ; ++SectionIndex)
	{
		const FSkelMeshSection& Section = SKMesh.Sections[SectionIndex];

		LODTriNum += Section.NumTriangles;
	}

	return LODTriNum;
}

int32 UCustomStandardSkeletalMeshObject::GetLODTrianglesNum(int32 LODIndex, AssetsInfoDisplayLevel& DisplayLevel, bool bStrictWithCategory)
{
	int32 TNum = this->GetLODTrianglesNum(LODIndex);

	if (!bStrictWithCategory)
	{
		FString SKAssetGlobalSection = "/AssetsManager/Global/Mesh";

		TArray<FString> ValidLevels = UManagerConfig::Get().GenerateStructKeyValueArray(
			*SKAssetGlobalSection,
			L"AssetTrianglesNumLevelDivide",
			L"Level");

		int32 LevelOut = 0;

		for (int32 levelIdx = 0; levelIdx < ValidLevels.Num(); ++levelIdx)
		{
			TSharedPtr<FString> LevelValue = UManagerConfig::Get().FindInSectionStructArray(
				*SKAssetGlobalSection,
				L"AssetTrianglesNumLevelDivide",
				L"Level",
				FString::FromInt(levelIdx),
				L"Value");

			if (!LevelValue.IsValid())
			{
				continue;
			}

			int BorderSize = UManagerConfig::Get().SToI(*LevelValue);

			LevelOut = levelIdx;

			if (TNum < BorderSize)
			{
				break;
			}
		}

		DisplayLevel = UAssetsChecker::IntToDisplayLevel(LevelOut);

		return TNum;
	}

	FString SKAssetGlobalSection = "/AssetsManager/Global/Mesh";

	TArray<FString> ValidLevels = UManagerConfig::Get().GenerateStructKeyValueArray(
		*SKAssetGlobalSection,
		L"AssetTrianglesNumLevelDivide",
		L"Level");

	int32 LevelOut = 0;

	for (int32 levelIdx = 0; levelIdx < ValidLevels.Num(); ++levelIdx)
	{
		TSharedPtr<FString> LevelValue = UManagerConfig::Get().FindInSectionStructArray(
			*SKAssetGlobalSection,
			L"AssetTrianglesNumLevelDivide",
			L"Level",
			FString::FromInt(levelIdx),
			L"Value");

		if (!LevelValue.IsValid())
		{
			continue;
		}

		int BorderSize = UManagerConfig::Get().SToI(*LevelValue);

		LevelOut = levelIdx;

		if (TNum < BorderSize)
		{
			break;
		}
	}

	DisplayLevel = UAssetsChecker::IntToDisplayLevel(LevelOut);

	return TNum;
}

int32 UCustomStandardSkeletalMeshObject::GetLODVerticesNum(int32 LODIndex)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return 0;
	}

	const FSkeletalMeshLODModel& SKMesh
		= this->Get()->GetImportedModel()->LODModels[LODIndex];

	int LODVertexNum = 0;

	for (int32 SectionIndex = 0; SectionIndex < SKMesh.Sections.Num(); ++SectionIndex)
	{
		const FSkelMeshSection& Section = SKMesh.Sections[SectionIndex];

		LODVertexNum += Section.NumVertices < 0 ? 0 : Section.NumVertices;
	}

	return LODVertexNum;
}

int32 UCustomStandardSkeletalMeshObject::GetLODVerticesNum(int32 LODIndex, AssetsInfoDisplayLevel& DisplayLevel, bool bStrictWithCategory)
{
	int32 VNum = this->GetLODVerticesNum(LODIndex);

	if (!bStrictWithCategory)
	{
		FString SKAssetGlobalSection = "/AssetsManager/Global/Mesh";

		TArray<FString> ValidLevels = UManagerConfig::Get().GenerateStructKeyValueArray(
			*SKAssetGlobalSection,
			L"AssetVerticesNumLevelDivide",
			L"Level");

		int32 LevelOut = 0;

		for (int32 levelIdx = 0; levelIdx < ValidLevels.Num(); ++levelIdx)
		{
			TSharedPtr<FString> LevelValue = UManagerConfig::Get().FindInSectionStructArray(
				*SKAssetGlobalSection,
				L"AssetVerticesNumLevelDivide",
				L"Level",
				FString::FromInt(levelIdx),
				L"Value");

			if (!LevelValue.IsValid())
			{
				continue;
			}

			int BorderSize = UManagerConfig::Get().SToI(*LevelValue);

			LevelOut = levelIdx;

			if (VNum < BorderSize)
			{
				break;
			}
		}

		DisplayLevel = UAssetsChecker::IntToDisplayLevel(LevelOut);

		return VNum;
	}

	FString SKAssetGlobalSection = "/AssetsManager/Global/Mesh";

	TArray<FString> ValidLevels = UManagerConfig::Get().GenerateStructKeyValueArray(
		*SKAssetGlobalSection,
		L"AssetVerticesNumLevelDivide",
		L"Level");

	int32 LevelOut = 0;

	for (int32 levelIdx = 0; levelIdx < ValidLevels.Num(); ++levelIdx)
	{
		TSharedPtr<FString> LevelValue = UManagerConfig::Get().FindInSectionStructArray(
			*SKAssetGlobalSection,
			L"AssetVerticesNumLevelDivide",
			L"Level",
			FString::FromInt(levelIdx),
			L"Value");

		if (!LevelValue.IsValid())
		{
			continue;
		}

		int BorderSize = UManagerConfig::Get().SToI(*LevelValue);

		LevelOut = levelIdx;

		if (VNum < BorderSize)
		{
			break;
		}
	}

	DisplayLevel = UAssetsChecker::IntToDisplayLevel(LevelOut);

	return VNum;
}

bool UCustomStandardSkeletalMeshObject::GetAllowCPUAccess(int32 LODIndex)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return false;
	}

	return this->Get()->GetLODInfo(LODIndex)->bAllowCPUAccess;
}

bool UCustomStandardSkeletalMeshObject::SetAllowCPUAccess(int32 LODIndex, bool CPUAccessState)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return false;
	}

	if (this->Get()->IsValidLODIndex(LODIndex))
	{
		this->Get()->MarkPackageDirty();

		if (GetAllowCPUAccess(LODIndex) != CPUAccessState)
		{
			this->Get()->Modify();

			this->Get()->GetLODInfo(LODIndex)->bAllowCPUAccess = CPUAccessState;

			FLODUtilities::RegenerateLOD(
				this->Get().Get(),
				GetTargetPlatformManagerRef().GetRunningTargetPlatform(),
				this->Get()->GetLODNum()
			);
		};
	}

	return UAssetsChecker::SaveAsset(this->Get().Get());
}

void UCustomStandardSkeletalMeshObject::SetLODsAllowCPUAccess(bool CPUAccess)
{
	if (!this->IsSkeletalMesh())
	{
		return;
	}

	for (int idx = 0; idx < this->Get()->GetLODNum(); ++idx)
	{
		if (GetAllowCPUAccess(idx) != CPUAccess)
		{
			this->Get()->Modify();

			this->Get()->GetLODInfo(idx)->bAllowCPUAccess = CPUAccess;
		}
	}

	FLODUtilities::RegenerateLOD(
		this->Get().Get(),
		GetTargetPlatformManagerRef().GetRunningTargetPlatform(),
		this->Get()->GetLODNum()
	);

	UAssetsChecker::SaveAsset(this->Get().Get());

	return;
}

TArray<FSkeletalMaterial> UCustomStandardSkeletalMeshObject::GetMaterialSlots()
{
	if (!this->IsSkeletalMesh())
	{
		return TArray<FSkeletalMaterial>();
	}

	return this->Get()->GetMaterials();
}

UMaterialInterface* UCustomStandardSkeletalMeshObject::GetSlotMaterialInterface(int32 SlotIndex)
{
	if (!this->IsSkeletalMesh() || SlotIndex >= this->GetMaterialSlots().Num())
	{
		return nullptr;
	}

	return this->GetMaterialSlots()[SlotIndex].MaterialInterface;
}

UMaterialInterface* UCustomStandardSkeletalMeshObject::GetSlotMaterialInterface(FName SlotName)
{
	if (!this->IsSkeletalMesh())
	{
		return nullptr;
	}

	for (FSkeletalMaterial mat : this->GetMaterialSlots())
	{
		if (mat.MaterialSlotName == SlotName)
		{
			return mat.MaterialInterface;
		}
	}

	return nullptr;
}

FName UCustomStandardSkeletalMeshObject::GetSlotName(int32 SlotIndex)
{
	if (!this->IsSkeletalMesh() || SlotIndex >= this->GetMaterialSlots().Num())
	{
		return FName("");
	}

	return this->GetMaterialSlots()[SlotIndex].MaterialSlotName;

}

int32 UCustomStandardSkeletalMeshObject::GetSlotIndex(FName SlotName)
{
	if (!this->IsSkeletalMesh())
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

TArray<int32> UCustomStandardSkeletalMeshObject::GetLODMaterialMap(int32 LODIndex)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return TArray<int32>();
	}

	return TArray<int32>();
}

void UCustomStandardSkeletalMeshObject::GetEditorOnlyLODSections(int32 LODIndex, TArray<FSkelMeshSection>& SectionsEx)
{
#if WITH_EDITOR
	SectionsEx.Empty();

	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return;
	}

	SectionsEx = this->Get()->GetImportedModel()->LODModels[LODIndex].Sections;
#endif
}

void UCustomStandardSkeletalMeshObject::ResetLODSectionsMaterial()
{
	for (int32 idx = 0; idx < this->GetLODNum(); ++idx)
	{
		this->SetLODMaterialMap(idx, TArray<int32>());
	};
}

bool UCustomStandardSkeletalMeshObject::SetLODMaterialMap(int32 LODIndex, TArray<int32> LODMaterialMap)
{
	if (!this->IsSkeletalMesh() || !this->Get()->IsValidLODIndex(LODIndex))
	{
		return false;
	}

#if WITH_EDITOR
	TArray<FSkelMeshSection> SectionEx;
	this->GetEditorOnlyLODSections(LODIndex, SectionEx);
	if (LODMaterialMap.Num() < SectionEx.Num())
	{
		for (int32 idx = 0; idx < SectionEx.Num() - LODMaterialMap.Num(); ++idx)
		{
			LODMaterialMap.Add(-1);
		}
	}
#else
	if (LODMaterialMap.Num() < GetMaterialSlots().Num())
	{
		for (int32 idx = 0; idx < GetMaterialSlots().Num() - LODMaterialMap.Num(); ++idx)
		{
			LODMaterialMap.Add(-1);
		}
	}
#endif

	this->Get()->GetLODInfo(LODIndex)->LODMaterialMap = LODMaterialMap;

	return UAssetsChecker::SaveAsset(this->Get().Get());
}
