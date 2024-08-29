// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/Material/MaterialCreatorSlate.h"

#include "AssetsManager.h"
#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"
#include "SlateBasics.h"

#include "AssetsManagerConfig.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "StandardAsset/FCustomStandardAssetData.h"

void SMaterialCreatorSlate::Construct(const FArguments& InArgs) 
{
	bCanSupportFocus = true;

	ChildSlot
	[
		SNew(SVerticalBox)

		+SVerticalBox::Slot()
		[
			SNew(SOverlay)

			+SOverlay::Slot()
			[
				ConstructNormalTextBlock(InArgs._TitleText, GetFontInfo(24))
			]
		]

		+SVerticalBox::Slot()
		[
			SNew(SSplitter)
			.Orientation(Orient_Vertical)

			+SSplitter::Slot()
			[
				SNew(SButton)
			]

			+SSplitter::Slot()
			[
				SNew(SButton)
			]
		]
	];
}
