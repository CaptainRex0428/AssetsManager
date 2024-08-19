// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateWidgets/SCommonSlate.h"

/**
 * 
 */
class ASSETSMANAGER_API SCustomTable : public SCommonSlate
{	

public:
	SLATE_BEGIN_ARGS(SCustomTable) {}
	
	SLATE_END_ARGS()

public:
	virtual void Construct(const SCustomTable::FArguments& InArgs);
};



