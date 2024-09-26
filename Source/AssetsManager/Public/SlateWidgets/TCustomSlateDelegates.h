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

	typedef TDelegate<void()> FOnTableCheckBoxStateChanged;
	typedef TDelegate<void(ArgumentType&)> FOnTableRowMouseButtonDoubleClicked;

	// for custom table header row
	typedef TDelegate<TSharedRef<SHeaderRow>(TSharedPtr<SHeaderRow>&)> FOnGenerateTableHeaderRow;

	// for custom table row

	typedef TDelegate<TSharedRef<SWidget>(const FName&,ArgumentType&)> FOnGenerateTableRowColumn;

	// for custom text

	typedef TDelegate<FText(ArgumentType&)> FOnItemToFText;
	typedef TDelegate<bool(const FText&,ETextCommit::Type,ArgumentType&)> FOnItemCommit;

	// for BatchRename

	typedef TDelegate<void()> FOnApply;
};
