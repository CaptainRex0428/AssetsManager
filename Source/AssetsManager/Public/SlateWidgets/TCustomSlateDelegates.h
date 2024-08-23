// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
template< typename ArgumentType >
class TCustomSlateDelegates : public TSlateDelegates<ArgumentType>
{
public:
	// for custom table
	typedef TDelegate<TArray<TSharedPtr<SWidget>>(ArgumentType)> FOnConstructRowWidgets;
	typedef TDelegate<void()> FOnTableCheckBoxStateChanged;
	typedef TDelegate<void(ArgumentType)> FOnTableRowMouseButtonDoubleClicked;

	// for custom text
	typedef TDelegate<FText(ArgumentType)> FOnItemToFText;
	typedef TDelegate<bool(const FText&,ETextCommit::Type,ArgumentType)> FOnItemCommit;
};
