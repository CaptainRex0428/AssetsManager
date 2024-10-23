// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HairStrandsCore/Public/GroomComponent.h"
#include "AssetsDisplay.generated.h"

UCLASS(BlueprintType)
class ASSETSMANAGER_API AAssetsDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssetsDisplay();

	virtual void SetComponentMinLOD(int32 MinLOD);
	virtual void SetComponentForcedLOD(int32 MinLOD);
};
