// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"
#include "SlateWidgets/SCustomTableRow.h"
#include "SlateWidgets/SCustomListView.h"

#include "SlateWidgets/TCustomSlateDelegates.h"

/*
 * this custom table can generate a list with default check box
 * the header should be constructed in some standard rules to avoid undefined columns being constructed
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{	
public:

	/*
	* This block defines some delegates that can be define outside this class
	* These delegates are defined in SlateWidgets/TCustomSlateDelegates.h
	*/

	using FOnTableCheckBoxStateChanged = TCustomSlateDelegates<ItemType>::FOnTableCheckBoxStateChanged;
	using FOnTableRowMouseButtonDoubleClicked = TCustomSlateDelegates<ItemType>::FOnTableRowMouseButtonDoubleClicked;

	using FOnGenerateTableHeaderRow = TCustomSlateDelegates<ItemType>::FOnGenerateTableHeaderRow;
	using FOnGenerateTableRowColumn = TCustomSlateDelegates<ItemType>::FOnGenerateTableRowColumn;

public:
	

	SLATE_BEGIN_ARGS(SCustomTable) {}

	SLATE_ARGUMENT(TArray<ItemType>*,SourceItems)

	SLATE_EVENT(FOnTableCheckBoxStateChanged, OnTableCheckBoxStateChanged)
	SLATE_EVENT(FOnTableRowMouseButtonDoubleClicked, OnTableRowMouseButtonDoubleClicked)
	
	SLATE_EVENT(FOnGenerateTableHeaderRow, OnGenerateTableHeaderRow)
	SLATE_EVENT(FOnGenerateTableRowColumn, OnGenerateTableRowColumn)

	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable<ItemType>::FArguments& InArgs);

	/*
	* These functions can be called to interact with table list
	*/

	virtual const TArray<ItemType> & GetSelectedItems();
	virtual const TArray<ItemType> & GetListItems();

	virtual void SelectAll();
	virtual void UnselectAll();

	virtual void RefreshTable(
		bool bRefreshHeader = true);

private:

	/*
	* these are some delegates should be initialized outside the class
	*/

	FOnTableCheckBoxStateChanged OnTableCheckBoxStateChanged;
	FOnTableRowMouseButtonDoubleClicked  OnTableRowMouseButtonDoubleClicked;

	FOnGenerateTableHeaderRow OnGenerateTableHeaderRow;
	FOnGenerateTableRowColumn OnGenerateTableRowColumn;

private:
	
	/*
	* these variables should be initialized outside the class
	*/

	TArray<ItemType>* SourceItems;

private:

	TSharedPtr<SCustomListView<ItemType>> MainTable;
	TSharedPtr<SHeaderRow> TableHeaderRow;

	TArray<TSharedPtr<SCheckBox>> CheckBoxArray;
	TArray<ItemType> CheckBoxSelected;

	int TestCount;

	TSharedPtr<SButton> TestButton;

private:
	/*
	*
	*/
	TSharedRef<SHeaderRow> ConstructTableHeaderRow(
		bool bIsGenerateHeader = false);


	/*
	*
	*/
	TSharedRef<SCustomListView<ItemType>> ConstructTableListView();

	TSharedRef<ITableRow> OnTableGenerateRowForlist(
		ItemType ItemIn,
		const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SWidget> OnTableGenerateListColumn(
		const FName& ColumnName,
		ItemType& ItemShow);

	void OnRowMouseButtonDoubleClicked(
		ItemType ItemIn);


	/*
	*
	*/
	TSharedRef<SCheckBox> ConstructRowCheckBox(
		const ItemType& ItemIn);
	
	void OnCheckBoxStateChanged(
		ECheckBoxState NewState,
		const ItemType ItemIn);

	FReply OnTestButtonClicked();
};

template<typename ItemType>
inline void SCustomTable<ItemType>::Construct(
	const SCustomTable<ItemType>::FArguments& InArgs)
{
	bCanSupportFocus = true;

	this->SourceItems = InArgs._SourceItems;

	this->OnTableCheckBoxStateChanged = InArgs._OnTableCheckBoxStateChanged;
	this->OnTableRowMouseButtonDoubleClicked = InArgs._OnTableRowMouseButtonDoubleClicked;

	this->OnGenerateTableHeaderRow = InArgs._OnGenerateTableHeaderRow;
	this->OnGenerateTableRowColumn = InArgs._OnGenerateTableRowColumn;

	this->CheckBoxArray.Empty();
	this->CheckBoxSelected.Empty();
	this->TestCount = 0;

	this->MainTable = ConstructTableListView();

	ChildSlot
	[
		SCommonSlate::ConstructOverlayOpaque(MainTable,3)
	];
}

template<typename ItemType>
inline TSharedRef<SCustomListView<ItemType>> SCustomTable<ItemType>::ConstructTableListView()
{
	ConstructTableHeaderRow(true);

	this->MainTable =
		SNew(SCustomListView<ItemType>)
		.HeaderRow(this->TableHeaderRow)
		.ItemHeight(36.f)
		.ListItemsSource(SourceItems)
		.OnGenerateRow(this, &SCustomTable<ItemType>::OnTableGenerateRowForlist)
		.OnMouseButtonDoubleClick(this, &SCustomTable<ItemType>::OnRowMouseButtonDoubleClicked);

	return this->MainTable.ToSharedRef();
}

template<typename ItemType>
inline TSharedRef<SHeaderRow> SCustomTable<ItemType>::ConstructTableHeaderRow(
	bool bIsGenerateHeader)
{
	if (bIsGenerateHeader) 
	{
		this->TableHeaderRow = SNew(SHeaderRow);
	}
	else
	{
		this->TableHeaderRow->ClearColumns();
	}
	
	SHeaderRow::FColumn::FArguments CheckBoxArgs;
	CheckBoxArgs.DefaultLabel(FText::FromString(""));
	CheckBoxArgs.ColumnId("CheckBox");
	CheckBoxArgs.FixedWidth(32.f);
	CheckBoxArgs.ShouldGenerateWidget(true);
	CheckBoxArgs.HAlignHeader(HAlign_Center);

	TableHeaderRow->AddColumn(CheckBoxArgs);

	this->OnGenerateTableHeaderRow.Execute(this->TableHeaderRow);

	return TableHeaderRow.ToSharedRef();
}

template<typename ItemType>
inline TSharedRef<ITableRow> SCustomTable<ItemType>::OnTableGenerateRowForlist(
	ItemType ItemIn,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	++TestCount;

	return SNew(SCustomTableRow<ItemType>, OwnerTable)
		.Padding(6.f)
		.ItemShow(ItemIn)
		.OnGenerateTableRowColumn(
			this,
			&SCustomTable<ItemType>::OnTableGenerateListColumn);
}

template<typename ItemType>
inline void SCustomTable<ItemType>::OnRowMouseButtonDoubleClicked(
	ItemType ItemIn)
{
	this->OnTableRowMouseButtonDoubleClicked.Execute(ItemIn);
}

template<typename ItemType>
inline TSharedRef<SWidget> SCustomTable<ItemType>::OnTableGenerateListColumn(
	const FName& ColumnName, 
	ItemType& ItemShow)
{
	if(ColumnName.ToString() == "CheckBox")
	{
		return ConstructRowCheckBox(ItemShow);
	}
	
	return this->OnGenerateTableRowColumn.Execute(ColumnName, ItemShow);
}

#pragma region CheckBox

template<typename ItemType>
inline TSharedRef<SCheckBox> SCustomTable<ItemType>::ConstructRowCheckBox(
	const ItemType& ItemIn)
{
	bool state = false;

	if(CheckBoxSelected.Contains(ItemIn))
	{
		state = true;
	}

	TSharedPtr<SCheckBox> CheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.Padding(FMargin(3.f))
		.HAlign(HAlign_Center)
		.IsChecked(state ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.Visibility(EVisibility::Visible)
		.OnCheckStateChanged(this, &SCustomTable<ItemType>::OnCheckBoxStateChanged, ItemIn);
	
	if (!CheckBoxArray.Contains(CheckBox))
	{
		CheckBoxArray.AddUnique(CheckBox);
	}
	
	return CheckBox.ToSharedRef();
}

template<typename ItemType>
inline void SCustomTable<ItemType>::OnCheckBoxStateChanged(
	ECheckBoxState NewState,
	const ItemType ItemIn)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		if (CheckBoxSelected.Contains(ItemIn))
		{
			CheckBoxSelected.Remove(ItemIn);
			this->OnTableCheckBoxStateChanged.Execute();
		}

		break;

	case ECheckBoxState::Checked:
		if (!CheckBoxSelected.Contains(ItemIn)) 
		{
			CheckBoxSelected.AddUnique(ItemIn);
			this->OnTableCheckBoxStateChanged.Execute();
		};

		break;

	case ECheckBoxState::Undetermined:
		break;

	default:
		break;
	}
}

#pragma endregion

template<typename ItemType>
inline FReply SCustomTable<ItemType>::OnTestButtonClicked()
{
	NtfyMsg("Test");

	NtfyMsg(FString::FromInt(CheckBoxArray.Num()));
	NtfyMsg(FString::FromInt(CheckBoxSelected.Num()));

	return FReply::Handled();
}


#pragma region ReadTable

template<typename ItemType>
inline const TArray<ItemType>& SCustomTable<ItemType>::GetSelectedItems()
{
	return CheckBoxSelected;
}

template<typename ItemType>
inline const TArray<ItemType>& SCustomTable<ItemType>::GetListItems()
{
	return *this->SourceItems;
}

template<typename ItemType>
inline void SCustomTable<ItemType>::SelectAll()
{
	if (SourceItems->Num() == 0 && CheckBoxArray.Num() == 0)
	{
		return;
	}

	for (ItemType & item: *SourceItems)
	{
		if(!CheckBoxSelected.Contains(item))
		{
			CheckBoxSelected.AddUnique(item);
		}
	}

	for (const TSharedPtr<SCheckBox>& CheckBox : CheckBoxArray)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	this->OnTableCheckBoxStateChanged.Execute();
}

template<typename ItemType>
inline void SCustomTable<ItemType>::UnselectAll()
{
	if (SourceItems->Num() == 0 && CheckBoxArray.Num() == 0)
	{
		return;
	}

	CheckBoxSelected.Empty();

	for (const TSharedPtr<SCheckBox> & CheckBox : CheckBoxArray)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	this->OnTableCheckBoxStateChanged.Execute();
}

template<typename ItemType>
inline void SCustomTable<ItemType>::RefreshTable(
	bool bRefreshHeader)
{
	if(bRefreshHeader)
	{
		ConstructTableHeaderRow(false);
	}

	this->CheckBoxArray.Empty();
	this->CheckBoxSelected.Empty();
	this->TestCount = 0;

	if (this->MainTable.IsValid())
	{
		this->MainTable->RebuildList();
	}
}

#pragma endregion