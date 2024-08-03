// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "SlateWidgets/ManagerSlate.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

void SAssetsCheckerTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	FSlateFontInfo TitleTextFont = GetFontInfo(25);

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

				// ToolBar
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
						
					+SHorizontalBox::Slot()
						.FillWidth(0.3f)
						[
							SNew(SComboButton)
						]
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
						ConstructAssetsListView()
					]
				]

				// action buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+SHorizontalBox::Slot()
					.FillWidth(.2f)
					.Padding(5.f)
					[
						ConstructDeleteAllButton()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(.35f)
					.Padding(5.f)
					[
						ConstructDeleteAllSelectedButton()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(.2f)
					.Padding(5.f)
					[
						ConstructSelectAllButton()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(.25f)
					.Padding(5.f)
					[
						ConstructDeselectAllButton()
					]
				]
		];
}
FSlateFontInfo SAssetsCheckerTab::GetFontInfo(float FontSize, const FString& FontName)
{
	FSlateFontInfo font = FCoreStyle::Get().GetFontStyle(FName(FontName));
	font.Size = FontSize;
	return font;
}

#pragma region OnGenerateRowForlist

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructStringRowBox(const FString& StringToDisplay, const FSlateFontInfo& FontInfo, const FColor& FontColor)
{
	TSharedRef<STextBlock> TextBlock
		= SNew(STextBlock).Text(FText::FromString(StringToDisplay))
		.Justification(ETextJustify::Left)
		.ColorAndOpacity(FontColor)
		.Font(FontInfo)
		.ToolTipText(FText::FromString(StringToDisplay));

	return TextBlock;
}



TSharedRef<ITableRow> SAssetsCheckerTab::OnGenerateRowForlist(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	FSlateFontInfo ContentTextFont = GetFontInfo(9);

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget
		= SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(6.f))
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
					ConstructAssetClassRowBox(AssetDataToDisplay, ContentTextFont)
				]
			// DisplayName
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.FillWidth(0.35f)
				[
					ConstructAssetNameRowBox(AssetDataToDisplay, ContentTextFont)
				]
			// DisplayPath
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					ConstructAssetPathRowBox(AssetDataToDisplay, ContentTextFont)
				]

			// DisplayButton
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(0.25f)
				[
					ConstructSingleAssetDeleteButtonBox(AssetDataToDisplay)
				]
		];

	return ListViewRowWidget;
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAssetsCheckerTab::ConstructAssetsListView()
{
	ConstructedAssetsListView = 
	SNew(SListView<TSharedPtr<FAssetData>>)
	.ItemHeight(36.f)
	.ListItemsSource(&StoredAssetsData)
	.OnGenerateRow(this, &SAssetsCheckerTab::OnGenerateRowForlist);
	
	return ConstructedAssetsListView.ToSharedRef();
}

void SAssetsCheckerTab::RefreshAssetsListView()
{
	if(ConstructedAssetsListView.IsValid())
	{
		ConstructedAssetsListView->RebuildList();
	}
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

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetNameRowBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	TSharedRef<STextBlock> AssetNameBox = ConstructStringRowBox(DisplayAssetName, FontInfo);

	return AssetNameBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetClassRowBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetClass = AssetDataToDisplay->GetClass()->GetName();

	TSharedRef<STextBlock> AssetClassBox = ConstructStringRowBox(DisplayAssetClass,FontInfo);

	return AssetClassBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetPathRowBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetPath = AssetDataToDisplay->ObjectPath.ToString();

	TSharedRef<STextBlock> AssetPathBox = ConstructStringRowBox(DisplayAssetPath, FontInfo);

	return AssetPathBox;
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleAssetDeleteButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleAssetDeleteButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,&SAssetsCheckerTab::OnSingleAssetDeleteButtonClicked,AssetDataToDisplay);

	return SingleAssetDeleteButtonBox;
}

FReply SAssetsCheckerTab::OnSingleAssetDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	if (UAssetsChecker::EGetAssetReferencesPath(ClickedAssetData).Num() > 0)
	{
		EAppReturnType::Type result = DlgMsg(EAppMsgType::OkCancel, ClickedAssetData->AssetName.ToString() + " was referenced.\n\nConfirm to delete this asset?");
		
		if (result != EAppReturnType::Ok)
		{
			return FReply::Handled();
		}

		NtfMsgLog("Clicked OK");
	}

	if (UAssetsChecker::EDeleteAsset(*ClickedAssetData))
	{
		// log
		NtfMsgLog("Successfully deleted " + ClickedAssetData->AssetName.ToString());

		// update slistview
		if (StoredAssetsData.Contains(ClickedAssetData))
		{
			StoredAssetsData.Remove(ClickedAssetData);
		}

		RefreshAssetsListView();
	};

	return FReply::Handled();
}

#pragma endregion

TSharedRef<SButton> SAssetsCheckerTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> DeleteAllButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnDeleteAllButtonClicked)
		.ContentPadding(FMargin(5.f));
		
	DeleteAllButton->SetContent(ConstructTextForButtons("Delete All"));

	return DeleteAllButton;
}

FReply SAssetsCheckerTab::OnDeleteAllButtonClicked()
{
	return FReply::Handled();
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructDeleteAllSelectedButton()
{
	TSharedRef<SButton> DeleteAllSelectedButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnDeleteAllSelectedButtonClicked)
		.ContentPadding(FMargin(5.f));
	
	DeleteAllSelectedButton->SetContent(ConstructTextForButtons("Delete All Selected"));

	return DeleteAllSelectedButton;
}

FReply SAssetsCheckerTab::OnDeleteAllSelectedButtonClicked()
{
	return FReply::Handled();
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> SelectAllButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnSelectAllButtonClicked)
		.ContentPadding(FMargin(5.f));

	SelectAllButton->SetContent(ConstructTextForButtons("Select All"));

	return SelectAllButton;
}

FReply SAssetsCheckerTab::OnSelectAllButtonClicked()
{
	return FReply::Handled();
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> DeselectAllButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnDeselectAllButtonClicked)
		.ContentPadding(FMargin(5.f));

	DeselectAllButton->SetContent(ConstructTextForButtons("Deselect All"));

	return DeselectAllButton;
}

FReply SAssetsCheckerTab::OnDeselectAllButtonClicked()
{
	return FReply::Handled();
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructTextForButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetFontInfo(15);

	TSharedRef<STextBlock> ContentBlock = 
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(ButtonTextFont)
		.Justification(ETextJustify::Center);

	return ContentBlock;
}