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

void AAssetsDisplay::CheckComponentsLOD(LODCheckType CheckType, int32 LOD, int32 GroomGroupIndex, int32 GroomRelative)
{
	if(CheckType == LODCheckType::MinLOD)
	{
		SetComponentsForcedLOD(0);
		SetComponentsMinLOD(LOD);

		// SetGroomComponentsForcedLOD(0);
		SetGroomComponentsLODBias(((LOD + GroomRelative > 0) ? LOD + GroomRelative : 0), GroomGroupIndex);
	}

	if (CheckType == LODCheckType::ForcedLOD)
	{
		SetComponentsMinLOD(0);
		SetComponentsForcedLOD(LOD);

		SetGroomComponentsLODBias(((LOD+GroomRelative > 0) ? LOD + GroomRelative : 0), GroomGroupIndex);
	}
}

void AAssetsDisplay::ResetComponentsLOD(int32 GroomGroupIndex)
{
	SetComponentsMinLOD(0);
	SetComponentsForcedLOD(0);

	SetGroomComponentsLODBias(0, GroomGroupIndex);

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

void AAssetsDisplay::SetGroomComponentsGroupAutoLOD(int32 GroupIndex, bool bAutoLOD)
{
	for (UActorComponent* Comp : this->GetComponents())
	{
		if (Comp->IsA<UGroomComponent>())
		{
			UGroomComponent * GroomComp = Cast<UGroomComponent>(Comp);
			SetGroomComponentGroupAutoLOD(GroomComp,GroupIndex,bAutoLOD);
		}
	}
}

void AAssetsDisplay::SetGroomComponentsForcedLOD(int32 LodForcedIndex)
{
	for (UActorComponent* Comp : this->GetComponents())
	{
		if (Comp->IsA<UGroomComponent>())
		{
			UGroomComponent* GroomComp = Cast<UGroomComponent>(Comp);
			SetGroomComponentForcedLOD(GroomComp, LodForcedIndex);
		}
	}
}

void AAssetsDisplay::SetGroomComponentsLODBias(float LodBias, int32 GroupIndex)
{
	for (UActorComponent* Comp : this->GetComponents())
	{
		if (Comp->IsA<UGroomComponent>())
		{
			UGroomComponent* GroomComp = Cast<UGroomComponent>(Comp);
			SetGroomComponentLODBias(GroomComp, LodBias, GroupIndex);
		}
	}
}

void AAssetsDisplay::SetGroomComponentGroupAutoLOD(UGroomComponent* InGroom, int32 GroupIndex, bool bAutoLOD )
{
	if (InGroom == nullptr)
	{
		MsgLog(FString::Printf(L"Set Groom(Group ID: %d) failed, InGroom is NULL",GroupIndex));
		return;
	}
	if (InGroom->GetGroupInstance(GroupIndex) == NULL)
	{
		MsgLog(FString::Printf(L"%s try to set groom LOD (Group ID: %d) but has no groom assets", *InGroom->GetOwner()->GetName(), GroupIndex));
		return;
	}

	InGroom->GetGroupInstance(GroupIndex)->HairGroupPublicData->bAutoLOD = bAutoLOD;
}

void AAssetsDisplay::SetGroomComponentForcedLOD(UGroomComponent* InGroom, int32 LodForcedIndex)
{
	if (InGroom == nullptr)
	{
		MsgLog("Set GroomForcedLod failed, InGroom is NULL");
		return;
	}

	InGroom->SetForcedLOD(LodForcedIndex);
}

void AAssetsDisplay::SetGroomComponentLODBias(UGroomComponent* InGroom, float LodBias, int32 GroupIndex)
{
	if (InGroom == nullptr)
	{
		MsgLog(FString::Printf(L"Set Groom(Group ID: %d) failed, InGroom is NULL", GroupIndex));
		return;
	}
	if (InGroom->GetGroupInstance(GroupIndex) == NULL)
	{
		MsgLog(FString::Printf(L"%s try to set groom LOD (Group ID: %d) but has no groom assets", *InGroom->GetOwner()->GetName(), GroupIndex));
		return;
	}

	InGroom->GetGroupInstance(GroupIndex)->Strands.Modifier.LODBias = LodBias;
}


