// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/ManagerSlate.h"
#include "SlateWidgets/SCustomTableRow.h"
#include "SlateWidgets/BatchRenameSlate.h"

#include "ManagerLogger.h"

#include "AssetsManager.h"
#include "AssetsManagerStyle.h"

#include "AssetsChecker/AssetsChecker.h"

#include "StandardAsset/FCustomStandardSkeletalMesh.h"
#include "StandardAsset/FCustomStandardTexture2D.h"
#include "StandardAsset/FCustomStandardAsset.h"


#include "ConfigManager.h"

#include "SlateBasics.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "EditorReimportHandler.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "Components/Image.h"

#include "HAL/FileManager.h"

#include "Engine/Texture.h"

#include "Widgets/Input/SSearchBox.h"

#include <iostream>
#include <fstream>

#pragma region Filter

#ifdef ZH_CN
#define CLASSFILTER TEXT("资产类型过滤: ")
#define CATEGORYFILTER TEXT("资产分组过滤: ")
#define USAGEFILTER TEXT("资产条件过滤")
#else
#define CLASSFILTER TEXT("Class Filter: ")
#define CATEGORYFLTER TEXT("Category Filter: ")
#define USAGEFILTER TEXT("Condition Filter: ")
#endif

#pragma endregion

#pragma region ClassFilterComboSourceItems
#define CLASS_LISTALL TEXT("All")
#pragma endregion

#pragma region CategoryFilterComboSourceItems

#ifdef ZH_CN

#define CATEGORY_LISTALL TEXT("All")
#define CATEGORY_NOGROUP TEXT("未分组")
#define CATEGORY_CHARACTER TEXT("角色")
#define CATEGORY_HAIR TEXT("头发")
#define CATEGORY_SCENE TEXT("场景")
#define CATEGORY_UI TEXT("界面")
#define CATEGORY_EFFECT TEXT("特效")

#else

#define CATEGORY_LISTALL TEXT("All")
#define CATEGORY_NOGROUP TEXT("Undefined Category")
#define CATEGORY_CHARACTER TEXT("Character")
#define CATEGORY_HAIR TEXT("Hair")
#define CATEGORY_SCENE TEXT("Scene")
#define CATEGORY_UI TEXT("UI")
#define CATEGORY_EFFECT TEXT("Effect")

#endif

#pragma endregion

#pragma region UsageFilterComboSourceItems

#ifdef ZH_CN

#define USAGE_NONE TEXT("无")
#define USAGE_UNUSED TEXT("未使用的资产")
#define USAGE_MAXINGAMESIZEERROR TEXT("游戏内贴图大小错误")
#define USAGE_SOURCESIZEERROR TEXT("原始贴图大小错误")
#define USAGE_PREFIXERROR TEXT("资产前缀错误")
#define USAGE_SAMENAMEASSETERROR TEXT("多资产重复命名错误")
#define USAGE_TEXTURESUBFIXERROR TEXT("贴图不规范后缀")
#define USAGE_TEXTURESETTINGSERROR TEXT("贴图资产设定错误")
#define USAGE_TEXTUREGROUPERROR TEXT("LOD纹理组设置错误")

#else

#define USAGE_NONE TEXT("None")
#define USAGE_UNUSED TEXT("Unused")
#define USAGE_MAXINGAMESIZEERROR TEXT("Error-MaxInGameSize")
#define USAGE_SOURCESIZEERROR TEXT("Error-SourceSize")
#define USAGE_PREFIXERROR TEXT("Error-Prefix")
#define USAGE_SAMENAMEASSETERROR TEXT("Error-RepeatedName")
#define USAGE_TEXTURESUBFIXERROR TEXT("Error-Subfix(Texture)")
#define USAGE_TEXTURESETTINGSERROR TEXT("Error-Settings(Texture)")
#define USAGE_TEXTUREGROUPERROR TEXT("Error-TextureGroup(Texture LOD Group)")

#endif

#pragma endregion

void SManagerSlateTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	this->bTextureSizeCheckStrictMode = false;
	this->bTextureSizeCheckStrictCheckBoxConstructed = false;

	this->bRecursiveRefMode = false;
	this->bRecursiveRefBoxConstructed = false;

	RegistryTab();

	this->m_ClassCheckState = DefaultClassCheckState;
	this->m_CategoryCheckState = AssetCategory::LastCatergory;
	this->m_UsageCheckState = DefaultUsageCheckState;

	FSlateFontInfo TitleTextFont = GetFontInfo(25);

	this->StoredFolderPaths = InArgs._SelectedFolderPaths;

	this->StoredAssetsData = UAssetsChecker::ListAssetsDataPtrUnderSelectedFolder(StoredFolderPaths);
	
	UAssetsChecker::ECopyAssetsPtrList(StoredAssetsData, SListViewUsageFilterAssetData);
	UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewClassFilterAssetData);
	UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewAssetData);

	this->ClassFilterDefault = MakeShared<FString>(CLASS_LISTALL);
	this->ClassFilterCurrent = ClassFilterDefault;
	this->ClassFilterComboSourceItems.Add(ClassFilterDefault);

	FString AssetGlobalSection = "/AssetsManager/Global";
	TArray<FString> Keys = 
	UManagerConfig::Get().GenerateStructKeyValueArray(
		*AssetGlobalSection,
		"UClassPrefix",
		"UClassName");

	for (FString Key: Keys)
	{
		ClassFilterComboSourceItems.Add(MakeShared<FString>(Key));
	}

	CategoryFilterDefault = MakeShared<FString>(CATEGORY_LISTALL);
	CategoryFilterCurrent = CategoryFilterDefault;

	CategoryFilterComboSourceItems.Add(CategoryFilterDefault);
	CategoryFilterComboSourceItems.Add(MakeShared<FString>(CATEGORY_NOGROUP));
	CategoryFilterComboSourceItems.Add(MakeShared<FString>(CATEGORY_CHARACTER));
	CategoryFilterComboSourceItems.Add(MakeShared<FString>(CATEGORY_HAIR));
	CategoryFilterComboSourceItems.Add(MakeShared<FString>(CATEGORY_UI));
	CategoryFilterComboSourceItems.Add(MakeShared<FString>(CATEGORY_SCENE));
	CategoryFilterComboSourceItems.Add(MakeShared<FString>(CATEGORY_EFFECT));

	UsageSelectedDefault = MakeShared<FString>(USAGE_NONE);

	UsageSelectionMaxInGameSizeError = MakeShared<FString>(USAGE_MAXINGAMESIZEERROR);
	UsageSelectionSourceSizeError = MakeShared<FString>(USAGE_SOURCESIZEERROR);
	UsageSelectionTextureSubfixError = MakeShared<FString>(USAGE_TEXTURESUBFIXERROR);
	UsageSelectionTextureSettinsError = MakeShared<FString>(USAGE_TEXTURESETTINGSERROR);
	UsageSelectionTextureLODGroupError = MakeShared<FString>(USAGE_TEXTUREGROUPERROR);

	UsageFilterComboSourceItems.Add(UsageSelectedDefault);
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_UNUSED));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_PREFIXERROR));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_SAMENAMEASSETERROR));

	ConstructSelectAllButton();
	ConstructDeselectAllButton();
	ConstructDeleteAllSelectedButton();
	ConstructFixSelectedButton();
	ConstructFixUpRedirectorButton();
	ConstructOutputViewListInfoButton();
	ConstructOpenLocalLogFolderButton();
	ConstructBatchSizeMapButtonButton();
	ConstructBatchRenameButton();

	ConstructDynamicHandleAllBox();

	TSharedPtr<SVerticalBox> MainUI = SNew(SVerticalBox);

	/*
	*	Title
	*/

	const FSlateBrush* TitleImage = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.AssetsManagerTitle");

	MainUI->AddSlot()
		.AutoHeight()
		[
			ConstructCommonImageBox(
				TitleImage,
				FVector2D(FVector2D(5332.f, 175.f)),
				EStretch::ScaleToFitX)
		];

	MainUI->AddSlot()
		.AutoHeight()
		[
			SNew(SBorder)
		];

	/*
	*	Info
	*/

	TSharedPtr<SSplitter> ContentBox = SNew(SSplitter)
		.Orientation(Orient_Vertical);

	TSharedPtr<SVerticalBox> InfoBox = SNew(SVerticalBox);
	TSharedPtr<SVerticalBox> HandleListBox = SNew(SVerticalBox);

	ContentBox->AddSlot()
		.MinSize(80.f)
		.Value(0.15f)
		[
			InfoBox.ToSharedRef()
		];

	ContentBox->AddSlot()
		.MinSize(200.f)
		[
			HandleListBox.ToSharedRef()
		];

	/*
	*	Drop down Menu
	*/

	HandleListBox->AddSlot()
		.AutoHeight()
		[
			ConstructDropDownMenuBox()
		];

	HandleListBox->AddSlot()
		.AutoHeight()
		[
			ConstructOverlayOpaque(ConstructSearchableBox(),3)
		];


	/*
	*	BuildList
	*/

	ConstructHeaderRow();

	this->CustomTableList = SNew(SCustomTable<TSharedPtr<FAssetData>>)
		.SourceItems(&SListViewAssetData)
		.OnGenerateTableHeaderRow(this,&SManagerSlateTab::OnTableGenerateHeaderRow)
		.OnTableCheckBoxStateChanged(this, &SManagerSlateTab::OnTableCheckBoxStateChanged)
		.OnTableRowMouseButtonDoubleClicked(this, &SManagerSlateTab::OnRowMouseButtonDoubleClicked)
		.OnGenerateTableRowColumn(this,&SManagerSlateTab::OnTableGenerateListColumn);

	InfoBox->AddSlot()
		.AutoHeight()
		[
			ConstructInfoBox(StoredFolderPaths, GetFontInfo(12))
		];

	HandleListBox->AddSlot()
		.VAlign(VAlign_Fill)
		[
			this->CustomTableList.ToSharedRef()
		];


	MainUI->AddSlot()[ContentBox.ToSharedRef()];


	/*
	*	batch processing box
	*/

	ConstructHandleAllBox();

	MainUI->AddSlot().AutoHeight()
		[
			ConstructOverlayOpaque(this->BatchHandleBox, 3)
		];

#pragma endregion

	ChildSlot
	[
		MainUI.ToSharedRef()

	];

}

void SManagerSlateTab::RegistryTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName(TABNAME_BATCHRENAME),
		FOnSpawnTab::CreateRaw(this, &SManagerSlateTab::OnSpawnBatchRenameTab))
		.SetDisplayName(FText::FromString(TEXT(TABNAME_BATCHRENAME)))
		.SetIcon(FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

TSharedRef<SDockTab> SManagerSlateTab::OnSpawnBatchRenameTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBatchRename<TSharedPtr<FAssetData>>)
				.ItemIn(this->CustomTableList->GetSelectedItems())
				.OnApply(this,&SManagerSlateTab::OnBatchRenameApply)
		];
}

void SManagerSlateTab::OnBatchRenameApply()
{
	NtfyMsg("CloseSomething");
}

void SManagerSlateTab::SListViewRemoveAssetData(
	TSharedPtr<FAssetData> AssetData)
{
	if (StoredAssetsData.Contains(AssetData))
	{
		StoredAssetsData.Remove(AssetData);
	}

	if (SListViewClassFilterAssetData.Contains(AssetData))
	{
		SListViewClassFilterAssetData.Remove(AssetData);
	}

	if (SListViewUsageFilterAssetData.Contains(AssetData))
	{
		SListViewUsageFilterAssetData.Remove(AssetData);
	}

	if (SListViewCategoryFilterAssetData.Contains(AssetData))
	{
		SListViewCategoryFilterAssetData.Remove(AssetData);
	}

	if (SListViewSearchFilterAssetData.Contains(AssetData))
	{
		SListViewSearchFilterAssetData.Remove(AssetData);
	}

	if (SListViewAssetData.Contains(AssetData))
	{
		SListViewAssetData.Remove(AssetData);
	}
	
}

#pragma region OnGenerateRowForlist

void SManagerSlateTab::OnRowMouseButtonDoubleClicked(
	TSharedPtr<FAssetData> & AssetDataToDisplay)
{
	TArray<FAssetData> AssetDataArray;
	AssetDataArray.Add(*AssetDataToDisplay);

	GEditor->SyncBrowserToObjects(AssetDataArray);
}

void SManagerSlateTab::RefreshAssetsListView(
	bool bRefreshTableHeader)
{
	// NtfyMsgLog("Refreshed");

	if (bRefreshTableHeader) ConstructHeaderRow();

	if (CustomTableList.IsValid())
	{
		CustomTableList->RefreshTable(bRefreshTableHeader);
	}
	
	AllInFolderViewCountBlock->SetText(FText::FromString(FString::FromInt(StoredAssetsData.Num())));
	ListViewCountBlock->SetText(FText::FromString(FString::FromInt(CustomTableList->GetListItems().Num())));
	SelectedCountBlock->SetText(FText::FromString(FString::FromInt(CustomTableList->GetSelectedItems().Num())));
	
}

void SManagerSlateTab::ConstructHeaderRow()
{
	SManagerCustomTableTitleRowColumnsType.Empty();

	SManagerCustomTableTitleRowColumnsType.Add(Column_UClass);
	SManagerCustomTableTitleRowColumnsType.Add(Column_AssetName);

	if (DetailMode)
	{
		SManagerCustomTableTitleRowColumnsType.Add(Column_DiskSize);
	}

	if (m_ClassCheckState == Texture)
	{
		if (DetailMode)
		{
			SManagerCustomTableTitleRowColumnsType.Add(Column_MemorySize);
		}

		SManagerCustomTableTitleRowColumnsType.Add(
			m_UsageCheckState == SourceSizeError ? 
			Column_TextureSourceSize :Column_TextureMaxInGameSize);

		if (m_UsageCheckState == TextureGroupError || DetailMode)
		{
			SManagerCustomTableTitleRowColumnsType.Add(Column_TextureGroup);
		}

		if (m_UsageCheckState == TextureSettingsError || DetailMode)
		{
			SManagerCustomTableTitleRowColumnsType.Add(Column_TextureSRGB);
			
			SManagerCustomTableTitleRowColumnsType.Add(Column_TextureCompressionSettings);
		}
	}

	if (m_ClassCheckState == SkeletalMesh)
	{
		if (DetailMode)
		{
			SManagerCustomTableTitleRowColumnsType.Add(Column_SkeletalMeshLODNum);
			SManagerCustomTableTitleRowColumnsType.Add(Column_SkeletalVertices);
			SManagerCustomTableTitleRowColumnsType.Add(Column_SkeletalTriangles);
			SManagerCustomTableTitleRowColumnsType.Add(Column_SkeletalAllowCPUAccess);
		}
	}

	if (!OnlyCheck)
	{
		SManagerCustomTableTitleRowColumnsType.Add(Column_PerAssetHandle);
	}

}

TSharedRef<SHeaderRow> SManagerSlateTab::OnTableGenerateHeaderRow(
	TSharedPtr<SHeaderRow>& TableHeaderRow)
{
	
	for (SCommonSlate::CustomTableColumnType ColumnIn : SManagerCustomTableTitleRowColumnsType)
	{
		SHeaderRow::FColumn::FArguments ColumnBoxArgs;

		ColumnBoxArgs.DefaultLabel(FText::FromString("[Undefined Column]"));
		ColumnBoxArgs.ColumnId("[Undefined]");
		ColumnBoxArgs.ShouldGenerateWidget(true);
		ColumnBoxArgs.HAlignHeader(HAlign_Center);

		switch (ColumnIn)
		{
		case Column_UClass:
			ColumnBoxArgs.FillWidth(0.07f);
			break;

		case Column_AssetName:
			ColumnBoxArgs.FillWidth(0.2f);
			break;

		case Column_AssetPath:
			ColumnBoxArgs.FillWidth(0.5f);
			break;

		case Column_PerAssetHandle:
			ColumnBoxArgs.FillWidth(DetailMode? .22f : .1f);
			break;

		case Column_TextureMaxInGameSize:
			ColumnBoxArgs.FillWidth(0.07f);
			break;

		case Column_TextureSourceSize:
			ColumnBoxArgs.FillWidth(0.07f);
			break;

		case Column_TextureCompressionSettings:
			ColumnBoxArgs.FillWidth(0.2f);
			break;

		case Column_TextureSRGB:
			ColumnBoxArgs.FillWidth(0.05f);
			break;

		case Column_TextureGroup:
			ColumnBoxArgs.FillWidth(0.2f);
			break;

		case Column_DiskSize:
			ColumnBoxArgs.FillWidth(0.1f);
			break;

		case Column_MemorySize:
			ColumnBoxArgs.FillWidth(0.1f);
			break;

		case Column_SkeletalMeshLODNum:
			ColumnBoxArgs.FillWidth(0.05f);
			break;

		case Column_SkeletalVertices:
			ColumnBoxArgs.FillWidth(0.12f);
			break;

		case Column_SkeletalTriangles:
			ColumnBoxArgs.FillWidth(0.12f);
			break;

		case Column_SkeletalAllowCPUAccess:
			ColumnBoxArgs.FillWidth(0.1f);
			break;

		default:
			ColumnBoxArgs.FillWidth(0.1f);
			break;
		}


		const FString* ColumnNamePtr = CustomTableColumnTypeToString.Find(ColumnIn);

		if (ColumnNamePtr)
		{
			ColumnBoxArgs.DefaultLabel(FText::FromString(*ColumnNamePtr));
			ColumnBoxArgs.ColumnId(FName(*ColumnNamePtr));
		}

		TableHeaderRow->AddColumn(ColumnBoxArgs);
	}

	return TableHeaderRow.ToSharedRef();
}

TSharedRef<SWidget> SManagerSlateTab::OnTableGenerateListColumn(
	const FName& ColumnName, 
	TSharedPtr<FAssetData>& AssetToDisplay)
{
	const SCommonSlate::CustomTableColumnType * KeyFound = 
		CustomTableColumnTypeToString.FindKey(ColumnName.ToString());

	if (KeyFound)
	{
		switch (*KeyFound)
		{

		case Column_UClass:
		{
			TSharedPtr<SHorizontalBox> ClassWidget =
				ConstructAssetClassRowBox(AssetToDisplay, GetFontInfo(9));
			
			return ClassWidget.ToSharedRef();
		}

		case Column_AssetName:
		{
			TSharedPtr<SHorizontalBox> NameWidget =
				ConstructEditAssetNameRowBox(AssetToDisplay, GetFontInfo(9));

			return NameWidget.ToSharedRef();
		}

		case Column_AssetPath:
		

		case Column_TextureMaxInGameSize:
		{
			TSharedPtr<STextBlock> TextureSizeWidget = 
				ConstructAssetTextureSizeRowBox(AssetToDisplay, GetFontInfo(9));
			TextureSizeWidget->SetAutoWrapText(true);
			TextureSizeWidget->SetJustification(ETextJustify::Center);
			TextureSizeWidget->SetMargin(FMargin(3.f));

			return TextureSizeWidget.ToSharedRef();
		}

		case Column_TextureSourceSize:
		{
			TSharedPtr<STextBlock> TextureSizeWidget = 
				ConstructAssetTextureSizeRowBox(AssetToDisplay, GetFontInfo(9));
			TextureSizeWidget->SetAutoWrapText(true);
			TextureSizeWidget->SetJustification(ETextJustify::Center);
			TextureSizeWidget->SetMargin(FMargin(3.f));

			return TextureSizeWidget.ToSharedRef();
		}

		case Column_TextureCompressionSettings:
		{
			TSharedPtr<STextBlock> TextureCompressionSettingsWidget = 
				ConstructAssetTextureCompressionSettingsRowBox(AssetToDisplay, GetFontInfo(9));
			TextureCompressionSettingsWidget->SetAutoWrapText(true);
			TextureCompressionSettingsWidget->SetJustification(ETextJustify::Center);
			TextureCompressionSettingsWidget->SetMargin(FMargin(3.f));

			return TextureCompressionSettingsWidget.ToSharedRef();
		}

		case Column_TextureSRGB:
		{
			TSharedPtr<STextBlock> TextureSRGBSettingsWidget = 
				ConstructAssetTextureSRGBRowBox(AssetToDisplay, GetFontInfo(9));
			TextureSRGBSettingsWidget->SetAutoWrapText(true);
			TextureSRGBSettingsWidget->SetJustification(ETextJustify::Center);
			TextureSRGBSettingsWidget->SetMargin(FMargin(3.f));

			return TextureSRGBSettingsWidget.ToSharedRef();
		}

		case Column_TextureGroup:
		{
			TSharedPtr<STextBlock> TextureLODGroup = 
				ConstructAssetTextureLODGroupRowBox(AssetToDisplay, GetFontInfo(9));
			TextureLODGroup->SetAutoWrapText(true);
			TextureLODGroup->SetJustification(ETextJustify::Left);
			TextureLODGroup->SetMargin(FMargin(3.f));

			return TextureLODGroup.ToSharedRef();
		}

		case Column_MemorySize:
		{
			TSharedPtr<SHorizontalBox> MemorySizeBox =
				ConstructAssetMemorySizeRowBox(AssetToDisplay, GetFontInfo(9));

			return MemorySizeBox.ToSharedRef();
		}

		case Column_DiskSize:
		{
			TSharedPtr<SHorizontalBox> DiskSizeBox =
				ConstructAssetDiskSizeRowBox(AssetToDisplay, GetFontInfo(9));
			

			return DiskSizeBox.ToSharedRef();
		}

		case Column_SkeletalMeshLODNum:
		{
			TSharedPtr<SHorizontalBox> LODNumBox =
				ConstructSkeletalMeshLODNumRowBox(AssetToDisplay, GetFontInfo(9));
			
			return LODNumBox.ToSharedRef();
		}

		case Column_SkeletalTriangles:
		{
			TSharedPtr<SVerticalBox> TriangleBox =
				ConstructSkeletalMeshTrianglesNumRowBox(AssetToDisplay, GetFontInfo(9));

			return TriangleBox.ToSharedRef();
		}

		case Column_SkeletalVertices:
		{
			TSharedPtr<SVerticalBox> VerticesBox =
				ConstructSkeletalMeshVerticesNumRowBox(AssetToDisplay, GetFontInfo(9));

			return VerticesBox.ToSharedRef();
		}

		case Column_SkeletalAllowCPUAccess:
		{
			TSharedPtr<SVerticalBox> AllowCPUAccessBox =
				ConstructSkeletalMeshLODAllowCPUAccessRowBox(AssetToDisplay, GetFontInfo(9));

			return AllowCPUAccessBox.ToSharedRef();
		}

		case Column_PerAssetHandle:
			return ConstructSingleDealPanel(AssetToDisplay);

		default:
			return ConstructNormalTextBlock("[Undefined Info]", GetFontInfo(9));
		}
	};


	return ConstructNormalTextBlock("[Undefined Info]", GetFontInfo(9));
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructSingleDealPanel(
	const TSharedPtr<FAssetData> & ClickedAssetData)
{
	TSharedPtr<SHorizontalBox> DealPanel = SNew(SHorizontalBox);

	if (m_ClassCheckState == Texture)
	{
		if (m_UsageCheckState == TextureSettingsError)
		{
			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleTextureAssetSettingsFixButtonBox(ClickedAssetData)
				];
		}

		if (m_UsageCheckState == SourceSizeError)
		{
			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleAssetReimportButtonBox(ClickedAssetData)
				];
		}

		if (m_UsageCheckState == MaxInGameSizeError)
		{
			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleTextureAsset2KButtonBox(ClickedAssetData)
				];

			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleTextureAsset1KButtonBox(ClickedAssetData)
				];

			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleTextureAsset512ButtonBox(ClickedAssetData)
				];

			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleTextureAssetResetButtonBox(ClickedAssetData)
				];
		}

		if (m_UsageCheckState == TextureGroupError)
		{
			DealPanel->AddSlot()
				.HAlign(HAlign_Fill)
				[
					ConstructSingleTextureLODGroupStandardFixButtonBox(ClickedAssetData)
				];
		}
	}
	DealPanel->AddSlot()
		.HAlign(HAlign_Fill)
		[
			ConstructSingleAssetEditButtonBox(ClickedAssetData)
		];

	DealPanel->AddSlot()
		.HAlign(HAlign_Fill)
		[
			ConstructSingleAssetSizeMapButtonBox(ClickedAssetData)
		];

	DealPanel->AddSlot()
		.HAlign(HAlign_Fill)
		[
				ConstructSingleAssetDeleteButtonBox(ClickedAssetData)
		];

	/*DealPanel->AddSlot()
		.HAlign(HAlign_Fill)
		[
			ConstructSingleAssetDebugButtonBox(ClickedAssetData)
		];*/

	return DealPanel.ToSharedRef();
}

void SManagerSlateTab::OnTableCheckBoxStateChanged()
{
	SelectedCountBlock->SetText(FText::FromString(FString::FromInt(CustomTableList->GetSelectedItems().Num())));
}

#pragma region ConstructAssetInfo

TSharedRef<SVerticalBox> SManagerSlateTab::ConstructInfoBox(
	const TArray<FString>& FolderPaths, 
	const FSlateFontInfo& FontInfo)
{
	TSharedRef<SVerticalBox> AssetsListViewInfoBox =
		SNew(SVerticalBox)

		// count slot
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(1.5f))
		[
			ConstructListAssetsCountInfo(FontInfo)
		]
		// path slot
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		.Padding(FMargin(3.f))
		[
			ConstructListPathsInfo(FolderPaths, GetFontInfo(9))
		]
		
		;
		

	return AssetsListViewInfoBox;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructListPathsInfo(
	const TArray<FString>& FolderPaths, 
	const FSlateFontInfo& FontInfo)
{
	FString PathsInfo;

#ifdef ZH_CN
	PathsInfo += TEXT("检查路径:\n");
#else
	PathsInfo += TEXT("Selected Folder Paths:\n");
#endif

	for (const FString & p : FolderPaths)
	{
		PathsInfo += p;
		PathsInfo += "\n";
	}

	return ConstructNormalTextBlock(PathsInfo, FontInfo, ETextJustify::Center, FColor::Cyan);
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructListAssetsCountInfo(
	const FSlateFontInfo& FontInfo)
{
	AllInFolderViewCountBlock = ConstructNormalTextBlock(FString::FromInt(StoredAssetsData.Num()), FontInfo, ETextJustify::Left, FColor::Orange);
	ClassListViewCountBlock = ConstructNormalTextBlock(FString::FromInt(SListViewClassFilterAssetData.Num()), FontInfo, ETextJustify::Left, FColor::Yellow);
	ListViewCountBlock = ConstructNormalTextBlock(FString::FromInt(SListViewUsageFilterAssetData.Num()), FontInfo, ETextJustify::Left, FColor::Green);
	SelectedCountBlock = ConstructNormalTextBlock(FString::FromInt(CustomTableList->GetSelectedItems().Num()), FontInfo, ETextJustify::Left, FColor::Emerald);
	
	TSharedRef<SHorizontalBox> ListAssetsCountInfo =

		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(4.f))
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
#ifdef ZH_CN
					ConstructNormalTextBlock(TEXT("总资产数 -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("选择文件夹下的所有资产数量"))
#else
					ConstructNormalTextBlock(TEXT("All Assets -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("All assets in selected folder(s)."))
#endif
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					AllInFolderViewCountBlock.ToSharedRef()
				]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(4.f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
#ifdef ZH_CN
				ConstructNormalTextBlock(TEXT("所选类(Class)资产数量 -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("所选类型的资产数量"))
#else
				ConstructNormalTextBlock(TEXT("After Class Filter -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("Assets collected after class filter."))
#endif
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				ClassListViewCountBlock.ToSharedRef()
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(4.f))
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
#ifdef ZH_CN
					ConstructNormalTextBlock(TEXT("筛选后资产数量 -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("视口中筛选出的素材数量"))
#else
					ConstructNormalTextBlock(TEXT("List Out Assets -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("Assets listed in view port."))
#endif
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					ListViewCountBlock.ToSharedRef()
				]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(4.f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[


#ifdef ZH_CN
				ConstructNormalTextBlock(TEXT("选择资产数量 -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("选择的资产数量"))
#else
				ConstructNormalTextBlock(TEXT("Selected Assets -> "), FontInfo, ETextJustify::Left, FColor::White, TEXT("Assets selected count."))
#endif
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SelectedCountBlock.ToSharedRef()
			]
		];
		

	return ListAssetsCountInfo;
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructEditAssetNameRowBox(
	TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	TSharedPtr<SCustomEditableText<TSharedPtr<FAssetData>>> AssetNameBox =
		SNew(SCustomEditableText<TSharedPtr<FAssetData>>)
		.SourceItem(&AssetDataToDisplay)
		.Font(GetFontInfo(9))
		.Justify(EHorizontalAlignment::HAlign_Left)
		.TextColor(FColor::White)
		.OnItemToDisplayText(this,&SManagerSlateTab::OnAssetDataToAssetNameEditableText)
		.OnItemToTipText(this,&SManagerSlateTab::OnAssetDataToAssetEditableTextTip)
		.OnItemCommit(this,&SManagerSlateTab::OnItemDataCommitted);

	return SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center)[AssetNameBox.ToSharedRef()];
}

FText SManagerSlateTab::OnAssetDataToAssetNameEditableText(
	TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	return FText::FromString(DisplayAssetName);
}

FText SManagerSlateTab::OnAssetDataToAssetEditableTextTip(
	TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	FString DisplayAssetPath = AssetDataToDisplay->GetSoftObjectPath().ToString();
	return FText::FromString(DisplayAssetPath);
}

bool SManagerSlateTab::OnItemDataCommitted(
	const FText& TextIn,
	ETextCommit::Type CommitType,
	TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	if (CommitType != ETextCommit::OnEnter) return false;

	const FString NewName = TextIn.ToString();
	
	bool result = UAssetsChecker::RenameAsset(AssetDataToDisplay, NewName);

	return result;
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructAssetNameRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	const FString DisplayAssetPath = AssetDataToDisplay->GetSoftObjectPath().ToString();

	TSharedPtr<STextBlock> AssetNameBox = 
		ConstructNormalTextBlock(
			DisplayAssetName, 
			FontInfo, 
			ETextJustify::Left, 
			FColor::White,DisplayAssetPath);

	return SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center)[AssetNameBox.ToSharedRef()];
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructAssetClassRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	FString DisplayAssetClass;

	if(AssetDataToDisplay->GetClass())
	{
		DisplayAssetClass = AssetDataToDisplay->GetClass()->GetName();
	}
	else
	{
		DisplayAssetClass = "[Undefined]";
	}

	TSharedRef<STextBlock> ClassWidget = ConstructNormalTextBlock(DisplayAssetClass,FontInfo);
	ClassWidget->SetAutoWrapText(true);
	ClassWidget->SetJustification(ETextJustify::Center);
	ClassWidget->SetMargin(FMargin(3.f));

	return SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center)[ClassWidget];
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetTextureSizeRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay,
	const FSlateFontInfo& FontInfo)
{
	FVector2D TextureAssetSize;

	if (m_ClassCheckState == Texture && m_UsageCheckState == SourceSizeError)
	{
		TextureAssetSize = UAssetsChecker::GetTextureAssetSourceSize(*AssetDataToDisplay);
	}
	else
	{
		TextureAssetSize = UAssetsChecker::GetTextureAssetMaxInGameSize(*AssetDataToDisplay);
	}
	
	const FString ShowString = FString::FromInt(TextureAssetSize.X) + "x" + FString::FromInt(TextureAssetSize.Y);
	TSharedRef<STextBlock> TextureAssetSizeBox = ConstructNormalTextBlock(ShowString, FontInfo);

	return TextureAssetSizeBox;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetTextureCompressionSettingsRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	UCustomStandardTexture2D STexture(AssetDataToDisplay->GetAsset());
	
	TSharedPtr<TextureCompressionSettings> CompressionSettings 
		= STexture.GetCompressionSettings();

	if (CompressionSettings.IsValid())
	{
		for (CompressionSettingsInfo CompressionConfig :ValidCompressionConfig)
		{
			if (*CompressionSettings == CompressionConfig.Setting)
			{
				const FString ShowString = CompressionConfig.DisplayName;
				TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
				return TextureCompressionSettinsBox;
			}
		}
	};

	const FString ShowString = "";
	TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
	return TextureCompressionSettinsBox;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetTextureSRGBRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	TSharedPtr<bool> SRGBSettings
		= UAssetsChecker::GetTextureAssetSRGBSettings(*AssetDataToDisplay);

	if (SRGBSettings.IsValid())
	{
		const FString ShowString = *SRGBSettings ? L"[ √ ]" : L"[    ]";
		TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
		return TextureCompressionSettinsBox;
	};

	const FString ShowString = "";
	TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
	return TextureCompressionSettinsBox;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetTextureLODGroupRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	TSharedPtr<TextureGroup> TextureGroupResult
		= UAssetsChecker::GetTextureAssetTextureGroup(*AssetDataToDisplay);

	if (TextureGroupResult.IsValid())
	{
		const FString ShowString = UTexture2D::GetTextureGroupString(*TextureGroupResult);
		TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
		return TextureCompressionSettinsBox;
	}

	const FString ShowString = "[-]";
	TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
	return TextureCompressionSettinsBox;
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructAssetDiskSizeRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	FCustomStandardAssetData StandardAsset(*AssetDataToDisplay);
	AssetsInfoDisplayLevel DisplayLevel = AssetsInfoDisplayLevel::AssetsInfoDisplay_Display;
	double AssetSize = UAssetsChecker::ByteConversion(StandardAsset.GetDiskSize(DisplayLevel), AssetSizeDisplayUnit::MB);

	FString AssetSizeStr =FString::Printf(L"%.4f%s", AssetSize, L"MB");

	TSharedRef<STextBlock> DiskSizeBox = ConstructNormalTextBlock(AssetSizeStr, FontInfo);

	DiskSizeBox->SetColorAndOpacity(UAssetsChecker::DisplayLevelToColor(DisplayLevel));
	DiskSizeBox->SetAutoWrapText(true);
	DiskSizeBox->SetJustification(ETextJustify::Center);
	DiskSizeBox->SetMargin(FMargin(3.f));

	return SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center)[DiskSizeBox];
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructAssetMemorySizeRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	double AssetSize = 0;
	AssetsInfoDisplayLevel DisplayLevel = AssetsInfoDisplayLevel::AssetsInfoDisplay_Display;

	if (AssetDataToDisplay->GetAsset()->IsA<UTexture2D>())
	{
		FCustomStandardTexture2DData StandardAsset(*AssetDataToDisplay);
		AssetSize = UAssetsChecker::ByteConversion(StandardAsset.Get().GetMemorySize(DisplayLevel), AssetSizeDisplayUnit::MB);
	}
	else
	{
		FCustomStandardAssetData StandardAsset(*AssetDataToDisplay);
		AssetSize = UAssetsChecker::ByteConversion(StandardAsset.Get().GetMemorySize(DisplayLevel), AssetSizeDisplayUnit::MB);
	}

	FString AssetSizeStr = FString::Printf(L"%.4f%s", AssetSize, L"MB");

	TSharedRef<STextBlock> MemorySizeBox = ConstructNormalTextBlock(AssetSizeStr, FontInfo);

	MemorySizeBox->SetColorAndOpacity(UAssetsChecker::DisplayLevelToColor(DisplayLevel));
	MemorySizeBox->SetAutoWrapText(true);
	MemorySizeBox->SetJustification(ETextJustify::Center);
	MemorySizeBox->SetMargin(FMargin(3.f));

	return SNew(SHorizontalBox) +SHorizontalBox::Slot().VAlign(VAlign_Center)[MemorySizeBox];
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructSkeletalMeshLODNumRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	FCustomStandardSkeletalMeshData StandardSkeletal(*AssetDataToDisplay);

	if(!StandardSkeletal.Get().IsSkeletalMesh())
	{
		return SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center)[ConstructNormalTextBlock(L"[-]", FontInfo)];
	}

	TSharedRef<STextBlock> LODNumBox = 
		ConstructNormalTextBlock(FString::FromInt(StandardSkeletal.Get().GetLODNum()), FontInfo);

	LODNumBox->SetAutoWrapText(true);
	LODNumBox->SetJustification(ETextJustify::Center);
	LODNumBox->SetMargin(FMargin(3.f));

	return SNew(SHorizontalBox) +SHorizontalBox::Slot().VAlign(VAlign_Center)[LODNumBox];
}

TSharedRef<SVerticalBox> SManagerSlateTab::ConstructSkeletalMeshVerticesNumRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo,
	bool bStricWithCategory)
{
	FCustomStandardSkeletalMeshData StandardSkeletal(*AssetDataToDisplay);

	if (!StandardSkeletal.Get().IsSkeletalMesh())
	{
		return SNew(SVerticalBox) +SVerticalBox::Slot()[ConstructNormalTextBlock(L"[-]", FontInfo)];
	}

	int32 LODNum = StandardSkeletal.Get().GetLODNum();
	FString DisplayContent;

	TSharedPtr<SVerticalBox> InfoList = SNew(SVerticalBox);

	for (int LODIdx = 0; LODIdx < LODNum; ++LODIdx)
	{
		AssetsInfoDisplayLevel DisplayLevel;
		int32 VerticesCount = StandardSkeletal.Get().GetLODVerticesNum(LODIdx, DisplayLevel, bStricWithCategory);

		DisplayContent =
			FString::Printf(L"LOD%d:%8.2fw",
				LODIdx, VerticesCount / 10000.0f);

		TSharedPtr<STextBlock> DisplayBox = ConstructNormalTextBlock(DisplayContent, FontInfo);
		DisplayBox->SetColorAndOpacity(UAssetsChecker::DisplayLevelToColor(DisplayLevel));
		DisplayBox->SetAutoWrapText(true);
		DisplayBox->SetJustification(ETextJustify::Center);
		DisplayBox->SetMargin(FMargin(1.f));

		InfoList->AddSlot().HAlign(HAlign_Fill).Padding(FMargin(1.f))[DisplayBox.ToSharedRef()];
	}
	
	return InfoList.ToSharedRef();
}

TSharedRef<SVerticalBox> SManagerSlateTab::ConstructSkeletalMeshTrianglesNumRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo,
	bool bStricWithCategory)
{
	FCustomStandardSkeletalMeshData StandardSkeletal(*AssetDataToDisplay);

	if (!StandardSkeletal.Get().IsSkeletalMesh())
	{
		return SNew(SVerticalBox) +SVerticalBox::Slot()[ConstructNormalTextBlock(L"[-]", FontInfo)];
	}

	int32 LODNum = StandardSkeletal.Get().GetLODNum();
	FString DisplayContent;

	TSharedPtr<SVerticalBox> InfoList = SNew(SVerticalBox);

	for (int LODIdx = 0; LODIdx < LODNum; ++LODIdx)
	{
		AssetsInfoDisplayLevel DisplayLevel;
		int32 VerticesCount = StandardSkeletal.Get().GetLODTrianglesNum(LODIdx, DisplayLevel, bStricWithCategory);

		DisplayContent =
			FString::Printf(L"LOD%d:%8.2fw",
				LODIdx, VerticesCount / 10000.0f);

		TSharedPtr<STextBlock> DisplayBox = ConstructNormalTextBlock(DisplayContent, FontInfo);
		DisplayBox->SetColorAndOpacity(UAssetsChecker::DisplayLevelToColor(DisplayLevel));
		DisplayBox->SetAutoWrapText(true);
		DisplayBox->SetJustification(ETextJustify::Center);
		DisplayBox->SetMargin(FMargin(1.f));

		InfoList->AddSlot().HAlign(HAlign_Fill).Padding(FMargin(1.f))[DisplayBox.ToSharedRef()];
	}

	return InfoList.ToSharedRef();
}

TSharedRef<SVerticalBox> SManagerSlateTab::ConstructSkeletalMeshLODAllowCPUAccessRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	FCustomStandardSkeletalMeshData StandardSkeletal(*AssetDataToDisplay);

	if (!StandardSkeletal.Get().IsSkeletalMesh())
	{
		return SNew(SVerticalBox)+SVerticalBox::Slot()[ConstructNormalTextBlock(L"[-]", FontInfo)];
	}

	int32 LODNum = StandardSkeletal.Get().GetLODNum();
	FString DisplayContent;

	TSharedPtr<SVerticalBox> InfoList = SNew(SVerticalBox);

	for (int LODIdx = 0; LODIdx < LODNum; ++LODIdx)
	{
		DisplayContent =
			FString::Printf(L"LOD%d: %s",
				LODIdx, StandardSkeletal.Get().GetAllowCPUAccess(LODIdx) ? L"[ √ ]" : L"[    ]");

		TSharedPtr<STextBlock> DisplayBox = ConstructNormalTextBlock(DisplayContent, FontInfo);
		DisplayBox->SetAutoWrapText(true);
		DisplayBox->SetJustification(ETextJustify::Center);
		DisplayBox->SetMargin(FMargin(1.f));

		InfoList->AddSlot().HAlign(HAlign_Fill).Padding(FMargin(1.f))[DisplayBox.ToSharedRef()];
	}

	return InfoList.ToSharedRef();
}

#pragma endregion

#pragma region ConstructSingleButton

TSharedRef<SButton> SManagerSlateTab::ConstructSingleAssetDeleteButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedPtr<SButton> SingleAssetDeleteButtonBox =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSingleAssetDeleteButtonClicked, AssetDataToDisplay)
		.ContentPadding(FMargin(2.f))
#ifdef ZH_CN
		.ToolTipText(FText::FromString(L"删除"));
#else
		.ToolTipText(FText::FromString(L"Delete"));
#endif

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerDelete");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);

	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		[
			ButtonImgComponent
		];

	SingleAssetDeleteButtonBox->SetContent(ButtonImgScaler);

	return SingleAssetDeleteButtonBox.ToSharedRef();
}

FReply SManagerSlateTab::OnSingleAssetDeleteButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	if (UAssetsChecker::GetAssetReferencesPath(ClickedAssetData).Num() > 0)
	{
#ifdef ZH_CN
		EAppReturnType::Type result = DlgMsg(EAppMsgType::OkCancel,
			ClickedAssetData->AssetName.ToString() 
			+ TEXT("已被其他资产引用\n\n确定删除资产？"));
#else
		EAppReturnType::Type result = DlgMsg(EAppMsgType::OkCancel,
			ClickedAssetData->AssetName.ToString()
			+ TEXT(" was referenced.\n\nConfirm to delete this asset?"));
#endif

		if (result != EAppReturnType::Ok)
		{
			return FReply::Handled();
		}
	}

	if (UAssetsChecker::DeleteAsset(*ClickedAssetData))
	{
		// log
#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功删除") + ClickedAssetData->AssetName.ToString());
#else
		NtfyMsgLog(TEXT("Successfully deleted ") + ClickedAssetData->AssetName.ToString());
#endif

		// update table list view
		SListViewRemoveAssetData(ClickedAssetData);
		RefreshAssetsListView(false);
	};

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleAssetDebugButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleAssetDebugButtonBox =
		SNew(SButton)
#ifdef ZH_CN
		.Text(FText::FromString(TEXT("测试")))
#else
		.Text(FText::FromString(TEXT("Debug")))
#endif
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this, 
			&SManagerSlateTab::OnSingleAssetDebugButtonClicked, 
			AssetDataToDisplay);

	return SingleAssetDebugButtonBox;
}

FReply SManagerSlateTab::OnSingleAssetDebugButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	FCustomStandardSkeletalMeshData SKAsset(*ClickedAssetData);
	
	if (SKAsset.Get().IsSkeletalMesh())
	{
		SKAsset.Get().SetLODsAllowCPUAccess(0);
		//NtfyMsg(SKAsset.IsLODModifiable(1)?"Yes":"No");
	}

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleAssetEditButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedPtr<SButton> SingleAssetEditButtonBox =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSingleAssetEditButtonClicked, AssetDataToDisplay)
		.ContentPadding(FMargin(2.f))
#ifdef ZH_CN
		.ToolTipText(FText::FromString(L"编辑"));
#else
		.ToolTipText(FText::FromString(L"Edit Asset"));
#endif

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerEdit");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);

	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		[
			ButtonImgComponent
		];

	SingleAssetEditButtonBox->SetContent(ButtonImgScaler);

	return SingleAssetEditButtonBox.ToSharedRef();
}

FReply SManagerSlateTab::OnSingleAssetEditButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	UAssetsChecker::OpenEditorUI(ClickedAssetData);
	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleAssetSizeMapButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedPtr<SButton> SingleAssetSizeMapButtonBox =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSingleAssetSizeMapButtonClicked, AssetDataToDisplay)
		.ContentPadding(FMargin(2.f))
		.ToolTipText(FText::FromString(L"SizeMap"));

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerSizeMap");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);

	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		[
			ButtonImgComponent
		];

	SingleAssetSizeMapButtonBox->SetContent(ButtonImgScaler);

	return SingleAssetSizeMapButtonBox.ToSharedRef();
}

FReply SManagerSlateTab::OnSingleAssetSizeMapButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	UAssetsChecker::OpenSizeMapUI(ClickedAssetData);

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleAssetReimportButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleAssetFixButtonBox =
		SNew(SButton)
#ifdef ZH_CN
		.Text(FText::FromString(TEXT("导入")))
#else
		.Text(FText::FromString(TEXT("Reimport")))
#endif
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this, 
			&SManagerSlateTab::OnSingleAssetReimportButtonClicked, 
			AssetDataToDisplay);

	return SingleAssetFixButtonBox;
}

FReply SManagerSlateTab::OnSingleAssetReimportButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	if (m_ClassCheckState == Texture && m_UsageCheckState == SourceSizeError)
	{
		FReimportManager::Instance()->Reimport(
			ClickedAssetData->GetAsset(), 
			true,true,L"",(FReimportHandler*)nullptr,-1,
			true,false);

		UEditorAssetLibrary::SaveAsset(ClickedAssetData->GetObjectPathString());
	}
	
	RefreshAssetsListView();

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureAsset2KButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("2K")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleTextureAsset2KButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SManagerSlateTab::OnSingleTextureAsset2KButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	double maxSize = 2048;

	UCustomStandardTexture2D StandardT2DObject(ClickedAssetData->GetAsset());

	if (m_ClassCheckState == Texture)
	{
		if (!StandardT2DObject.CustomMaxInGameSize(maxSize, true, true))
		{

			NtfyMsg(ClickedAssetData->AssetName.ToString() + 
#ifdef ZH_CN
				TEXT("\n此贴图无需修复 或 修复失败")
#else
				TEXT("\nFaild or no need to fix this texture.")
#endif
			);
		}
		else
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\n成功限制贴图大小为") 
				+ FString::FromInt(maxSize));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\nSuccessfully resize to ")
				+ FString::FromInt(maxSize));
#endif
		};
	}

	RefreshAssetsListView();

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureAsset1KButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("1K")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleTextureAsset1KButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SManagerSlateTab::OnSingleTextureAsset1KButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	double maxSize = 1024;

	UCustomStandardTexture2D StandardT2DObject(ClickedAssetData->GetAsset());

	if (m_ClassCheckState == Texture)
	{
		if (!StandardT2DObject.CustomMaxInGameSize(maxSize, true, true))
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\n此贴图无需修复 或 修复失败"));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\nFaild or no need to fix this texture."));
#endif
		}
		else
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\n成功限制贴图大小为")
				+ FString::FromInt(maxSize));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\nSuccessfully resize to ")
				+ FString::FromInt(maxSize));
#endif
		};
	}

	RefreshAssetsListView();

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureAsset512ButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("512")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleTextureAsset512ButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SManagerSlateTab::OnSingleTextureAsset512ButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	double maxSize = 512;
	UCustomStandardTexture2D StandardT2DObject(ClickedAssetData->GetAsset());

	if (m_ClassCheckState == Texture)
	{
		if (!StandardT2DObject.CustomMaxInGameSize(maxSize, true, true))
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() + 
				TEXT("\n此贴图无需修复 或 修复失败"));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\nFaild or no need to fix this texture."));
#endif
			;
		}
		else
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\n成功限制贴图大小为") 
				+ FString::FromInt(maxSize));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\nSuccessfully resize to ") 
				+ FString::FromInt(maxSize));
#endif
		};
	}

	RefreshAssetsListView();

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureAssetResetButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("R")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleTextureAssetResetButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SManagerSlateTab::OnSingleTextureAssetResetButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	double maxSize = 0;
	UCustomStandardTexture2D StandardT2DObject(ClickedAssetData->GetAsset());

	if (m_ClassCheckState == Texture)
	{
		if (!StandardT2DObject.CustomMaxInGameSize(maxSize, true, true))
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() + 
				TEXT("\n此贴图无需修复 或 修复失败"));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				TEXT(+ "\nFaild or no need to fix this texture."));
#endif;
			;
	}
		else
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\n成功解除贴图大小限制"));
#else
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\nSuccessfully resize to origion"));
#endif
			;
};
	}

	RefreshAssetsListView();

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureAssetSettingsFixButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedPtr<SButton> SingleTextureAssetSettingsFixButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSingleTextureAssetSettingsFixButtonClicked, AssetDataToDisplay)
		.ContentPadding(FMargin(2.f))
#ifdef ZH_CN
		.ToolTipText(FText::FromString(L"修复"));
#else
		.ToolTipText(FText::FromString(L"Fix"));
#endif

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerFix");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);

	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		[
			ButtonImgComponent
		];

	SingleTextureAssetSettingsFixButton->SetContent(ButtonImgScaler);

	return SingleTextureAssetSettingsFixButton.ToSharedRef();
}

FReply SManagerSlateTab::OnSingleTextureAssetSettingsFixButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	UAssetsChecker::SetTextureStandardSettings(*ClickedAssetData);
	RefreshAssetsListView();
	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureLODGroupStandardFixButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedPtr<SButton> SingleTextureLODGroupStandardFixButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSingleTextureLODGroupStandardFixButtonClicked, AssetDataToDisplay)
		.ContentPadding(FMargin(2.f))
#ifdef ZH_CN
		.ToolTipText(FText::FromString(L"修复"));
#else
		.ToolTipText(FText::FromString(L"Fix"));
#endif

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerFix");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);

	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		[
			ButtonImgComponent
		];

	SingleTextureLODGroupStandardFixButton->SetContent(ButtonImgScaler);

	return SingleTextureLODGroupStandardFixButton.ToSharedRef();
}

FReply SManagerSlateTab::OnSingleTextureLODGroupStandardFixButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	FCustomStandardTexture2DData TAsset(*ClickedAssetData);

	TSharedPtr<TextureGroup> STLODGroup =
		TAsset.Get().GetStandardLODGroup();

	if (STLODGroup)
	{
		TAsset.Get().SetLODGroup( *STLODGroup, true);

		RefreshAssetsListView(false);
	}

	return FReply::Handled();
}

#pragma endregion

#pragma endregion

#pragma region GenerateHandleAllButton

TSharedRef<SVerticalBox> SManagerSlateTab::ConstructHandleAllBox()
{
	this->BatchHandleBox = SNew(SVerticalBox);

	this->BatchHandleBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(.2f)
				.Padding(5.f)
				[
					this->SelectAllButton.ToSharedRef()
				]

				+ SHorizontalBox::Slot()
				.FillWidth(.25f)
				.Padding(5.f)
				[
					this->UnselectAllButton.ToSharedRef()
				]
		];

	this->BatchHandleBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(.35f)
				.Padding(5.f)
				[
					this->DeleteAllSelectedButton.ToSharedRef()
				]
		];

	this->BatchHandleBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(.35f)
				.Padding(5.f)
				[
					this->BatchRenameButton.ToSharedRef()
				]
		];

	this->BatchHandleBox->AddSlot()
		.AutoHeight()
		[
			this->DynamicHandleAllBox.ToSharedRef()
		];

	this->BatchHandleBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(.5f)
				.Padding(5.f)
				[
					this->FixUpRedirectorButton.ToSharedRef()
				]
		];

	this->BatchHandleBox->AddSlot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(.9f)
					.Padding(5.f)
					[
						this->OutputViewListInfoButton.ToSharedRef()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(.05f)
					.Padding(5.f)
					[
						this->OpenLocalLogFolderButton.ToSharedRef()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(.05f)
					.Padding(5.f)
					[
						this->BatchSizeMapButton.ToSharedRef()
					]
			];

	return BatchHandleBox.ToSharedRef();
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructDynamicHandleAllBox()
{
	if (!this->DynamicHandleAllBox.IsValid()) 
	{
		this->DynamicHandleAllBox = SNew(SHorizontalBox);
	}

	if( !OnlyCheck &&
		(m_UsageCheckState == TextureSettingsError || 
		m_UsageCheckState == PrefixError ||
		m_UsageCheckState == TextureGroupError))
	{
		this->DynamicHandleAllBox->RemoveSlot(this->FixSelectedButton.ToSharedRef());

		this->DynamicHandleAllBox->InsertSlot(0)
			.Padding(5.f)
			[
				this->FixSelectedButton.ToSharedRef()
			];
	}
	else
	{
		this->DynamicHandleAllBox->RemoveSlot(this->FixSelectedButton.ToSharedRef());
	}

	return this->DynamicHandleAllBox.ToSharedRef();
}

#pragma endregion

#pragma region DeleteAllSelectedButton

TSharedRef<SButton> SManagerSlateTab::ConstructDeleteAllSelectedButton()
{
	this -> DeleteAllSelectedButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnDeleteAllSelectedButtonClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	this->DeleteAllSelectedButton->SetContent(ConstructTextForButtons(TEXT("删除选择的资产")));
#else
	this->DeleteAllSelectedButton->SetContent(ConstructTextForButtons(TEXT("Delete All Selected")));
#endif
	return this->DeleteAllSelectedButton.ToSharedRef();
}

FReply SManagerSlateTab::OnDeleteAllSelectedButtonClicked()
{
	TArray<TSharedPtr<FAssetData>> AssetList = CustomTableList->GetSelectedItems();

	if (AssetList.Num() == 0)
	{
		return FReply::Handled();
	}

	TArray<FAssetData> AssetsDataToDelete;

	for (TSharedPtr<FAssetData> AssetDataPtr : AssetList)
	{
		AssetsDataToDelete.Add(*AssetDataPtr.Get());
	}

	if (UAssetsChecker::DeleteAssets(AssetsDataToDelete))
	{
		for (TSharedPtr<FAssetData> AssetDataPtr : AssetList)
		{
			SListViewRemoveAssetData(AssetDataPtr);
		}

		UpdateDisplayListSource();
		RefreshAssetsListView(false);
	}

	return FReply::Handled();
}

#pragma endregion

#pragma region De/SelectAllButton

TSharedRef<SButton> SManagerSlateTab::ConstructSelectAllButton()
{
	this->SelectAllButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSelectAllButtonClicked)
		.ContentPadding(FMargin(5.f));

#ifdef ZH_CN
	this->SelectAllButton->SetContent(ConstructTextForButtons(TEXT("全选")));
#else
	this->SelectAllButton->SetContent(ConstructTextForButtons(TEXT("Select All")));
#endif

	return this->SelectAllButton.ToSharedRef();
}

FReply SManagerSlateTab::OnSelectAllButtonClicked()
{
	CustomTableList->SelectAll();

	// NtfyMsg("List:" + FString::FromInt(CustomTableList->GetListItems().Num()));
	// NtfyMsg("Selected:" + FString::FromInt(CustomTableList->GetSelectedItems().Num()));

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructDeselectAllButton()
{
	this -> UnselectAllButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnDeselectAllButtonClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	this->UnselectAllButton->SetContent(ConstructTextForButtons(TEXT("取消全选")));
#else
	this->UnselectAllButton->SetContent(ConstructTextForButtons(TEXT("Deselect All")));
#endif

	return this->UnselectAllButton.ToSharedRef();
}

FReply SManagerSlateTab::OnDeselectAllButtonClicked()
{
	CustomTableList->UnselectAll();
	return FReply::Handled();
}

#pragma endregion

#pragma region FixAllSelectedButton

TSharedRef<SButton> SManagerSlateTab::ConstructFixSelectedButton()
{
	this->FixSelectedButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnSelectFixSelectedClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	this->FixSelectedButton->SetContent(ConstructTextForButtons(TEXT("修复选择的资产")));
#else
	this->FixSelectedButton->SetContent(ConstructTextForButtons(TEXT("Fix All Selected")));
#endif

	return this->FixSelectedButton.ToSharedRef();
}

FReply SManagerSlateTab::OnSelectFixSelectedClicked()
{
	TArray< TSharedPtr<FAssetData>> AssetsList = CustomTableList->GetSelectedItems();

	if (m_UsageCheckState == PrefixError)
	{
		TArray<TSharedPtr<FAssetData>> AssetsReadyRename;
		TArray<TSharedPtr<FAssetData>> AssetShouldRename;

		for (TSharedPtr<FAssetData> AssetData: AssetsList)
		{
			AssetsReadyRename.Add(AssetData);
		}

		bool result = UAssetsChecker::ConfirmPrefixes(AssetsReadyRename, AssetShouldRename);

		if (result)
		{
			TArray<FAssetData> AssetToRename;

			for (TSharedPtr<FAssetData> & Asset : AssetShouldRename)
			{
				AssetToRename.Add(*Asset);
			}

			UAssetsChecker::AddPrefixes(AssetToRename);
			StoredAssetsData = UAssetsChecker::ListAssetsDataPtrUnderSelectedFolder(StoredFolderPaths);
			UpdateClassFilterList(ClassFilterCurrent);
		}

		RefreshAssetsListView(false);
		return FReply::Handled();
	}

	if (m_UsageCheckState == TextureSettingsError)
	{
		for (TSharedPtr<FAssetData> AssetData : AssetsList)
		{
			if(UAssetsChecker::SetTextureStandardSettings(*AssetData))
			{
				if (SListViewUsageFilterAssetData.Contains(AssetData))
				{
					SListViewUsageFilterAssetData.Remove(AssetData);
				}
			}
		}

		RefreshAssetsListView(false);
		return FReply::Handled();
	}

	if (m_UsageCheckState == TextureGroupError)
	{
		for (TSharedPtr<FAssetData> AssetData : AssetsList)
		{
			FCustomStandardTexture2DData TAsset(*AssetData);

			TSharedPtr<TextureGroup> STLODGroup =
				TAsset.Get().GetStandardLODGroup();

			if (STLODGroup)
			{
				TAsset.Get().SetLODGroup( *STLODGroup, true);
			}
		}

		RefreshAssetsListView(false);
		return FReply::Handled();
	}

#ifdef ZH_CN
	DlgMsgLog(EAppMsgType::Ok, FString(USAGEFILTER) + FString(TEXT("选择错误\n应该选择[资产前缀错误/资产设置错误]")));
#else
	DlgMsgLog(EAppMsgType::Ok, FString(TEXT("Choose a valid ")) + FString(USAGEFILTER) + FString(TEXT(" type!\nShould be[PrefixError/TextureSettinsError]."))));
#endif
	return FReply::Handled();
}

#pragma endregion

#pragma region FixRedirectorsButton

TSharedRef<SButton> SManagerSlateTab::ConstructFixUpRedirectorButton()
{
	this->FixUpRedirectorButton = 
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnFixUpRedirectorButtonClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	this->FixUpRedirectorButton->SetContent(
		ConstructTextForButtons(TEXT("-- 修复所选文件夹中的重定向器(Redirector) --")));
#else
	this->FixUpRedirectorButton->SetContent(
		ConstructTextForButtons(TEXT("-- Fix Up Redirectors In Selected Folders --")));
#endif

	return this->FixUpRedirectorButton.ToSharedRef();
}

FReply SManagerSlateTab::OnFixUpRedirectorButtonClicked()
{
	UAssetsChecker::FixUpRedirectors(StoredFolderPaths);

	this->StoredAssetsData = UAssetsChecker::ListAssetsDataPtrUnderSelectedFolder(StoredFolderPaths);
	UpdateDisplayListSource();

	return FReply::Handled();
}
#pragma endregion

#pragma region OutputViewListInfo

TSharedRef<SButton> SManagerSlateTab::ConstructOutputViewListInfoButton()
{
	this->OutputViewListInfoButton =
		SNew(SButton)
		.OnClicked(this,&SManagerSlateTab::OnOutputViewListInfoButtonClicked)
		.ContentPadding(FMargin(5.f));

#ifdef ZH_CN
	this->OutputViewListInfoButton->SetContent(ConstructTextForButtons(TEXT("-- 输出列表到文件 --")));
#else
	this->OutputViewListInfoButton->SetContent(ConstructTextForButtons(TEXT("-- Output view list to log file --")));
#endif

	return this->OutputViewListInfoButton.ToSharedRef();
}

FReply SManagerSlateTab::OnOutputViewListInfoButtonClicked()
{


	FString CSVContent;
	
	// Info
	CSVContent += FString::Printf(L"%s\n", L"Path Included");

	for (FString path : StoredFolderPaths)
	{
		CSVContent += FString::Printf(L"%s\n", *path);
	}

	CSVContent += FString::Printf(L"\n%s\t%s\t%s\t%s\n%s\t%s\t%s\t%s\n\n",
#ifdef ZH_CN
		CLASSFILTER, CATEGORYFILTER,USAGEFILTER,L"条件反向",
#else
		CLASSFILTER, CATEGORYFILTER, USAGEFILTER, L"Condition Reverse",
#endif
		* ClassFilterComboDisplayText->GetText().ToString(),
		* CategoryFilterComboDisplayText->GetText().ToString(),
		* UsageFilterComboDisplayText->GetText().ToString(),
		ReverseCondition ? TEXT("√") : TEXT(""));
	
	// Header
#ifdef ZH_CN
	CSVContent += L"类\t资产名\t磁盘大小\t";

	if (m_ClassCheckState == Texture)
	{
		CSVContent += L"压缩大小\t压缩尺寸\t源尺寸\t";
	}

	if (m_ClassCheckState == SkeletalMesh)
	{
		CSVContent += L"LOD层数\tLOD面数\tLOD顶点数\t";
	}

	CSVContent += L"资产路径\t\n";
#else
	CSVContent += L"Class\tAssetName\tDiskSize\t";

	if (m_ClassCheckState == Texture)
	{
		CSVContent += L"ResourceSize\tMaxInGame\tSource\t";
	}

	if (m_ClassCheckState == SkeletalMesh)
	{
		CSVContent += L"LODNum\tLODTri\tLODVer\t";
	}

	CSVContent += L"AssetPath\t\n";
#endif

	TArray<TSharedPtr<FAssetData>> OutputData;
	OutputData.Empty();

	if (this->CustomTableList->GetSelectedItems().Num() > 0)
	{
		OutputData = this->CustomTableList->GetSelectedItems();
	}
	else
	{
		OutputData = this->CustomTableList->GetListItems();
	}

	if (m_ClassCheckState == DefaultClassCheckState)
	{
		OutputData.Sort([](const TSharedPtr<FAssetData>& ip1, const TSharedPtr<FAssetData>& ip2)
			{
				FString c1 = ip1->GetClass()->GetName();
				FString c2 = ip2->GetClass()->GetName();
				return UAssetsChecker::JudgeSort(c1, c2);
			});
	}

	if (m_ClassCheckState == Texture)
	{
		OutputData.Sort([](const TSharedPtr<FAssetData>& ip1, const TSharedPtr<FAssetData>& ip2) 
			{
				FCustomStandardTexture2DData T1(*ip1);
				FCustomStandardTexture2DData T2(*ip2);
				FVector2D c1 = T1.Get().GetMaxInGameSize();
				FVector2D c2 = T2.Get().GetMaxInGameSize();
				return UAssetsChecker::JudgeSort(c1,c2,true);
			});
	}

	int32 TaskNum = OutputData.Num();
	FSlowTask WritingTask(TaskNum, FText::FromString("Writing Data ..."));
	WritingTask.Initialize();
	WritingTask.MakeDialog();
	
	TArray<int64> LODVerticesAudit;
	TArray<int64> LODTrianglesAudit;

	// Content
	for (TSharedPtr<FAssetData> asset: OutputData)
	{
		WritingTask.EnterProgressFrame(.9f);

		FCustomStandardAssetData StandardAsset(*asset);
		
		FString DiskSize;
		UAssetsChecker::ByteConversion(StandardAsset.GetDiskSize(), DiskSize, false);

		CSVContent += FString::Printf(L"%s\t%s\t%s\t",
			*StandardAsset.GetClass()->GetName(),
			*StandardAsset.AssetName.ToString(),
			*DiskSize);
		
		if (m_ClassCheckState == Texture)
		{
			FCustomStandardTexture2DData StandardTexture(*asset);

			FString ResourceSize;
			UAssetsChecker::ByteConversion(StandardTexture.Get().GetMemorySize(), ResourceSize, false);

			FVector2D MaxInGameTextureSize = StandardTexture.Get().GetMaxInGameSize();
			FVector2D SourceTextureSize = StandardTexture.Get().GetSourceSize();

			FString MaxInGameTextureSizeString = FString::Printf(L"(%dx%d)", (int32)MaxInGameTextureSize.X, (int32)MaxInGameTextureSize.Y);
			FString SourceTextureSizeString = FString::Printf(L"(%dx%d)", (int32)SourceTextureSize.X, (int32)SourceTextureSize.Y);

			CSVContent += FString::Printf(L"%s\t%s\t%s\t",
				*ResourceSize,
				*MaxInGameTextureSizeString,
				*SourceTextureSizeString);
		}

		if (m_ClassCheckState == SkeletalMesh)
		{
			FCustomStandardSkeletalMeshData StandardSkeletal(*asset);

			int32 LODNum = StandardSkeletal.Get().GetLODNum();
			
			FString LODVer;
			FString LODTri;

			for (int32 Idx = 0; Idx < LODNum; ++Idx)
			{
				int32 VerN = StandardSkeletal.Get().GetLODVerticesNum(Idx);
				int32 TriN = StandardSkeletal.Get().GetLODTrianglesNum(Idx);

				if (LODVerticesAudit.Num()-1 < Idx)
				{
					LODVerticesAudit.Add(VerN);
				}
				else
				{
					LODVerticesAudit[Idx] += VerN;
				}

				if (LODTrianglesAudit.Num() - 1 < Idx)
				{
					LODTrianglesAudit.Add(TriN);
				}
				else
				{
					LODTrianglesAudit[Idx] += TriN;
				}

				LODVer += UAssetsChecker::IntStrAddColumn(FString::Printf(L"%d", VerN));
				LODTri += UAssetsChecker::IntStrAddColumn(FString::Printf(L"%d", TriN));
				
				if (Idx < LODNum - 1)
				{
					LODVer += L"/";
					LODTri += L"/";
				}
			}

			CSVContent += FString::Printf(L"%d\t%s\t%s\t",
				LODNum,
				*LODTri,
				*LODVer);

		}
		
		CSVContent += FString::Printf(L"%s\t\n",
			*StandardAsset.GetObjectPathString());
	}

	if (m_ClassCheckState == SkeletalMesh)
	{
		CSVContent += L"\nLODAudit\tTri\tVer\n";

		int32 Layers = LODTrianglesAudit.Num() > LODVerticesAudit.Num() ? LODTrianglesAudit.Num() : LODVerticesAudit.Num();

		for (int32 Idx = 0; Idx < Layers; ++Idx)
		{
			CSVContent += FString::Printf(L"%d\t%s\t%s\n",
				Idx,
				(Idx < LODTrianglesAudit.Num() ? *UAssetsChecker::IntStrAddColumn(FString::Printf(L"%d", LODTrianglesAudit[Idx])) : L"0"),
				(Idx < LODVerticesAudit.Num() ? *UAssetsChecker::IntStrAddColumn(FString::Printf(L"%d", LODVerticesAudit[Idx])) : L"0"));
		}

	}

	FDateTime Time = FDateTime::Now();
	FString TimeStr = FString::Printf(L"%.2d%.2d%.2d%.2d%.2d%.2d%.3d",
		Time.GetYear(),
		Time.GetMonth(),
		Time.GetDay(),
		Time.GetHour(),
		Time.GetMinute(),
		Time.GetSecond(),
		Time.GetMillisecond());

	FString FileName = "AssetsManagerLog_"+ TimeStr + "_"
		+ ClassFilterComboDisplayText->GetText().ToString()
		+ L"_"
		+ CategoryFilterComboDisplayText->GetText().ToString()
		+ L"_"
		+ UsageFilterComboDisplayText->GetText().ToString()
		+ (ReverseCondition ? L"(R)":L"");
	
	// CSV output
	FString CSVFilePath = ASSETSMANAGER_LOGFOLDER + FileName + ".csv";

	if (FFileHelper::SaveStringToFile(CSVContent, *CSVFilePath,
		FFileHelper::EEncodingOptions::ForceUnicode,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append))
	{
		WritingTask.EnterProgressFrame(.1f * TaskNum);
		WritingTask.Destroy();

#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功输出文件到") + CSVFilePath);
#else
		NtfyMsgLog(TEXT("Successfully saved assets manager log to ") + FilePath);
#endif

		UAssetsChecker::OpenLocalFolder(ASSETSMANAGER_LOGFOLDER);

		return FReply::Handled();
	};

	WritingTask.EnterProgressFrame(.1f * TaskNum);
	WritingTask.Destroy();

#ifdef ZH_CN
	NtfyMsgLog(TEXT("输出文件到") + CSVFilePath + TEXT("失败"));
#else
	NtfyMsgLog(TEXT("Failed saving assets manager log to ") + FilePath);
#endif
	;	return FReply::Handled();
	
}

TSharedRef<SButton> SManagerSlateTab::ConstructOpenLocalLogFolderButton()
{
	this->OpenLocalLogFolderButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnOpenLocalLogFolderButtonClicked)
		.ContentPadding(FMargin(1.f))
		.OnHovered(FSimpleDelegate::CreateLambda([this]() {this->OpenLocalLogFolderButton->SetColorAndOpacity(FColor::White); }))
		.OnPressed(FSimpleDelegate::CreateLambda([this]() {this->OpenLocalLogFolderButton->SetColorAndOpacity(FColor::Cyan); }))
		.OnUnhovered(FSimpleDelegate::CreateLambda([this]() {this->OpenLocalLogFolderButton->SetColorAndOpacity(FColor(200,200,200)); }))
		.ToolTipText(FText::FromString(L"Open Assets Manager log folder."));

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerLogFolder");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);
	
	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		.OverrideScreenSize(FVector2D(4,4))
		[
			ButtonImgComponent
		];

	this->OpenLocalLogFolderButton->SetContent(ButtonImgScaler);

	return this->OpenLocalLogFolderButton.ToSharedRef();
}

FReply SManagerSlateTab::OnOpenLocalLogFolderButtonClicked()
{
	if (!UAssetsChecker::OpenLocalFolder(ASSETSMANAGER_LOGFOLDER))
	{
#ifdef ZH_CN
		NtfyMsgLog(FString::Printf(L"打开日志文件夹失败 %s",*(ASSETSMANAGER_LOGFOLDER)));
#else
		NtfyMsgLog(FString::Printf(L"Open log folder failed. %s", *(ASSETSMANAGER_LOGFOLDER)));
#endif
	};
	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructBatchSizeMapButtonButton()
{
	this->BatchSizeMapButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnBatchSizeMapButtonClicked)
		.ContentPadding(FMargin(1.f))
		.OnHovered(FSimpleDelegate::CreateLambda([this]() {this->BatchSizeMapButton->SetColorAndOpacity(FColor::White); }))
		.OnPressed(FSimpleDelegate::CreateLambda([this]() {this->BatchSizeMapButton->SetColorAndOpacity(FColor::Cyan); }))
		.OnUnhovered(FSimpleDelegate::CreateLambda([this]() {this->BatchSizeMapButton->SetColorAndOpacity(FColor(200, 200, 200)); }))
		.ToolTipText(FText::FromString(L"Batch SizeMap"));

	const FSlateBrush* ButtonImg = FAssetsMangerStyle::GetStyleSet()->GetBrush("ContentBrowser.ManagerSizeMap");
	TSharedRef<SImage> ButtonImgComponent = SNew(SImage).Image(ButtonImg);

	TSharedRef<SScaleBox> ButtonImgScaler =
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFitY)
		.OverrideScreenSize(FVector2D(4, 4))
		[
			ButtonImgComponent
		];

	this->BatchSizeMapButton->SetContent(ButtonImgScaler);

	return this->BatchSizeMapButton.ToSharedRef();
}

FReply SManagerSlateTab::OnBatchSizeMapButtonClicked()
{
	
	TArray<TSharedPtr<FAssetData>> SelectedItems = CustomTableList->GetSelectedItems();

	if (SelectedItems.Num() == 0)
	{
#ifdef ZH_CN
		NtfyMsgLog(L"未选择任何资产");
#else
		NtfyMsgLog(L"No asset selected.");
#endif
		return FReply::Handled();
	}

	UAssetsChecker::OpenSizeMapUI(SelectedItems);

	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructBatchRenameButton()
{
	this->BatchRenameButton =
		SNew(SButton)
		.OnClicked(this, &SManagerSlateTab::OnBatchRenameButtonClicked)
		.ContentPadding(FMargin(5.f));

#ifdef ZH_CN
	this->BatchRenameButton->SetContent(ConstructTextForButtons(TEXT("批量重命名已选择的资产")));
#else
	this->BatchRenameButton->SetContent(ConstructTextForButtons(TEXT("-- Batch Rename Selected Assets --")));
#endif

	return this->BatchRenameButton.ToSharedRef();
}

FReply SManagerSlateTab::OnBatchRenameButtonClicked()
{
	TSharedPtr<SDockTab> DockerGenerated = FGlobalTabmanager::Get()->TryInvokeTab(FName(TABNAME_BATCHRENAME));

	DockerGenerated->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda(
		[this](TSharedRef<SDockTab>)
		{ 
			RefreshAssetsListView(false);
		}
	));

	EAppReturnType::Type result = DlgMsg(EAppMsgType::YesNo, "Close?");

	if (result == EAppReturnType::Yes)
	{
		DockerGenerated->RequestCloseTab();
	}

	return FReply::Handled();
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructDropDownMenuBox()
{

	DropDownContent =
		SNew(SHorizontalBox);

	DropDownContent->AddSlot()
		.FillWidth(.25f)
		.Padding(FMargin(2.f))
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
						.Font(GetFontInfo(12))
						.Text(FText::FromString(CLASSFILTER))
						.Justification(ETextJustify::Right)
						.ColorAndOpacity(FColor::White)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(.5f)
				[
					ConstructClassFilterButton()
				]
		];

	DropDownContent->AddSlot()
		.FillWidth(.12f)
		.Padding(FMargin(2.f))
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
						.Font(GetFontInfo(12))
						.Text(FText::FromString(CATEGORYFILTER))
						.Justification(ETextJustify::Right)
						.ColorAndOpacity(FColor::White)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(.5f)
				[
					ConstructCategoryFilterButton()
				]
		];


	DropDownContent->AddSlot()
		.FillWidth(.2f)
		.Padding(FMargin(2.f))
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
						.Font(GetFontInfo(12))
						.Text(FText::FromString(USAGEFILTER))
						.Justification(ETextJustify::Right)
						.ColorAndOpacity(FColor::White)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(.5f)
				[
					ConstructUsageFilterButton()
				]

		];

	DropDownContent->AddSlot()
		.FillWidth(.05f)
		.Padding(FMargin(2.f))
		[
			ConstructReverseConditionCheckBox()
		];

	DropDownContent->AddSlot()
		.FillWidth(.05f)
		.Padding(FMargin(2.f))
		[
			ConstructDetailModeBox()
		];

	DropDownContent->AddSlot()
		.FillWidth(.05f)
		.Padding(FMargin(2.f))
		[
			ConstructOnlyCheckBox()
		];

	TSharedRef<SHorizontalBox> DropDownMenu =

		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		[
			ConstructOverlayOpaque(DropDownContent,3)
		];

	return DropDownMenu;
}

void SManagerSlateTab::UpdateDisplayListSource(bool bRefreshTableHeader)
{
	/*-------------------------- Class filter----------------------------*/

	UpdateClassFilterList(ClassFilterCurrent);

	// Construct Only Texture Selections
	{
		TArray<TSharedPtr<FString>> OnlyTextureCollection =
		{
			UsageSelectionMaxInGameSizeError,
			UsageSelectionSourceSizeError,
			UsageSelectionTextureSubfixError,
			UsageSelectionTextureSettinsError,
			UsageSelectionTextureLODGroupError
		};

		if (m_ClassCheckState == Texture)
		{
			for (TSharedPtr<FString>& TextureSelect : OnlyTextureCollection)
			{
				if (!UsageFilterComboSourceItems.Contains(TextureSelect))
				{
					UsageFilterComboSourceItems.Add(TextureSelect);
				}
			}
		}
		else
		{
			for (TSharedPtr<FString>& TextureSelect : OnlyTextureCollection)
			{
				if (UsageFilterComboSourceItems.Contains(TextureSelect))
				{
					UsageFilterComboSourceItems.Remove(TextureSelect);
				}
			}
		}
	}

	// Construct SkeletalMesh Selections
	{
		TArray<TSharedPtr<FString>> OnlySkeletalCollection =
		{
		};

		if (m_ClassCheckState == SkeletalMesh)
		{
			for (TSharedPtr<FString>& SkeletalSelect : OnlySkeletalCollection)
			{
				if (!UsageFilterComboSourceItems.Contains(SkeletalSelect))
				{
					UsageFilterComboSourceItems.Add(SkeletalSelect);
				}
			}
		}
		else
		{
			for (TSharedPtr<FString>& SkeletalSelect : OnlySkeletalCollection)
			{
				if (UsageFilterComboSourceItems.Contains(SkeletalSelect))
				{
					UsageFilterComboSourceItems.Remove(SkeletalSelect);
				}
			}
		}
	}

	/*-------------------------- Category filter----------------------------*/

	UpdateCategoryFilterList();


	/*-------------------------- Usage filter----------------------------*/

	UpdateUsageFilterAssetData();
	
	/*-------------------------- Usage filter----------------------------*/
	UpdateSearchablbeBox();

	UAssetsChecker::ECopyAssetsPtrList(SListViewSearchFilterAssetData, SListViewAssetData);

	/*-------------------------- Dynamic Buttons Component----------------------------*/

	ConstructDynamicHandleAllBox();

	/*-------------------------- Strict Check Component----------------------------*/

	if (m_UsageCheckState == MaxInGameSizeError || m_UsageCheckState == SourceSizeError)
	{
		if (!bTextureSizeCheckStrictCheckBoxConstructed)
		{
			ConstructTextureSizeStrictCheckBox(
				bTextureSizeCheckStrictMode ? 
				ECheckBoxState::Checked : ECheckBoxState::Unchecked);
			
			DropDownContent->InsertSlot(3)
				.FillWidth(.05f)
				.Padding(FMargin(2.f))
				[
					TextureSizeCheckStrictBox.ToSharedRef()
				];

			bTextureSizeCheckStrictCheckBoxConstructed = true;
		}
	}
	else
	{
		if (bTextureSizeCheckStrictCheckBoxConstructed)
		{
			DropDownContent->RemoveSlot(TextureSizeCheckStrictBox.ToSharedRef());
			bTextureSizeCheckStrictCheckBoxConstructed = false;
		}
	}

	if (m_UsageCheckState == Unused)
	{
		if (!bRecursiveRefBoxConstructed)
		{
			ConstructRecursiveRefCheckBox(
				bRecursiveRefMode ?
				ECheckBoxState::Checked : ECheckBoxState::Unchecked);

			DropDownContent->InsertSlot(3)
				.FillWidth(.05f)
				.Padding(FMargin(2.f))
				[
					RecursiveRefHorizontalBox.ToSharedRef()
				];

			bRecursiveRefBoxConstructed = true;
		}
	}
	else
	{
		if (bRecursiveRefBoxConstructed)
		{
			DropDownContent->RemoveSlot(RecursiveRefHorizontalBox.ToSharedRef());
			bRecursiveRefBoxConstructed = false;
		}
	}

	/*-------------------------- Reverse Check Component----------------------------*/

	if (ReverseCondition)
	{
		ReverseConditionCheckBox->SetIsChecked(ECheckBoxState::Unchecked);
		ReverseCondition = false;
	}

	RefreshAssetsListView(bRefreshTableHeader);
}

#pragma endregion

#pragma region SComboListFilter

TSharedRef<SComboBox<TSharedPtr<FString>>> SManagerSlateTab::ConstructClassFilterButton()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> ClassFilterButton =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ClassFilterComboSourceItems)
		.OnGenerateWidget(this, &SManagerSlateTab::OnGenerateClassFilterButton)
		.OnSelectionChanged(this, &SManagerSlateTab::OnClassFilterButtonChanged)
		[
			SAssignNew(ClassFilterComboDisplayText, STextBlock)
				.Text(FText::FromString(CLASS_LISTALL))
		];
		
	this->ClassFilterComboBox = ClassFilterButton.ToSharedPtr();

	return ClassFilterButton;
}

TSharedRef<SWidget> SManagerSlateTab::OnGenerateClassFilterButton(
	TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SManagerSlateTab::UpdateClassFilterList(
	TSharedPtr<FString> SelectedOption)
{
	if (*SelectedOption.Get() == CLASS_LISTALL)
	{
		UAssetsChecker::ECopyAssetsPtrList(StoredAssetsData, SListViewClassFilterAssetData);

	}
	else
	{
		TArray<TSharedPtr<FAssetData>> NewAssetViewList;

		for (const TSharedPtr<FAssetData>& AssetD : StoredAssetsData)
		{
			FString DisplayAssetClass;

			if (AssetD->GetClass())
			{
				DisplayAssetClass = AssetD->GetClass()->GetName();
			}
			else
			{
				DisplayAssetClass = "[Undefined]";
			}

			if (DisplayAssetClass == *SelectedOption.Get())
			{
				if (!NewAssetViewList.Contains(AssetD))
				{
					NewAssetViewList.AddUnique(AssetD);
				}
			}

		}

		UAssetsChecker::ECopyAssetsPtrList(NewAssetViewList, SListViewClassFilterAssetData);
		
	}

	// Update count
	ClassListViewCountBlock->SetText(FText::FromString(FString::FromInt(SListViewClassFilterAssetData.Num())));
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SManagerSlateTab::ConstructCategoryFilterButton()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> CategoryFilterButton =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&CategoryFilterComboSourceItems)
		.OnGenerateWidget(this, &SManagerSlateTab::OnGenerateCategoryFilterButton)
		.OnSelectionChanged(this, &SManagerSlateTab::OnCategoryFilterButtonChanged)
		[
			SAssignNew(CategoryFilterComboDisplayText, STextBlock)
				.Text(FText::FromString(CATEGORY_LISTALL))
		];

	this->CategoryFilterComboBox = CategoryFilterButton.ToSharedPtr();

	return CategoryFilterButton;
}

TSharedRef<SWidget> SManagerSlateTab::OnGenerateCategoryFilterButton(
	TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SManagerSlateTab::OnCategoryFilterButtonChanged(
	TSharedPtr<FString> SelectedOption, 
	ESelectInfo::Type InSelectInfo)
{
	CategoryFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));

	CategoryFilterCurrent = SelectedOption;

	// add option

	m_CategoryCheckState = AssetCategory::LastCatergory;

	if(*SelectedOption.Get() == CATEGORY_NOGROUP)
	{
		m_CategoryCheckState = AssetCategory::Undefined;
	}

	if (*SelectedOption.Get() == CATEGORY_CHARACTER)
	{
		m_CategoryCheckState = AssetCategory::Character;
	}

	if (*SelectedOption.Get() == CATEGORY_HAIR)
	{
		m_CategoryCheckState = AssetCategory::Hair;
	}

	if (*SelectedOption.Get() == CATEGORY_UI)
	{
		m_CategoryCheckState = AssetCategory::UI;
	}

	if (*SelectedOption.Get() == CATEGORY_SCENE)
	{
		m_CategoryCheckState = AssetCategory::Scene;
	}

	if (*SelectedOption.Get() == CATEGORY_EFFECT)
	{
		m_CategoryCheckState = AssetCategory::Effect;
	}

	UpdateDisplayListSource();
	RefreshAssetsListView();
}

void SManagerSlateTab::UpdateCategoryFilterList()
{
	if (m_CategoryCheckState == AssetCategory::LastCatergory)
	{
		UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewCategoryFilterAssetData);
	}
	else
	{
		TArray<TSharedPtr<FAssetData>> NewAssetsList;
		NewAssetsList.Empty();

		for (TSharedPtr<FAssetData> AssetData : SListViewClassFilterAssetData)
		{
			FCustomStandardAssetData StandardAsset(*AssetData);

			if (StandardAsset.Get().GetCommonAssetCategory() == m_CategoryCheckState)
			{
				NewAssetsList.AddUnique(AssetData);
			};
		}

		UAssetsChecker::ECopyAssetsPtrList(NewAssetsList, SListViewCategoryFilterAssetData);
	}
}

void SManagerSlateTab::OnClassFilterButtonChanged(
	TSharedPtr<FString> SelectedOption, 
	ESelectInfo::Type InSelectInfo)
{
	ClassFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));

	ClassFilterCurrent = SelectedOption;
	
	// add option

	m_ClassCheckState = DefaultClassCheckState;
	
	if (*SelectedOption.Get() == UTexture2D::StaticClass()->GetName() ||
		*SelectedOption.Get() == UTexture2DArray::StaticClass()->GetName())
	{
		m_ClassCheckState = Texture;
	}
	
	if (*SelectedOption.Get() == USkeletalMesh::StaticClass()->GetName())
	{
		m_ClassCheckState = SkeletalMesh;

	}

	UpdateDisplayListSource();
	RefreshAssetsListView();
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SManagerSlateTab::ConstructUsageFilterButton()
{
	
	this->UsageFilterComboBox =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&UsageFilterComboSourceItems)
		.OnGenerateWidget(this,&SManagerSlateTab::OnGenerateUsageFilterButton)
		.OnSelectionChanged(this,&SManagerSlateTab::OnUsageFilterButtonChanged)
		[
			SAssignNew(UsageFilterComboDisplayText,STextBlock)
				.Text(FText::FromString(USAGE_NONE))
		];

	UsageFilterCurrent = USAGE_NONE;

	return this->UsageFilterComboBox.ToSharedRef();
}

TSharedRef<SWidget> SManagerSlateTab::OnGenerateUsageFilterButton(
	TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SManagerSlateTab::OnUsageFilterButtonChanged(
	TSharedPtr<FString> SelectedOption, 
	ESelectInfo::Type InSelectInfo)
{
	UsageFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));

	UsageFilterCurrent = *SelectedOption.Get();

	if (UsageFilterCurrent == USAGE_NONE)
	{
		m_UsageCheckState = DefaultUsageCheckState;
	}

	if (UsageFilterCurrent == USAGE_UNUSED)
	{
		m_UsageCheckState = Unused;
	}

	if (UsageFilterCurrent == USAGE_PREFIXERROR)
	{
		m_UsageCheckState = PrefixError;
	}

	if (UsageFilterCurrent == USAGE_SAMENAMEASSETERROR)
	{
		m_UsageCheckState = SameNameAssetError;
	}

	if (UsageFilterCurrent == USAGE_MAXINGAMESIZEERROR)
	{
		m_UsageCheckState = MaxInGameSizeError;
	}

	if (UsageFilterCurrent == USAGE_SOURCESIZEERROR)
	{
		m_UsageCheckState = SourceSizeError;
	}

	if (UsageFilterCurrent == USAGE_TEXTURESUBFIXERROR)
	{
		m_UsageCheckState = SubfixError;
	}

	if (UsageFilterCurrent == USAGE_TEXTURESETTINGSERROR)
	{
		m_UsageCheckState = TextureSettingsError;
	}

	if (UsageFilterCurrent == USAGE_TEXTUREGROUPERROR)
	{
		m_UsageCheckState = TextureGroupError;
	}

	UpdateDisplayListSource();
	RefreshAssetsListView();
}

void SManagerSlateTab::UpdateUsageFilterAssetData()
{
	if (m_UsageCheckState == DefaultUsageCheckState)
	{
		UAssetsChecker::ECopyAssetsPtrList(SListViewCategoryFilterAssetData, SListViewUsageFilterAssetData);
	}

	if (m_UsageCheckState == Unused)
	{
		//UAssetsChecker::FilterUnusedAssetsForAssetList(SListViewCategoryFilterAssetData, SListViewUsageFilterAssetData);
		UAssetsChecker::FilterUnusedAssetsForAssetList(
			SListViewCategoryFilterAssetData, 
			SListViewUsageFilterAssetData, 
			StoredFolderPaths,
			bRecursiveRefMode);
	}

	if (m_UsageCheckState == PrefixError)
	{
		UAssetsChecker::FilterPrefixErrorAssetsForAssetList(SListViewCategoryFilterAssetData, SListViewUsageFilterAssetData);
	}


	if (m_UsageCheckState == SameNameAssetError)
	{
		UAssetsChecker::FilterSameNameErrorAssetsForAssetList(SListViewCategoryFilterAssetData, SListViewUsageFilterAssetData);
	}


	if (m_UsageCheckState == MaxInGameSizeError)
	{
		UAssetsChecker::FilterMaxInGameSizeErrorAssetsForAssetList(
			SListViewCategoryFilterAssetData,
			SListViewUsageFilterAssetData, 
			bTextureSizeCheckStrictMode);
	}

	if (m_UsageCheckState == SourceSizeError)
	{
		UAssetsChecker::FilterSourceSizeErrorAssetsForAssetList(
			SListViewCategoryFilterAssetData,
			SListViewUsageFilterAssetData, 
			bTextureSizeCheckStrictMode);
	}


	if (m_UsageCheckState == SubfixError)
	{

		UAssetsChecker::FilterTextureSubfixErrorAssetsForAssetList(
			SListViewCategoryFilterAssetData,
			SListViewUsageFilterAssetData);
	}


	if (m_UsageCheckState == TextureSettingsError)
	{

		UAssetsChecker::FilterTextureSettingsErrorAssetsForAssetList(
			SListViewCategoryFilterAssetData,
			SListViewUsageFilterAssetData);
	}

	if (m_UsageCheckState == TextureGroupError)
	{
		UAssetsChecker::FilterTextureLODGroupErrorAssetsForAssetList(SListViewCategoryFilterAssetData, SListViewUsageFilterAssetData);
	}
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructTextureSizeStrictCheckBox(
	ECheckBoxState State)
{
	TextureSizeCheckStrictBox =
		SNew(SHorizontalBox);

	// bTextureSizeCheckStrictCheckBoxConstructed = true;

	TextureSizeCheckStrictCheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.Padding(FMargin(3.f))
		.HAlign(HAlign_Center)
		.IsChecked(State)
		.Visibility(EVisibility::Visible)
		.OnCheckStateChanged(this, &SManagerSlateTab::OnTextureSizeStrictCheckBoxStateChanged);

	TextureSizeCheckStrictBox ->AddSlot()
		.AutoWidth()
		.Padding(FMargin(2.f))
		[
			TextureSizeCheckStrictCheckBox.ToSharedRef()
		];

	TextureSizeCheckStrictBox ->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#ifdef ZH_CN
			ConstructNormalTextBlock(TEXT("严格筛选"), GetFontInfo(12))
#else
			ConstructNormalTextBlock(TEXT("Strict Filter"), GetFontInfo(12))
#endif
		];

	return 	TextureSizeCheckStrictBox.ToSharedRef();
}

void SManagerSlateTab::OnTextureSizeStrictCheckBoxStateChanged(
	ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	{
		if (bTextureSizeCheckStrictMode)
		{
			bTextureSizeCheckStrictMode = false;

			if (ReverseConditionCheckBox->GetCheckedState() == ECheckBoxState::Checked)
			{
				ReverseConditionCheckBox->ToggleCheckedState();
			}

			UpdateDisplayListSource();
			RefreshAssetsListView(false);
		}
		break;
	}
	case ECheckBoxState::Checked:
	{
		if (!bTextureSizeCheckStrictMode)
		{
			bTextureSizeCheckStrictMode = true;

			if (ReverseConditionCheckBox->GetCheckedState() == ECheckBoxState::Checked)
			{
				ReverseConditionCheckBox->ToggleCheckedState();
			}

			UpdateDisplayListSource();
			RefreshAssetsListView(false);
		}
		break;
	}
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructRecursiveRefCheckBox(ECheckBoxState State)
{
	RecursiveRefHorizontalBox =
		SNew(SHorizontalBox);

	RecursiveRefCheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.Padding(FMargin(3.f))
		.HAlign(HAlign_Center)
		.IsChecked(State)
		.Visibility(EVisibility::Visible)
		.OnCheckStateChanged(this, &SManagerSlateTab::OnRecursiveRefCheckBoxStateChanged);

	RecursiveRefHorizontalBox->AddSlot()
		.AutoWidth()
		.Padding(FMargin(2.f))
		[
			RecursiveRefCheckBox.ToSharedRef()
		];

	RecursiveRefHorizontalBox->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#ifdef ZH_CN
			ConstructNormalTextBlock(TEXT("递归引用"), GetFontInfo(12))
#else
			ConstructNormalTextBlock(TEXT("Recursive Ref"), GetFontInfo(12))
#endif
		];

	return 	RecursiveRefHorizontalBox.ToSharedRef();
}

void SManagerSlateTab::OnRecursiveRefCheckBoxStateChanged(
	ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	{
		if (bRecursiveRefMode)
		{
			bRecursiveRefMode = false;
			UpdateDisplayListSource();
			RefreshAssetsListView();
		}

		break;
	}
	case ECheckBoxState::Checked:
	{
		if (!bRecursiveRefMode)
		{
			bRecursiveRefMode = true;
			UpdateDisplayListSource();
			RefreshAssetsListView();
		}

		break;
	}
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructReverseConditionCheckBox()
{
	TSharedPtr<SHorizontalBox> HorizontalBox =
		SNew(SHorizontalBox);

	ReverseConditionCheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.Padding(FMargin(3.f))
		.HAlign(HAlign_Center)
		.IsChecked(ECheckBoxState::Unchecked)
		.Visibility(EVisibility::Visible)
		.OnCheckStateChanged(this, &SManagerSlateTab::OnReverseConditionCheckBoxStateChanged);

	HorizontalBox->AddSlot()
		.AutoWidth()
		.Padding(FMargin(2.f))
		[
			ReverseConditionCheckBox.ToSharedRef()
		];

	HorizontalBox->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#ifdef ZH_CN
			ConstructNormalTextBlock(TEXT("条件反选"), GetFontInfo(12))
#else
			ConstructNormalTextBlock(TEXT("ReverseCondition"), GetFontInfo(12))
#endif
		];

	return HorizontalBox.ToSharedRef();
}

void SManagerSlateTab::OnReverseConditionCheckBoxStateChanged(
	ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	{
		if (ReverseCondition)
		{
			ReverseCondition = false;

			UAssetsChecker::ECopyAssetsPtrList(ReversedTempArray, SListViewUsageFilterAssetData);
			ReversedTempArray.Empty();
			UpdateSearchablbeBox();
			UAssetsChecker::ECopyAssetsPtrList(SListViewSearchFilterAssetData, SListViewAssetData);
			RefreshAssetsListView(false);

		}
		break;
	}

	case ECheckBoxState::Checked:
	{
		if (!ReverseCondition)
		{
			ReverseCondition = true;

			UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, ReversedTempArray);

			TArray<TSharedPtr<FAssetData>> RestArray;
			RestArray.Empty();

			for (TSharedPtr<FAssetData> assetData : SListViewCategoryFilterAssetData)
			{
				if(!SListViewUsageFilterAssetData.Contains(assetData))
				{
					RestArray.Add(assetData);
				}
			}

			UAssetsChecker::ECopyAssetsPtrList(RestArray, SListViewUsageFilterAssetData);
			UpdateSearchablbeBox();
			UAssetsChecker::ECopyAssetsPtrList(SListViewSearchFilterAssetData, SListViewAssetData);
			RefreshAssetsListView(false);
		}
		break;
	}

	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructDetailModeBox()
{
	TSharedPtr<SHorizontalBox> HorizontalBox = 
		SNew(SHorizontalBox);

	HorizontalBox->AddSlot()
		.AutoWidth()
		.Padding(FMargin(2.f))
		[
			SNew(SCheckBox)
				.Type(ESlateCheckBoxType::CheckBox)
				.Padding(FMargin(3.f))
				.HAlign(HAlign_Center)
				.IsChecked(ECheckBoxState::Unchecked)
				.Visibility(EVisibility::Visible)
				.OnCheckStateChanged(this, &SManagerSlateTab::OnDetailModeCheckBoxStateChanged)
		];

	HorizontalBox->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#ifdef ZH_CN
			ConstructNormalTextBlock(TEXT("详细信息"),GetFontInfo(12))
#else
			ConstructNormalTextBlock(TEXT("DetailMode"), GetFontInfo(12))
#endif
		];

	return HorizontalBox.ToSharedRef();
}

void SManagerSlateTab::OnDetailModeCheckBoxStateChanged(
	ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	{
		DetailMode = false;
		RefreshAssetsListView();
		break;
	}
		
	case ECheckBoxState::Checked:
	{
		DetailMode = true;
		RefreshAssetsListView();
		break;
	}
		
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructOnlyCheckBox()
{
	TSharedPtr<SHorizontalBox> HorizontalBox =
		SNew(SHorizontalBox);

	HorizontalBox->AddSlot()
		.AutoWidth()
		.Padding(FMargin(2.f))
		[
			SNew(SCheckBox)
				.Type(ESlateCheckBoxType::CheckBox)
				.Padding(FMargin(3.f))
				.HAlign(HAlign_Center)
				.IsChecked(ECheckBoxState::Unchecked)
				.Visibility(EVisibility::Visible)
				.OnCheckStateChanged(this, &SManagerSlateTab::OnOnlyCheckBoxStateChanged)
		];

	HorizontalBox->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#ifdef ZH_CN
			ConstructNormalTextBlock(TEXT("仅检查"), GetFontInfo(12))
#else
			ConstructNormalTextBlock(TEXT("CheckOnly"), GetFontInfo(12))
#endif
		];

	return HorizontalBox.ToSharedRef();
}

void SManagerSlateTab::OnOnlyCheckBoxStateChanged(ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	{
		OnlyCheck = false;
		RefreshAssetsListView();
		ConstructDynamicHandleAllBox();
		break;
	}

	case ECheckBoxState::Checked:
	{
		OnlyCheck = true;
		RefreshAssetsListView();
		ConstructDynamicHandleAllBox();
		break;
	}

	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}

TSharedRef<SVerticalBox> SManagerSlateTab::ConstructSearchableBox()
{
	ListSearchBox = SNew(SSearchBox)
		.OnTextChanged(this, &SManagerSlateTab::OnSearchTextChanged)
		.ToolTipText(FText::FromString(L"Input Tag to filter current list."));

	return SNew(SVerticalBox) + SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(2.0f))
		[
			ListSearchBox.ToSharedRef()
		];
}

void SManagerSlateTab::OnSearchTextChanged(const FText& InSearchText)
{
	SearchString = InSearchText.ToString();

	//UpdateDisplayListSource();
	UpdateSearchablbeBox();
	UAssetsChecker::ECopyAssetsPtrList(SListViewSearchFilterAssetData, SListViewAssetData);
	RefreshAssetsListView();
}

void SManagerSlateTab::UpdateSearchablbeBox()
{
	if (SearchString.IsEmpty())
	{
		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewSearchFilterAssetData);
		return;
	}

	SListViewSearchFilterAssetData.Empty();

	for (TSharedPtr<FAssetData>& AssetD : SListViewUsageFilterAssetData)
	{
		FString AssetName = AssetD->AssetName.ToString();

		if(UAssetsChecker::StringMatchPattern(SearchString,AssetName))
		{
			SListViewSearchFilterAssetData.Add(AssetD);
		}
	}
}

#pragma endregion