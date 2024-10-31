// Fill out your copyright notice in the Description page of Project Settings.


#include "DSAssets/DSAssetsDisplay.h"

ADSAssetsDisplay::ADSAssetsDisplay()
{
	Body = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("Body");
	Body->SetVisibility(true);
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Body->SetRenderCustomDepth(true);
	Body->SetCastInsetShadow(true);
	Body->SetCastCapsuleIndirectShadow(true);

	Face = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("Face");
	Face->SetVisibility(true);
	Face->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Face->SetupAttachment(Body);
	Face->bUseAttachParentBound = true;
	Face->SetRenderCustomDepth(true);
	Face->SetCastInsetShadow(true);
	Face->SetLeaderPoseComponent(Body);

	ParasiteB = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("ParasiteB");
	ParasiteB->SetVisibility(true);
	ParasiteB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParasiteB->SetupAttachment(Body);
	ParasiteB->SetLeaderPoseComponent(Body);
	ParasiteB->bUseAttachParentBound = true;
	ParasiteB->SetRenderCustomDepth(true);
	ParasiteB->SetCastInsetShadow(true);
	ParasiteB->SetLeaderPoseComponent(Body);

	ParasiteA = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("ParasiteA");
	ParasiteA->SetVisibility(true);
	ParasiteA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParasiteA->SetupAttachment(Body);
	ParasiteA->SetLeaderPoseComponent(Body);
	ParasiteA->bUseAttachParentBound = true;
	ParasiteA->SetRenderCustomDepth(true);
	ParasiteA->SetCastInsetShadow(true);
	ParasiteA->SetLeaderPoseComponent(Body);

	WeaponR = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("WeaponR");
	WeaponR->SetVisibility(true);
	WeaponR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponR->bUseAttachParentBound = true;
	WeaponR->SetRenderCustomDepth(true);
	WeaponR->SetCastInsetShadow(true); 
	WeaponR->SetupAttachment(Body,"socket_weapon_r");

	WeaponL = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("WeaponL");
	WeaponL->SetVisibility(true);
	WeaponL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponL->bUseAttachParentBound = true;
	WeaponL->SetRenderCustomDepth(true);
	WeaponL->SetCastInsetShadow(true);
	WeaponL->SetupAttachment(Body, "socket_weapon_l");

	Groom_C = CreateOptionalDefaultSubobject<UGroomComponent>(TEXT("GroomC"));
	if (Groom_C)
	{
		Groom_C->SetVisibility(true);
		Groom_C->AlwaysLoadOnServer = false;
		Groom_C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Groom_C->SetupAttachment(Face);
		Groom_C->PostLoad();
		Groom_C->SetRenderCustomDepth(true);
		Groom_C->SetCastInsetShadow(true);
		Groom_C->SimulationSettings.SimulationSetup.LocalBone = "UnrealRoot";
	}

	Groom_B = CreateOptionalDefaultSubobject<UGroomComponent>(TEXT("GroomB"));
	if (Groom_B)
	{
		Groom_B->SetVisibility(true);
		Groom_B->AlwaysLoadOnServer = false;
		Groom_B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Groom_B->SetupAttachment(Face);
		Groom_B->PostLoad();
		Groom_B->SetRenderCustomDepth(true);
		Groom_B->SetCastInsetShadow(true);
		Groom_B->SimulationSettings.SimulationSetup.LocalBone = "UnrealRoot";
	}

	Groom_A = CreateOptionalDefaultSubobject<UGroomComponent>(TEXT("GroomA"));
	if (Groom_A)
	{
		Groom_A->SetVisibility(true);
		Groom_A->AlwaysLoadOnServer = false;
		Groom_A->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Groom_A->SetupAttachment(Face);
		Groom_A->PostLoad();
		Groom_A->SetRenderCustomDepth(true);
		Groom_A->SetCastInsetShadow(true);
		Groom_A->SimulationSettings.SimulationSetup.LocalBone = "UnrealRoot";
	}

	this->SetComponentsMinLOD(1);
}

ADSAssetsDisplay::~ADSAssetsDisplay()
{
}
