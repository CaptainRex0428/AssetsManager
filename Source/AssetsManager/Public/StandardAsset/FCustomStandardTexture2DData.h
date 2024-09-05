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

	double GetStandardMaxSize();
	double GetStandardMaxSizeStrict();

protected:
	const TMap<FString, TextureGroup> TextureGroupMap
	{
		{VNAME(TEXTUREGROUP_World),TEXTUREGROUP_World},
		{VNAME(TEXTUREGROUP_WorldNormalMap),TEXTUREGROUP_WorldNormalMap},
		{VNAME(TEXTUREGROUP_WorldSpecular),TEXTUREGROUP_WorldSpecular},
		{VNAME(TEXTUREGROUP_Character),TEXTUREGROUP_Character},
		{VNAME(TEXTUREGROUP_CharacterNormalMap),TEXTUREGROUP_CharacterNormalMap},
		{VNAME(TEXTUREGROUP_CharacterSpecular),TEXTUREGROUP_CharacterSpecular},
		{VNAME(TEXTUREGROUP_Weapon),TEXTUREGROUP_Weapon},
		{VNAME(TEXTUREGROUP_WeaponNormalMap),TEXTUREGROUP_WeaponNormalMap},
		{VNAME(TEXTUREGROUP_WeaponSpecular),TEXTUREGROUP_WeaponSpecular},
		{VNAME(TEXTUREGROUP_Vehicle),TEXTUREGROUP_Vehicle},
		{VNAME(TEXTUREGROUP_VehicleNormalMap),TEXTUREGROUP_VehicleNormalMap},
		{VNAME(TEXTUREGROUP_VehicleSpecular),TEXTUREGROUP_VehicleSpecular},
		{VNAME(TEXTUREGROUP_Cinematic),TEXTUREGROUP_Cinematic},
		{VNAME(TEXTUREGROUP_Effects),TEXTUREGROUP_Effects},
		{VNAME(TEXTUREGROUP_EffectsNotFiltered),TEXTUREGROUP_EffectsNotFiltered},
		{VNAME(TEXTUREGROUP_Skybox),TEXTUREGROUP_Skybox},
		{VNAME(TEXTUREGROUP_UI),TEXTUREGROUP_UI},
		{VNAME(TEXTUREGROUP_Lightmap),TEXTUREGROUP_Lightmap},
		{VNAME(TEXTUREGROUP_RenderTarget),TEXTUREGROUP_RenderTarget},
		{VNAME(TEXTUREGROUP_MobileFlattened),TEXTUREGROUP_MobileFlattened},
		{VNAME(TEXTUREGROUP_ProcBuilding_Face),TEXTUREGROUP_ProcBuilding_Face},
		{VNAME(TEXTUREGROUP_ProcBuilding_LightMap),TEXTUREGROUP_ProcBuilding_LightMap},
		{VNAME(TEXTUREGROUP_Shadowmap),TEXTUREGROUP_Shadowmap},
		{VNAME(TEXTUREGROUP_ColorLookupTable),TEXTUREGROUP_ColorLookupTable},
		{VNAME(TEXTUREGROUP_Terrain_Heightmap),TEXTUREGROUP_Terrain_Heightmap},
		{VNAME(TEXTUREGROUP_Terrain_Weightmap),TEXTUREGROUP_Terrain_Weightmap},
		{VNAME(TEXTUREGROUP_Bokeh),TEXTUREGROUP_Bokeh},
		{VNAME(TEXTUREGROUP_IESLightProfile),TEXTUREGROUP_IESLightProfile},
		{VNAME(TEXTUREGROUP_Pixels2D),TEXTUREGROUP_Pixels2D},
		{VNAME(TEXTUREGROUP_HierarchicalLOD),TEXTUREGROUP_HierarchicalLOD},
		{VNAME(TEXTUREGROUP_Impostor),TEXTUREGROUP_Impostor},
		{VNAME(TEXTUREGROUP_ImpostorNormalDepth),TEXTUREGROUP_ImpostorNormalDepth},
		{VNAME(TEXTUREGROUP_8BitData),TEXTUREGROUP_8BitData},
		{VNAME(TEXTUREGROUP_16BitData),TEXTUREGROUP_16BitData},
		{VNAME(TEXTUREGROUP_Project01),TEXTUREGROUP_Project01},
		{VNAME(TEXTUREGROUP_Project02),TEXTUREGROUP_Project02},
		{VNAME(TEXTUREGROUP_Project03),TEXTUREGROUP_Project03},
		{VNAME(TEXTUREGROUP_Project04),TEXTUREGROUP_Project04},
		{VNAME(TEXTUREGROUP_Project05),TEXTUREGROUP_Project05},
		{VNAME(TEXTUREGROUP_Project06),TEXTUREGROUP_Project06},
		{VNAME(TEXTUREGROUP_Project07),TEXTUREGROUP_Project07},
		{VNAME(TEXTUREGROUP_Project08),TEXTUREGROUP_Project08},
		{VNAME(TEXTUREGROUP_Project09),TEXTUREGROUP_Project09},
		{VNAME(TEXTUREGROUP_Project10),TEXTUREGROUP_Project10},
		{VNAME(TEXTUREGROUP_Project11),TEXTUREGROUP_Project11},
		{VNAME(TEXTUREGROUP_Project12),TEXTUREGROUP_Project12},
		{VNAME(TEXTUREGROUP_Project13),TEXTUREGROUP_Project13},
		{VNAME(TEXTUREGROUP_Project14),TEXTUREGROUP_Project14},
		{VNAME(TEXTUREGROUP_Project15),TEXTUREGROUP_Project15},
		{VNAME(TEXTUREGROUP_Project16),TEXTUREGROUP_Project16},
		{VNAME(TEXTUREGROUP_Project17),TEXTUREGROUP_Project17},
		{VNAME(TEXTUREGROUP_Project18),TEXTUREGROUP_Project18},
		{VNAME(TEXTUREGROUP_Project19),TEXTUREGROUP_Project19},
		{VNAME(TEXTUREGROUP_Project20),TEXTUREGROUP_Project20},
		{VNAME(TEXTUREGROUP_Project21),TEXTUREGROUP_Project21},
		{VNAME(TEXTUREGROUP_Project22),TEXTUREGROUP_Project22},
		{VNAME(TEXTUREGROUP_Project23),TEXTUREGROUP_Project23},
		{VNAME(TEXTUREGROUP_Project24),TEXTUREGROUP_Project24},
		{VNAME(TEXTUREGROUP_Project25),TEXTUREGROUP_Project25},
		{VNAME(TEXTUREGROUP_Project26),TEXTUREGROUP_Project26},
		{VNAME(TEXTUREGROUP_Project27),TEXTUREGROUP_Project27},
		{VNAME(TEXTUREGROUP_Project28),TEXTUREGROUP_Project28},
		{VNAME(TEXTUREGROUP_Project29),TEXTUREGROUP_Project29},
		{VNAME(TEXTUREGROUP_Project30),TEXTUREGROUP_Project30},
		{VNAME(TEXTUREGROUP_Project31),TEXTUREGROUP_Project31},
		{VNAME(TEXTUREGROUP_Project32),TEXTUREGROUP_Project32},
		{VNAME(TEXTUREGROUP_MAX),TEXTUREGROUP_MAX}
	};

protected:
	TSharedPtr<FString> TextureGlobalConfigSection;
	TSharedPtr<FString> TextureCategoryCommonConfigSection;
	TSharedPtr<FString> TextureCategoryStrictConfigSection;

	bool bTexture2D;
	
	double GlobalMaxSize;
	double MaxSize;
	double MaxInGameSizeX;
	double MaxInGameSizeY;
	double SourceSizeX;
	double SourceSizeY;


};
