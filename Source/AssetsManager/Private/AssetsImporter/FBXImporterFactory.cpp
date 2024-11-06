// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardAsset.h"
#include "ManagerLogger.h"
#include "AssetsImporter/FBXImporterFactory.h"

// #include "FbxImporter.h"
#include "InterchangeManager.h"
#include "InterchangeSourceData.h"
#include "InterchangeTranslatorBase.h"
#include "InterchangeMeshUtilities.h"
#include "Engine/Engine.h"


UFBXImporterFactory::UFBXImporterFactory()
{
	SupportedClass = USkeletalMesh::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	Formats.Add(TEXT("fbx;FBX Model"));
	ImportPriority = DefaultImportPriority + 10;
}

UObject* UFBXImporterFactory::FactoryCreateFile(
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


	if (ShouldCancelImport(Filename, InParent, InName))
	{
		NtfyMsgLog(FString::Printf(TEXT("Import canceled for file: %s"), *Filename));
		bOutOperationCanceled = true;
		return nullptr;
	}

	UObject* ImportedAsset = Super::FactoryCreateFile(InClass, InParent, InName, Flags, Filename, Parms, Warn, bOutOperationCanceled);

	MsgLog(FString::Printf(TEXT("Completed custom FBX import: %s"), *Filename));
	return ImportedAsset;
}

bool UFBXImporterFactory::ShouldCancelImport(const FString& Filename, UObject* InParent, FName InName)
{
	return false;
}