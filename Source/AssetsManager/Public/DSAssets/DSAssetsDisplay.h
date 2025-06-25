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
	UPROPERTY(Category = "AssetsManager",VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Body;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Face;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ParasiteA;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ParasiteB;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponL;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponR;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite)
	UGroomComponent* Groom_A;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite)
	UGroomComponent* Groom_B;

	UPROPERTY(Category = "AssetsManager", VisibleAnywhere, BlueprintReadWrite)
	UGroomComponent* Groom_C;
};
