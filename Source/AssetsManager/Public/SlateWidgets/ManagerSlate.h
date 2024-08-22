// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define CONTENTFOLDERMANAGERTABNAME "AssetsManager"

#include "SlateWidgets/SCommonSlate.h"
#include "SlateWidgets/SCustomTable.h"

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

	TArray<CustomTableColumnType> SManagerCustomTableTitleRowColumnsType;
	TArray<float> SManagerCustomTableTitleRowColumnsInitWidth;

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
	
	TSharedPtr<SCustomTable<TSharedPtr<FAssetData>>> CustomTableList;

	void ConstructHeaderRow();

	TArray<TSharedPtr<SWidget>> OnConstructTableRow(
		TSharedPtr<FAssetData> AssetToDisplay);

	TSharedRef<SHorizontalBox> ConstructSingleDealPanel(
		TSharedPtr<FAssetData> ClickedAssetData);

	void OnTableCheckBoxStateChanged();

	void OnRowMouseButtonDoubleClicked(
		TSharedPtr<FAssetData> AssetDataToDisplay);

	void RefreshAssetsListView();

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
	TSharedRef<SEditableTextBox> ConstructEditAssetNameRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	void OnEditableAssetNameRowBoxCommitted(
		const FText& NewText, 
		ETextCommit::Type CommitType,
		const TSharedPtr<FAssetData>& AssetDataToDisplay) const;

	TSharedRef<STextBlock> ConstructAssetNameRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

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

	TSharedPtr<SVerticalBox> HandleAllBox;
	TSharedRef<SVerticalBox> ConstructHandleAllBox();

	TSharedPtr<SHorizontalBox> DynamicHandleAllBox;
	TSharedRef<SHorizontalBox> ConstructDynamicHandleAllBox();

#pragma endregion

#pragma region DeleteAllSelectedButton
	TSharedPtr<SButton> DeleteAllSelectedButton;
	TSharedRef<SButton> ConstructDeleteAllSelectedButton();
	FReply OnDeleteAllSelectedButtonClicked();
#pragma endregion

#pragma region De/SelectAllButton
	TSharedPtr<SButton> SelectAllButton;
	TSharedRef<SButton> ConstructSelectAllButton();
	FReply OnSelectAllButtonClicked();

	TSharedPtr<SButton> UnselectAllButton;
	TSharedRef<SButton> ConstructDeselectAllButton();
	FReply OnDeselectAllButtonClicked();
#pragma endregion
	
#pragma region FixAllSelectedButton
	TSharedPtr<SButton> FixSelectedButton;
	TSharedRef<SButton> ConstructFixSelectedButton();
	FReply OnSelectFixSelectedClicked();
#pragma endregion

#pragma region FixRedirectorsButton
	TSharedPtr<SButton> FixUpRedirectorButton;
	TSharedRef<SButton> ConstructFixUpRedirectorButton();
	FReply OnFixUpRedirectorButtonClicked();
#pragma endregion

#pragma region OutputViewListInfo
	TSharedPtr<SButton> OutputViewListInfoButton;
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