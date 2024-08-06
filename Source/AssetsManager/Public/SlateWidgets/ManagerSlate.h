// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define CONTENTFOLDERMANAGERTABNAME "AssetsManager"

#include "Widgets/SCompoundWidget.h"

class SAssetsCheckerTab: public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAssetsCheckerTab){}
	SLATE_ARGUMENT(FString,TitleText)
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, StoredAssetsData)
	SLATE_ARGUMENT(TArray<FString>, SelectedFolderPaths)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TArray<FString> StoredFolderPaths;
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;
	TArray<TSharedPtr<FAssetData>> SListViewAssetData;
	TArray<TSharedRef<SCheckBox>>  CheckBoxesArray;
	TArray<TSharedPtr<FAssetData>> AssetsDataSelected;

	TArray<TSharedPtr<FAssetData>> SListViewAssetData;

	FSlateFontInfo GetFontInfo(float FontSize, const FString& FontName = "EmbossedText");

#pragma region OnGenerateRowForlist
	// construct / refresh list view
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetsListView();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetsListView;
	void RefreshAssetsListView();

	// For Assets List
	TSharedRef<ITableRow> OnGenerateRowForlist(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);

	// CheckBox Config
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	// Asset Info Config
	TSharedRef<STextBlock> ConstructStringRowBox(const FString& StringToDisplay, const FSlateFontInfo& FontInfo, const FColor& FontColor = FColor::White);

	TSharedRef<STextBlock> ConstructAssetNameRowBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo & FontInfo);
	TSharedRef<STextBlock> ConstructAssetClassRowBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo);
	TSharedRef<STextBlock> ConstructAssetPathRowBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo);

	// Button Config
	TSharedRef<SButton> ConstructSingleAssetDeleteButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
#pragma endregion

#pragma region DeleteButtonCustruct
	TSharedRef<SButton> ConstructDeleteAllSelectedButton();
	FReply OnDeleteAllSelectedButtonClicked();
#pragma endregion

#pragma region SelectButtonCustruct
	TSharedRef<SButton> ConstructSelectAllButton();
	FReply OnSelectAllButtonClicked();

	TSharedRef<SButton> ConstructDeselectAllButton();
	FReply OnDeselectAllButtonClicked();
#pragma endregion

#pragma region FixRedirectorsButton
	TSharedRef<SButton> ConstructFixUpRedirectorButton();
	FReply OnFixUpRedirectorButtonClicked();
#pragma endregion

#pragma region ButtonCustruct
	TSharedRef<STextBlock> ConstructTextForButtons(const FString& TextContent);
#pragma endregion

#pragma region SComboListFilter
	TArray<TSharedPtr<FString>> ClassFilterComboSourceItems;
	TSharedPtr<STextBlock> ClassFilterComboDisplayText;
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructClassFilterButton();
	TSharedRef<SWidget> OnGenerateClassFilterButton(TSharedPtr<FString> SourceItem);
	void OnClassFilterButtonChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);

	TArray<TSharedPtr<FString>> UsageFilterComboSourceItems;
	TSharedPtr<STextBlock> UsageFilterComboDisplayText;
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructUsageFilterButton();
	TSharedRef<SWidget> OnGenerateUsageFilterButton(TSharedPtr<FString> SourceItem);
	void OnUsageFilterButtonChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);

	TSharedPtr<FString> UsageSelectedDefault;
	TSharedPtr<FString> UsageSelectedCurrent;
#pragma endregion
};