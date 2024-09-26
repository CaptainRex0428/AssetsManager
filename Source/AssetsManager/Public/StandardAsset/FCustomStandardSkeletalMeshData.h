// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "StandardAsset/FCustomStandardAssetData.h"

#include "LODUtilities.h"

#include "Engine/RendererSettings.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/SkinnedAssetAsyncCompileUtils.h"
#include "Engine/SkeletalMeshLODSettings.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"


#include "Factories/FbxSkeletalMeshImportData.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardSkeletalMeshData : public FCustomStandardAssetData
{
public:
	FCustomStandardSkeletalMeshData(FAssetData & AssetData);
	~FCustomStandardSkeletalMeshData();

	bool IsSkeletalMesh() const;

	USkeletalMesh* GetSkeletalMesh();

	bool HasLODMeshDescription(int32 LODIdx);

	int32 GetLODNum();
	int32 GetLODTrianglesNum(int32 LODIndex);
	int32 GetLODVertexNum(int32 LODIndex);

	bool GetAllowCPUAccess(int32 LODIndex = 0);
	bool SetAllowCPUAccess(int32 LODIndex = 0,bool CPUAccessState=true);
	void SetLODsAllowCPUAccess(bool CPUAccess = true);

	TArray<FSkeletalMaterial> GetMaterialSlots();
	UMaterialInterface* GetSlotMaterialInterface(int32 SlotIndex);
	UMaterialInterface* GetSlotMaterialInterface(FName SlotName);

	FName GetSlotName(int32 SlotIndex);
	int32 GetSlotIndex(FName SlotName);

	TArray<int32> GetLODMaterialMap(int32 LODIndex);
	bool SetLODMaterialMap(int32 LODIndex, TArray<int32> LODMaterialMap);

	void GetEditorOnlyLODSections(int32 LODIndex, TArray<FSkelMeshSection>& SectionsEx);

	void ResetLODSectionsMaterial();

private:
	bool bSkeletalMesh;
};
