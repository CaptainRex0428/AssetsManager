// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCommonSlate.h"

#define TABNAME_BATCHRENAME "BatchRename"

/**
 *
 */
template <typename ItemType>
class ASSETSMANAGER_API SBatchRename : public SCommonSlate
{
public:
	SLATE_BEGIN_ARGS(SBatchRename<ItemType>):
		_ItemIn()
		{}
	SLATE_ARGUMENT(TArray<ItemType>, ItemIn)
	SLATE_END_ARGS()

public:
	inline void Construct(
		const SBatchRename<ItemType>::FArguments& InArgs)
	{
		FString OutputStr;

		for (auto a : InArgs._ItemIn)
		{
			OutputStr += a->AssetName.ToString();
			OutputStr += "\n";
		}

		ChildSlot
			[
				ConstructNormalTextBlock(OutputStr,GetFontInfo(9))
			];
	}

};
