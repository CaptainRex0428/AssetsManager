// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ASSETSMANAGER_API FStructLikeString:public FString
{
public:
	FStructLikeString(FString InString);
	~FStructLikeString();

	bool IsStructLike();
	TMap<FString, FString>& GetMap();

protected:
	TArray<FString> SplitStringRecursive(
		const FString& InStr,
		const FString& SpliteTag);

private:
	bool isStructLike;
	TMap<FString, FString> Map;
};
