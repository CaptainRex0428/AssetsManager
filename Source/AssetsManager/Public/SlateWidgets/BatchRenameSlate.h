// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SCommonSlate.h"

#include "AssetsManagerStyle.h"
#include "SlateWidgets/TCustomSlateDelegates.h"

#define TABNAME_BATCHRENAME "BatchRename"

/**
 *
 */
template <typename ItemType>
class ASSETSMANAGER_API SBatchRename : public SCommonSlate
{
	using FOnApply = TCustomSlateDelegates<ItemType>::FOnApply;

public:
	SLATE_BEGIN_ARGS(SBatchRename<ItemType>):
		_ItemIn()
		{}
	SLATE_ARGUMENT(TArray<ItemType>, ItemIn)

	SLATE_EVENT(FOnApply,OnApply)

	SLATE_END_ARGS()

private:
	FOnApply OnApply;

	TArray<ItemType> RenameList;

	TSharedPtr<SButton> ApplyButton;
	TSharedRef<SButton> ConstructApplyButton();
	FReply OnApplyButtonClicked();

public:
	inline void Construct(
		const SBatchRename<ItemType>::FArguments& InArgs)
	{
		this->OnApply = InArgs._OnApply;

		const FSlateBrush* TitleImage = 
			FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.BatchRenameTile");

		TSharedPtr<SVerticalBox> MainUI = SNew(SVerticalBox);

		/*
		* Title
		*/

		MainUI->AddSlot()
			.AutoHeight()
			[
				ConstructCommonImageBox(
					TitleImage,
					FVector2D(FVector2D(5332.f, 175.f)),
					EStretch::ScaleToFitX)
			];

		MainUI->AddSlot()
			.AutoHeight()
			[
				SNew(SBorder)
			];

		/*
		* List
		*/

		FString OutputStr;

		for (auto a : InArgs._ItemIn)
		{
			OutputStr += a->AssetName.ToString();
			OutputStr += "\n";
		}

		MainUI->AddSlot()
			.HAlign(HAlign_Fill)
			[
				ConstructNormalTextBlock(OutputStr, GetFontInfo(9))
			];

		/*
		* Apply
		*/

		ConstructApplyButton();

		MainUI->AddSlot()
			.AutoHeight()
			[
				this->ApplyButton.ToSharedRef()
			];

		ChildSlot
			[
				MainUI.ToSharedRef()
			];
	}
};

template<typename ItemType>
inline TSharedRef<SButton> SBatchRename<ItemType>::ConstructApplyButton()
{
	this->ApplyButton =
		SNew(SButton)
		.OnClicked(this, &SBatchRename<ItemType>::OnApplyButtonClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	this->ApplyButton->SetContent(ConstructTextForButtons(TEXT("应用修改")));
#else
	this->ApplyButton->SetContent(ConstructTextForButtons(TEXT("Delete All Selected")));
#endif
	return this->ApplyButton.ToSharedRef();
}

template<typename ItemType>
inline FReply SBatchRename<ItemType>::OnApplyButtonClicked()
{
	this->OnApply.Execute();
	return FReply::Handled();
}
