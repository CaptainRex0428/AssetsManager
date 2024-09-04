// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/ManagerSlate.h"
#include "SlateWidgets/SCustomTableRow.h"
#include "AssetsManagerStyle.h"
#include "ConfigManager.h"


#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

#include "AssetsManagerConfig.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "StandardAsset/FCustomStandardAssetData.h"

#include "EditorReimportHandler.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "Components/Image.h"

#include "HAL/FileManager.h"

#include "Engine/Texture.h"

#pragma region Filter

#ifdef ZH_CN
#define CLASSFILTER TEXT("资产类型过滤: ")
#define USAGEFILTER TEXT("条件过滤")
#else
#define CLASSFILTER TEXT("Class Filter: ")
#define USAGEFILTER TEXT("Condition Filter: ")
#endif

#pragma endregion

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
#define CLASS_ANIMATION TEXT("Animation")
#define CLASS_ANIMATIONMONTAGE TEXT("AnimationMontage")
#define CLASS_PHYSICSASSET TEXT("PhysicsAsset")
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

	m_ClassCheckState = DefaultClassCheckState;
	m_UsageCheckState = DefaultUsageCheckState;

	FSlateFontInfo TitleTextFont = GetFontInfo(25);

	StoredFolderPaths = InArgs._SelectedFolderPaths;
	StoredAssetsData = *InArgs._StoredAssetsData;
	
	UAssetsChecker::ECopyAssetsPtrList(StoredAssetsData, SListViewUsageFilterAssetData);
	UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewClassFilterAssetData);
	UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewAssetData);

	ClassFilterDefault = MakeShared<FString>(CLASS_LISTALL);
	ClassFilterCurrent = ClassFilterDefault;
	
	ClassFilterComboSourceItems.Add(ClassFilterDefault);
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
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_ANIMATION));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_ANIMATIONMONTAGE));
	ClassFilterComboSourceItems.Add(MakeShared<FString>(CLASS_PHYSICSASSET));

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
	if (bRefreshTableHeader) ConstructHeaderRow();

	if (CustomTableList.IsValid())
	{
		CustomTableList->RefreshTable(bRefreshTableHeader);
	}

	ListViewCountBlock->SetText(FText::FromString(FString::FromInt(CustomTableList->GetListItems().Num())));
	SelectedCountBlock->SetText(FText::FromString(FString::FromInt(CustomTableList->GetSelectedItems().Num())));
	
}

void SManagerSlateTab::ConstructHeaderRow()
{
	SManagerCustomTableTitleRowColumnsType.Empty();

	SManagerCustomTableTitleRowColumnsType.Add(Column_UClass);
	SManagerCustomTableTitleRowColumnsType.Add(Column_AssetName);

	if (!OnlyCheck)
	{
		SManagerCustomTableTitleRowColumnsType.Add(Column_PerAssetHandle);
	}

	if (m_ClassCheckState == Texture)
	{
		SManagerCustomTableTitleRowColumnsType.Insert(
			m_UsageCheckState == SourceSizeError ? 
			Column_TextureSourceSize :Column_TextureMaxInGameSize, 1);

		if (m_UsageCheckState == TextureGroupError || DetailMode)
		{
			SManagerCustomTableTitleRowColumnsType.Insert(Column_TextureGroup, 3);
		}

		if (m_UsageCheckState == TextureSettingsError || DetailMode)
		{
			SManagerCustomTableTitleRowColumnsType.Insert(Column_TextureSRGB,3);
			
			SManagerCustomTableTitleRowColumnsType.Insert(Column_TextureCompressionSettings,3);
		}
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
			ColumnBoxArgs.FillWidth(0.25f);
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
			TSharedPtr<STextBlock> ClassWidget = ConstructAssetClassRowBox(AssetToDisplay, GetFontInfo(9));
			ClassWidget->SetAutoWrapText(true);
			ClassWidget->SetJustification(ETextJustify::Center);
			ClassWidget->SetMargin(FMargin(3.f));

			return ClassWidget.ToSharedRef();
		}

		case Column_AssetName:
		{
			TSharedPtr<SCustomEditableText<TSharedPtr<FAssetData>>> NameWidget =
				ConstructEditAssetNameRowBox(AssetToDisplay, GetFontInfo(9));

			return NameWidget.ToSharedRef();
		}

		case Column_AssetPath:
		

		case Column_TextureMaxInGameSize:
		{
			TSharedPtr<STextBlock> TextureSizeWidget = ConstructAssetTextureSizeRowBox(AssetToDisplay, GetFontInfo(9));
			TextureSizeWidget->SetAutoWrapText(true);
			TextureSizeWidget->SetJustification(ETextJustify::Center);
			TextureSizeWidget->SetMargin(FMargin(3.f));

			return TextureSizeWidget.ToSharedRef();
		}

		case Column_TextureSourceSize:
		{
			TSharedPtr<STextBlock> TextureSizeWidget = ConstructAssetTextureSizeRowBox(AssetToDisplay, GetFontInfo(9));
			TextureSizeWidget->SetAutoWrapText(true);
			TextureSizeWidget->SetJustification(ETextJustify::Center);
			TextureSizeWidget->SetMargin(FMargin(3.f));

			return TextureSizeWidget.ToSharedRef();
		}

		case Column_TextureCompressionSettings:
		{
			TSharedPtr<STextBlock> TextureCompressionSettingsWidget = ConstructAssetTextureCompressionSettingsRowBox(AssetToDisplay, GetFontInfo(9));
			TextureCompressionSettingsWidget->SetAutoWrapText(true);
			TextureCompressionSettingsWidget->SetJustification(ETextJustify::Center);
			TextureCompressionSettingsWidget->SetMargin(FMargin(3.f));

			return TextureCompressionSettingsWidget.ToSharedRef();
		}

		case Column_TextureSRGB:
		{
			TSharedPtr<STextBlock> TextureSRGBSettingsWidget = ConstructAssetTextureSRGBRowBox(AssetToDisplay, GetFontInfo(9));
			TextureSRGBSettingsWidget->SetAutoWrapText(true);
			TextureSRGBSettingsWidget->SetJustification(ETextJustify::Center);
			TextureSRGBSettingsWidget->SetMargin(FMargin(3.f));

			return TextureSRGBSettingsWidget.ToSharedRef();
		}

		case Column_TextureGroup:
		{
			TSharedPtr<STextBlock> TextureLODGroup = ConstructAssetTextureLODGroupRowBox(AssetToDisplay, GetFontInfo(9));
			TextureLODGroup->SetAutoWrapText(true);
			TextureLODGroup->SetJustification(ETextJustify::Left);
			TextureLODGroup->SetMargin(FMargin(3.f));

			return TextureLODGroup.ToSharedRef();
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
				ConstructSingleAssetDeleteButtonBox(ClickedAssetData)
		];

	DealPanel->AddSlot()
		.HAlign(HAlign_Fill)
		[
			ConstructSingleAssetDebugButtonBox(ClickedAssetData)
		];

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
					ConstructNormalTextBlock(FString::FromInt(StoredAssetsData.Num()), FontInfo, ETextJustify::Left, FColor::Orange)
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

TSharedRef<SCustomEditableText<TSharedPtr<FAssetData>>> SManagerSlateTab::ConstructEditAssetNameRowBox(
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

	return AssetNameBox.ToSharedRef();
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
	
	bool result = UAssetsChecker::ERenameAsset(AssetDataToDisplay, NewName);

	return result;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetNameRowBox(
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

	return AssetNameBox.ToSharedRef();
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetClassRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetClass = AssetDataToDisplay->GetClass()->GetName();

	TSharedRef<STextBlock> AssetClassBox = ConstructNormalTextBlock(DisplayAssetClass,FontInfo);
	return AssetClassBox;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetTextureSizeRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay,
	const FSlateFontInfo& FontInfo)
{
	FVector2D TextureAssetSize;

	if (m_ClassCheckState == Texture && m_UsageCheckState == SourceSizeError)
	{
		TextureAssetSize = UAssetsChecker::EGetTextureAssetSourceSize(*AssetDataToDisplay);
	}
	else
	{
		TextureAssetSize = UAssetsChecker::EGetTextureAssetMaxInGameSize(*AssetDataToDisplay);
	}
	
	const FString ShowString = FString::FromInt(TextureAssetSize.X) + "x" + FString::FromInt(TextureAssetSize.Y);
	TSharedRef<STextBlock> TextureAssetSizeBox = ConstructNormalTextBlock(ShowString, FontInfo);

	return TextureAssetSizeBox;
}

TSharedRef<STextBlock> SManagerSlateTab::ConstructAssetTextureCompressionSettingsRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	TSharedPtr<TextureCompressionSettings> CompressionSettings 
		= UAssetsChecker::EGetTextureAssetCompressionSettings(*AssetDataToDisplay);

	if (CompressionSettings.IsValid())
	{
		const FString ShowString = *TextureCompressionMap.Find(*CompressionSettings);
		TSharedRef<STextBlock> TextureCompressionSettinsBox = ConstructNormalTextBlock(ShowString, FontInfo);
		return TextureCompressionSettinsBox;
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
		= UAssetsChecker::EGetTextureAssetSRGBSettings(*AssetDataToDisplay);

	if (SRGBSettings.IsValid())
	{
		const FString ShowString = *SRGBSettings ? "sRGB" : "NsRGB";
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
		= UAssetsChecker::EGetTextureAssetTextureGroup(*AssetDataToDisplay);

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

#pragma endregion

#pragma region ConstructSingleButton

TSharedRef<SButton> SManagerSlateTab::ConstructSingleAssetDeleteButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleAssetDeleteButtonBox =
		SNew(SButton)
#ifdef ZH_CN
		.Text(FText::FromString(TEXT("删除")))
#else
		.Text(FText::FromString(TEXT("Delete")))
#endif
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleAssetDeleteButtonClicked,
			AssetDataToDisplay);

	return SingleAssetDeleteButtonBox;
}

FReply SManagerSlateTab::OnSingleAssetDeleteButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	if (UAssetsChecker::EGetAssetReferencesPath(ClickedAssetData).Num() > 0)
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

	if (UAssetsChecker::EDeleteAsset(*ClickedAssetData))
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

	if (m_ClassCheckState == Texture)
	{
		if (!UAssetsChecker::EFixTextureMaxSizeInGame(*ClickedAssetData, maxSize, true))
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

	if (m_ClassCheckState == Texture)
	{
		if (!UAssetsChecker::EFixTextureMaxSizeInGame(*ClickedAssetData, maxSize, true))
		{
#ifdef ZH_CN
			NtfyMsg(ClickedAssetData->AssetName.ToString() 
				+ TEXT("\n此贴图无需修复 或 修复失败"));
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

	if (m_ClassCheckState == Texture)
	{
		if (!UAssetsChecker::EFixTextureMaxSizeInGame(*ClickedAssetData, maxSize, true))
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

	if (m_ClassCheckState == Texture)
	{
		if (!UAssetsChecker::EFixTextureMaxSizeInGame(*ClickedAssetData, maxSize, true))
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
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
#ifdef ZH_CN
		.Text(FText::FromString(TEXT("修复")))
#else
		.Text(FText::FromString(TEXT("Fix")))
#endif
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleTextureAssetSettingsFixButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SManagerSlateTab::OnSingleTextureAssetSettingsFixButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	UAssetsChecker::ESetTextureStandardSettings(*ClickedAssetData);
	RefreshAssetsListView();
	return FReply::Handled();
}

TSharedRef<SButton> SManagerSlateTab::ConstructSingleTextureLODGroupStandardFixButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
#ifdef ZH_CN
		.Text(FText::FromString(TEXT("修复")))
#else
		.Text(FText::FromString(TEXT("Fix")))
#endif
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SManagerSlateTab::OnSingleTextureLODGroupStandardFixButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SManagerSlateTab::OnSingleTextureLODGroupStandardFixButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	TSharedPtr<TextureGroup> STLODGroup = UAssetsChecker::EGetTextureLODStandardGroup(*ClickedAssetData,false);

	if (STLODGroup)
	{
		UAssetsChecker::ESetTextureLODGroup(*ClickedAssetData, *STLODGroup);

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
					.FillWidth(.5f)
					.Padding(5.f)
					[
						this->OutputViewListInfoButton.ToSharedRef()
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

	if (UAssetsChecker::EDeleteAssets(AssetsDataToDelete))
	{
		for (TSharedPtr<FAssetData> AssetDataPtr : AssetList)
		{
			SListViewRemoveAssetData(AssetDataPtr);
		}

		RefreshAssetsListView();
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

		bool result = UAssetsChecker::EConfirmPrefixes(AssetsReadyRename, AssetShouldRename);

		if (result)
		{
			TArray<FAssetData> AssetToRename;

			for (TSharedPtr<FAssetData> & Asset : AssetShouldRename)
			{
				AssetToRename.Add(*Asset);
			}

			UAssetsChecker::EAddPrefixes(AssetToRename);
			StoredAssetsData = UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(StoredFolderPaths);
			ConstuctClassFilterList(ClassFilterCurrent);
		}

		RefreshAssetsListView(false);
		return FReply::Handled();
	}

	if (m_UsageCheckState == TextureSettingsError)
	{
		for (TSharedPtr<FAssetData> AssetData : AssetsList)
		{
			if(UAssetsChecker::ESetTextureStandardSettings(*AssetData))
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
			TSharedPtr<TextureGroup> STLODGroup = 
				UAssetsChecker::EGetTextureLODStandardGroup(*AssetData, false);

			if (STLODGroup)
			{
				UAssetsChecker::ESetTextureLODGroup(*AssetData, *STLODGroup);
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
	UAssetsChecker::EFixUpRedirectors(StoredFolderPaths);
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
	// Construct Output

	FString Output;

	Output += L"Path Include:\n";

	for (FString path : StoredFolderPaths)
	{
		Output += path;
		Output += "\n";
	}

	Output += "\n";
	Output += CLASSFILTER;
	Output += L":\n";
	Output += ClassFilterComboDisplayText->GetText().ToString();
	Output += "\n\n";

	Output += USAGEFILTER;
	Output += L":\n";
	Output += UsageFilterComboDisplayText->GetText().ToString();
#ifdef ZH_CN
	Output += ReverseCondition ? TEXT("(反选)") : TEXT("");
#else
	Output += ReverseCondition ? TEXT("(Reverse)") : TEXT("");
#endif
	Output += "\n\n";

	Output += L"Files:\n";
	
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

	float TargetNum = OutputData.Num();

	FSlowTask WritingTask(TargetNum, FText::FromString("Writing Data ..."));
	WritingTask.Initialize();
	WritingTask.MakeDialog();

	for (TSharedPtr<FAssetData> asset: OutputData)
	{
		WritingTask.EnterProgressFrame(.9f);

		FString AssetName = asset->AssetName.ToString();
		FString AssetPath = asset->GetObjectPathString();
		FString AssetClass = asset->GetClass()->GetName();

		FVector2D MaxInGameTextureSize = UAssetsChecker::EGetTextureAssetMaxInGameSize(*asset);
		FVector2D SourceTextureSize = UAssetsChecker::EGetTextureAssetSourceSize(*asset);

		FString MaxInGameTextureSizeString = FString::FromInt(MaxInGameTextureSize.X) + "x" + FString::FromInt(MaxInGameTextureSize.Y);
		FString SourceTextureSizeString = FString::FromInt(SourceTextureSize.X) + "x" + FString::FromInt(SourceTextureSize.Y);

		TArray<FStringFormatArg> args;
		args.Add(FStringFormatArg(AssetClass));
		args.Add(FStringFormatArg(AssetName));
		args.Add(FStringFormatArg(AssetPath));
		args.Add(FStringFormatArg(MaxInGameTextureSizeString));
		args.Add(FStringFormatArg(SourceTextureSizeString));

		if (m_ClassCheckState == Texture)
		{
			Output += FString::Format(TEXT("[{0}] [MaxInGameSize:{3}] [SourceGameSize:{4}] {1} ({2})\n"),args);
		}
		else
		{
			Output += FString::Format(TEXT("[{0}] {1} ({2})\n"), args);
		}
	}

	

	// Output

	// DlgMsg(EAppMsgType::Ok, Output);

	FDateTime Time = FDateTime::Now();
	FString FileName = "AssetsManagerLog_"
		+FString::FromInt(Time.GetYear())
		+ FString::FromInt(Time.GetMonth())
		+ FString::FromInt(Time.GetDay())
		+ FString::FromInt(Time.GetHour())
		+ FString::FromInt(Time.GetMinute())
		+ FString::FromInt(Time.GetSecond())
		+ "_"
		+ FString::FromInt(Time.GetMillisecond())
		+ "_"
		+ ClassFilterComboDisplayText->GetText().ToString()
		+ "_"
		+ UsageFilterComboDisplayText->GetText().ToString();

	FString FilePath = ASSETSMANAGER_LOGFOLDER + FileName + ".log";

	if (FFileHelper::SaveStringToFile(Output, *FilePath,
		FFileHelper::EEncodingOptions::ForceUTF8,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append))
	{
		WritingTask.EnterProgressFrame(.1f * TargetNum);
		WritingTask.Destroy();

#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功输出文件到") + FilePath);
#else
		NtfyMsgLog(TEXT("Successfully saved assets manager log to ") + FilePath);
#endif
		return FReply::Handled();
	};

	WritingTask.EnterProgressFrame(.1f * TargetNum);
	WritingTask.Destroy();

#ifdef ZH_CN
	NtfyMsgLog(TEXT("输出文件到") + FilePath + TEXT("失败"));
#else
	NtfyMsgLog(TEXT("Failed saving assets manager log to ") + FilePath);
#endif
	;	return FReply::Handled();
	
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
		.FillWidth(.25f)
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

void SManagerSlateTab::ConstuctClassFilterList(
	TSharedPtr<FString> SelectedOption)
{
	if (*SelectedOption.Get() == CLASS_LISTALL)
	{
		UAssetsChecker::ECopyAssetsPtrList(StoredAssetsData, SListViewClassFilterAssetData);

	}
	else
	{
		TArray<TSharedPtr<FAssetData>> NewAssetViewList;

		for (TSharedPtr<FAssetData>& AssetD : StoredAssetsData)
		{
			FString assetName = AssetD->GetClass()->GetName();

			UClass* selectClass = *AssetFullNameMap.Find(*SelectedOption.Get());
			FString selectName = selectClass->GetName();

			if (assetName == selectName)
			{
				if (!NewAssetViewList.Contains(AssetD))
				{
					NewAssetViewList.AddUnique(AssetD);
				}
			}

		}

		UAssetsChecker::ECopyAssetsPtrList(NewAssetViewList, SListViewClassFilterAssetData);
		
	}
}

void SManagerSlateTab::OnClassFilterButtonChanged(
	TSharedPtr<FString> SelectedOption, 
	ESelectInfo::Type InSelectInfo)
{
	ClassFilterCurrent = SelectedOption;

	ClassFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));

	ConstuctClassFilterList(ClassFilterCurrent);
	UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewUsageFilterAssetData);
	UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	
	// Update count

	ClassListViewCountBlock->SetText(FText::FromString(FString::FromInt(SListViewClassFilterAssetData.Num())));
	
	// add option

	m_ClassCheckState = DefaultClassCheckState;

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

		if (*SelectedOption.Get() == CLASS_TEXTURE ||
			*SelectedOption.Get() == CLASS_TEXTUREARRAY)
		{
			m_ClassCheckState = Texture;

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

	RefreshAssetsListView();

	//set Usage Filter to default 
	UsageFilterComboBox->SetSelectedItem(UsageSelectedDefault);
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
	
	if (ReverseCondition)
	{
		ReverseConditionCheckBox->SetIsChecked(ECheckBoxState::Unchecked);
		ReverseCondition = false;
	}

	UsageFilterCurrent = *SelectedOption.Get();

	UpdateUsageFilterAssetData(*SelectedOption.Get());

	RefreshAssetsListView();
}

void SManagerSlateTab::UpdateUsageFilterAssetData(const FString& Selection)
{
	if (Selection == USAGE_NONE)
	{
		m_UsageCheckState = DefaultUsageCheckState;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewUsageFilterAssetData);
		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}

	if (Selection == USAGE_UNUSED)
	{
		m_UsageCheckState = Unused;
		ConstructDynamicHandleAllBox();

		if (SListViewClassFilterAssetData.Num() > 64)
		{
#ifdef ZH_CN
			EAppReturnType::Type result = DlgMsgLog(EAppMsgType::YesNo,
				TEXT("选择的文件太多[")
				+ FString::FromInt(SListViewClassFilterAssetData.Num())
				+ TEXT("个文件]\n由于需要查找所有引用项，这将会消耗大量时间!!!\n\n是否继续?"));
#else
			EAppReturnType::Type result = DlgMsgLog(EAppMsgType::YesNo,
				TEXT("The list selected to check is too large.[")
				+ FString::FromInt(SListViewClassFilterAssetData.Num())
				+ TEXT(" assets]\nFilter unused assets will cost a lot of time.\n\nReady to proceed?"));
#endif

			if (result != EAppReturnType::Yes)
			{
				UsageFilterComboBox->SetSelectedItem(UsageSelectedDefault);
				return;
			}
		}

		UAssetsChecker::EListUnusedAssetsForAssetList(SListViewClassFilterAssetData, SListViewUsageFilterAssetData);
		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}


	if (Selection == USAGE_PREFIXERROR)
	{
		m_UsageCheckState = PrefixError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListPrefixErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewUsageFilterAssetData);
		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}


	if (Selection == USAGE_SAMENAMEASSETERROR)
	{
		m_UsageCheckState = SameNameAssetError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListSameNameErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewUsageFilterAssetData);
		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}


	if (Selection == USAGE_MAXINGAMESIZEERROR)
	{
		m_UsageCheckState = MaxInGameSizeError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListMaxInGameSizeErrorAssetsForAssetList(
			SListViewClassFilterAssetData, 
			SListViewUsageFilterAssetData, 
			bTextureSizeCheckStrictMode);

		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}


	if (Selection == USAGE_SOURCESIZEERROR)
	{
		m_UsageCheckState = SourceSizeError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListSourceSizeErrorAssetsForAssetList(
			SListViewClassFilterAssetData, 
			SListViewUsageFilterAssetData, 
			bTextureSizeCheckStrictMode);

		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}


	if (Selection == USAGE_TEXTURESUBFIXERROR)
	{
		m_UsageCheckState = SubfixError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListTextureSubfixErrorAssetsForAssetList(
			SListViewClassFilterAssetData, 
			SListViewUsageFilterAssetData);

		UAssetsChecker::ECopyAssetsPtrList(
			SListViewUsageFilterAssetData, 
			SListViewAssetData);
	}


	if (Selection == USAGE_TEXTURESETTINGSERROR)
	{
		m_UsageCheckState = TextureSettingsError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListTextureSettingsErrorAssetsForAssetList(
			SListViewClassFilterAssetData, 
			SListViewUsageFilterAssetData);

		UAssetsChecker::ECopyAssetsPtrList(
			SListViewUsageFilterAssetData, 
			SListViewAssetData);
	}

	if (Selection == USAGE_TEXTUREGROUPERROR)
	{
		m_UsageCheckState = TextureGroupError;
		ConstructDynamicHandleAllBox();

		UAssetsChecker::EListTextureLODGroupErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewUsageFilterAssetData);
		UAssetsChecker::ECopyAssetsPtrList(SListViewUsageFilterAssetData, SListViewAssetData);
	}

	// dynamic components

	if (Selection == USAGE_MAXINGAMESIZEERROR || Selection == USAGE_SOURCESIZEERROR)
	{
		if (bTextureSizeCheckStrictCheckBoxConstructed)
		{
			DropDownContent->RemoveSlot(TextureSizeCheckStrictBox.ToSharedRef());
		}

		DropDownContent->InsertSlot(2)
			.FillWidth(.05f)
			.Padding(FMargin(2.f))
			[
				ConstructTextureSizeStrictCheckBox(
					bTextureSizeCheckStrictMode ?
					ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			];
	}
	else
	{
		if(bTextureSizeCheckStrictCheckBoxConstructed)
		{
			DropDownContent->RemoveSlot(TextureSizeCheckStrictBox.ToSharedRef());
		}
	}
}

TSharedRef<SHorizontalBox> SManagerSlateTab::ConstructTextureSizeStrictCheckBox(
	ECheckBoxState State)
{
	TextureSizeCheckStrictBox =
		SNew(SHorizontalBox);

	bTextureSizeCheckStrictCheckBoxConstructed = true;

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
			UpdateUsageFilterAssetData(this->UsageFilterCurrent);
			RefreshAssetsListView(false);
		}
		break;
	}
	case ECheckBoxState::Checked:
	{
		if (!bTextureSizeCheckStrictMode)
		{
			bTextureSizeCheckStrictMode = true;
			UpdateUsageFilterAssetData(this->UsageFilterCurrent);
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

			UpdateUsageFilterAssetData(this->UsageFilterCurrent);

			RefreshAssetsListView(false);

		}
		break;
	}

	case ECheckBoxState::Checked:
	{
		if (!ReverseCondition)
		{
			ReverseCondition = true;

			SListViewAssetData.Empty();

			for (TSharedPtr<FAssetData> assetData : SListViewClassFilterAssetData)
			{
				if(!SListViewUsageFilterAssetData.Contains(assetData))
				{
					SListViewAssetData.Add(assetData);
				}
			}

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

#pragma endregion