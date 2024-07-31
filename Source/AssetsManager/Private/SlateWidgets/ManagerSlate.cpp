// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/ManagerSlate.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

void SAssetsCheckerTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 25;

	StoredAssetsData = InArgs._StoredAssetsData;

	ChildSlot
		[
			// main box
			SNew(SVerticalBox)

				// title slot
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
						.Text(FText::FromString(InArgs._TitleText))
						.Font(TitleTextFont)
						.Justification(ETextJustify::Center)
						.ColorAndOpacity(FColor::White)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
				]

				// drop down menu
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					
				]

				// info list
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				[
					SNew(SScrollBox)

					+ SScrollBox::Slot()
					[
						SNew(SListView<TSharedPtr<FAssetData>>)
							.ItemHeight(36.f)
							.ListItemsSource(&StoredAssetsData)
							.OnGenerateRow(this,&SAssetsCheckerTab::OnGenerateRowForlist)
					]
				]

				// action buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
				]
		];
}

TSharedRef<ITableRow> SAssetsCheckerTab::OnGenerateRowForlist(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	FSlateFontInfo ContentTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	ContentTextFont.Size = 9;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget
		= SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			// CheckBox
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.08f)
				[
					ConstructCheckBox(AssetDataToDisplay)
				]
			// DisplayClass
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.25f)
				[
					ConstructAssetClassBox(AssetDataToDisplay, ContentTextFont)
				]
			// DisplayName
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.FillWidth(0.35f)
				[
					ConstructAssetNameBox(AssetDataToDisplay, ContentTextFont)
				]
			// DisplayPath
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					ConstructAssetPathBox(AssetDataToDisplay, ContentTextFont)
				]

			// DisplayButton
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(0.15f)
				[
					ConstructSingleAssetDeleteButtonBox(AssetDataToDisplay)
				]
		];

	return ListViewRowWidget;
}

TSharedRef<SCheckBox> SAssetsCheckerTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> CheckBoxWidget =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this,&SAssetsCheckerTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);

	return CheckBoxWidget;
}

void SAssetsCheckerTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch(NewState)
	{
	case ECheckBoxState::Unchecked:
		break;

	case ECheckBoxState::Checked:
		break;

	case ECheckBoxState::Undetermined:
		break;

	default:
		break;
	}
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetNameBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	TSharedRef<STextBlock> AssetNameBox =
		SNew(STextBlock)
		.Text(FText::FromString(DisplayAssetName))
		.Justification(ETextJustify::Left)
		.ColorAndOpacity(FColor::White)
		.Font(FontInfo);

	return AssetNameBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetClassBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetClass = AssetDataToDisplay->GetClass()->GetName();

	TSharedRef<STextBlock> AssetClassBox =
		SNew(STextBlock)
		.Text(FText::FromString(DisplayAssetClass))
		.Justification(ETextJustify::Left)
		.ColorAndOpacity(FColor::White)
		.Font(FontInfo);

	return AssetClassBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetPathBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetPath = AssetDataToDisplay->ObjectPath.ToString();

	TSharedRef<STextBlock> AssetPathBox =
		SNew(STextBlock)
		.Text(FText::FromString(DisplayAssetPath))
		.Justification(ETextJustify::Left)
		.ColorAndOpacity(FColor::White)
		.Font(FontInfo);

	return AssetPathBox;
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleAssetDeleteButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleAssetDeleteButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center);

	return SingleAssetDeleteButtonBox;
}
