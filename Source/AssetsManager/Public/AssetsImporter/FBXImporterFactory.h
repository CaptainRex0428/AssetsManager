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
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
		bool& bOutOperationCanceled) override
	{
		UE_LOG(LogTemp, Log, TEXT("Starting custom FBX import: %s"), *Filename);

		// �Զ����߼���������֤�ļ�·�����޸ĵ���ѡ���¼����Ŀ�ʼʱ���

		UObject* ImportedAsset = Super::FactoryCreateFile(InClass, InParent, InName, Flags, Filename, Parms, Warn, bOutOperationCanceled);

		// ��������󣬿���������ִ�������߼��������¼�����������������ʲ������ߴ�����������

		UE_LOG(LogTemp, Log, TEXT("Completed custom FBX import: %s"), *Filename);
		return ImportedAsset;
	}
	
	
};
