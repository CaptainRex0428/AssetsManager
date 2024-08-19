// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"


template< typename ArgumentType >
class TCustomSlateDelegates : public TSlateDelegates<ArgumentType>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(
		int32 , 
		FOnGenerateSplitterRow, 
		int32)
};

/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{	
public:
	using FOnGenerateSplitterRow = TCustomSlateDelegates<ItemType>::FOnGenerateSplitterRow;

public:
	

	SLATE_BEGIN_ARGS(SCustomTable) {}
	
	SLATE_ARGUMENT(TArray<SCommonSlate::CustomTableColumnType>*,ColumnsType)
	SLATE_ARGUMENT(TArray<float>*, ColumnsInitWidth)
	SLATE_ARGUMENT(TArray<ItemType>*,SourceItems)

	SLATE_EVENT(FOnGenerateSplitterRow,OnGenerateSplitterRow)
	
	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable<ItemType>::FArguments& InArgs);

	virtual void RefreshTable();

private:

	FOnGenerateSplitterRow OnGenerateSplitterRow;

	TArray<SCommonSlate::CustomTableColumnType>* ColumnsType;
	TArray<float>* ColumnsInitWidth;
	TArray<ItemType>* SourceItems;
	uint32 ColumnsCount;

	TSharedPtr<SVerticalBox> MainTable;

	TSharedPtr<SSplitter> TableHeaderRow;
	TSharedRef<SSplitter> ConstructTableHeaderRow();

	TSharedPtr<SListView<ItemType>> TableListView;
	TSharedRef<SListView<ItemType>> ConstructTableListView();

	TArray<TSharedPtr<SSplitter>> RowsAsSplitter;
	
	void OnTableColumnResized(
		float size, 
		uint32 index);

	TSharedPtr<SButton> TestButton;
	FReply OnTestButtonClicked();
};

template<typename ItemType>
inline void SCustomTable<ItemType>::Construct(const SCustomTable<ItemType>::FArguments& InArgs)
{
	bCanSupportFocus = true;

	this->ColumnsType = InArgs._ColumnsType;
	this->ColumnsInitWidth = InArgs._ColumnsInitWidth;
	this->SourceItems = InArgs._SourceItems;

	this->OnGenerateSplitterRow = InArgs._OnGenerateSplitterRow;

	MainTable = SNew(SVerticalBox);

	ConstructTableHeaderRow();

#pragma region TestButton
	TestButton = 
		SNew(SButton)
		.OnClicked(this, &SCustomTable<ItemType>::OnTestButtonClicked)
		.ContentPadding(FMargin(5.f));
	TestButton->SetContent(ConstructTextForButtons(TEXT("测试")));

	MainTable->AddSlot()
		.AutoHeight()[TestButton.ToSharedRef()];
#pragma endregion

	ChildSlot
	[
		MainTable.ToSharedRef()
	];
}

template<typename ItemType>
inline void SCustomTable<ItemType>::RefreshTable()
{
	ConstructTableHeaderRow();

	TableListView->RebuildList();
}

template<typename ItemType>
inline TSharedRef<SSplitter> SCustomTable<ItemType>::ConstructTableHeaderRow()
{
	if (MainTable->IsValidSlotIndex(0) && MainTable->GetSlot(0).GetWidget() == TableHeaderRow.ToSharedRef())
	{
		MainTable->RemoveSlot(TableHeaderRow.ToSharedRef());
	}
	
	if (RowsAsSplitter.Contains(TableHeaderRow))
	{
		RowsAsSplitter.Remove(TableHeaderRow);
	}

	TableHeaderRow = SNew(SSplitter)
		.Orientation(Orient_Horizontal)
		.HitDetectionSplitterHandleSize(10.f);

	MainTable->InsertSlot(0)
		.AutoHeight()
		[TableHeaderRow.ToSharedRef()];

	this->ColumnsCount = ColumnsType->Num() + 1;

	if(!RowsAsSplitter.Contains(TableHeaderRow))
	{
		RowsAsSplitter.Insert(TableHeaderRow,0);
	}

	TableHeaderRow->AddSlot(0)
		.Value(0.02f)
		.Resizable(false)
		.OnSlotResized(this, &SCustomTable<ItemType>::OnTableColumnResized, uint32(0))
		[
			SNew(SOverlay)

				+ SOverlay::Slot()
				[
					SNew(SBorder)
						.BorderBackgroundColor(FLinearColor::Gray)
						.ColorAndOpacity(FColor::Black)
				]

				+ SOverlay::Slot()
				[
					ConstructTitleTextBlock("*", GetFontInfo(10))
				]
		];

	for(uint32 columnIndex = 0; columnIndex <this->ColumnsCount-1; columnIndex ++)
	{
		float widthInit = 0.2f;

		if (columnIndex < (uint32)(ColumnsInitWidth->Num()))
		{
			widthInit = (*ColumnsInitWidth)[columnIndex];
		}

		CustomTableColumnType type = (*ColumnsType)[columnIndex];

		TSharedPtr<FString> ColumnName = GetCustomTableColumnTypeToString(type);

		if(!ColumnName.IsValid())
		{
			ColumnName = MakeShared<FString>(TEXT("[Undefined]"));
		}

		TableHeaderRow->AddSlot(columnIndex+1)
			.Value(widthInit)
			.MinSize(10.f)
			.OnSlotResized(this, &SCustomTable<ItemType>::OnTableColumnResized, columnIndex+1)
			[
				SNew(SOverlay)

				+SOverlay::Slot()
				[
					SNew(SBorder)
					.BorderBackgroundColor(FLinearColor::Gray)
					.ColorAndOpacity(FColor::Black)
				]

				+ SOverlay::Slot()
				.Padding(0.f,3.f,0.f,3.f)
				[
					ConstructTitleTextBlock(*ColumnName, GetFontInfo(13))
				]
				
			];
			
	}

	return TableHeaderRow.ToSharedRef();
}

template<typename ItemType>
inline TSharedRef<SListView<ItemType>> SCustomTable<ItemType>::ConstructTableListView()
{
	FScrollBarStyle scrollbarStyle;
	scrollbarStyle.SetThickness(30);

	TableListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(36.f)
		.ScrollBarStyle(&scrollbarStyle)
		.ListItemsSource(SourceItems);
		//.OnGenerateRow(this, &SManagerSlateTab::OnGenerateRowForlist)
		//.OnMouseButtonDoubleClick(this, &SManagerSlateTab::OnRowMouseButtonDoubleClicked);

	return TableListView.ToSharedRef();
}

template<typename ItemType>
inline void SCustomTable<ItemType>::OnTableColumnResized(
	float size, 
	uint32 index)
{
	for (TSharedPtr<SSplitter> splitter : RowsAsSplitter)
	{
		splitter->SlotAt(index).SetSizeValue(size);
	}
}

template<typename ItemType>
inline FReply SCustomTable<ItemType>::OnTestButtonClicked()
{
	NtfyMsg("Test");

	int a = OnGenerateSplitterRow.Execute(5);

	NtfyMsg(FString::FromInt(a));

	return FReply::Handled();
}


