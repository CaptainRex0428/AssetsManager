// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"

#define ModuleConfigMaster TEXT("/AssetsManager")

#define ASSETSMANAGER_CONFIGFOLDER FPaths::ProjectDir() + "/Plugins/AssetsManager/Config/"

/**
 * 
 */
class ASSETSMANAGER_API FConfigManager
{
public:
	FConfigManager(FConfigManager&) = delete;
	
	static FConfigManager& Get();

	double SToD(const FString& string);
	float SToF(const FString& string);
	int SToI(const FString& string);

	void SetConfigPath(
		const FString& FilePath);	
	void SetConfigDefault();
	TArray<FString> GetConfigsFullPath();
	TArray<FString> GetConfigsFileName();

	const FConfigSection * GetSection(
		const TCHAR * SectionTag);

	const FName * GetSectionKey(
		const TCHAR* SectionTag,
		const FConfigValue& ValueTag);

	const FConfigValue * GetSectionValue(
		const TCHAR* SectionTag,
		const FName & KeyTag);

	TArray<FString> GetArrayValue(
		const TCHAR* SectionTag,
		const TCHAR* KeyTag
	);

	/*void GetArrayValue(
		const FConfigSection* Section,
		const FName& KeyTag
	);

	void GetStructValue(
		const TCHAR* SectionTag,
		const FName& KeyTag
	);

	void GetStructValue(
		const FConfigSection* Section,
		const FName& KeyTag
	);*/

	const TMap<UClass*, FString> GetUClassTagMap();
	const TMap<UClass*, FString> GetUClassPrefixMap();

private:
	FConfigManager();
	virtual ~FConfigManager() = default;

	void RefreshConfig();

private:
	FString ConfigPath;
	FString DefaultConfigPath;

	TMap<UClass*, FString> UClassTagMap;
	TMap<UClass*, FString> UClassPrefixMap;
};