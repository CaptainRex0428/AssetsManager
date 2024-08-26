// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/ScommonSlate.h"
/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTableRow : public SMultiColumnTableRow<ItemType>
{
	SLATE_BEGIN_ARGS(SCustomTableRow<ItemType>) :
		_ItemShow()
		{}

	SLATE_ARGUMENT(ItemType, ItemShow)
	SLATE_ARGUMENT(float, Padding)
	SLATE_END_ARGS()

public:
	typedef SMultiColumnTableRow< ItemType > FSuperRowType;

public:
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		ItemShow = InArgs._ItemShow;


		FSuperRowType::Construct(
			FSuperRowType::FArguments().Padding(InArgs._Padding),
			InOwnerTableView);
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		if (ColumnName == TEXT("Type"))
		{
			return SNew(STextBlock)
				.Text(FText::FromString("TypeTest"));
		}
		else
		{
			return SNew(STextBlock)
				.Text(FText::FromString("Test"));
		}
	}

private:
	ItemType ItemShow;
};
