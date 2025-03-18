// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsManagerStyle.h"
#include "SlateWidgets/ManagerSlate.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr<FSlateStyleSet> FAssetsMangerStyle::AssetsManagerSlateStyleSet = nullptr;

void FAssetsMangerStyle::Initialize()
{
	if(!AssetsManagerSlateStyleSet.IsValid())
	{
		AssetsManagerSlateStyleSet = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*AssetsManagerSlateStyleSet);
	}
	
}

void FAssetsMangerStyle::ShutDown()
{
	if (AssetsManagerSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*AssetsManagerSlateStyleSet);
		ensure(AssetsManagerSlateStyleSet.IsUnique());
		AssetsManagerSlateStyleSet.Reset();
	}
}

FName & FAssetsMangerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AssetsManagerStyle"));
	return StyleSetName;
}

void FAssetsMangerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const TSharedPtr<FSlateStyleSet>& FAssetsMangerStyle::GetStyleSet()
{
	return AssetsManagerSlateStyleSet;
}

const FVector2D Icon16x16(16.f, 16.f);
const FVector2D Icon32x32(32.f, 32.f);
const FVector2D Icon64x64(64.f, 64.f);

const FVector2D TitleBar(5400.f, 200.f);
const FVector2D ListIcon(22.f, 22.f);

TSharedRef<FSlateStyleSet> FAssetsMangerStyle::Create()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

	const FString IconDirectory =  
		IPluginManager::Get().FindPlugin(TEXT("AssetsManager"))->GetBaseDir() / "Resources";

	CustomStyleSet->SetContentRoot(IconDirectory);
	

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
		"ContentBrowser.BatchRenameTitle",
		new FSlateImageBrush(IconDirectory / "BatchRenameBar.png", TitleBar));

	CustomStyleSet->Set(
		"ContentBrowser.ManagerLogFolder",
		new FSlateImageBrush(IconDirectory / "Icon_Log_32.png", ListIcon));

	CustomStyleSet->Set(
		"ContentBrowser.ManagerFix",
		new FSlateImageBrush(IconDirectory / "Icon_Fix_32.png", ListIcon));

	CustomStyleSet->Set(
		"ContentBrowser.ManagerEdit",
		new FSlateImageBrush(IconDirectory / "Icon_Edit_32.png", ListIcon));

	CustomStyleSet->Set(
		"ContentBrowser.ManagerSizeMap",
		new FSlateImageBrush(IconDirectory / "Icon_SizeMap_32.png", ListIcon));

	CustomStyleSet->Set(
		"ContentBrowser.ManagerDelete",
		new FSlateImageBrush(IconDirectory / "Icon_Delete_32.png", ListIcon));

	CustomStyleSet->Set(
		"LevelEditor.ManagerLookDev",
		new FSlateImageBrush(IconDirectory / "Icon_LOOKDEV_32.png", ListIcon));
	
	CustomStyleSet->Set(
		"LevelEditor.CharacterLookDev",
		new FSlateImageBrush(IconDirectory / "Icon_CharacterLookDev32.png", ListIcon));

	CustomStyleSet->Set(
		"ContentBrowser.Import",
		new FSlateImageBrush(IconDirectory / "Icon_Import_32.png", ListIcon));

	CustomStyleSet->Set(
		"ContentBrowser.LimitSize",
		new FSlateImageBrush(IconDirectory / "Icon_ResizeSource_32.png", ListIcon));

	return CustomStyleSet;
}
