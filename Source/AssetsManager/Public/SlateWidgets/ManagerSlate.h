// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	// For Assets List
	TSharedRef<ITableRow> OnGenerateRowForlist(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase> & OwnerTable);
	
	// CheckBox Config
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	// Asset Info Config
	TSharedRef<STextBlock> ConstructAssetNameBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo & FontInfo);
	TSharedRef<STextBlock> ConstructAssetClassBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo);
	TSharedRef<STextBlock> ConstructAssetPathBox(const TSharedPtr<FAssetData>& AssetDataToDisplay, const FSlateFontInfo& FontInfo);

	// Button Config
	TSharedRef<SButton> ConstructSingleAssetDeleteButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
};