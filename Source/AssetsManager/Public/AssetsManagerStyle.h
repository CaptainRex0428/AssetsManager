// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class FAssetsMangerStyle
{
public:
	static void Initialize();

	static void ShutDown();
	
	static const TSharedPtr<FSlateStyleSet>& GetStyleSet();

	static FName& GetStyleSetName();

	static void ReloadTextures();

private:
	static TSharedRef<FSlateStyleSet> Create();
	static TSharedPtr<FSlateStyleSet> AssetsManagerSlateStyleSet;
};