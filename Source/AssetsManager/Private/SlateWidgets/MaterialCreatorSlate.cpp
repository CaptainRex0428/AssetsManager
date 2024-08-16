// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/MaterialCreatorSlate.h"

#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

#include "AssetsManagerConfig.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "StandardAsset/StandardAsset.h"

void SMaterialCreatorSlate::Construct(const FArguments& InArgs) 
{
	bCanSupportFocus = true;

	ChildSlot
	[
		SNew(SVerticalBox)

		+SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[

			ConstructTitleTextBlock(InArgs._TitleText, GetFontInfo(24))
		]

		+SVerticalBox::Slot()
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			.PhysicalSplitterHandleSize(20.f)

			+SSplitter::Slot()
			.MinSize(300.f)
			[
				SNew(SVerticalBox)

				+SVerticalBox::Slot()
				.AutoHeight()
				[
					ConstructHeader()
				]

				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
				]
			]

			+SSplitter::Slot()
			.MinSize(100.f)
			[
				SNew(SButton)
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			ConstructTitleTextBlock("FOOTER", GetFontInfo(16))
		]
	];
}

TSharedRef<SWidget> SMaterialCreatorSlate::ConstructPropertyTable()
{
	FPropertyEditorModule& PropertyEditorModule 
		= FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	TSharedRef<IPropertyTable> table = PropertyEditorModule.CreatePropertyTable();
	TSharedRef<SWidget> widget = PropertyEditorModule.CreatePropertyTableWidget(table);

	return widget;
}

TSharedRef<SHeaderRow> SMaterialCreatorSlate::ConstructHeader()
{
	TSharedRef<SHeaderRow>  Header =
		SNew(SHeaderRow)
		+SHeaderRow::Column(FName("A"))
		.DefaultLabel(FText::FromString(TEXT("A")))

		+SHeaderRow::Column(FName("B"))
		.DefaultLabel(FText::FromString(TEXT("B")))

		+SHeaderRow::Column(FName("C"))
		.DefaultLabel(FText::FromString(TEXT("C")));

	return Header;
}


