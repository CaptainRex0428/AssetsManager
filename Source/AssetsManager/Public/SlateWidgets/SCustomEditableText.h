// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"
#include "SlateWidgets/TCustomSlateDelegates.h"

/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomEditableText: public SCommonSlate
{
public:
	using FOnItemToFText = TCustomSlateDelegates<ItemType>::FOnItemToFText;
	using FOnItemCommit = TCustomSlateDelegates<ItemType>::FOnItemCommit;

public:
	SLATE_BEGIN_ARGS(SCustomEditableText) {}
	SLATE_ARGUMENT(ItemType, SourceItem)
	SLATE_ARGUMENT(FSlateFontInfo, Font)
	SLATE_ARGUMENT(EHorizontalAlignment, Justify)
	SLATE_ARGUMENT(FSlateColor, TextColor)
	SLATE_ARGUMENT(FText, TextTips)

	SLATE_EVENT(FOnItemToFText,OnItemToFText)
	SLATE_EVENT(FOnItemCommit,OnItemCommit)

	SLATE_END_ARGS()


public:
	virtual void Construct(const SCustomEditableText<ItemType>::FArguments& InArgs);

private:
	TSharedRef<STextBlock> ConstructTextBlockFromFText();
	FReply OnTextBlockDoubleClicked(const FGeometry& Geo, const FPointerEvent& e);

	TSharedRef<SEditableTextBox> ConstructEditableTextBlockFromFText();
	void OnEditableTextBlockCommitted(const FText& TextIn, ETextCommit::Type CommitType);

public:
	ItemType SourceItem;

private:
	FOnItemToFText OnItemToFText;
	FOnItemCommit OnItemCommit;

	
	FSlateFontInfo FontInfo;
	EHorizontalAlignment Justify;
	FSlateColor TextColor;
	FText TextTips;

	TSharedPtr <SHorizontalBox> MainBlock;
	TSharedPtr <SWidget> DisplayBox;
	
	FText TextToDisplay;
};

template<typename ItemType>
inline void SCustomEditableText<ItemType>::Construct(
	const SCustomEditableText<ItemType>::FArguments& InArgs)
{
	this->SourceItem = InArgs._SourceItem;
	this->FontInfo = InArgs._Font;
	this->Justify = InArgs._Justify;
	this->TextColor = InArgs._TextColor;
	this->TextTips = InArgs._TextTips;

	this->OnItemToFText = InArgs._OnItemToFText;
	this->OnItemCommit = InArgs._OnItemCommit;

	MainBlock = SNew(SHorizontalBox);
	TextToDisplay = this->OnItemToFText.Execute(this->SourceItem);
	
	DisplayBox = ConstructTextBlockFromFText();

	MainBlock->AddSlot()
		.HAlign(this->Justify)
		[
			DisplayBox.ToSharedRef()
		];
		
	ChildSlot
		[
			MainBlock.ToSharedRef()
		];
}

template<typename ItemType>
inline TSharedRef<STextBlock> SCustomEditableText<ItemType>::ConstructTextBlockFromFText()
{
	TSharedPtr<STextBlock> TextBlock =
		SNew(STextBlock)
		.Text(this->TextToDisplay)
		.Font(this->FontInfo)
		.Justification(ETextJustify::Center)
		.ColorAndOpacity(this->TextColor)
		.ToolTipText(this->TextTips)
		.OnDoubleClicked(this,&SCustomEditableText<ItemType>::OnTextBlockDoubleClicked);

	return TextBlock.ToSharedRef();
}

template<typename ItemType>
inline FReply SCustomEditableText<ItemType>::OnTextBlockDoubleClicked(
	const FGeometry& Geo, 
	const FPointerEvent & e)
{
	MainBlock->RemoveSlot(DisplayBox.ToSharedRef());
	
	DisplayBox = ConstructEditableTextBlockFromFText();
	
	MainBlock->AddSlot()
		.HAlign(this->Justify)
		[
			DisplayBox.ToSharedRef()
		];

	return FReply::Handled();
}

template<typename ItemType>
inline TSharedRef<SEditableTextBox> SCustomEditableText<ItemType>::ConstructEditableTextBlockFromFText()
{
	TSharedPtr<SEditableTextBox> EditableTextBox =
		SNew(SEditableTextBox)
		.Text(TextToDisplay)
		.Font(this->FontInfo)
		.Justification(ETextJustify::Center)
		.ToolTipText(this->TextTips)
		.OnTextCommitted(this,&SCustomEditableText<ItemType>::OnEditableTextBlockCommitted);

	return EditableTextBox.ToSharedRef();
}

template<typename ItemType>
inline void SCustomEditableText<ItemType>::OnEditableTextBlockCommitted(
	const FText& TextIn, 
	ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if(this->OnItemCommit.Execute(TextIn, CommitType, SourceItem))
		{
			this->TextToDisplay = TextIn;

			NtfyMsg(TextToDisplay.ToString());
		};
	}

	MainBlock->RemoveSlot(DisplayBox.ToSharedRef());

	DisplayBox = ConstructTextBlockFromFText();

	MainBlock->AddSlot()
		.HAlign(this->Justify)
		[
			DisplayBox.ToSharedRef()
		];

}
