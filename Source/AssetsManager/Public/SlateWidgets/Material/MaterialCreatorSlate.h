// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define CONTENTFOLDER_MATERIALCREATORTAB_NAME "MatrialCreator"

#include "SlateWidgets/SCommonSlate.h"

/**
 * 
 */
class SMaterialCreatorSlate : public SCommonSlate
{
	SLATE_BEGIN_ARGS(SMaterialCreatorSlate){}

	SLATE_ARGUMENT(FString, TitleText)

	SLATE_END_ARGS()

public:
	virtual void Construct(
		const FArguments& InArgs);

};
