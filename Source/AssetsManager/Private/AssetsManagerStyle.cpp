// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsManagerStyle.h"
#include "SlateWidgets/ManagerSlate.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FAssetsMangerStyle::StyleSetName = FName("AssetsManagerStyle");
TSharedPtr<FSlateStyleSet> FAssetsMangerStyle::CreatedSlateStyleSet = nullptr;

void FAssetsMangerStyle::InitializeIcons()
{
	if(!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
	
}

void FAssetsMangerStyle::ShutDown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}

const FName & FAssetsMangerStyle::GetStyleName()
{
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FAssetsMangerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory =  
		IPluginManager::Get().FindPlugin(TEXT("AssetsManager"))->GetBaseDir() / "Resources";

	CustomStyleSet->SetContentRoot(IconDirectory);
	const FVector2D Icon16x16(16.f, 16.f);
	const FVector2D Icon32x32(32.f, 32.f);
	const FVector2D Icon64x64(64.f, 64.f);

	CustomStyleSet->Set(
		"ContentBrowser.AssetsManager",
		new FSlateImageBrush(IconDirectory / "LOGO_GREY_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.DeleteUnusedFolders",
		new FSlateImageBrush(IconDirectory / "LOGO_RED_256.png", Icon64x64));

	return CustomStyleSet;
}
