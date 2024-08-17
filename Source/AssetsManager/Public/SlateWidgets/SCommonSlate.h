// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SSplitter.h"
/**
 * 
 */
class ASSETSMANAGER_API SCommonSlate : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCommonSlate) {}
	SLATE_END_ARGS()

public:

#pragma region FontSet
	FSlateFontInfo GetFontInfo(float FontSize,
		const FString& FontName = "EmbossedText");
#pragma endregion

#pragma region CustructTextBlock

	// ConstrucStandard Name String
	TSharedRef<STextBlock> ConstructNormalTextBlock(
		const FString& StringToDisplay,
		const FSlateFontInfo& FontInfo,
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

#pragma region ConstructCommonSpliterRow
	
	TSharedRef<SSplitter> ConstructCommonSpliterRow(
		int SubComponentCount,
		TArray<TSharedRef<SWidget>> Blocks,
		float MinSize = 10.f,
		EOrientation Orient = Orient_Horizontal);

	void OnCheck(int32 index);

#pragma endregion
};
