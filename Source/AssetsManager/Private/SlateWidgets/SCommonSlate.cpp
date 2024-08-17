
#include "SlateWidgets/SCommonSlate.h"

#include "ManagerLogger.h"

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

TSharedRef<SSplitter> SCommonSlate::ConstructCommonSpliterRow(
	int SubComponentCount,
	TArray<TSharedRef<SWidget>>Blocks,
	TArray<float> BlocksSize,
	float MinSize,
	EOrientation Orient)
{
	TSharedRef<SSplitter> Spliter =
		SNew(SSplitter)
		.Orientation(Orient)
		.OnSplitterFinishedResizing(this, &SCommonSlate::OnCheck);

	for (int i = 0; i<SubComponentCount; i++)
	{
		float size = 0.2f;

		if (i < BlocksSize.Num())
		{
			size = BlocksSize[i];
		}

		if (i < Blocks.Num()) 
		{
			Spliter
			->AddSlot(i)
			.MinSize(MinSize)
			.Value(size)
			[Blocks[i]];
			continue;
		}

		Spliter
		->AddSlot(i)
		.MinSize(MinSize)
		.Value(size)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Unknown")))
				.Font(GetFontInfo(9))
			]
		];
	}

	return Spliter;
}

void SCommonSlate::OnCheck()
{
	NtfyMsg("Handle");
}
