// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateWidgets/SCommonSlate.h"
#include "SlateWidgets/SCustomTable.h"
#include "SlateWidgets/SCustomEditableText.h"

#include "StandardAsset/CustomStandardAsset.h"

#define CONTENTFOLDER_MANAGERTAB_NAME "AssetsManager"

class ASSETSMANAGER_API SManagerSlateTab: public SCommonSlate
{
	SLATE_BEGIN_ARGS(SManagerSlateTab){}
	SLATE_ARGUMENT(FString,TitleText)
	SLATE_ARGUMENT(TArray<FString>, SelectedFolderPaths)
	SLATE_END_ARGS()

private:

	enum ClassCheckState
	{
		DefaultClassCheckState = 0,
		Texture,
		Material,
		MaterialConst,
		SkeletalMesh
	};

	enum UsageCheckState
	{
		DefaultUsageCheckState = 0,
		Unused,
		// for texture,
		MaxInGameSizeError, SourceSizeError,
		SubfixError, TextureSettingsError, TextureGroupError,
		// for prefix
		PrefixError,
		//for same name asset,
		SameNameAssetError
	};

public:
	virtual void Construct(const FArguments& InArgs);

	void RegistryTab();

private:

#pragma region ManagerSlateStateData
	
	TArray<FString> StoredFolderPaths;
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;
	TArray<TSharedPtr<FAssetData>> SListViewClassFilterAssetData;
	TArray<TSharedPtr<FAssetData>> SListViewCategoryFilterAssetData;
	TArray<TSharedPtr<FAssetData>> SListViewUsageFilterAssetData;

	TArray<TSharedPtr<FAssetData>> SListViewAssetData;

	TArray<CustomTableColumnType> SManagerCustomTableTitleRowColumnsType;

	ClassCheckState m_ClassCheckState;
	AssetCategory m_CategoryCheckState;
	UsageCheckState m_UsageCheckState;

	TSharedPtr<SCustomTable<TSharedPtr<FAssetData>>> CustomTableList;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> CustomTableListView;

	TSharedPtr<SHorizontalBox> DropDownContent;

#pragma endregion


#pragma region ConstructListRow

	void ConstructHeaderRow();

	TSharedRef<SHeaderRow> OnTableGenerateHeaderRow(
		TSharedPtr<SHeaderRow>& TableHeaderRow);

	TSharedRef<SWidget> OnTableGenerateListColumn(
		const FName& ColumnName,
		TSharedPtr<FAssetData>& AssetToDisplay);


	void OnTableCheckBoxStateChanged();

	void OnRowMouseButtonDoubleClicked(
		TSharedPtr<FAssetData>& AssetDataToDisplay);


	void SListViewRemoveAssetData(
		TSharedPtr<FAssetData> AssetData);

	void RefreshAssetsListView(
		bool bRefreshTableHeader = true);


#pragma region ConstructManagerSlateStateInfo

	/*
	Construct Manager Slate State Info Box
	*/
	TSharedRef<SVerticalBox> ConstructInfoBox(
		const TArray<FString> & FolderPaths,
		const FSlateFontInfo& FontInfo);

	/*
	Construct Manager Slate Paths Included Info
	*/
	TSharedRef<STextBlock> ConstructListPathsInfo(
		const TArray<FString>& FolderPaths,
		const FSlateFontInfo& FontInfo);

	/*
	Construct Manager Slate Paths Included Info
	*/
	TSharedRef<SHorizontalBox> ConstructListAssetsCountInfo(
		const FSlateFontInfo& FontInfo);

	TSharedPtr<STextBlock> AllInFolderViewCountBlock;
	TSharedPtr<STextBlock> ListViewCountBlock;
	TSharedPtr<STextBlock> ClassListViewCountBlock;
	TSharedPtr<STextBlock> SelectedCountBlock;

#pragma endregion


#pragma region ConstructAssetInfo
	/* 
	Construct standard editable name info box
	*/
	TSharedRef<SHorizontalBox> ConstructEditAssetNameRowBox(
		TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	FText OnAssetDataToAssetNameEditableText(
		TSharedPtr<FAssetData>& AssetDataToDisplay);

	FText OnAssetDataToAssetEditableTextTip(
		TSharedPtr<FAssetData>& AssetDataToDisplay);

	bool OnItemDataCommitted(
		const FText& TextIn,
		ETextCommit::Type CommitType,
		TSharedPtr<FAssetData>& AssetDataToDisplay);


	/*
	Construct standard name info box
	*/
	TSharedRef<SHorizontalBox> ConstructAssetNameRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	Construct standard class info box
	*/
	TSharedRef<SHorizontalBox> ConstructAssetClassRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay, 
		const FSlateFontInfo& FontInfo);
	
	/*
	Construct standard texture size info box
	*/
	TSharedRef<STextBlock> ConstructAssetTextureSizeRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	Construct standard texture Size Box
	*/
	TSharedRef<STextBlock> ConstructAssetTextureCompressionSettingsRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	Construct standard texture sRGB info box
	*/
	TSharedRef<STextBlock> ConstructAssetTextureSRGBRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	Construct standard texture LODGroup info box
	*/
	TSharedRef<STextBlock> ConstructAssetTextureLODGroupRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	* Construct disk size info box
	*/
	TSharedRef<SHorizontalBox> ConstructAssetDiskSizeRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	* Construct memory size info box
	*/
	TSharedRef<SHorizontalBox> ConstructAssetMemorySizeRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	* Construct SkeletalMesh LODNumBox
	*/
	TSharedRef<SHorizontalBox> ConstructSkeletalMeshLODNumRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);

	/*
	* Construct SkeletalMesh Vertices
	*/
	TSharedRef<SVerticalBox> ConstructSkeletalMeshVerticesNumRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo,
		bool bStricWithCategory = false);

	/*
	* Construct SkeletalMesh LODNumBox
	*/
	TSharedRef<SVerticalBox> ConstructSkeletalMeshTrianglesNumRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo,
		bool bStricWithCategory = false);

	/*
	* Construct SkeletalMesh LOD AllowCPUAccessInfoBox
	*/
	TSharedRef<SVerticalBox> ConstructSkeletalMeshLODAllowCPUAccessRowBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay,
		const FSlateFontInfo& FontInfo);



#pragma endregion


#pragma region ConstructSingleHandlerButton

	/* 
	Construct handling buttons for any single item
	This is the main construct for single handler
	It is constructed by the state of checker
	*/
	TSharedRef<SHorizontalBox> ConstructSingleDealPanel(
		const TSharedPtr<FAssetData>& ClickedAssetData);

	/*
	Construct Debug Button
	*/
	TSharedRef<SButton> ConstructSingleAssetDebugButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetDebugButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	TSharedRef<SButton> ConstructSingleAssetEditButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetEditButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	TSharedRef<SButton> ConstructSingleAssetSizeMapButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetSizeMapButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	/*
	Construct Delete Button
	To delete the asset referenced in table row
	If the asset is referenced by other assets, it will call dialog message to confirm
	*/
	TSharedRef<SButton> ConstructSingleAssetDeleteButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetDeleteButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);
	
	/*
	Construct texture reimporting button
	*/
	TSharedRef<SButton> ConstructSingleAssetReimportButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetReimportButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	TSharedRef<SButton> ConstructSingleAssetResizeTextureSourceButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleAssetResizeTextureSourceButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	/*
	Construct texture 2K restricting button
	*/
	TSharedRef<SButton> ConstructSingleTextureAsset2KButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAsset2KButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	/*
	Construct texture 1K restricting button
	*/
	TSharedRef<SButton> ConstructSingleTextureAsset1KButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAsset1KButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	/*
	Construct texture 512 restricting button
	*/
	TSharedRef<SButton> ConstructSingleTextureAsset512ButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAsset512ButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	/*
	Construct texture MaxInGameSize Reset Button
	It will set the max in game size value of the texture to 0 (default, no limit)
	*/
	TSharedRef<SButton> ConstructSingleTextureAssetResetButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAssetResetButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	
	/*
	Construct texture settings fix button
	Automatic fix the texture asset's compression settings & sRGB by standard rules
	*/
	TSharedRef<SButton> ConstructSingleTextureAssetSettingsFixButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureAssetSettingsFixButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

	/*
	Construct texture LODGroup fix button
	Automatic fix the texture asset's LOD group by standard rules
	*/
	TSharedRef<SButton> ConstructSingleTextureLODGroupStandardFixButtonBox(
		const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnSingleTextureLODGroupStandardFixButtonClicked(
		TSharedPtr<FAssetData> ClickedAssetData);

#pragma endregion

#pragma endregion


#pragma region GenerateHandleAllButton

	TSharedPtr<SVerticalBox> BatchHandleBox;
	TSharedRef<SVerticalBox> ConstructHandleAllBox();

	TSharedPtr<SHorizontalBox> DynamicHandleAllBox;
	TSharedRef<SHorizontalBox> ConstructDynamicHandleAllBox();

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

#pragma region OpenLocalLogFolder
	TSharedPtr<SButton> OpenLocalLogFolderButton;
	TSharedRef<SButton> ConstructOpenLocalLogFolderButton();
	FReply OnOpenLocalLogFolderButtonClicked();
#pragma endregion

#pragma region OpenLocalLogFolder
	TSharedPtr<SButton> BatchSizeMapButton;
	TSharedRef<SButton> ConstructBatchSizeMapButtonButton();
	FReply OnBatchSizeMapButtonClicked();
#pragma endregion

#pragma region BatchRename
	TSharedRef<SDockTab> OnSpawnBatchRenameTab(const FSpawnTabArgs& SpawnTabArgs);
	void OnBatchRenameApply();

	TSharedPtr<SButton> BatchRenameButton;
	TSharedRef<SButton> ConstructBatchRenameButton();
	FReply OnBatchRenameButtonClicked();
#pragma endregion

#pragma region SkeletonMeshBatchAudit



#pragma endregion

#pragma endregion


#pragma region ComboListFilter

	TSharedRef<SHorizontalBox> ConstructDropDownMenuBox();

	void UpdateDisplayListSource(bool bRefreshTableHeader = false);

#pragma region ClassFilter

	TSharedPtr<SComboBox<TSharedPtr<FString>>> ClassFilterComboBox;

	TArray<TSharedPtr<FString>> ClassFilterComboSourceItems;

	TSharedPtr<STextBlock> ClassFilterComboDisplayText;

	TSharedPtr<FString> ClassFilterCurrent;
	TSharedPtr<FString> ClassFilterDefault;

	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructClassFilterButton();

	TSharedRef<SWidget> OnGenerateClassFilterButton(
		TSharedPtr<FString> SourceItem);

	void OnClassFilterButtonChanged(
		TSharedPtr<FString> SelectedOption,
		ESelectInfo::Type InSelectInfo);

	void UpdateClassFilterList(
		TSharedPtr<FString> SelectedOption);

	

#pragma endregion

#pragma region CategoryFilter
	TSharedPtr<SComboBox<TSharedPtr<FString>>> CategoryFilterComboBox;

	TArray<TSharedPtr<FString>> CategoryFilterComboSourceItems;

	TSharedPtr<STextBlock> CategoryFilterComboDisplayText;

	TSharedPtr<FString> CategoryFilterCurrent;
	TSharedPtr<FString> CategoryFilterDefault;

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructCategoryFilterButton();

	TSharedRef<SWidget> OnGenerateCategoryFilterButton(
		TSharedPtr<FString> SourceItem);

	void OnCategoryFilterButtonChanged(
		TSharedPtr<FString> SelectedOption,
		ESelectInfo::Type InSelectInfo);

	void UpdateCategoryFilterList();
#pragma endregion
	

#pragma region UsageFilter

	TSharedPtr<SComboBox<TSharedPtr<FString>>> UsageFilterComboBox;

	FString UsageFilterCurrent;

	TArray<TSharedPtr<FString>> UsageFilterComboSourceItems;
	TSharedPtr<STextBlock> UsageFilterComboDisplayText;

	TSharedPtr<FString> UsageSelectedDefault;

	// Selection items only for texture

	TSharedPtr<FString> UsageSelectionMaxInGameSizeError;
	TSharedPtr<FString> UsageSelectionSourceSizeError;
	TSharedPtr<FString> UsageSelectionTextureSubfixError;
	TSharedPtr<FString> UsageSelectionTextureSettinsError;
	TSharedPtr<FString> UsageSelectionTextureLODGroupError;


	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructUsageFilterButton();

	TSharedRef<SWidget> OnGenerateUsageFilterButton(
		TSharedPtr<FString> SourceItem);

	void OnUsageFilterButtonChanged(
		TSharedPtr<FString> SelectedOption, 
		ESelectInfo::Type InSelectInfo);

	void UpdateUsageFilterAssetData();

#pragma endregion

#pragma region CategoryFilter



#pragma endregion

#pragma region TextureSizeCheckStrictMode
	bool bTextureSizeCheckStrictMode;
	bool bTextureSizeCheckStrictCheckBoxConstructed;
	TSharedPtr<SCheckBox> TextureSizeCheckStrictCheckBox;
	TSharedPtr<SHorizontalBox> TextureSizeCheckStrictBox;
	TSharedRef<SHorizontalBox> ConstructTextureSizeStrictCheckBox(
		ECheckBoxState State = ECheckBoxState::Unchecked);
	void OnTextureSizeStrictCheckBoxStateChanged(
		ECheckBoxState NewState);

#pragma endregion

#pragma region UnusedCheckRecursiveMode
	bool bRecursiveRefMode;
	bool bRecursiveRefBoxConstructed;
	TSharedPtr<SCheckBox> RecursiveRefCheckBox;
	TSharedPtr<SHorizontalBox> RecursiveRefHorizontalBox;
	TSharedRef<SHorizontalBox> ConstructRecursiveRefCheckBox(
		ECheckBoxState State = ECheckBoxState::Unchecked);
	void OnRecursiveRefCheckBoxStateChanged(
		ECheckBoxState NewState);

#pragma endregion

#pragma region ReverseCondition
	bool ReverseCondition;
	TArray<TSharedPtr<FAssetData>> ReversedTempArray;
	TSharedPtr<SCheckBox> ReverseConditionCheckBox;
	TSharedRef<SHorizontalBox> ConstructReverseConditionCheckBox();
	void OnReverseConditionCheckBoxStateChanged(
		ECheckBoxState NewState);
#pragma endregion

#pragma region DetailMode
	bool DetailMode;

	TSharedRef<SHorizontalBox> ConstructDetailModeBox();
	void OnDetailModeCheckBoxStateChanged(
		ECheckBoxState NewState);
#pragma endregion

#pragma region OnlyCheck
	bool OnlyCheck;

	TSharedRef<SHorizontalBox> ConstructOnlyCheckBox();
	void OnOnlyCheckBoxStateChanged(
		ECheckBoxState NewState);
#pragma endregion

#pragma region SearchableBox
	FString SearchString;
	TArray<TSharedPtr<FAssetData>> SListViewSearchFilterAssetData;
	TSharedPtr<SSearchBox> ListSearchBox;
	TSharedRef<SVerticalBox> ConstructSearchableBox();
	void OnSearchTextChanged(const FText& InSearchText);
	void UpdateSearchablbeBox();
#pragma endregion

#pragma endregion

};