// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "StandardAsset/FCustomStandardAssetData.h"

/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardSkeletalMeshData : public FCustomStandardAssetData
{
public:
	FCustomStandardSkeletalMeshData(FAssetData & AssetData);
	~FCustomStandardSkeletalMeshData();

	bool IsSkeletalMesh() const;

	bool HasLODMeshDescription(int32 LODIdx);


	int32 GetLODNum();
	int32 GetLODTrianglesNum(int32 Index);
	int32 GetLODVertexNum(int32 Index);

	bool GetAllowCPUAccess(int32 LODIndex = 0);
	bool SetAllowCPUAccess(int32 LODIndex = 0,bool CPUAccessState=true);
	void SetLODsAllowCPUAccess(bool CPUAccess = true);

private:
	bool bSkeletalMesh;
};
