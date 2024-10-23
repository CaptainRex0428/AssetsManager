// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetsChecker/AssetsDisplay.h"
#include "DSAssetsDisplay.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ASSETSMANAGER_API ADSAssetsDisplay : public AAssetsDisplay
{
	GENERATED_BODY()
	
public:
	ADSAssetsDisplay();
	virtual ~ADSAssetsDisplay();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Face;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ParasiteA;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ParasiteB;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGroomComponent* Groom_A;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGroomComponent* Groom_B;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGroomComponent* Groom_C;
};
