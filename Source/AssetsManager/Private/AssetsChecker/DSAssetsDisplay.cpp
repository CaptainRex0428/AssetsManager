// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsChecker/DSAssetsDisplay.h"

ADSAssetsDisplay::ADSAssetsDisplay()
{
	Body = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("Body");
	Body->SetVisibility(true);
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Face = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("Face");
	Face->SetVisibility(true);
	Face->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Face->SetupAttachment(Body);
	Face->bUseAttachParentBound = true;

	ParasiteB = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("ParasiteB");
	ParasiteB->SetVisibility(true);
	ParasiteB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParasiteB->SetupAttachment(Body);
	ParasiteB->SetLeaderPoseComponent(Body);
	ParasiteB->bUseAttachParentBound = true;

	ParasiteA = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("ParasiteA");
	ParasiteA->SetVisibility(true);
	ParasiteA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParasiteA->SetupAttachment(Body);
	ParasiteA->SetLeaderPoseComponent(Body);
	ParasiteA->bUseAttachParentBound = true;

	WeaponR = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("WeaponR");
	WeaponR->SetVisibility(true);
	WeaponR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponR->bUseAttachParentBound = true;

	WeaponL = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("WeaponL");
	WeaponL->SetVisibility(true);
	WeaponL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponL->bUseAttachParentBound = true;

	Groom_C = CreateOptionalDefaultSubobject<UGroomComponent>(TEXT("GroomC"));
	if (Groom_C)
	{
		Groom_C->SetVisibility(true);
		Groom_C->AlwaysLoadOnServer = false;
		Groom_C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Groom_C->SetupAttachment(Face);
		Groom_C->PostLoad();
	}

	Groom_B = CreateOptionalDefaultSubobject<UGroomComponent>(TEXT("GroomB"));
	if (Groom_B)
	{
		Groom_B->SetVisibility(true);
		Groom_B->AlwaysLoadOnServer = false;
		Groom_B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Groom_B->SetupAttachment(Face);
		Groom_B->PostLoad();
	}

	Groom_A = CreateOptionalDefaultSubobject<UGroomComponent>(TEXT("GroomA"));
	if (Groom_A)
	{
		Groom_A->SetVisibility(true);
		Groom_A->AlwaysLoadOnServer = false;
		Groom_A->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Groom_A->SetupAttachment(Face);
		Groom_A->PostLoad();
	}
}

ADSAssetsDisplay::~ADSAssetsDisplay()
{
}
