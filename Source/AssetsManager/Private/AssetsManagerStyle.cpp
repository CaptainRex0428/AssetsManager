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

const TSharedPtr<FSlateStyleSet>& FAssetsMangerStyle::GetStyleSet()
{
	return CreatedSlateStyleSet;
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
	const FVector2D TitleBar(5400.f, 200.f);

	CustomStyleSet->Set(
		"ContentBrowser.DeleteUnusedFolders",
		new FSlateImageBrush(IconDirectory / "LOGO_ORANGE_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.AssetsManager",
		new FSlateImageBrush(IconDirectory / "LOGO_GREY_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.MaterialManager",
		new FSlateImageBrush(IconDirectory / "LOGO_GREEN_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.SkeletalMeshManager",
		new FSlateImageBrush(IconDirectory / "LOGO_RED_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.StaticMeshManager",
		new FSlateImageBrush(IconDirectory / "LOGO_BLUE_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.ViewportManager",
		new FSlateImageBrush(IconDirectory / "LOGO_PURPLE_256.png", Icon64x64));

	CustomStyleSet->Set(
		"ContentBrowser.AssetsManagerTitle",
		new FSlateImageBrush(IconDirectory / "TitleBar.png", TitleBar));

	CustomStyleSet->Set(
		"ContentBrowser.MaterialCreatorTitle",
		new FSlateImageBrush(IconDirectory / "MaterialBar.png", TitleBar));

	CustomStyleSet->Set(
		"ContentBrowser.BatchRenameTile",
		new FSlateImageBrush(IconDirectory / "BatchRenameBar.png", TitleBar));

	CustomStyleSet->Set(
		"ContentBrowser.ManagerLogFolder",
		new FSlateImageBrush(IconDirectory / "Icon_Log_32.png", Icon32x32));

	return CustomStyleSet;
}
