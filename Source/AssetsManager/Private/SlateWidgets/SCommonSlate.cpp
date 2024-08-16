
#include "SlateWidgets/SCommonSlate.h"

FSlateFontInfo SCommonSlate::GetFontInfo(
	float FontSize, 
	const FString& FontName)
{
	FSlateFontInfo font 
		= FCoreStyle::Get().GetFontStyle(FName(FontName));

	font.Size = FontSize;

	return font;
}

TSharedRef<STextBlock> SCommonSlate::ConstructNormalTextBlock(
	const FString& StringToDisplay, 
	const FSlateFontInfo& FontInfo, 
	const FColor& FontColor, 
	const FString& ToolTip)
{
	TSharedRef<STextBlock> TextBlock
		= SNew(STextBlock).Text(FText::FromString(StringToDisplay))
		.Justification(ETextJustify::Left)
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
