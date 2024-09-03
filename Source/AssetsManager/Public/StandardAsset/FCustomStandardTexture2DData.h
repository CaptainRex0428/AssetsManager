// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCustomStandardAssetData.h"
/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardTexture2DData: public FCustomStandardAssetData
{
public:
	FCustomStandardTexture2DData(const FAssetData& AssetData);
	virtual ~FCustomStandardTexture2DData();

	bool isTexture2D();

	bool isTextureMaxInGameOverSize(bool bStrict = false);
	bool isTextureSourceOverSize(bool bStrict = false);

	FVector2D GetSourceSize();
	FVector2D GetMaxInGameSize();

	TSharedPtr<TextureCompressionSettings> GetCompressionSettings();

protected:
	bool bTexture2D;
	
	double GlobalMaxSize;
	double MaxSize;
	double MaxInGameSizeX;
	double MaxInGameSizeY;
	double SourceSizeX;
	double SourceSizeY;
};
