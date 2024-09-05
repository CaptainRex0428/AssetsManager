// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ASSETSMANAGER_API FStructLikeString
{
public:
	FStructLikeString(FString & InString);
	~FStructLikeString();

private:
	bool isStruct;
	TMap<FString, FString> Map;
};
