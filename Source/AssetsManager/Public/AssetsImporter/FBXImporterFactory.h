// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/FbxFactory.h"

#include "FBXImporterFactory.generated.h"

/**
 * 
 */
UCLASS()
class ASSETSMANAGER_API UFBXImporterFactory : public UFbxFactory
{
	GENERATED_BODY()
	
public:
		
	UFBXImporterFactory();

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
		const FString& Filename, 
		UObject* InParent, 
		FName InName);
};
