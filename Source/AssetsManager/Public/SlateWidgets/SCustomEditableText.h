// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"
#include "SlateWidgets/TCustomSlateDelegates.h"
#include "AssetsManagerConfig.h"

/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomEditableText: public SCommonSlate
{
public:
	using FOnItemToDisplayText = TCustomSlateDelegates<ItemType>::FOnItemToFText;
	using FOnItemToTipText = TCustomSlateDelegates<ItemType>::FOnItemToFText;
	using FOnItemCommit = TCustomSlateDelegates<ItemType>::FOnItemCommit;

public:
	SLATE_BEGIN_ARGS(SCustomEditableText) {}
	SLATE_ARGUMENT(ItemType*, SourceItem)
	SLATE_ARGUMENT(FSlateFontInfo, Font)
	SLATE_ARGUMENT(EHorizontalAlignment, Justify)
	SLATE_ARGUMENT(FSlateColor, TextColor)

	SLATE_EVENT(FOnItemToDisplayText,OnItemToDisplayText)
	SLATE_EVENT(FOnItemToTipText, OnItemToTipText)
	SLATE_EVENT(FOnItemCommit,OnItemCommit)

	SLATE_END_ARGS()


public:
	virtual void Construct(const SCustomEditableText<ItemType>::FArguments& InArgs);

private:
	TSharedRef<STextBlock> ConstructTextBlockFromFText();
	FReply OnTextBlockDoubleClicked(const FGeometry& Geo, const FPointerEvent& e);

	TSharedRef<SEditableTextBox> ConstructEditableTextBlockFromFText();
	void OnEditableTextBlockCommitted(const FText& TextIn, ETextCommit::Type CommitType);

private:
	ItemType SourceItem;

	FOnItemToDisplayText OnItemToDisplayText;
	FOnItemToTipText OnItemToTipText;
	FOnItemCommit OnItemCommit;

	
	FSlateFontInfo FontInfo;
	EHorizontalAlignment Justify;
	FSlateColor TextColor;


	TSharedPtr <SHorizontalBox> MainBlock;
	TSharedPtr <SWidget> DisplayBox;
	
	FText TextToDisplay;
	FText TextTips;
};

template<typename ItemType>
inline void SCustomEditableText<ItemType>::Construct(
	const SCustomEditableText<ItemType>::FArguments& InArgs)
{
	this->SourceItem = *InArgs._SourceItem;
	this->FontInfo = InArgs._Font;
	this->Justify = InArgs._Justify;
	this->TextColor = InArgs._TextColor;

	this->OnItemToDisplayText = InArgs._OnItemToDisplayText;
	this->OnItemToTipText = InArgs._OnItemToTipText;
	this->OnItemCommit = InArgs._OnItemCommit;

	this->MainBlock = SNew(SHorizontalBox);
	this->TextToDisplay = this->OnItemToDisplayText.Execute(this->SourceItem);
	this->TextTips = this->OnItemToTipText.Execute(this->SourceItem);
	
	this->DisplayBox = ConstructTextBlockFromFText();

	this->MainBlock->AddSlot()
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
	this->MainBlock->RemoveSlot(DisplayBox.ToSharedRef());
	
	this->DisplayBox = ConstructEditableTextBlockFromFText();
	
	this->MainBlock->AddSlot()
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
		if(this->OnItemCommit.Execute(TextIn, CommitType, this->SourceItem))
		{
			this->TextToDisplay = this->OnItemToDisplayText.Execute(this->SourceItem);
			this->TextTips = this->OnItemToTipText.Execute(this->SourceItem);

#ifdef ZH_CN
			NtfyMsgLog(TEXT("成功命名资产为") + this->TextToDisplay.ToString());
#else
			NtfyMsgLog(TEXT("Successfully renamed the asset to ") + this->TextToDisplay);
#endif
		};
	}

	this->MainBlock->RemoveSlot(DisplayBox.ToSharedRef());

	this->DisplayBox = ConstructTextBlockFromFText();

	this->MainBlock->AddSlot()
		.HAlign(this->Justify)
		[
			DisplayBox.ToSharedRef()
		];

}
