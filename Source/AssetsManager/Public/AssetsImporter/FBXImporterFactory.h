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

		// 自定义逻辑，例如验证文件路径，修改导入选项，记录导入的开始时间等

		UObject* ImportedAsset = Super::FactoryCreateFile(InClass, InParent, InName, Flags, Filename, Parms, Warn, bOutOperationCanceled);

		// 导入结束后，可以在这里执行其他逻辑，例如记录导入结果，处理依赖资产，或者触发后续操作

		UE_LOG(LogTemp, Log, TEXT("Completed custom FBX import: %s"), *Filename);
		return ImportedAsset;
	}
	
	
};
