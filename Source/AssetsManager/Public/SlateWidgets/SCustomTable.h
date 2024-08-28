// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"
#include "SlateWidgets/TCustomSlateDelegates.h"
#include "SlateWidgets/SCustomTableRow.h"
#include "SlateWidgets/SCustomListView.h"

/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{	
public:
	using FOnTableCheckBoxStateChanged = TCustomSlateDelegates<ItemType>::FOnTableCheckBoxStateChanged;
	using FOnTableRowMouseButtonDoubleClicked = TCustomSlateDelegates<ItemType>::FOnTableRowMouseButtonDoubleClicked;

	using FOnGenerateTableRowColumn = TCustomSlateDelegates<ItemType>::FOnGenerateTableRowColumn;

public:
	

	SLATE_BEGIN_ARGS(SCustomTable) {}
	
	SLATE_ARGUMENT(TArray<SCommonSlate::CustomTableColumnType>*,ColumnsType)
	SLATE_ARGUMENT(TArray<SCommonSlate::CustomTableColumnType>*,CanGenerateColumnsType)
	SLATE_ARGUMENT(TArray<ItemType>*,SourceItems)

	SLATE_EVENT(FOnTableCheckBoxStateChanged, OnTableCheckBoxStateChanged)
	SLATE_EVENT(FOnTableRowMouseButtonDoubleClicked, OnTableRowMouseButtonDoubleClicked)
	
	SLATE_EVENT(FOnGenerateTableRowColumn, OnGenerateTableRowColumn)

	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable<ItemType>::FArguments& InArgs);

	virtual const TArray<ItemType> & GetSelectedItems();
	virtual const TArray<ItemType> & GetListItems();
	virtual void SelectAll();
	virtual void UnselectAll();

	virtual void RefreshTable(bool bRefreshHeader = true);

private:

	FOnTableCheckBoxStateChanged OnTableCheckBoxStateChanged;
	FOnTableRowMouseButtonDoubleClicked  OnTableRowMouseButtonDoubleClicked;

	FOnGenerateTableRowColumn OnGenerateTableRowColumn;

	TArray<ItemType>* SourceItems;
	TArray<SCommonSlate::CustomTableColumnType>* ColumnsType;
	TArray<SCommonSlate::CustomTableColumnType>* CanGenerateColumnsType;
	TArray<float>* ColumnsInitWidth;

private:

	TSharedRef<SCustomListView<ItemType>> ConstructTableListView();
	TSharedRef<SHeaderRow> ConstructTableHeaderRow(bool bIsGenerateHeader = false);

	TSharedRef<ITableRow> OnTableGenerateRowForlist(
		ItemType ItemIn,
		const TSharedRef<STableViewBase>& OwnerTable);

	void OnRowMouseButtonDoubleClicked(
		ItemType ItemIn);

	TSharedRef<SWidget> OnTableGenerateListColumn(
		const FName& ColumnName,
		ItemType& ItemShow);

	TSharedRef<SCheckBox> ConstructRowCheckBox(
		const ItemType ItemIn);
	
	void OnCheckBoxStateChanged(
		ECheckBoxState NewState,
		const ItemType ItemIn);

	FReply OnTestButtonClicked();

private:

	TSharedPtr<SCustomListView<ItemType>> MainTable;
	TSharedPtr<SHeaderRow> TableHeaderRow;

	TArray<TSharedPtr<SCheckBox>> CheckBoxArray;
	TArray<ItemType> CheckBoxSelected;

	TSharedPtr<SButton> TestButton;
};

template<typename ItemType>
inline void SCustomTable<ItemType>::Construct(
	const SCustomTable<ItemType>::FArguments& InArgs)
{
	bCanSupportFocus = true;

	this->ColumnsType = InArgs._ColumnsType;
	this->CanGenerateColumnsType = InArgs._CanGenerateColumnsType;
	this->SourceItems = InArgs._SourceItems;

	this->OnTableCheckBoxStateChanged = InArgs._OnTableCheckBoxStateChanged;
	this->OnTableRowMouseButtonDoubleClicked = InArgs._OnTableRowMouseButtonDoubleClicked;

	this->OnGenerateTableRowColumn = InArgs._OnGenerateTableRowColumn;

	this->CheckBoxArray.Empty();
	this->CheckBoxSelected.Empty();

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

	for(SCommonSlate::CustomTableColumnType ColumnIn: * this->ColumnsType)
	{
		SHeaderRow::FColumn::FArguments ColumnBoxArgs;

		ColumnBoxArgs.DefaultLabel(FText::FromString("[Undefined Column]"));
		ColumnBoxArgs.ColumnId("[Undefined]");
		ColumnBoxArgs.ShouldGenerateWidget(true);
		ColumnBoxArgs.HAlignHeader(HAlign_Center);

		switch (ColumnIn)
		{
		case Column_UClass:
			ColumnBoxArgs.FillWidth(0.07f);
			break;

		case Column_AssetName:
			ColumnBoxArgs.FillWidth(0.3f);
			break;

		case Column_AssetPath:
			ColumnBoxArgs.FillWidth(0.5f);
			break;
			
		case Column_PerAssetHandle:
			ColumnBoxArgs.FillWidth(0.25f);
			break;

		case Column_TextureMaxInGameSize:
			ColumnBoxArgs.FillWidth(0.07f);
			break;

		case Column_TextureSourceSize:
			ColumnBoxArgs.FillWidth(0.07f);
			break;

		case Column_TextureCompressionSettings:
			ColumnBoxArgs.FillWidth(0.2f);
			break;

		case Column_TextureSRGB:
			ColumnBoxArgs.FillWidth(0.05f);
			break;
				
		case Column_TextureGroup:
			ColumnBoxArgs.FillWidth(0.2f);
			break;

		default:
			ColumnBoxArgs.FillWidth(0.1f);
			break;
		}
		
		
		const FString* ColumnNamePtr = CustomTableColumnTypeToString.Find(ColumnIn);

		if (ColumnNamePtr)
		{
			ColumnBoxArgs.DefaultLabel(FText::FromString(*ColumnNamePtr));
			ColumnBoxArgs.ColumnId(FName(*ColumnNamePtr));
		}

		TableHeaderRow->AddColumn(ColumnBoxArgs);
	}

	return TableHeaderRow.ToSharedRef();
}

template<typename ItemType>
inline TSharedRef<ITableRow> SCustomTable<ItemType>::OnTableGenerateRowForlist(
	ItemType ItemIn,
	const TSharedRef<STableViewBase>& OwnerTable)
{
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
	const ItemType ItemIn)
{
	TSharedPtr<SCheckBox> CheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.Padding(FMargin(3.f))
		.HAlign(HAlign_Center)
		.IsChecked(ECheckBoxState::Unchecked)
		.Visibility(EVisibility::Visible)
		.OnCheckStateChanged(this, &SCustomTable<ItemType>::OnCheckBoxStateChanged, ItemIn);

	CheckBoxArray.Add(CheckBox);

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
		}

		this->OnTableCheckBoxStateChanged.Execute();

		break;

	case ECheckBoxState::Checked:
		CheckBoxSelected.AddUnique(ItemIn);

		this->OnTableCheckBoxStateChanged.Execute();

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
	if (CheckBoxArray.Num() == 0)
	{
		return;
	}

	for (const TSharedPtr<SCheckBox> CheckBox : CheckBoxArray)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}
}

template<typename ItemType>
inline void SCustomTable<ItemType>::UnselectAll()
{
	if (CheckBoxArray.Num() == 0)
	{
		return;
	}

	for (const TSharedPtr<SCheckBox> CheckBox : CheckBoxArray)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}
}

template<typename ItemType>
inline void SCustomTable<ItemType>::RefreshTable(
	bool bRefreshHeader)
{
	CheckBoxArray.Empty();
	CheckBoxSelected.Empty();
	
	if(bRefreshHeader)
	{
		ConstructTableHeaderRow(false);
	}

	if (this->MainTable.IsValid())
	{
		this->MainTable->RebuildList();
	}
}

#pragma endregion