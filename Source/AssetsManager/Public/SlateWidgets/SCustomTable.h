// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"


template< typename ArgumentType >
class TCustomSlateDelegates : public TSlateDelegates<ArgumentType>
{
public:
	typedef TDelegate<int32(int32)> FOnGenerateSplitterRow;
	typedef TDelegate<TArray<TSharedPtr<SWidget>>(ArgumentType)> FOnConstructRowWidgets;
};

/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{	
public:
	using FOnGenerateSplitterRow = TCustomSlateDelegates<ItemType>::FOnGenerateSplitterRow;
	using FOnConstructRowWidgets = TCustomSlateDelegates<ItemType>::FOnConstructRowWidgets;

public:
	

	SLATE_BEGIN_ARGS(SCustomTable) {}
	
	SLATE_ARGUMENT(TArray<SCommonSlate::CustomTableColumnType>*,ColumnsType)
	SLATE_ARGUMENT(TArray<float>*, ColumnsInitWidth)
	SLATE_ARGUMENT(TArray<ItemType>*,SourceItems)

	SLATE_EVENT(FOnGenerateSplitterRow,OnGenerateSplitterRow)
	SLATE_EVENT(FOnConstructRowWidgets,OnConstructRowWidgets)
	
	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable<ItemType>::FArguments& InArgs);

	virtual void RefreshTable();

private:

	FOnGenerateSplitterRow OnGenerateSplitterRow;
	FOnConstructRowWidgets OnConstructRowWidgets;

	TArray<SCommonSlate::CustomTableColumnType>* ColumnsType;
	TArray<float>* ColumnsInitWidth;
	TArray<ItemType>* SourceItems;
	uint32 ColumnsCount;

	TSharedPtr<SVerticalBox> MainTable;

	TSharedPtr<SSplitter> TableHeaderRow;
	TSharedRef<SSplitter> ConstructTableHeaderRow();

	TSharedPtr<SListView<ItemType>> TableListView;
	TSharedRef<SListView<ItemType>> ConstructTableListView();

	TSharedRef<ITableRow> OnTableGenerateRowForlist(
		ItemType ItemIn,
		const TSharedRef<STableViewBase>& OwnerTable);

	TArray<TSharedPtr<SSplitter>> RowsAsSplitter;
	
	void OnTableColumnResized(
		float size, 
		uint32 index);

	FReply OnTitleColumnClicked(uint32 ColumnIndex);

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
	this->OnConstructRowWidgets = InArgs._OnConstructRowWidgets;

	MainTable = SNew(SVerticalBox);

	ConstructTableHeaderRow();

	ConstructTableListView();

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
		[
			TableHeaderRow.ToSharedRef()
		];

	this->ColumnsCount = ColumnsType->Num() + 1;

	if(!RowsAsSplitter.Contains(TableHeaderRow))
	{
		RowsAsSplitter.Insert(TableHeaderRow,0);
	}

	for(uint32 columnIndex = 0; columnIndex <this->ColumnsCount; columnIndex ++)
	{
		float widthInit = 0.2f;

		TSharedPtr<SButton> ColumnButton =
			SNew(SButton)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.OnClicked(this, &SCustomTable<ItemType>::OnTitleColumnClicked, columnIndex);

		if(columnIndex == 0)
		{
			ColumnButton->SetContent(ConstructTitleTextBlock("*", GetFontInfo(13)));
			widthInit = 0.03f;
		}
		else 
		{
			if ((columnIndex - 1) < (uint32)(ColumnsInitWidth->Num()))
			{
				widthInit = (*ColumnsInitWidth)[columnIndex - 1];
			}

			CustomTableColumnType type = (*ColumnsType)[columnIndex - 1];

			TSharedPtr<FString> ColumnName = GetCustomTableColumnTypeToString(type);

			if (!ColumnName.IsValid())
			{
				ColumnName = MakeShared<FString>(TEXT("[Undefined]"));
			}

			ColumnButton->SetContent(ConstructTitleTextBlock(*ColumnName, GetFontInfo(13)));
		}

		TableHeaderRow->AddSlot(columnIndex)
			.Value(widthInit)
			.MinSize(10.f)
			.Resizable(columnIndex ? true : false)
			.OnSlotResized(this, &SCustomTable<ItemType>::OnTableColumnResized, columnIndex)
			[
				ConstructOverlayOpaque(ColumnButton,3)
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
		.ListItemsSource(SourceItems)
		.OnGenerateRow(this, &SCustomTable<ItemType>::OnTableGenerateRowForlist);
		//.OnMouseButtonDoubleClick(this, &SManagerSlateTab::OnRowMouseButtonDoubleClicked);

		MainTable->AddSlot().VAlign(VAlign_Fill)
			[
				SNew(SScrollBox)
				+SScrollBox::Slot()
				[
					TableListView.ToSharedRef()
				]
			];

	return TableListView.ToSharedRef();
}

template<typename ItemType>
inline TSharedRef<ITableRow> SCustomTable<ItemType>::OnTableGenerateRowForlist(
	ItemType ItemIn,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	TArray<TSharedPtr<SWidget>> WidgetList = this->OnConstructRowWidgets.Execute(ItemIn);
	
	TSharedPtr<SSplitter> WidgetRow = SNew(SSplitter);  

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget
		= SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(6.f))
		[
			// Splitter
			WidgetRow.ToSharedRef()
		];

	return ListViewRowWidget;
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
inline FReply SCustomTable<ItemType>::OnTitleColumnClicked(uint32 ColumnIndex)
{
	NtfyMsg(FString::FromInt(ColumnIndex));

	return FReply::Handled();
}

template<typename ItemType>
inline FReply SCustomTable<ItemType>::OnTestButtonClicked()
{
	NtfyMsg("Test");

	int a = OnGenerateSplitterRow.Execute(5);

	NtfyMsg(FString::FromInt(a));

	return FReply::Handled();
}
