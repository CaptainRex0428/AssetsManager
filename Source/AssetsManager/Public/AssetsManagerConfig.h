#pragma once

#include "Materials/Material.h"
#include "Materials/MaterialinstanceConstant.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Engine/Texture.h"
#include "Engine/Texture2DArray.h"
#include "Blueprint/Userwidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "PhysicsEngine/PhysicsAsset.h"

#include "StandardAsset/FCustomStandardAssetData.h"

#define ModuleLanguage_ZH 1

#if ModuleLanguage_ZH
#define ZH_CN
#else
#define EN
#endif

#define ASSETSMANAGER_LOGFOLDER FPaths::ProjectDir() + "/Saved/AssetsManager/"

const TMap<UClass*, FString> PrefixMap =
{
	{UBlueprint::StaticClass(),TEXT("BP")},
	{UStaticMesh::StaticClass(),TEXT("SM")},
	{UMaterial::StaticClass(),TEXT("M") },
	{UMaterialInstanceConstant::StaticClass(),TEXT("MI")},
	{UMaterialFunctionInterface::StaticClass(), TEXT("MF")},
	{UParticleSystem::StaticClass(),TEXT("PS")},
	{USoundCue::StaticClass(),TEXT("SC")},
	{USoundWave::StaticClass(), TEXT("SW")},
	{UTexture2D::StaticClass(),TEXT("T")},
	{UTexture2DArray::StaticClass(),TEXT("TArray")},
	{UUserWidget::StaticClass(), TEXT("WBP")},
	{USkeletalMesh::StaticClass(), TEXT("SK")},
	{UNiagaraSystem::StaticClass(),TEXT("NS")},
	{UNiagaraEmitter::StaticClass(), TEXT("NE")},
	{UAnimSequence::StaticClass(), TEXT("A")},
	{UAnimMontage::StaticClass(), TEXT("AM")},
	{UPhysicsAsset::StaticClass(), TEXT("PHYS")}

};

const TMap<FString, UClass*> AssetFullNameMap =
{
	{TEXT("Blueprint"),UBlueprint::StaticClass()},
	{TEXT("StaticMesh"),UStaticMesh::StaticClass()},
	{TEXT("Material"),UMaterial::StaticClass()},
	{TEXT("MaterialInstance"),UMaterialInstanceConstant::StaticClass()},
	{TEXT("MaterialFunction"),UMaterialFunctionInterface::StaticClass()},
	{TEXT("ParticleSystem"),UParticleSystem::StaticClass()},
	{TEXT("SoundCue"),USoundCue::StaticClass()},
	{TEXT("SoundWave"),USoundWave::StaticClass()},
	{TEXT("Texture"),UTexture2D::StaticClass()},
	{TEXT("TextureArray"),UTexture2DArray::StaticClass()},
	{TEXT("UserWidget"),UUserWidget::StaticClass()},
	{TEXT("SkeletalMesh"),USkeletalMesh::StaticClass()},
	{TEXT("NiagaraSystem"),UNiagaraSystem::StaticClass()},
	{TEXT("NiagaraEmitter"),UNiagaraEmitter::StaticClass()},
	{TEXT("Animation"),UAnimSequence::StaticClass()},
	{TEXT("AnimationMontage"),UAnimMontage::StaticClass()},
	{TEXT("PhysicsAsset"),UPhysicsAsset::StaticClass()}

};

const TMap<TextureCompressionSettings, FString> TextureCompressionMap =
{
	{TC_Default ,"Default (BC1 or BC3 with A)"},
	{TC_Normalmap,"NormalMap (BC5)"},
	{TC_Masks,"Masks (no sRGB)"},
	{TC_Grayscale,"Grayscale (G8/16, RGB8 sRGB)"},
	{TC_Displacementmap,"DisplacementMap (G8/16)"},
	{TC_VectorDisplacementmap,"VectorDisplacementmap (RGBA8)"},
	{TC_HDR,"HDR (RGBA16F, no sRGB)"},
	{TC_EditorIcon,"UserInterface2D (RGBA8)"},
	{TC_Alpha,"Alpha (no sRGB, BC4)"},
	{TC_DistanceFieldFont,"DistanceFieldFont (G8)"},
	{TC_HDR_Compressed,"HDR Compressed (RGB, BC6H)"},
	{TC_BC7,"BC7 (RGBA)"},
	{TC_HalfFloat,"Half Float (R16F)"},
	{TC_LQ,"Low Quality (BGR565/BGR555A1)"},
	{TC_EncodedReflectionCapture,"EncodedReflectionCapture"},
	{TC_SingleFloat,"Single Float (R32F)"},
	{TC_HDR_F32,"HDR High Precision (RGBA32F)"},
	{TC_MAX,"MAX"}
};

const TMap<FString, TextureCompressionSettings>
TextureSubfixCompressionSettingsMap =
{
	{"D",TC_Default},
	{"E",TC_Default},
	{"ARMS",TC_Masks},
	{"Mask",TC_Masks},
	{"N",TC_Normalmap},
	{"Attr",TC_VectorDisplacementmap},
	{"Detail",TC_VectorDisplacementmap},
	{"AO",TC_Grayscale}
};

const TMap<FString, bool> TextureSubfixSRGBSettingsMap =
{
	{"D",true},
	{"E",true},
	{"ARMS",false},
	{"Mask",false},
	{"N",false},
	{"Attr",false},
	{"Detail",false},
	{"AO",false}
};

const TMap<FString, TextureGroup> TextureLODGroupForCharacterBySubfix =
{
	{"D",TEXTUREGROUP_Character},
	{"E",TEXTUREGROUP_Project02},
	{"ARMS",TEXTUREGROUP_CharacterSpecular},
	{"Mask",TEXTUREGROUP_CharacterSpecular},
	{"N",TEXTUREGROUP_CharacterNormalMap},
	{"Attr",TEXTUREGROUP_Project01},
	{"Detail",TEXTUREGROUP_Project01},
	{"AO",TEXTUREGROUP_Project03}
};

const TMap<FString, float> TextureSubfixMaxSizeSettingsMap =
{
	{"D",2048.f},
	{"E",2048.f},
	{"ARMS",512.f},
	{"Mask",512.f},
	{"N",1024.f},
	{"Attr",512.f},
	{"Detail",512.f},
	{"AO",512.f}
};

const TMap<FString, FCustomStandardAssetData::Category> AssetCategoryMap =
{
	// Character
	{"Character", FCustomStandardAssetData::Character},
	{"Player", FCustomStandardAssetData::Character},
	{"Weapon", FCustomStandardAssetData::Character},
	{"WeaponL", FCustomStandardAssetData::Character},
	{"WeaponR", FCustomStandardAssetData::Character},
	{"WeaponBullet", FCustomStandardAssetData::Character},
	{"Parasite", FCustomStandardAssetData::Character},
	{"ParasiteA", FCustomStandardAssetData::Character},
	{"ParasiteB", FCustomStandardAssetData::Character},
	{"Face", FCustomStandardAssetData::Character},

	// Hair
	{"Hair", FCustomStandardAssetData::Hair},
	{"Groom", FCustomStandardAssetData::Hair},
	
	//Effect
	{"Effect", FCustomStandardAssetData::Effect},
	{"VFX", FCustomStandardAssetData::Effect},

	//Scene
	{"Scene", FCustomStandardAssetData::Scene},
	{"Prop", FCustomStandardAssetData::Scene},
	{"Level", FCustomStandardAssetData::Scene},
	

	//
	{"UI", FCustomStandardAssetData::UI},
	{"Hero", FCustomStandardAssetData::UI}
};

const TMap<FCustomStandardAssetData::Category, TextureGroup> TextureLODGroupForCategroyDefault =
{
	{FCustomStandardAssetData::Undefined, TEXTUREGROUP_World},
	{FCustomStandardAssetData::Character, TEXTUREGROUP_Character},
	{FCustomStandardAssetData::Hair, TEXTUREGROUP_Project03},
	{FCustomStandardAssetData::Effect, TEXTUREGROUP_Effects},
	{FCustomStandardAssetData::Scene, TEXTUREGROUP_World},
	{FCustomStandardAssetData::UI, TEXTUREGROUP_UI}

};