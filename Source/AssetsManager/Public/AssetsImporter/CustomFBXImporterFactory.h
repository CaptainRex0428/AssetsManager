// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/FbxFactory.h"

#include "CustomFBXImporterFactory.generated.h"

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UCustomFBXImporterFactory : public UFbxFactory
{
	GENERATED_BODY()
	
public:
		
	UCustomFBXImporterFactory();

	virtual UObject* FactoryCreateFile(
		UClass* InClass, 
		UObject* InParent, 
		FName InName, 
		EObjectFlags Flags,
		const FString& Filename, 
		const TCHAR* Parms, 
		FFeedbackContext* Warn,
		bool& bOutOperationCanceled) override;
	
private:
	bool ShouldCancelImport(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		const FString& Filename,
		const TCHAR* Parms,
		FFeedbackContext* Warn,
		bool& bOutOperationCanceled);
};
