// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"
/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{

public:
	using FOnGenerateRowList = typename TSlateDelegates<ItemType>::FOnGenerateRow;
	/*using FOnItemScrolledIntoView = typename TSlateDelegates< ItemType >::FOnItemScrolledIntoView;
	using FOnSelectionChanged = typename TSlateDelegates< NullableItemType >::FOnSelectionChanged;
	using FIsSelectableOrNavigable = typename TSlateDelegates< ItemType >::FIsSelectableOrNavigable;
	using FOnMouseButtonClick = typename TSlateDelegates< ItemType >::FOnMouseButtonClick;*/
	using FOnMouseButtonDoubleClickListRow = typename TSlateDelegates< ItemType >::FOnMouseButtonDoubleClickListRow;

	

public:
	SLATE_BEGIN_ARGS(SCustomTable) :
		_TitleColumns(),
		_ColumnsInitWidth(),
		_OnGenerateRowList(),
		_OnGenerateHeaderRow(),
		_OnMouseButtonDoubleClickListRow(),
		_OnSlotResizedRows(),
		_ListSourceItems()
	{}

	SLATE_ARGUMENT(TSharedPtr<TArray<FString>>&,TitleColumns)
	SLATE_ARGUMENT(TSharedPtr<TArray<float>>&,ColumnsInitWidth)

	SLATE_ARGUMENT(TArray<ItemType>&, ListSourceItems)

	SLATE_EVENT(FOnGenerateRowList,OnGenerateRowList)
	SLATE_EVENT(FOnGenerateHeaderRow, OnGenerateHeaderRow)
	SLATE_EVENT(FOnMouseButtonDoubleClickListRow, OnMouseButtonDoubleClickListRow)

	DECLARE_DELEGATE_OneParam(
		FOnSlotResizedRows,
		/** The new size coefficient of the slot */
		float);

	SLATE_EVENT(FOnSlotResizedRows, OnSlotResizedRows)
	
	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable<ItemType>::FArguments& InArgs);

	void Rebuilt();
	

protected:
	FOnGenerateRowList OnGenerateRowList;
	FOnMouseButtonDoubleClickListRow OnMouseButtonDoubleClickListRow;
	FOnSlotResizedRows OnSlotResizedRows;

private:
	TSharedPtr<SVerticalBox> MainTable;

	TSharedPtr<TArray<FString>> TitleColumns;
	TSharedPtr<float> ColumnsInitWidth;
	uint32 ColumnsNum;
	TSharedPtr<SSplitter> HeaderRow;

	TSharedPtr<SListView<ItemType>> ListView;

	TSharedPtr<TArray<TSharedPtr<SSplitter>>> SplitterRows;

private:
	float GetColumnInitWidth(uint32 index);
	float GetColumnWidth(uint32 index);

	void OnSplitterColumResized(
		float size,
		uint32 index);


	TSharedRef<ITableRow> OnGenerateRowForlist(
		ItemType ItemData,
		const TSharedRef<STableViewBase>& OwnerTable);

	void OnRowMouseButtonDoubleClicked(
		ItemType ItemData);

	void ConstructHeaderRow();
};



