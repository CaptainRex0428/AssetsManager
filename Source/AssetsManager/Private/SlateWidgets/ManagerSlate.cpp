// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "SlateWidgets/ManagerSlate.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

#pragma region ClassFilterComboSourceItems
#define CLASS_LISTALL TEXT("All")
#define CLASS_BLUEPRINT TEXT("Blueprint")
#define CLASS_STATICMESH TEXT("StaticMesh")
#define CLASS_MATERIAL TEXT("Material")
#define CLASS_MATERIALINSTSNCECONST TEXT("MaterialInstance")
#define CLASS_MATERIALFUNCTION TEXT("MaterialFunction")
#define CLASS_PARTICLESYSTEM TEXT("ParticleSystem")
#define CLASS_SOUNDCUE TEXT("SoundCue")
#define CLASS_SOUNDWAVE TEXT("SoundWave")
#define CLASS_TEXTURE TEXT("Texture")
#define CLASS_TEXTUREARRAY TEXT("TextureArray")
#define CLASS_USERWIDGET TEXT("UserWidget")
#define CLASS_SKELETALMESH TEXT("SkeletalMesh")
#define CLASS_NIAGARASYSTEM TEXT("NiagaraSystem")
#define CLASS_NIAGARAEMITTER TEXT("NiagaraEmitter")
#pragma endregion

#pragma region UsageFilterComboSourceItems
#define USAGE_NONE TEXT("None")
#define USAGE_UNUSED TEXT("Unused")
#define USAGE_SIZEERROR TEXT("SizeError")
#define USAGE_PREFIXERROR TEXT("PrefixError")
#pragma endregion

void SAssetsCheckerTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	FSlateFontInfo TitleTextFont = GetFontInfo(25);

	StoredFolderPaths = InArgs._SelectedFolderPaths;
	StoredAssetsData = InArgs._StoredAssetsData;
	CheckBoxesArray.Empty();
	AssetsDataSelected.Empty();

	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_LISTALL));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_BLUEPRINT));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_STATICMESH));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_MATERIAL));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_MATERIALINSTSNCECONST));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_MATERIALFUNCTION));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_PARTICLESYSTEM));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_SOUNDCUE));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_SOUNDWAVE));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_TEXTURE));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_TEXTUREARRAY));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_USERWIDGET));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_SKELETALMESH));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_NIAGARASYSTEM));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_NIAGARAEMITTER));

	UsageSelectedDefault = MakeShared<FString>(USAGE_NONE);
	UsageFilterComboSourceItems.Add(UsageSelectedDefault);
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_UNUSED));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_PREFIXERROR));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_SIZEERROR));

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
				]

				// drop down menu
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
						
					+SHorizontalBox::Slot()
					.FillContentWidth(.25f)
					.Padding(FMargin(2.f))
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Font(GetFontInfo(12))
							.Text(FText::FromString("Class Filter:"))
							.Justification(ETextJustify::Right)
							.ColorAndOpacity(FColor::White)
						]

						+SHorizontalBox::Slot()
						.FillWidth(.5f)
						[
							ConstructClassFilterButton()
						]
					]

					+ SHorizontalBox::Slot()
					.FillContentWidth(.25f)
					.Padding(FMargin(2.f))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Font(GetFontInfo(12))
							.Text(FText::FromString("Check Filter:"))
							.Justification(ETextJustify::Right)
							.ColorAndOpacity(FColor::White)
						]

						+ SHorizontalBox::Slot()
						.FillWidth(.5f)
						[
							ConstructUsageFilterButton()
						]

					]
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

				// select buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

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

				// Handle Buttons
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.FillWidth(.35f)
						.Padding(5.f)
						[
							ConstructDeleteAllSelectedButton()
						]
				]

				// Fix Buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.FillWidth(.5f)
						.Padding(5.f)
						[
							ConstructFixUpRedirectorButton()
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
	CheckBoxesArray.Empty();
	AssetsDataSelected.Empty();

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

	CheckBoxesArray.AddUnique(CheckBoxWidget);

	return CheckBoxWidget;
}

void SAssetsCheckerTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch(NewState)
	{
	case ECheckBoxState::Unchecked:
		if (AssetsDataSelected.Contains(AssetData))
		{
			AssetsDataSelected.Remove(AssetData);
		}
		break;

	case ECheckBoxState::Checked:
		AssetsDataSelected.AddUnique(AssetData);
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
	const FString DisplayAssetPath = AssetDataToDisplay->GetSoftObjectPath().ToString();

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

#pragma region DeleteButtonCustruct
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
	if (AssetsDataSelected.Num() == 0)
	{
		return FReply::Handled();
	}

	TArray<FAssetData> AssetsDataToDelete;

	for (TSharedPtr<FAssetData> AssetDataPtr : AssetsDataSelected)
	{
		AssetsDataToDelete.Add(*AssetDataPtr.Get());
	}

	if (UAssetsChecker::EDeleteAssets(AssetsDataToDelete))
	{
		for (TSharedPtr<FAssetData> AssetDataPtr : AssetsDataSelected)
		{
			if (StoredAssetsData.Contains(AssetDataPtr))
			{
				StoredAssetsData.Remove(AssetDataPtr);
			}
		}
	}
	
	RefreshAssetsListView();

	return FReply::Handled();
}
#pragma endregion

#pragma region SelectButtonCustruct
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
	if (CheckBoxesArray.Num() == 0)
	{
		return FReply::Handled();
	}

	for (const TSharedRef<SCheckBox> CheckBox : CheckBoxesArray)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

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
	if (CheckBoxesArray.Num() == 0)
	{
		return FReply::Handled();
	}

	for (const TSharedRef<SCheckBox> CheckBox : CheckBoxesArray)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	return FReply::Handled();
}
#pragma endregion

#pragma region FixRedirectorsButton
TSharedRef<SButton> SAssetsCheckerTab::ConstructFixUpRedirectorButton()
{
	TSharedRef<SButton> FixUpRedirectorButton = 
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnFixUpRedirectorButtonClicked)
		.ContentPadding(FMargin(5.f));
	
	FixUpRedirectorButton->SetContent(ConstructTextForButtons("Fix Up Redirectors In Selected Folders"));

	return FixUpRedirectorButton;
}

FReply SAssetsCheckerTab::OnFixUpRedirectorButtonClicked()
{
	UAssetsChecker::EFixUpRedirectors(StoredFolderPaths);
	return FReply::Handled();
}

#pragma endregion

#pragma region ButtonCustruct
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
#pragma endregion

#pragma region SComboListFilter

TSharedRef<SComboBox<TSharedPtr<FString>>> SAssetsCheckerTab::ConstructClassFilterButton()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> ClassFilterButton =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ClassFilterComboSourceItems)
		.OnGenerateWidget(this, &SAssetsCheckerTab::OnGenerateClassFilterButton)
		.OnSelectionChanged(this, &SAssetsCheckerTab::OnClassFilterButtonChanged)
		[
			SAssignNew(ClassFilterComboDisplayText, STextBlock)
				.Text(FText::FromString(CLASS_LISTALL))
		];
	
	return ClassFilterButton;
}

TSharedRef<SWidget> SAssetsCheckerTab::OnGenerateClassFilterButton(TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SAssetsCheckerTab::OnClassFilterButtonChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	ClassFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));
	UsageFilterComboDisplayText->SetText(FText::FromString(USAGE_NONE));
	UsageSelectedCurrent = UsageSelectedDefault;
}


TSharedRef<SComboBox<TSharedPtr<FString>>> SAssetsCheckerTab::ConstructUsageFilterButton()
{
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> UsageFilterButton =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&UsageFilterComboSourceItems)
		.OnGenerateWidget(this,&SAssetsCheckerTab::OnGenerateUsageFilterButton)
		.OnSelectionChanged(this,&SAssetsCheckerTab::OnUsageFilterButtonChanged)
		[
			SAssignNew(UsageFilterComboDisplayText,STextBlock)
				.Text(FText::FromString(USAGE_NONE))
		];

	return UsageFilterButton;
}

TSharedRef<SWidget> SAssetsCheckerTab::OnGenerateUsageFilterButton(TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SAssetsCheckerTab::OnUsageFilterButtonChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	UsageFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));
	UsageSelectedCurrent = SelectedOption;
}


#pragma endregion