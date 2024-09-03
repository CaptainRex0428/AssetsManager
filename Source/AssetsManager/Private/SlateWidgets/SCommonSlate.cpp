
#include "SlateWidgets/SCommonSlate.h"
#include "AssetsManagerStyle.h"

FSlateFontInfo SCommonSlate::GetFontInfo(
	float FontSize, 
	const FString& FontName)
{
	FSlateFontInfo font 
		= FCoreStyle::Get().GetFontStyle(FName(FontName));

	font.Size = FontSize;

	return font;
}

TSharedRef<SScaleBox> SCommonSlate::ConstructCommonImageBox(
	const FSlateBrush * ImageBrush,
	FVector2D ImageSize,
	EStretch::Type ImageStrech,
	TSharedPtr<SImage> * OutImagePtr)
{
	TSharedPtr<SImage> ImageCommon =
		SNew(SImage)
		.Image(ImageBrush)
		.DesiredSizeOverride(ImageSize);

	if (OutImagePtr)
	{
		*OutImagePtr = ImageCommon;
	}

	TSharedPtr<SScaleBox> ScaleResult =
		SNew(SScaleBox)
		.Stretch(ImageStrech)
		.OverrideScreenSize(ImageSize)
		[
			ImageCommon.ToSharedRef()
		];

	return ScaleResult.ToSharedRef();
}

TSharedRef<STextBlock> SCommonSlate::ConstructNormalTextBlock(
	const FString& StringToDisplay, 
	const FSlateFontInfo& FontInfo, 
	const ETextJustify::Type Alignment,
	const FColor& FontColor, 
	const FString& ToolTip)
{
	TSharedRef<STextBlock> TextBlock
		= SNew(STextBlock).Text(FText::FromString(StringToDisplay))
		.Justification(Alignment)
		.ColorAndOpacity(FontColor)
		.Font(FontInfo)
		.ToolTipText(FText::FromString(ToolTip));

	return TextBlock;
}

TSharedRef<STextBlock> SCommonSlate::ConstructTitleTextBlock(
	const FString& StringToDisplay, 
	const FSlateFontInfo& FontInfo, 
	const ETextJustify::Type Alignment,
	const FColor& FontColor, 
	const FString& ToolTip)
{
	TSharedRef<STextBlock> TextBlock
		= SNew(STextBlock).Text(FText::FromString(StringToDisplay))
		.Justification(Alignment)
		.ColorAndOpacity(FontColor)
		.Font(FontInfo)
		.ToolTipText(FText::FromString(ToolTip));

	return TextBlock;
}

TSharedRef<STextBlock> SCommonSlate::ConstructTextForButtons(
	const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetFontInfo(15);

	TSharedRef<STextBlock> ContentBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(ButtonTextFont)
		.Justification(ETextJustify::Center);

	return ContentBlock;
}

TSharedRef<SOverlay> SCommonSlate::ConstructOverlayOpaque(
	TSharedPtr<SWidget> DisplayWidget, 
	int DisplayLayer)
{
	TSharedPtr<SOverlay> Overlay = SNew(SOverlay);

	for (int32 LayerIndex = 0; LayerIndex < DisplayLayer -2; ++ LayerIndex)
	{
		Overlay->AddSlot()
			[
				SNew(SHorizontalBox)
			];
	}

	Overlay->AddSlot()
		[
			SNew(SHeaderRow)
		];

	Overlay->AddSlot()
		[
			DisplayWidget.ToSharedRef()
		];

	return Overlay.ToSharedRef();
}
