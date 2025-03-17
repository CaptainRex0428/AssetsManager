// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCustomStandardAsset.h"

struct CompressionSettingsInfo
{
	FString ConfigName;
	TextureCompressionSettings Setting;
	FString DisplayName;
};

CompressionSettingsInfo ConstructCompressionConfigPairs(
	FString ConfigName,
	TextureCompressionSettings Setting,
	FString DisplayName);

class ASSETSMANAGER_API UCustomStandardTexture2D : public UCustomStandardObject
{
public:
	UCustomStandardTexture2D(UObject* InObj, bool StricCheckMode = false);
	UCustomStandardTexture2D(UTexture2D* InTexture2D, bool StricCheckMode = false);
	virtual ~UCustomStandardTexture2D();

	TWeakObjectPtr<UTexture2D> Get();

	bool IsTexture2D();

	bool IsSuffixStandarized();

	TIndirectArray<struct FTexture2DMipMap>* GetTextureMipMaps();

	void ResizeSource(int targetSize, bool forceSave = false);
	void ResizeSourcePowerOf2(bool forceSave = false);
	void ConvertSourceTo8bit(bool NormalMapsKeep16bits, bool forceSave= false);
	void CompressWithJPEG(bool forceSave = false);

	FVector2D GetSourceSize();
	FVector2D GetMaxInGameSize();
	bool IsTextureMaxInGameOverSize();
	bool IsTextureSourceOverSize();
	bool SetMaxInGameSize(double maxSize, bool forceSave = false);
	bool FixMaxInGameSize(double size,bool forced, bool forceSave = false);

	double GetStandardMaxSize();
	double GetStandardMaxSizeStrict();

	TSharedPtr<TextureCompressionSettings> GetCompressionSettings();
	TSharedPtr<TextureCompressionSettings> GetStandardCompressionSettings(bool forced = false);
	TSharedPtr<CompressionSettingsInfo> GetCompressionSettingsInfo();
	bool SetCompressionSettings(const TEnumAsByte<TextureCompressionSettings>& CompressionSetting, bool forceSave = false);
	
	TSharedPtr<bool> GetsRGBSettings();
	TSharedPtr<bool> GetStandardsRGBSettings(bool forced = false);
	bool SetSRGBSettings(const bool& sRGB, bool forceSave = false);

	bool IsTextureSettingsStandarized();

	virtual int64 GetMemorySize(bool bEstimatedTotal = true);
	virtual int64 GetMemorySize(AssetsInfoDisplayLevel& DisplayLevel, bool bEstimatedTotal = true);

	TSharedPtr<TextureGroup> GetLODGroup();
	TSharedPtr<TextureGroup> GetStandardLODGroup(bool forced = false);
	bool SetLODGroup(TextureGroup InTextureGroup, bool forceSave = false);

	bool IsTextureLODGroupStandarized();

	static ETextureSourceFormat GetReducedTextureSourceFormat(const TextureCompressionSettings TC, const ETextureSourceFormat InTSF, const bool NormalMapsKeep16bits);

	FString GetTextureVaidSection();

protected:
	
	
	TSharedPtr<FString> TextureGlobalConfigSection;
	TSharedPtr<FString> TextureCategoryCommonConfigSection;
	TSharedPtr<FString> TextureCategoryStrictConfigSection;
};

/**
 * 
 */
class ASSETSMANAGER_API FCustomStandardTexture2DData: public FCustomStandardAssetData
{

public:
	FCustomStandardTexture2DData(
		const FAssetData& AssetData, 
		bool StrictCheckMode = false);
	virtual ~FCustomStandardTexture2DData();

	UCustomStandardTexture2D& Get();
	UTexture2D* GetTexture2D();

	

	

protected:

	UCustomStandardTexture2D StandardTexture2DObject;

};

static const TArray<CompressionSettingsInfo> ValidCompressionConfig =
{
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_Default),"Default (BC1 or BC3 with A)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_Normalmap),"NormalMap (BC5)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_Masks),"Masks (no sRGB)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_Grayscale),"Grayscale (G8/16, RGB8 sRGB)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_Displacementmap),"DisplacementMap (G8/16)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_VectorDisplacementmap),"VectorDisplacementmap (RGBA8)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_HDR),"HDR (RGBA16F, no sRGB)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_EditorIcon),"UserInterface2D (RGBA8)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_Alpha),"Alpha (no sRGB, BC4)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_DistanceFieldFont),"DistanceFieldFont (G8)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_HDR_Compressed),"HDR Compressed (RGB, BC6H)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_BC7),"BC7 (RGBA)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_HalfFloat),"Half Float (R16F)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_LQ),"Low Quality (BGR565/BGR555A1)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_EncodedReflectionCapture),"EncodedReflectionCapture"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_SingleFloat),"Single Float (R32F)"),
	ConstructCompressionConfigPairs(VNAME_STRUCT(TC_HDR_F32),"HDR High Precision (RGBA32F)")
};

static const TMap<FString, TextureGroup> TextureGroupNameMap =
{
	{VNAME_STRUCT(TEXTUREGROUP_World)},
	{VNAME_STRUCT(TEXTUREGROUP_WorldNormalMap)},
	{VNAME_STRUCT(TEXTUREGROUP_WorldSpecular)},
	{VNAME_STRUCT(TEXTUREGROUP_Character)},
	{VNAME_STRUCT(TEXTUREGROUP_CharacterNormalMap)},
	{VNAME_STRUCT(TEXTUREGROUP_CharacterSpecular)},
	{VNAME_STRUCT(TEXTUREGROUP_Weapon)},
	{VNAME_STRUCT(TEXTUREGROUP_WeaponNormalMap)},
	{VNAME_STRUCT(TEXTUREGROUP_WeaponSpecular)},
	{VNAME_STRUCT(TEXTUREGROUP_Vehicle)},
	{VNAME_STRUCT(TEXTUREGROUP_VehicleNormalMap)},
	{VNAME_STRUCT(TEXTUREGROUP_VehicleSpecular)},
	{VNAME_STRUCT(TEXTUREGROUP_Cinematic)},
	{VNAME_STRUCT(TEXTUREGROUP_Effects)},
	{VNAME_STRUCT(TEXTUREGROUP_EffectsNotFiltered)},
	{VNAME_STRUCT(TEXTUREGROUP_Skybox)},
	{VNAME_STRUCT(TEXTUREGROUP_UI)},
	{VNAME_STRUCT(TEXTUREGROUP_Lightmap)},
	{VNAME_STRUCT(TEXTUREGROUP_RenderTarget)},
	{VNAME_STRUCT(TEXTUREGROUP_MobileFlattened)},
	{VNAME_STRUCT(TEXTUREGROUP_ProcBuilding_Face)},
	{VNAME_STRUCT(TEXTUREGROUP_ProcBuilding_LightMap)},
	{VNAME_STRUCT(TEXTUREGROUP_Shadowmap)},
	{VNAME_STRUCT(TEXTUREGROUP_ColorLookupTable)},
	{VNAME_STRUCT(TEXTUREGROUP_Terrain_Heightmap)},
	{VNAME_STRUCT(TEXTUREGROUP_Terrain_Weightmap)},
	{VNAME_STRUCT(TEXTUREGROUP_Bokeh)},
	{VNAME_STRUCT(TEXTUREGROUP_IESLightProfile)},
	{VNAME_STRUCT(TEXTUREGROUP_Pixels2D)},
	{VNAME_STRUCT(TEXTUREGROUP_HierarchicalLOD)},
	{VNAME_STRUCT(TEXTUREGROUP_Impostor)},
	{VNAME_STRUCT(TEXTUREGROUP_ImpostorNormalDepth)},
	{VNAME_STRUCT(TEXTUREGROUP_8BitData)},
	{VNAME_STRUCT(TEXTUREGROUP_16BitData)},
	{VNAME_STRUCT(TEXTUREGROUP_Project01)},
	{VNAME_STRUCT(TEXTUREGROUP_Project02)},
	{VNAME_STRUCT(TEXTUREGROUP_Project03)},
	{VNAME_STRUCT(TEXTUREGROUP_Project04)},
	{VNAME_STRUCT(TEXTUREGROUP_Project05)},
	{VNAME_STRUCT(TEXTUREGROUP_Project06)},
	{VNAME_STRUCT(TEXTUREGROUP_Project07)},
	{VNAME_STRUCT(TEXTUREGROUP_Project08)},
	{VNAME_STRUCT(TEXTUREGROUP_Project09)},
	{VNAME_STRUCT(TEXTUREGROUP_Project10)},
	{VNAME_STRUCT(TEXTUREGROUP_Project11)},
	{VNAME_STRUCT(TEXTUREGROUP_Project12)},
	{VNAME_STRUCT(TEXTUREGROUP_Project13)},
	{VNAME_STRUCT(TEXTUREGROUP_Project14)},
	{VNAME_STRUCT(TEXTUREGROUP_Project15)},
	{VNAME_STRUCT(TEXTUREGROUP_Project16)},
	{VNAME_STRUCT(TEXTUREGROUP_Project17)},
	{VNAME_STRUCT(TEXTUREGROUP_Project18)},
	{VNAME_STRUCT(TEXTUREGROUP_Project19)},
	{VNAME_STRUCT(TEXTUREGROUP_Project20)},
	{VNAME_STRUCT(TEXTUREGROUP_Project21)},
	{VNAME_STRUCT(TEXTUREGROUP_Project22)},
	{VNAME_STRUCT(TEXTUREGROUP_Project23)},
	{VNAME_STRUCT(TEXTUREGROUP_Project24)},
	{VNAME_STRUCT(TEXTUREGROUP_Project25)},
	{VNAME_STRUCT(TEXTUREGROUP_Project26)},
	{VNAME_STRUCT(TEXTUREGROUP_Project27)},
	{VNAME_STRUCT(TEXTUREGROUP_Project28)},
	{VNAME_STRUCT(TEXTUREGROUP_Project29)},
	{VNAME_STRUCT(TEXTUREGROUP_Project30)},
	{VNAME_STRUCT(TEXTUREGROUP_Project31)},
	{VNAME_STRUCT(TEXTUREGROUP_Project32)},
	{VNAME_STRUCT(TEXTUREGROUP_MAX)}
};
