// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/ManagerSlate.h"

#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

#include "AssetsManagerConfig.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "EditorReimportHandler.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "HAL/FileManager.h"


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

#else

#define USAGE_NONE TEXT("None")
#define USAGE_UNUSED TEXT("Unused")
#define USAGE_MAXINGAMESIZEERROR TEXT("MaxInGameSizeError")
#define USAGE_SOURCESIZEERROR TEXT("SourceSizeError")
#define USAGE_PREFIXERROR TEXT("PrefixError")
#define USAGE_SAMENAMEASSETERROR TEXT("SameNameError")

#endif

#pragma endregion

void SAssetsCheckerTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	m_ClassCheckState = DefaultClassCheckState;
	m_UsageCheckState = DefaultUsageCheckState;

	FSlateFontInfo TitleTextFont = GetFontInfo(25);

	StoredFolderPaths = InArgs._SelectedFolderPaths;
	StoredAssetsData = InArgs._StoredAssetsData;
	
	UAssetsChecker::ECopyAssetsPtrList(StoredAssetsData, SListViewAssetData);
	UAssetsChecker::ECopyAssetsPtrList(SListViewAssetData, SListViewClassFilterAssetData);

	CheckBoxesArray.Empty();
	AssetsDataSelected.Empty();

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

	UsageFilterComboSourceItems.Add(UsageSelectedDefault);
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_UNUSED));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_PREFIXERROR));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_SAMENAMEASSETERROR));

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

				// InfoBar
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					ConstructInfoBox(StoredFolderPaths,GetFontInfo(12))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
				]

				// drop down menu
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
						
					+SHorizontalBox::Slot()
					.FillWidth(.25f)
					.Padding(FMargin(2.f))
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Font(GetFontInfo(12))
							.Text(FText::FromString(CLASSFILTER))
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

				// Handle Select
				+ SVerticalBox::Slot()
				.AutoHeight()
				[					
					ConstructHandleAllButtons()
				]

				
		];
}

void SAssetsCheckerTab::SListViewRemoveAssetData(TSharedPtr<FAssetData> AssetData)
{
	if (StoredAssetsData.Contains(AssetData))
	{
		StoredAssetsData.Remove(AssetData);
	}

	if (SListViewAssetData.Contains(AssetData))
	{
		SListViewAssetData.Remove(AssetData);
	}

	if (SListViewClassFilterAssetData.Contains(AssetData))
	{
		SListViewClassFilterAssetData.Remove(AssetData);
	}
}

#pragma region FontSet
FSlateFontInfo SAssetsCheckerTab::GetFontInfo(
	float FontSize, 
	const FString& FontName)
{
	FSlateFontInfo font = FCoreStyle::Get().GetFontStyle(FName(FontName));
	font.Size = FontSize;
	return font;
}
#pragma endregion

#pragma region ButtonTextCustruct
TSharedRef<STextBlock> SAssetsCheckerTab::ConstructTextForButtons(
	const FString& TextContent)
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

#pragma region CustructTextBlock

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructNormalTextBlock(
	const FString& StringToDisplay, 
	const FSlateFontInfo& FontInfo, 
	const FColor& FontColor,
	const FString& ToolTip)
{
	TSharedRef<STextBlock> TextBlock
		= SNew(STextBlock).Text(FText::FromString(StringToDisplay))
		.Justification(ETextJustify::Left)
		.ColorAndOpacity(FontColor)
		.Font(FontInfo)
		.ToolTipText(FText::FromString(ToolTip));

	return TextBlock;
}

#pragma endregion

#pragma region OnGenerateRowForlist

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAssetsCheckerTab::ConstructAssetsListView()
{
	FScrollBarStyle scrollbarStyle;
	scrollbarStyle.SetThickness(30);

	ConstructedAssetsListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(36.f)
		.ScrollBarStyle(&scrollbarStyle)
		.ListItemsSource(&SListViewAssetData)
		.OnGenerateRow(this, &SAssetsCheckerTab::OnGenerateRowForlist)
		.OnMouseButtonDoubleClick(this,&SAssetsCheckerTab::OnRowMouseButtonDoubleClicked);

	return ConstructedAssetsListView.ToSharedRef();
}

TSharedRef<ITableRow> SAssetsCheckerTab::OnGenerateRowForlist(
	TSharedPtr<FAssetData> AssetDataToDisplay, 
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if (m_ClassCheckState == Texture )
	{
		if (m_UsageCheckState == SourceSizeError)
		{
			return GenerateTextureRowForList_SourceSizeError(AssetDataToDisplay, OwnerTable);
		}

		return GenerateTextureRowForList_MaxInGameSizeError(AssetDataToDisplay, OwnerTable);
	}

	return GenerateDefaultRowForList(AssetDataToDisplay, OwnerTable);
}

void SAssetsCheckerTab::OnRowMouseButtonDoubleClicked(TSharedPtr<FAssetData> AssetDataToDisplay)
{
	TArray<FAssetData> AssetDataArray;
	AssetDataArray.Add(*AssetDataToDisplay);

	GEditor->SyncBrowserToObjects(AssetDataArray);
}

TSharedRef<STableRow<TSharedPtr<FAssetData>>> SAssetsCheckerTab::GenerateDefaultRowForList(
	TSharedPtr<FAssetData> AssetDataToDisplay, 
	const TSharedRef<STableViewBase>& OwnerTable)
{
	FSlateFontInfo ContentTextFont = GetFontInfo(9);

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget
		= SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(6.f))
		[
			SNew(SHorizontalBox)
				// CheckBox
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.05f)
				[
					ConstructCheckBox(AssetDataToDisplay)
				]
				// DisplayClass
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.15f)
				[
					ConstructAssetClassRowBox(AssetDataToDisplay, ContentTextFont)
				]
				// DisplayName
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.FillWidth(0.35f)
				[
					ConstructAssetNameRowBox(AssetDataToDisplay, ContentTextFont)
				]

				// DisplayButton
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(0.15f)
				[
					ConstructSingleAssetDeleteButtonBox(AssetDataToDisplay)
				]
		];

	return ListViewRowWidget;
}

TSharedRef<STableRow<TSharedPtr<FAssetData>>> SAssetsCheckerTab::GenerateTextureRowForList_MaxInGameSizeError(
	TSharedPtr<FAssetData> AssetDataToDisplay, 
	const TSharedRef<STableViewBase>& OwnerTable)
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
				.FillWidth(0.05f)
				[
					ConstructCheckBox(AssetDataToDisplay)
				]
				// DisplayClass
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.1f)
				[
					ConstructAssetClassRowBox(AssetDataToDisplay, ContentTextFont)
				]
				// DisplaySize
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.15f)
				[
					ConstructAssetTextureSizeRowBox(AssetDataToDisplay, ContentTextFont)
				]
				// DisplayName
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.FillWidth(0.25f)
				[
					ConstructAssetNameRowBox(AssetDataToDisplay, ContentTextFont)
				]

				// DisplayButton
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructSingleAssetDeleteButtonBox(AssetDataToDisplay)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructSingleTextureAsset2KButtonBox(AssetDataToDisplay)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructSingleTextureAsset1KButtonBox(AssetDataToDisplay)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.1f)
				[
					ConstructSingleTextureAsset512ButtonBox(AssetDataToDisplay)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.1f)
				[
					ConstructSingleTextureAssetResetButtonBox(AssetDataToDisplay)
				]
		];

	return ListViewRowWidget;
}

TSharedRef<STableRow<TSharedPtr<FAssetData>>> SAssetsCheckerTab::GenerateTextureRowForList_SourceSizeError(
	TSharedPtr<FAssetData> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable)
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
				.FillWidth(0.05f)
				[
					ConstructCheckBox(AssetDataToDisplay)
				]
				// DisplayClass
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.1f)
				[
					ConstructAssetClassRowBox(AssetDataToDisplay, ContentTextFont)
				]
				// DisplaySize
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.15f)
				[
					ConstructAssetTextureSizeRowBox(AssetDataToDisplay, ContentTextFont)
				]
				// DisplayName
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.FillWidth(0.25f)
				[
					ConstructAssetNameRowBox(AssetDataToDisplay, ContentTextFont)
				]

				// DisplayButton
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructSingleAssetDeleteButtonBox(AssetDataToDisplay)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructSingleAssetReimportButtonBox(AssetDataToDisplay)
				]
		];

	return ListViewRowWidget;
}

void SAssetsCheckerTab::RefreshAssetsListView()
{
	CheckBoxesArray.Empty();
	AssetsDataSelected.Empty();
	
	SelectedCountBlock->SetText(FText::FromString(FString::FromInt(0)));

	if(ConstructedAssetsListView.IsValid())
	{
		ConstructedAssetsListView->RebuildList();
	}

	ListViewCountBlock->SetText(FText::FromString(FString::FromInt(SListViewAssetData.Num())));
}

TSharedRef<SCheckBox> SAssetsCheckerTab::ConstructCheckBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> CheckBoxWidget =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this,&SAssetsCheckerTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);

	CheckBoxesArray.AddUnique(CheckBoxWidget);

	return CheckBoxWidget;
}

void SAssetsCheckerTab::OnCheckBoxStateChanged(
	ECheckBoxState NewState, 
	TSharedPtr<FAssetData> AssetData)
{
	switch(NewState)
	{
	case ECheckBoxState::Unchecked:
		if (AssetsDataSelected.Contains(AssetData))
		{
			AssetsDataSelected.Remove(AssetData);
		}

		SelectedCountBlock->SetText(FText::FromString(FString::FromInt(AssetsDataSelected.Num())));

		break;

	case ECheckBoxState::Checked:
		AssetsDataSelected.AddUnique(AssetData);
		SelectedCountBlock->SetText(FText::FromString(FString::FromInt(AssetsDataSelected.Num())));

		break;

	case ECheckBoxState::Undetermined:
		break;

	default:
		break;
	}
}

#pragma region ConstructAssetInfo

TSharedRef<SVerticalBox> SAssetsCheckerTab::ConstructInfoBox(
	const TArray<FString>& FolderPaths, 
	const FSlateFontInfo& FontInfo)
{
	TSharedRef<SVerticalBox> AssetsListViewInfoBox =
		SNew(SVerticalBox)

		// path slot
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		.Padding(FMargin(3.f))
		[
			ConstructListPathsInfo(FolderPaths, GetFontInfo(9))
		]

		// count slot
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(1.5f))
		[
			ConstructListAssetsCountInfo(FontInfo)
		];

	return AssetsListViewInfoBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructListPathsInfo(
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

	return ConstructNormalTextBlock(PathsInfo, FontInfo, FColor::Cyan);
}

TSharedRef<SHorizontalBox> SAssetsCheckerTab::ConstructListAssetsCountInfo(
	const FSlateFontInfo& FontInfo)
{
	ListViewCountBlock = ConstructNormalTextBlock(FString::FromInt(SListViewAssetData.Num()), FontInfo, FColor::Green);
	SelectedCountBlock = ConstructNormalTextBlock(FString::FromInt(AssetsDataSelected.Num()), FontInfo, FColor::Emerald);
	ClassListViewCountBlock = ConstructNormalTextBlock(FString::FromInt(SListViewClassFilterAssetData.Num()), FontInfo, FColor::Yellow);

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
					ConstructNormalTextBlock(TEXT("总资产数 -> "), FontInfo, FColor::White, TEXT("选择文件夹下的所有资产数量"))
#else
					ConstructNormalTextBlock(TEXT("All Assets -> "), FontInfo, FColor::White, TEXT("All assets in selected folder(s)."))
#endif
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					ConstructNormalTextBlock(FString::FromInt(StoredAssetsData.Num()), FontInfo, FColor::Orange)
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
				ConstructNormalTextBlock(TEXT("所选类(Class)资产数量 -> "), FontInfo, FColor::White, TEXT("所选类型的资产数量"))
#else
				ConstructNormalTextBlock(TEXT("After Class Filter -> "), FontInfo, FColor::White, TEXT("Assets collected after class filter."))
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
					ConstructNormalTextBlock(TEXT("筛选后资产数量 -> "), FontInfo, FColor::White, TEXT("视口中筛选出的素材数量"))
#else
					ConstructNormalTextBlock(TEXT("List Out Assets -> "), FontInfo, FColor::White, TEXT("Assets listed in view port."))
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
				ConstructNormalTextBlock(TEXT("选择资产数量 -> "), FontInfo, FColor::White, TEXT("选择的资产数量"))
#else
				ConstructNormalTextBlock(TEXT("Selected Assets -> "), FontInfo, FColor::White, TEXT("Assets selected count."))
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

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetNameRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	const FString DisplayAssetPath = AssetDataToDisplay->GetSoftObjectPath().ToString();

	TSharedRef<STextBlock> AssetNameBox = ConstructNormalTextBlock(DisplayAssetName, FontInfo,FColor::White,DisplayAssetPath);

	return AssetNameBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetClassRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetClass = AssetDataToDisplay->GetClass()->GetName();

	TSharedRef<STextBlock> AssetClassBox = ConstructNormalTextBlock(DisplayAssetClass,FontInfo);

	return AssetClassBox;
}

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetTextureSizeRowBox(
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

#pragma endregion

#pragma region ConstructSingleButton

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleAssetDeleteButtonBox(
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
			&SAssetsCheckerTab::OnSingleAssetDeleteButtonClicked,
			AssetDataToDisplay);

	return SingleAssetDeleteButtonBox;
}

FReply SAssetsCheckerTab::OnSingleAssetDeleteButtonClicked(
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

		// NtfMsgLog("Clicked OK");
	}

	if (UAssetsChecker::EDeleteAsset(*ClickedAssetData))
	{
		// log
#ifdef ZH_CN
		NtfMsgLog(TEXT("成功删除") + ClickedAssetData->AssetName.ToString());
#else
		NtfMsgLog(TEXT("Successfully deleted ") + ClickedAssetData->AssetName.ToString());
#endif

		// update slistview
		SListViewRemoveAssetData(ClickedAssetData);
		RefreshAssetsListView();
	};

	return FReply::Handled();
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleAssetDebugButtonBox(
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
			&SAssetsCheckerTab::OnSingleAssetDebugButtonClicked, 
			AssetDataToDisplay);

	return SingleAssetDebugButtonBox;
}

FReply SAssetsCheckerTab::OnSingleAssetDebugButtonClicked(
	TSharedPtr<FAssetData> ClickedAssetData)
{
	FVector2D s = UAssetsChecker::EGetTextureAssetSourceSize(*ClickedAssetData);

	NtfyMsg(FString::FromInt(s.X) + "x" + FString::FromInt(s.Y));

	return FReply::Handled();
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleAssetReimportButtonBox(
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
			&SAssetsCheckerTab::OnSingleAssetReimportButtonClicked, 
			AssetDataToDisplay);

	return SingleAssetFixButtonBox;
}

FReply SAssetsCheckerTab::OnSingleAssetReimportButtonClicked(
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

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleTextureAsset2KButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("2K")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SAssetsCheckerTab::OnSingleTextureAsset2KButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SAssetsCheckerTab::OnSingleTextureAsset2KButtonClicked(
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

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleTextureAsset1KButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("1K")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SAssetsCheckerTab::OnSingleTextureAsset1KButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SAssetsCheckerTab::OnSingleTextureAsset1KButtonClicked(
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

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleTextureAsset512ButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("512")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SAssetsCheckerTab::OnSingleTextureAsset512ButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SAssetsCheckerTab::OnSingleTextureAsset512ButtonClicked(
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

TSharedRef<SButton> SAssetsCheckerTab::ConstructSingleTextureAssetResetButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("R")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SAssetsCheckerTab::OnSingleTextureAssetResetButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SAssetsCheckerTab::OnSingleTextureAssetResetButtonClicked(
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

#pragma endregion

#pragma endregion

#pragma region GenerateHandleAllButton

TSharedRef<SVerticalBox> SAssetsCheckerTab::ConstructHandleAllButtons()
{
	TSharedRef<SVerticalBox> HandleBox =
		SNew(SVerticalBox)
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
		+ SVerticalBox::Slot()
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
				.FillWidth(.35f)
				.Padding(5.f)
				[
					ConstructFixSelectedButton()
				]
		]

		// Fix Up Redirectors
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

		// Output
		+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(.5f)
					.Padding(5.f)
					[
						ConstructOutputViewListInfoButton()
					]
			];

	

	return HandleBox;
};

#pragma endregion

#pragma region DeleteAllSelectedButton

TSharedRef<SButton> SAssetsCheckerTab::ConstructDeleteAllSelectedButton()
{
	TSharedRef<SButton> DeleteAllSelectedButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnDeleteAllSelectedButtonClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	DeleteAllSelectedButton->SetContent(ConstructTextForButtons(TEXT("删除选择的资产")));
#else
	DeleteAllSelectedButton->SetContent(ConstructTextForButtons(TEXT("Delete All Selected")));
#endif
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
			SListViewRemoveAssetData(AssetDataPtr);
		}
	}

	RefreshAssetsListView();

	return FReply::Handled();
}

#pragma endregion

#pragma region De/SelectAllButton

TSharedRef<SButton> SAssetsCheckerTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> SelectAllButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnSelectAllButtonClicked)
		.ContentPadding(FMargin(5.f));

#ifdef ZH_CN
	SelectAllButton->SetContent(ConstructTextForButtons(TEXT("全选")));
#else
	SelectAllButton->SetContent(ConstructTextForButtons(TEXT("Select All")));
#endif

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
#ifdef ZH_CN
	DeselectAllButton->SetContent(ConstructTextForButtons(TEXT("取消全选")));
#else
	DeselectAllButton->SetContent(ConstructTextForButtons(TEXT("Deselect All")));
#endif

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

#pragma region FixAllSelectedButton

TSharedRef<SButton> SAssetsCheckerTab::ConstructFixSelectedButton()
{
	TSharedRef<SButton> DeselectAllButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnSelectFixSelectedClicked)
		.ContentPadding(FMargin(5.f));
#ifdef ZH_CN
	DeselectAllButton->SetContent(ConstructTextForButtons(TEXT("修复选择的资产")));
#else
	DeselectAllButton->SetContent(ConstructTextForButtons(TEXT("Fix All Selected")));
#endif

	return DeselectAllButton;
}

FReply SAssetsCheckerTab::OnSelectFixSelectedClicked()
{
	if (m_UsageCheckState == PrefixError)
	{
		TArray<TSharedPtr<FAssetData>> AssetReayRename;
		TArray<TSharedPtr<FAssetData>> AssetShouldRename;

		for (TSharedPtr<FAssetData> AssetData: AssetsDataSelected)
		{
			AssetReayRename.Add(AssetData);
		}

		bool result = UAssetsChecker::EConfirmPrefixes(AssetReayRename, AssetShouldRename);

		if (result)
		{
			TArray<FAssetData> AssetToRename;

			for (TSharedPtr<FAssetData> & Asset : AssetShouldRename)
			{
				AssetToRename.Add(*Asset);

				if (SListViewAssetData.Contains(Asset))
				{
					SListViewAssetData.Remove(Asset);
				}
			}

			UAssetsChecker::EAddPrefixes(AssetToRename);
			StoredAssetsData = UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(StoredFolderPaths);
			// ConstuctClassFilterList(ClassFilterCurrent);
			ConstuctClassFilterList(ClassFilterCurrent);
		}

		RefreshAssetsListView();
		return FReply::Handled();
	}
#ifdef ZH_CN
	DlgMsgLog(EAppMsgType::Ok, FString(USAGEFILTER) + FString(TEXT("选择错误\n应该选择[资产前缀错误]")));
#else
	DlgMsgLog(EAppMsgType::Ok, FString(TEXT("Choose a valid ")) + FString(USAGEFILTER) + FString(TEXT(" type!\nShould be[PrefixError]."))));
#endif
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
#ifdef ZH_CN
	FixUpRedirectorButton->SetContent(ConstructTextForButtons(TEXT("-- 修复所选文件夹中的重定向器(Redirector) --")));
#else
	FixUpRedirectorButton->SetContent(ConstructTextForButtons(TEXT("-- Fix Up Redirectors In Selected Folders --")));
#endif

	return FixUpRedirectorButton;
}

FReply SAssetsCheckerTab::OnFixUpRedirectorButtonClicked()
{
	UAssetsChecker::EFixUpRedirectors(StoredFolderPaths);
	return FReply::Handled();
}
#pragma endregion

#pragma region OutputViewListInfo

TSharedRef<SButton> SAssetsCheckerTab::ConstructOutputViewListInfoButton()
{
	TSharedRef<SButton> FixUpRedirectorButton =
		SNew(SButton)
		.OnClicked(this,&SAssetsCheckerTab::OnOutputViewListInfoButtonClicked)
		.ContentPadding(FMargin(5.f));

#ifdef ZH_CN
	FixUpRedirectorButton->SetContent(ConstructTextForButtons(TEXT("-- 输出列表到文件 --")));
#else
	FixUpRedirectorButton->SetContent(ConstructTextForButtons(TEXT("-- Output view list to log file --")));
#endif

	return FixUpRedirectorButton;
}

FReply SAssetsCheckerTab::OnOutputViewListInfoButtonClicked()
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
	Output += "\n\n";

	Output += L"Files:\n";
	
	for (TSharedPtr<FAssetData> asset: SListViewAssetData)
	{
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
#ifdef ZH_CN
		NtfMsgLog(TEXT("成功输出文件到") + FilePath);
#else
		NtfMsgLog(TEXT("Successfully saved assets manager log to ") + FilePath);
#endif
		return FReply::Handled();
	};

#ifdef ZH_CN
	NtfMsgLog(TEXT("输出文件到") + FilePath + TEXT("失败"));
#else
	NtfMsgLog(TEXT("Failed saving assets manager log to ") + FilePath);
#endif
	;	return FReply::Handled();
	
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
		
	ClassFilterComboBox = ClassFilterButton.ToSharedPtr();

	return ClassFilterButton;
}

TSharedRef<SWidget> SAssetsCheckerTab::OnGenerateClassFilterButton(
	TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SAssetsCheckerTab::ConstuctClassFilterList(
	TSharedPtr<FString> SelectedOption)
{
	if (*SelectedOption.Get() == CLASS_LISTALL)
	{
		UAssetsChecker::ECopyAssetsPtrList(StoredAssetsData, SListViewClassFilterAssetData);

	}
	else
	{
		TArray<TSharedPtr<FAssetData>> NewAssetViewList;

		for (TSharedPtr<FAssetData> AssetD : StoredAssetsData)
		{
			FString assetName = AssetD->GetClass()->GetName();

			UClass* selectClass = *AssetFullNameMap.Find(*SelectedOption.Get());
			FString selectName = selectClass->GetName();

			if (assetName == selectName)
			{
				NewAssetViewList.Add(AssetD);
			}

		}

		UAssetsChecker::ECopyAssetsPtrList(NewAssetViewList, SListViewClassFilterAssetData);
		
	}
}

void SAssetsCheckerTab::OnClassFilterButtonChanged(
	TSharedPtr<FString> SelectedOption, 
	ESelectInfo::Type InSelectInfo)
{
	ClassFilterCurrent = SelectedOption;

	ClassFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));

	ConstuctClassFilterList(ClassFilterCurrent);
	UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewAssetData);
	
	// Update count

	ClassListViewCountBlock->SetText(FText::FromString(FString::FromInt(SListViewClassFilterAssetData.Num())));
	
	// add option

	m_ClassCheckState = DefaultClassCheckState;
	
	if(*SelectedOption.Get() == CLASS_TEXTURE ||
		*SelectedOption.Get() == CLASS_TEXTUREARRAY)
	{
		m_ClassCheckState = Texture;

		if (!UsageFilterComboSourceItems.Contains(UsageSelectionMaxInGameSizeError))
		{
			UsageFilterComboSourceItems.Add(UsageSelectionMaxInGameSizeError);
		}

		if (!UsageFilterComboSourceItems.Contains(UsageSelectionSourceSizeError))
		{
			UsageFilterComboSourceItems.Add(UsageSelectionSourceSizeError);
		}
	}
	else
	{
		if (UsageFilterComboSourceItems.Contains(UsageSelectionMaxInGameSizeError))
		{
			UsageFilterComboSourceItems.Remove(UsageSelectionMaxInGameSizeError);
		}

		if (UsageFilterComboSourceItems.Contains(UsageSelectionSourceSizeError))
		{
			UsageFilterComboSourceItems.Remove(UsageSelectionSourceSizeError);
		}
	}

	RefreshAssetsListView();

	//set Usage Filter to default 
	UsageFilterComboBox->SetSelectedItem(UsageSelectedDefault);
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

	UsageFilterComboBox = UsageFilterButton.ToSharedPtr();

	return UsageFilterButton;
}

TSharedRef<SWidget> SAssetsCheckerTab::OnGenerateUsageFilterButton(
	TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> SourceItemWidget =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return SourceItemWidget;
}

void SAssetsCheckerTab::OnUsageFilterButtonChanged(
	TSharedPtr<FString> SelectedOption, 
	ESelectInfo::Type InSelectInfo)
{
	UsageFilterComboDisplayText->SetText(FText::FromString(*SelectedOption.Get()));
	
	if (*SelectedOption.Get() == USAGE_NONE)
	{
		m_UsageCheckState = DefaultUsageCheckState;
		UAssetsChecker::ECopyAssetsPtrList(SListViewClassFilterAssetData, SListViewAssetData);
	}

	if (*SelectedOption.Get() == USAGE_UNUSED)
	{
		m_UsageCheckState = Unused;

		if (SListViewClassFilterAssetData.Num() > 64)
		{
#ifdef ZH_CN
			EAppReturnType::Type result = DlgMsgLog(EAppMsgType::YesNo,
				TEXT("选择的文件太多[")
				+FString::FromInt(SListViewClassFilterAssetData.Num()) 
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

		UAssetsChecker::EListUnusedAssetsForAssetList(SListViewClassFilterAssetData, SListViewAssetData);
	}

	if (*SelectedOption.Get() == USAGE_PREFIXERROR)
	{
		m_UsageCheckState = PrefixError;
		UAssetsChecker::EListPrefixErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewAssetData);
	}

	if (*SelectedOption.Get() == USAGE_SAMENAMEASSETERROR)
	{
		m_UsageCheckState = SameNameAssetError;
		UAssetsChecker::EListSameNameErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewAssetData);
	}

	if (*SelectedOption.Get() == USAGE_MAXINGAMESIZEERROR)
	{
		m_UsageCheckState = MaxInGameSizeError;
		UAssetsChecker::EListMaxInGameSizeErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewAssetData);
	}

	if (*SelectedOption.Get() == USAGE_SOURCESIZEERROR)
	{
		m_UsageCheckState = SourceSizeError;
		UAssetsChecker::EListSourceSizeErrorAssetsForAssetList(SListViewClassFilterAssetData, SListViewAssetData);
	}

	RefreshAssetsListView();
}

#pragma endregion