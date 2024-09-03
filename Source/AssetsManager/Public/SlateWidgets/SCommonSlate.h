// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SScaleBox.h"

#include "ManagerLogger.h"

/**
 * 
 */
class ASSETSMANAGER_API SCommonSlate : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCommonSlate) {}
	SLATE_END_ARGS()

public:

	static enum CustomTableColumnType
	{
		//basic
		Column_UClass= 0, 
		Column_AssetName, 
		Column_AssetPath, 
		Column_PerAssetHandle,

		//texture
		Column_TextureMaxInGameSize, 
		Column_TextureSourceSize, 
		Column_TextureCompressionSettings, 
		Column_TextureSRGB, 
		Column_TextureGroup
	};

	const TMap<CustomTableColumnType, FString> CustomTableColumnTypeToString =
	{
		{Column_UClass,TEXT("Class")},
		{Column_AssetName,TEXT("Name")},
		{Column_AssetPath,TEXT("Path")},
		{Column_PerAssetHandle,TEXT("Handler")},

		{Column_TextureMaxInGameSize,TEXT("MaxInGameSize")},
		{Column_TextureSourceSize,TEXT("SourceSize")},
		{Column_TextureCompressionSettings,TEXT("CompressionSettings")},
		{Column_TextureSRGB,TEXT("sRGB")},
		{Column_TextureGroup, TEXT("TextureGroup")}
	};

	inline const TMap<CustomTableColumnType, FString>& GetCustomTableColumnTypeToStringMap() 
	{ return CustomTableColumnTypeToString; };
	
	inline TSharedPtr<FString> GetCustomTableColumnTypeToString(
		CustomTableColumnType type) 
	{
		const FString * StrTab = CustomTableColumnTypeToString.Find(type);

		if (StrTab)
		{
			return MakeShared<FString>(*StrTab);
		}
		
		return nullptr; 
	};


#pragma region ConstructFontSet

	FSlateFontInfo GetFontInfo(
		float FontSize,
		const FString& FontName = "EmbossedText");

#pragma endregion

#pragma region ConstructImgBlock

	TSharedRef<SScaleBox> ConstructCommonImageBox(
		const FSlateBrush* ImageBrush,
		FVector2D ImageSize,
		EStretch::Type ImageStrech = EStretch::ScaleToFitX,
		TSharedPtr<SImage>* OutImagePtr = nullptr);

#pragma endregion

#pragma region CustructTextBlock

	// ConstrucStandard Name String
	TSharedRef<STextBlock> ConstructNormalTextBlock(
		const FString& StringToDisplay,
		const FSlateFontInfo& FontInfo,
		const ETextJustify::Type Alignment = ETextJustify::Left,
		const FColor& FontColor = FColor::White,
		const FString& ToolTip = L"");

	TSharedRef<STextBlock> ConstructTitleTextBlock(
		const FString& StringToDisplay,
		const FSlateFontInfo& FontInfo,
		const ETextJustify::Type Alignment = ETextJustify::Center,
		const FColor& FontColor = FColor::White,
		const FString& ToolTip = L"");

	TSharedRef<STextBlock> ConstructTitleTextBlock(
		const FString& StringToDisplay,
		const FSlateFontInfo& FontInfo,
		const ETextJustify::Type Alignment = ETextJustify::Center,
		const FColor& FontColor = FColor::White,
		const FString& ToolTip = L"");

#pragma endregion

#pragma region ButtonTextCustruct

	TSharedRef<STextBlock> ConstructTextForButtons(
		const FString& TextContent);

#pragma endregion

#pragma region BKGFill

	TSharedRef<SOverlay> ConstructOverlayOpaque(
		TSharedPtr<SWidget> DisplayWidget, 
		int DisplayLayer);

#pragma endregion
};
