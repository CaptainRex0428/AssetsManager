// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HairStrandsCore/Public/GroomComponent.h"
#include "AssetsDisplay.generated.h"

UENUM(BlueprintType)
enum class LODCheckType : uint8
{
	MinLOD = 0 UMETA(DisplayName = "MinLOD"),
	ForcedLOD = 1 UMETA(DisplayName = "ForcedLOD")
};

UCLASS(BlueprintType)
class ASSETSMANAGER_API AAssetsDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssetsDisplay();

	// Check LOD

	UFUNCTION(BlueprintCallable,Category = "AssetsManager|AssetsDisplay")
	virtual void SetComponentsMinLOD(int32 MinLOD);

	UFUNCTION(BlueprintCallable, Category = "AssetsManager|AssetsDisplay")
	virtual void SetComponentsForcedLOD(int32 ForcedLOD);

	UFUNCTION(BlueprintCallable, Category = "AssetsManager|AssetsDisplay")
	virtual void SetComponentsAttachParentLOD(bool bAttach);

	UFUNCTION(BlueprintCallable, Category = "AssetsManager|AssetsDisplay")
	virtual void CheckComponentsLOD(LODCheckType CheckType,int32 LOD);

	UFUNCTION(BlueprintCallable, Category = "AssetsManager|AssetsDisplay")
	virtual void ResetComponentsLOD();
};
