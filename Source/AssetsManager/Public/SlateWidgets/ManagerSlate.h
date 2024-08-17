// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define CONTENTFOLDERMANAGERTABNAME "AssetsManager"

#include "SlateWidgets/SCommonSlate.h"

class ASSETSMANAGER_API SManagerSlateTab: public SCommonSlate
{
	SLATE_BEGIN_ARGS(SManagerSlateTab){}
	SLATE_ARGUMENT(FString,TitleText)
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, StoredAssetsData)
	SLATE_ARGUMENT(TArray<FString>, SelectedFolderPaths)
	SLATE_END_ARGS()

public:
	virtual void Construct(const FArguments& InArgs);

protected:

private:
	TArray<FString> StoredFolderPaths;
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;
	TArray<TSharedPtr<FAssetData>> SListViewClassFilterAssetData;
	TArray<TSharedPtr<FAssetData>> SListViewAssetData;

	TArray<TSharedRef<SCheckBox>>  CheckBoxesArray;
	TArray<TSharedPtr<FAssetData>> AssetsDataSelected;

	enum ClassCheckState
	{
		DefaultClassCheckState = 0,
		Texture
	};

	ClassCheckState m_ClassCheckState;

	enum UsageCheckState
	{
		DefaultUsageCheckState = 0,
		Unused,
		// for texture,
		MaxInGameSizeError, SourceSizeError,SubfixError,TextureSettingsError,
		// for prefix
		PrefixError,
		//for same name asset,
		SameNameAssetError
	};

	UsageCheckState m_UsageCheckState;

	void SListViewRemoveAssetData(TSharedPtr<FAssetData> AssetData);

#pragma region OnGenerateRowForlist
	// construct / refresh list view
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetsListView();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetsListView;
	void RefreshAssetsListView();

	// For Assets List
	TSharedRef<ITableRow> OnGenerateRowForlist(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable);

	// For List item button clicked
	void OnRowMouseButtonDoubleClicked(
		TSharedPtr<FAssetData> AssetDataToDisplay);

	// CheckState = Default
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> GenerateDefaultRowForList(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable);

	// CheckState = Texture
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> GenerateTextureRowForList_MaxInGameSizeError(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> GenerateTextureRowForList_SourceSizeError(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> GenerateTextureRowForList_SettingsError(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable);

#pragma region ConstructAssetInfo

	// Construct CheckBox
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

#pragma endregion

#pragma region ConstructAssetInfo
	
	// Construct Assets List info 
	TSharedRef<SVerticalBox> ConstructInfoBox(
		const TArray<FString> & FolderPaths,
		const FSlateFontInfo& FontInfo);

	TSharedRef<STextBlock> ConstructListPathsInfo(
		const TArray<FString>& FolderPaths,
		const FSlateFontInfo& FontInfo);

	TSharedRef<SHorizontalBox> ConstructListAssetsCountInfo(
		const FSlateFontInfo& FontInfo);
	TSharedPtr<STextBlock> ListViewCountBlock;
	TSharedPtr<STextBlock> ClassListViewCountBlock;
	TSharedPtr<STextBlock> SelectedCountBlock;

	// Construct Standard Name Box
	TSharedRef<STextBlock> ConstructAssetNameRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo & FontInfo);

	// Construct Standard Class Box
	TSharedRef<STextBlock> ConstructAssetClassRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay, 
		const FSlateFontInfo& FontInfo);
	
	// Construct Standard Texture Size
	TSharedRef<STextBlock> ConstructAssetTextureSizeRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	TSharedRef<STextBlock> ConstructAssetTextureCompressionSettingsRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	TSharedRef<STextBlock> ConstructAssetTextureSRGBRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

#pragma endregion

#pragma region ConstructSingleButton
	// Construct Delete Button
	TSharedRef<SButton> ConstructSingleAssetDeleteButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
	
	// Construct Debug Button
	TSharedRef<SButton> ConstructSingleAssetDebugButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetDebugButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
	
	//Construct Fix Button
	TSharedRef<SButton> ConstructSingleAssetReimportButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetReimportButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

	//Construct Texture 2K restrict Button
	TSharedRef<SButton> ConstructSingleTextureAsset2KButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAsset2KButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

	//Construct Texture 1K restrict Button
	TSharedRef<SButton> ConstructSingleTextureAsset1KButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAsset1KButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

	//Construct Texture 512 restrict Button
	TSharedRef<SButton> ConstructSingleTextureAsset512ButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAsset512ButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
	
	//Construct Texture Reset Button
	TSharedRef<SButton> ConstructSingleTextureAssetResetButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAssetResetButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

	//Construct Texture Reset Button
	TSharedRef<SButton> ConstructSingleTextureAssetSettingsFixButtonBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAssetSettingsFixButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

#pragma endregion

#pragma endregion

#pragma region GenerateHandleAllButton

	TSharedRef<SVerticalBox> ConstructHandleAllButtons();

#pragma endregion

#pragma region DeleteAllSelectedButton
	TSharedRef<SButton> ConstructDeleteAllSelectedButton();
	FReply OnDeleteAllSelectedButtonClicked();
#pragma endregion

#pragma region De/SelectAllButton
	TSharedRef<SButton> ConstructSelectAllButton();
	FReply OnSelectAllButtonClicked();

	TSharedRef<SButton> ConstructDeselectAllButton();
	FReply OnDeselectAllButtonClicked();
#pragma endregion
	
#pragma region FixAllSelectedButton
	TSharedRef<SButton> ConstructFixSelectedButton();
	FReply OnSelectFixSelectedClicked();
#pragma endregion

#pragma region FixRedirectorsButton
	TSharedRef<SButton> ConstructFixUpRedirectorButton();
	FReply OnFixUpRedirectorButtonClicked();
#pragma endregion

#pragma region OutputViewListInfo
	TSharedRef<SButton> ConstructOutputViewListInfoButton();
	FReply OnOutputViewListInfoButtonClicked();
#pragma endregion

#pragma region SComboListFilter

	TSharedRef<SHorizontalBox> ConstructDropDownMenuBox();

	//------> ClassFilter
	TArray<TSharedPtr<FString>> ClassFilterComboSourceItems;
	TSharedPtr<STextBlock> ClassFilterComboDisplayText;
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructClassFilterButton();
	TSharedRef<SWidget> OnGenerateClassFilterButton(TSharedPtr<FString> SourceItem);
	void ConstuctClassFilterList(TSharedPtr<FString> SelectedOption);
	void OnClassFilterButtonChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	
	TSharedPtr<FString> ClassFilterCurrent;
	TSharedPtr<FString> ClassFilterDefault;

	TSharedPtr<SComboBox<TSharedPtr<FString>>> ClassFilterComboBox;

	//------> UsageFilter
	TArray<TSharedPtr<FString>> UsageFilterComboSourceItems;
	TSharedPtr<STextBlock> UsageFilterComboDisplayText;
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructUsageFilterButton();
	TSharedRef<SWidget> OnGenerateUsageFilterButton(TSharedPtr<FString> SourceItem);
	void OnUsageFilterButtonChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	
	TSharedPtr<SComboBox<TSharedPtr<FString>>> UsageFilterComboBox;
	
	//------> UsageFilter DefaultStore
	TSharedPtr<FString> UsageSelectedDefault;
	
	// only for texture
	TSharedPtr<FString> UsageSelectionMaxInGameSizeError;
	TSharedPtr<FString> UsageSelectionSourceSizeError;
	TSharedPtr<FString> UsageSelectionSubfixError;
	TSharedPtr<FString> UsageSelectionTextureSettinsError;
	
#pragma endregion

};