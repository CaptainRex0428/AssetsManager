// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"
#include "Framework/SlateDelegates.h"
/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomListView : public SListView<ItemType>
{
public:
	typedef typename SListView<ItemType> FSuperListType;

public:
	SLATE_BEGIN_ARGS(SCustomListView<ItemType>) :
		_ItemHeight(24.f),
		_OnGenerateRow(),
		_OnMouseButtonDoubleClick()
		{}

		SLATE_ARGUMENT(TSharedPtr<SHeaderRow>,HeaderRow)
		SLATE_ARGUMENT(float, ItemHeight)

		SLATE_ARGUMENT(typename TArray<ItemType>*, ListItemsSource)

		SLATE_EVENT(typename FSuperListType::FOnGenerateRow, OnGenerateRow)
		SLATE_EVENT(typename FSuperListType::FOnMouseButtonDoubleClick, OnMouseButtonDoubleClick)

	SLATE_END_ARGS()

private:
	TSharedPtr<SHeaderRow> InHeaderRow;
	float ItemHeight;
	TArray<ItemType> * ListItemsSource;
	typename FSuperListType::FOnGenerateRow OnGenerateRow;
	typename FSuperListType::FOnMouseButtonDoubleClick OnMouseButtonDoubleClick;

	TSharedRef<ITableRow> OnGenerateRowForlist(
		ItemType ItemIn,
		const TSharedRef<STableViewBase>& OwnerTable);

	void OnRowMouseButtonDoubleClicked(
		ItemType ItemIn);

public:
	inline void Construct(
		const SCustomListView<ItemType>::FArguments& InArgs)
	{
		this->InHeaderRow = InArgs._HeaderRow;
		this->ItemHeight = InArgs._ItemHeight;

		this->ListItemsSource = InArgs._ListItemsSource;

		this->OnGenerateRow = InArgs._OnGenerateRow;
		this->OnMouseButtonDoubleClick = InArgs._OnMouseButtonDoubleClick;

		FSuperListType::Construct(FSuperListType::FArguments()
			.HeaderRow(this->InHeaderRow)
			//.ItemHeight(this->ItemHeight)
			.ListItemsSource(this->ListItemsSource)
			.OnGenerateRow(this, &SCustomListView<ItemType>::OnGenerateRowForlist)
			.OnMouseButtonDoubleClick(this, &SCustomListView<ItemType>::OnRowMouseButtonDoubleClicked));
			
	}
};

template<typename ItemType>
inline TSharedRef<ITableRow> SCustomListView<ItemType>::OnGenerateRowForlist(
	ItemType ItemIn, 
	const TSharedRef<STableViewBase>& OwnerTable)
{
	return this->OnGenerateRow.Execute(ItemIn, OwnerTable);
}

template<typename ItemType>
inline void SCustomListView<ItemType>::OnRowMouseButtonDoubleClicked(
	ItemType ItemIn)
{
	return this->OnMouseButtonDoubleClick.Execute(ItemIn);
}
