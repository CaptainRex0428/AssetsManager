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

	int32 GetLODNum();
	int32 GetLODTrianglesNum(int32 Index);
	int32 GetLODVertexNum(int32 Index);

private:
	bool bSkeletalMesh;
};
