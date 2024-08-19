
#include "SlateWidgets/SCustomTable.h"

template <typename ItemType>
void SCustomTable<ItemType>::Construct(const SCustomTable<ItemType>::FArguments& InArgs)
{
	bCanSupportFocus = true;

	MainTable = SNew(SVerticalBox);

	this->TitleColumns = InArgs._TitleColumns;
	this->ColumnsNum = this->TitleColumns->Num();
	this->ColumnsInitWidth = InArgs._ColumnsInitWidth;

	// this->OnGenerateRowList = InArgs._OnGenerateRowList;
	// this->OnGenerateHeaderRow = InArgs._OnGenerateHeaderRow;
	// this->OnMouseButtonDoubleClickListRow = InArgs._OnMouseButtonDoubleClickListRow;

	// Construct Header
	SplitterRows->Empty();
	ConstructHeaderRow();

	// Construct Rows
	FScrollBarStyle scrollbarStyle;
	scrollbarStyle.SetThickness(30);

	this->ListView = SNew(SListView<ItemType>)
	.ItemHeight(36.f)
	.ScrollBarStyle(&scrollbarStyle)
	.ListItemsSource(&InArgs._ListSourceItems)
	.OnGenerateRow(
		this, 
		&SCustomTable<ItemType>::OnGenerateRowForlist)
	.OnMouseButtonDoubleClick(
		this,
		&SCustomTable<ItemType>::OnMouseButtonDoubleClickListRow);

	ChildSlot
	[
		MainTable.ToSharedRef()
	];
}

template<typename ItemType>
void SCustomTable<ItemType>::Rebuilt()
{
	SplitterRows->Empty();

	ConstructHeaderRow();	
	ListView->RebuildList();
}

template<typename ItemType>
float SCustomTable<ItemType>::GetColumnInitWidth(uint32 index)
{
	if (index < ColumnsInitWidth->Num())
	{
		return ColumnsInitWidth[index];
	}
	return 0.5f;
}

template<typename ItemType>
float SCustomTable<ItemType>::GetColumnWidth(uint32 index)
{
	if (index < ColumnsNum)
	{
		return this->HeaderRow->SlotAt(index).GetSizeValue();
	}

	return 0.0f;
}

template<typename ItemType>
void SCustomTable<ItemType>::OnSplitterColumResized(
	float size,
	uint32 index)
{
	for(TSharedPtr<SSplitter> Splitter : *SplitterRows)
	{
		this->Splitter->SlotAt(index).SetSizeValue(size);
	}
}

template<typename ItemType>
TSharedRef<ITableRow> SCustomTable<ItemType>::OnGenerateRowForlist(
	ItemType ItemData, 
	const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedPtr<SSplitter> SplitterRow = OnGenerateRowList(
		ItemData, 
		this->ColumnsNum,
		this->ColumnsInitWidth,
		this->OnSlotResizedRows,
		this->SplitterRows);

	SplitterRows->Add(SplitterRow);

	TSharedRef<STableRow<ItemType>> ListViewRowWidget
		= SNew(STableRow<ItemType>, OwnerTable)
		.Padding(FMargin(6.f))
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			[
				SplitterRow.ToSharedRef()
			]
		];

	return ListViewRowWidget;
}

template<typename ItemType>
void SCustomTable<ItemType>::ConstructHeaderRow()
{
	this->HeaderRow = SNew(SSplitter)
		.Orientation(Orient_Horizontal);
	this->SplitterRows->Add(HeaderRow);

	this->ColumnsNum = this->TitleColumns->Num();

	for (uint32 i; i < ColumnsNum; i++)
	{
		HeaderRow->AddSlot()
			.OnSlotResized(this, &SCustomTable<ItemType>::OnSlotResizedRows, i, SplitterRows)
			.Value(GetColumnInitWidth(i))
			[
				SNew(SOverlay)
					+ SOverlay::Slot()
					.Padding(1.f, 2.f, 1.f, 3.f)
					[
						ConstructTitleTextBlock(
							(*TitleColumns)[i],
							GetFontInfo(10))
					]
			];
	}
}
