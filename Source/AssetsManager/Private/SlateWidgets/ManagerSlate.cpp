// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/ManagerSlate.h"

#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "EditorReimportHandler.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

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
#define USAGE_MAXINGAMESIZEERROR TEXT("MaxInGameSizeError")
#define USAGE_SOURCESIZEERROR TEXT("SourceSizeError")
#define USAGE_PREFIXERROR TEXT("PrefixError")
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

	UsageSelectedDefault = MakeShared<FString>(USAGE_NONE);
	UsageSelectionMaxInGameSizeError = MakeShared<FString>(USAGE_MAXINGAMESIZEERROR);
	UsageSelectionSourceSizeError = MakeShared<FString>(USAGE_SOURCESIZEERROR);

	UsageFilterComboSourceItems.Add(UsageSelectedDefault);
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_UNUSED));
	UsageFilterComboSourceItems.Add(MakeShared<FString>(USAGE_PREFIXERROR));

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
					.FillWidth(.25f)
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
					.FillWidth(.25f)
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
	const FColor& FontColor)
{
	TSharedRef<STextBlock> TextBlock
		= SNew(STextBlock).Text(FText::FromString(StringToDisplay))
		.Justification(ETextJustify::Left)
		.ColorAndOpacity(FontColor)
		.Font(FontInfo)
		.ToolTipText(FText::FromString(StringToDisplay));

	return TextBlock;
}

#pragma endregion

#pragma region OnGenerateRowForlist

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAssetsCheckerTab::ConstructAssetsListView()
{
	ConstructedAssetsListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(36.f)
		.ListItemsSource(&SListViewAssetData)
		.OnGenerateRow(this, &SAssetsCheckerTab::OnGenerateRowForlist);

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
				.FillWidth(0.15f)
				[
					ConstructSingleAssetDeleteButtonBox(AssetDataToDisplay)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(0.15f)
				[
					ConstructBrowserAssetButtonBox(AssetDataToDisplay)
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
				// DisplayPath
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					ConstructAssetPathRowBox(AssetDataToDisplay, ContentTextFont)
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
				
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructBrowserAssetButtonBox(AssetDataToDisplay)
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
				// DisplayPath
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					ConstructAssetPathRowBox(AssetDataToDisplay, ContentTextFont)
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

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.FillWidth(0.12f)
				[
					ConstructBrowserAssetButtonBox(AssetDataToDisplay)
				]
		];

	return ListViewRowWidget;
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

#pragma region ConstructAssetInfo

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetNameRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	TSharedRef<STextBlock> AssetNameBox = ConstructNormalTextBlock(DisplayAssetName, FontInfo);

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

TSharedRef<STextBlock> SAssetsCheckerTab::ConstructAssetPathRowBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay, 
	const FSlateFontInfo& FontInfo)
{
	const FString DisplayAssetPath = AssetDataToDisplay->GetSoftObjectPath().ToString();

	TSharedRef<STextBlock> AssetPathBox = ConstructNormalTextBlock(DisplayAssetPath, FontInfo);
	return AssetPathBox;
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
		.Text(FText::FromString(TEXT("Delete")))
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
		.Text(FText::FromString(TEXT("Debug")))
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
		.Text(FText::FromString(TEXT("Reimport")))
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
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nFaild or no need to fix this texture.");
		}
		else
		{
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nSuccessfully resize to " + FString::FromInt(maxSize));
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
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nFaild or no need to fix this texture.");
		}
		else
		{
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nSuccessfully resize to " + FString::FromInt(maxSize));
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
		if (!UAssetsChecker::EFixTextureMaxSizeInGame(*ClickedAssetData, maxSize,true))
		{
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nFaild or no need to fix this texture.");
		}
		else
		{
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nSuccessfully resize to " + FString::FromInt(maxSize));
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
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nFaild or no need to fix this texture.");
		}
		else
		{
			NtfyMsg(ClickedAssetData->AssetName.ToString() + "\nSuccessfully rest MaxInGameSize to 0.");
		};
	}

	RefreshAssetsListView();

	return FReply::Handled();
}

TSharedRef<SButton> SAssetsCheckerTab::ConstructBrowserAssetButtonBox(
	const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> SingleTextureFixButtonBox =
		SNew(SButton)
		.Text(FText::FromString(TEXT("Target")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked(this,
			&SAssetsCheckerTab::OnBrowserAssetButtonClicked,
			AssetDataToDisplay);

	return SingleTextureFixButtonBox;
}

FReply SAssetsCheckerTab::OnBrowserAssetButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	TArray<FAssetData> AssetDataArray;
	AssetDataArray.Add(*ClickedAssetData);

	GEditor->SyncBrowserToObjects(AssetDataArray);

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

#pragma region FixAllSelectedButton

TSharedRef<SButton> SAssetsCheckerTab::ConstructFixSelectedButton()
{
	TSharedRef<SButton> DeselectAllButton =
		SNew(SButton)
		.OnClicked(this, &SAssetsCheckerTab::OnSelectFixSelectedClicked)
		.ContentPadding(FMargin(5.f));

	DeselectAllButton->SetContent(ConstructTextForButtons("Fix All Selected"));

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

	DlgMsgLog(EAppMsgType::Ok,"Choose a valid check filter type!\nShouldn't be [None].");
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
	
	FixUpRedirectorButton->SetContent(ConstructTextForButtons("-- Fix Up Redirectors In Selected Folders --"));

	return FixUpRedirectorButton;
}

FReply SAssetsCheckerTab::OnFixUpRedirectorButtonClicked()
{
	UAssetsChecker::EFixUpRedirectors(StoredFolderPaths);
	return FReply::Handled();
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
	// for texture to add option

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
		m_UsageCheckState = DefaultUsageCheckState;

		if (SListViewClassFilterAssetData.Num() > 64)
		{
			EAppReturnType::Type result = DlgMsgLog(EAppMsgType::YesNo,
				"The list selected to check is too large.["+FString::FromInt(SListViewClassFilterAssetData.Num()) + " assets]\nFilter unused assets will cost a lot of time.\n\nReady to proceed?");

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