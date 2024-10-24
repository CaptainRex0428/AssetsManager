// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsChecker/AssetsDisplay.h"
#include "ManagerLogger.h"


// Sets default values
AAssetsDisplay::AAssetsDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAssetsDisplay::SetComponentsMinLOD(int32 MinLOD)
{

	for (UActorComponent * Comp:this->GetComponents())
	{
		if (Comp->IsA<USkeletalMeshComponent>())
		{
			USkeletalMeshComponent * SKComp = Cast<USkeletalMeshComponent>(Comp);
			SKComp->OverrideMinLOD(MinLOD);

			if (MinLOD == 0)
			{
				SKComp->bOverrideMinLod = false;
			}
		}

		if (Comp->IsA<UStaticMeshComponent>())
		{
			UStaticMeshComponent * SMComp = Cast<UStaticMeshComponent>(Comp);
			SMComp->bOverrideMinLOD = (MinLOD == 0 ? false: true);
			SMComp->MinLOD = MinLOD;
		}
	}
}

void AAssetsDisplay::SetComponentsForcedLOD(int32 ForcedLOD)
{

	for (UActorComponent* Comp : this->GetComponents())
	{
		if (Comp->IsA<USkeletalMeshComponent>())
		{
			USkeletalMeshComponent* SKComp = Cast<USkeletalMeshComponent>(Comp);
			SKComp->SetForcedLOD(ForcedLOD);
		}

		if (Comp->IsA<UStaticMeshComponent>())
		{
			UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(Comp);
			SMComp->SetForcedLodModel(ForcedLOD);
		}
	}
}

void AAssetsDisplay::CheckComponentsLOD(LODCheckType CheckType, int32 LOD)
{
	if(CheckType == LODCheckType::MinLOD)
	{
		SetComponentsForcedLOD(0);
		SetComponentsMinLOD(LOD);
	}

	if (CheckType == LODCheckType::ForcedLOD)
	{
		SetComponentsMinLOD(0);
		SetComponentsForcedLOD(LOD);
	}
}

void AAssetsDisplay::ResetComponentsLOD()
{
	SetComponentsMinLOD(0);
	SetComponentsForcedLOD(0);
}

void AAssetsDisplay::SetComponentsAttachParentLOD(bool bAttach)
{
	for (UActorComponent* Comp : this->GetComponents())
	{
		if (Comp->IsA<USkeletalMeshComponent>())
		{
			USkeletalMeshComponent* SKComp = Cast<USkeletalMeshComponent>(Comp);
			SKComp->bSyncAttachParentLOD = bAttach;
		}
	}
}
