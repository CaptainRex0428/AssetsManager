﻿
#include "AssetsImporter/CustomFBXImporterFactory.h"

#include "StandardAsset/CustomStandardAsset.h"
#include "ManagerLogger.h"
#include "InterchangeManager.h"
#include "InterchangeSourceData.h"
#include "InterchangeTranslatorBase.h"
#include "InterchangeMeshUtilities.h"
#include "Engine/Engine.h"


UCustomFBXImporterFactory::UCustomFBXImporterFactory()
{
	SupportedClass = USkeletalMesh::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	Formats.Add(TEXT("fbx;FBX Model"));
	ImportPriority = DefaultImportPriority + 10;
}

UObject* UCustomFBXImporterFactory::FactoryCreateFile(
	UClass* InClass, 
	UObject* InParent, 
	FName InName, 
	EObjectFlags Flags, 
	const FString& Filename, 
	const TCHAR* Parms, 
	FFeedbackContext* Warn, 
	bool& bOutOperationCanceled)
{

	MsgLog(FString::Printf(TEXT("Starting custom FBX import: %s"), *Filename));


	if (ShouldCancelImport(InClass,InParent,InName,Flags,Filename,Parms,Warn,bOutOperationCanceled))
	{
		NtfyMsgLog(FString::Printf(TEXT("Import canceled for file: %s"), *Filename));
		bOutOperationCanceled = true;
		return nullptr;
	}

	UObject* ImportedAsset = Super::FactoryCreateFile(InClass, InParent, InName, Flags, Filename, Parms, Warn, bOutOperationCanceled);

	MsgLog(FString::Printf(TEXT("Completed custom FBX import: %s"), *Filename));
	return ImportedAsset;
}

bool UCustomFBXImporterFactory::ShouldCancelImport(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	const FString& Filename,
	const TCHAR* Parms,
	FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	return false;
}