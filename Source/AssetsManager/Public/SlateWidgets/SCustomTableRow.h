// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/ScommonSlate.h"
#include "SlateWidgets/TCustomSlateDelegates.h"
/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTableRow : public SMultiColumnTableRow<ItemType>
{
public:

	using FOnGenerateTableRowColumn = TCustomSlateDelegates<ItemType>::FOnGenerateTableRowColumn;

	typedef SMultiColumnTableRow<ItemType> FSuperRowType;

public:

	SLATE_BEGIN_ARGS(SCustomTableRow<ItemType>) :
		_ItemShow()
		{}

	SLATE_ARGUMENT(ItemType, ItemShow)
	SLATE_ARGUMENT(float, Padding)

	SLATE_EVENT(FOnGenerateTableRowColumn, OnGenerateTableRowColumn)

	SLATE_END_ARGS()



public:
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		this->ItemShow = InArgs._ItemShow;
		this->OnGenerateTableRowColumn = InArgs._OnGenerateTableRowColumn;

		FSuperRowType::Construct(
			FSuperRowType::FArguments().Padding(InArgs._Padding),
			InOwnerTableView);
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{

		 return this->OnGenerateTableRowColumn.Execute(ColumnName,this->ItemShow);		
	}

private:
	ItemType ItemShow;

	FOnGenerateTableRowColumn OnGenerateTableRowColumn;
};
