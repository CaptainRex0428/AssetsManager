// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsChecker/AssetsDisplay.h"
#include "ManagerLogger.h"


// Sets default values
AAssetsDisplay::AAssetsDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAssetsDisplay::SetComponentMinLOD(int32 MinLOD)
{
}

void AAssetsDisplay::SetComponentForcedLOD(int32 MinLOD)
{
}
