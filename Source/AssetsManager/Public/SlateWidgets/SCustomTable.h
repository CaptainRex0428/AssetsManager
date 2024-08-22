// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"


template< typename ArgumentType >
class TCustomSlateDelegates : public TSlateDelegates<ArgumentType>
{
public:
	typedef TDelegate<TArray<TSharedPtr<SWidget>>(ArgumentType)> FOnConstructRowWidgets;
	typedef TDelegate<void()> FOnTableCheckBoxStateChanged;
	typedef TDelegate<void(ArgumentType)> FOnTableRowMouseButtonDoubleClicked;
};

/**
 * 
 */
template <typename ItemType>
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{	
public:
	using FOnConstructRowWidgets = TCustomSlateDelegates<ItemType>::FOnConstructRowWidgets;
	using FOnTableCheckBoxStateChanged = TCustomSlateDelegates<ItemType>::FOnTableCheckBoxStateChanged;
	using FOnTableRowMouseButtonDoubleClicked = TCustomSlateDelegates<ItemType>::FOnTableRowMouseButtonDoubleClicked;

public:
	

	SLATE_BEGIN_ARGS(SCustomTable) {}
	
	SLATE_ARGUMENT(TArray<SCommonSlate::CustomTableColumnType>*,ColumnsType)
	SLATE_ARGUMENT(TArray<float>*, ColumnsInitWidth)
	SLATE_ARGUMENT(TArray<ItemType>*,SourceItems)

	SLATE_EVENT(FOnConstructRowWidgets,OnConstructRowWidgets)
	SLATE_EVENT(FOnTableCheckBoxStateChanged, OnTableCheckBoxStateChanged)
	SLATE_EVENT(FOnTableRowMouseButtonDoubleClicked, OnTableRowMouseButtonDoubleClicked)
	
	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable<ItemType>::FArguments& InArgs);

	virtual const TArray<ItemType> & GetSelectedItems();
	virtual const TArray<ItemType> & GetListItems();
	virtual void SelectAll();
	virtual void UnselectAll();

	virtual void RefreshTable();

private:

	FOnConstructRowWidgets OnConstructRowWidgets;
	FOnTableCheckBoxStateChanged OnTableCheckBoxStateChanged;
	FOnTableRowMouseButtonDoubleClicked  OnTableRowMouseButtonDoubleClicked;

	TArray<SCommonSlate::CustomTableColumnType>* ColumnsType;
	TArray<float>* ColumnsInitWidth;
	TArray<ItemType>* SourceItems;
	uint32 ColumnsCount;

private:

	TSharedPtr<SVerticalBox> MainTable;
	TSharedPtr<SSplitter> TableHeaderRow;
	TSharedRef<SSplitter> ConstructTableHeaderRow();

	TSharedPtr<SListView<ItemType>> TableListView;
	TSharedRef<SListView<ItemType>> ConstructTableListView();

	TArray<TSharedPtr<SCheckBox>> CheckBoxArray;
	TArray<ItemType> CheckBoxSelected;
	
	TSharedRef<SCheckBox> ConstructRowCheckBox(
		const ItemType ItemIn);
	
	void OnCheckBoxStateChanged(
		ECheckBoxState NewState,
		const ItemType ItemIn);

	TSharedRef<ITableRow> OnTableGenerateRowForlist(
		ItemType ItemIn,
		const TSharedRef<STableViewBase>& OwnerTable);

	TArray<TSharedPtr<SSplitter>> RowsAsSplitter;

	void OnRowMouseButtonDoubleClicked(ItemType ItemIn);
	
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

	this->OnConstructRowWidgets = InArgs._OnConstructRowWidgets;
	this->OnTableCheckBoxStateChanged = InArgs._OnTableCheckBoxStateChanged;
	this->OnTableRowMouseButtonDoubleClicked = InArgs._OnTableRowMouseButtonDoubleClicked;

	this->CheckBoxArray.Empty();
	this->CheckBoxSelected.Empty();

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
inline void SCustomTable<ItemType>::RefreshTable()
{
	CheckBoxArray.Empty();
	CheckBoxSelected.Empty();
	RowsAsSplitter.Empty();

	ConstructTableHeaderRow();

	if (TableListView.IsValid()) 
	{
		TableListView->RebuildList();
	}
}

template<typename ItemType>
inline TSharedRef<SSplitter> SCustomTable<ItemType>::ConstructTableHeaderRow()
{
	if (MainTable->IsValidSlotIndex(0) 
		&& MainTable->GetSlot(0).GetWidget() == TableHeaderRow.ToSharedRef())
	{
		MainTable->RemoveSlot(TableHeaderRow.ToSharedRef());
	}
	
	if (RowsAsSplitter.Contains(TableHeaderRow))
	{
		RowsAsSplitter.Remove(TableHeaderRow);
	}

	TableHeaderRow = SNew(SSplitter)
		.Orientation(Orient_Horizontal)
		.HitDetectionSplitterHandleSize(5.f);

	MainTable->InsertSlot(0)
		.AutoHeight()
		[
			TableHeaderRow.ToSharedRef()
		];

	if(!RowsAsSplitter.Contains(TableHeaderRow))
	{
		RowsAsSplitter.Insert(TableHeaderRow,0);
	}

	this->ColumnsCount = ColumnsType->Num() + 1;

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
			ColumnButton->SetContent(ConstructTitleTextBlock("*", GetFontInfo(9)));
			widthInit = 0.02f;
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
			.MinSize(20.f)
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
		SNew(SListView<ItemType>)
		.ItemHeight(36.f)
		.ScrollBarStyle(&scrollbarStyle)
		.ListItemsSource(SourceItems)
		.OnGenerateRow(this, &SCustomTable<ItemType>::OnTableGenerateRowForlist)
		.OnMouseButtonDoubleClick(this, &SCustomTable<ItemType>::OnRowMouseButtonDoubleClicked);

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
		.OnCheckStateChanged(this,&SCustomTable<ItemType>::OnCheckBoxStateChanged,ItemIn);

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

template<typename ItemType>
inline TSharedRef<ITableRow> SCustomTable<ItemType>::OnTableGenerateRowForlist(
	ItemType ItemIn,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	TArray<TSharedPtr<SWidget>> WidgetList = this->OnConstructRowWidgets.Execute(ItemIn);
	
	TSharedPtr<SSplitter> WidgetRow = SNew(SSplitter);
	RowsAsSplitter.Add(WidgetRow);

	for(uint32 ColumnIndex = 0 ; ColumnIndex < ColumnsCount; ++ColumnIndex)
	{
		
		TSharedRef<SWidget> RowWidget = ConstructNormalTextBlock(TEXT("[No Info]"),GetFontInfo(9),ETextJustify::Center);

		if (ColumnIndex == (uint32)0)
		{
			RowWidget = SNew(SHorizontalBox)
				+SHorizontalBox::Slot().HAlign(HAlign_Center)
				[ConstructRowCheckBox(ItemIn)];
		}
		else
		{
			if (ColumnIndex - 1 < (uint32)WidgetList.Num())
			{
				RowWidget = WidgetList[ColumnIndex - 1].ToSharedRef();
			}
		}

		WidgetRow->AddSlot(ColumnIndex)
			.OnSlotResized(this, &SCustomTable<ItemType>::OnTableColumnResized, ColumnIndex)
			.Resizable(TableHeaderRow->SlotAt(ColumnIndex).CanBeResized())
			.Value(TableHeaderRow->SlotAt(ColumnIndex).GetSizeValue())
			[
				RowWidget
			];
		
	}

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget
		= SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(6.f))
		[
			WidgetRow.ToSharedRef()
		];

	return ListViewRowWidget;
}

template<typename ItemType>
inline void SCustomTable<ItemType>::OnRowMouseButtonDoubleClicked(ItemType ItemIn)
{
	OnTableRowMouseButtonDoubleClicked.Execute(ItemIn);
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
	if (ColumnIndex == 0)
	{
		if(CheckBoxArray.Num() != CheckBoxSelected.Num())
		{
			this->SelectAll();
			return FReply::Handled();
		}

		this->UnselectAll();
		return FReply::Handled();
	}

	NtfyMsg(FString::FromInt(ColumnIndex));

	return FReply::Handled();
}

template<typename ItemType>
inline FReply SCustomTable<ItemType>::OnTestButtonClicked()
{
	NtfyMsg("Test");

	NtfyMsg(FString::FromInt(CheckBoxArray.Num()));
	NtfyMsg(FString::FromInt(CheckBoxSelected.Num()));

	return FReply::Handled();
}
