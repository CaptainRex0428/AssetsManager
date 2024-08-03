// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define CONTENTFOLDERMANAGERTABNAME "AssetsManager"

#include "Widgets/SCompoundWidget.h"

class SAssetsCheckerTab: public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAssetsCheckerTab){}
	SLATE_ARGUMENT(FString,TitleText)
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, StoredAssetsData)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;

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

	TSharedRef<SButton> ConstructDeleteAllButton();
	FReply OnDeleteAllButtonClicked();

	TSharedRef<SButton> ConstructDeleteAllSelectedButton();
	FReply OnDeleteAllSelectedButtonClicked();

	TSharedRef<SButton> ConstructSelectAllButton();
	FReply OnSelectAllButtonClicked();

	TSharedRef<SButton> ConstructDeselectAllButton();
	FReply OnDeselectAllButtonClicked();

	TSharedRef<STextBlock> ConstructTextForButtons(const FString& TextContent);
};