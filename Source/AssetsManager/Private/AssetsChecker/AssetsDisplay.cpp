// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsChecker/AssetsDisplay.h"


// Sets default values
AAssetsDisplay::AAssetsDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAssetsDisplay::SetComponentMinLOD(int32 MinLOD)
{
	TArray<TObjectPtr<UActorComponent>> Components = GetComponents();
	for (TObjectPtr<UActorComponent> ChildCompoent : Components)
	{
		ChildCompoent->Rename();

		if(ChildCompoent->IsA<USkeletalMeshComponent>())
		{

		}

		if (ChildCompoent->IsA<UStaticMeshComponent>())
		{

		}

		if (ChildCompoent->IsA<UGroomComponent>())
		{

		}
	}
}

void AAssetsDisplay::SetComponentForcedLOD(int32 MinLOD)
{
}

TArray<TObjectPtr<UActorComponent>>& AAssetsDisplay::GetComponents()
{
	return this->BlueprintCreatedComponents;
	// return this->OwnedComponents;
}

