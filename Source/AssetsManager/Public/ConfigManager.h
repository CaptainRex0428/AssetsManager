// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"

#define ASSETSMANAGER_CONFIGFOLDER FPaths::ProjectDir() + "/Plugins/AssetsManager/Config/"

/**
 * 
 */
class ASSETSMANAGER_API ConfigManager
{
public:
	ConfigManager(ConfigManager&) = delete;
	
	static ConfigManager& Get();

	double SToD(const FString& string);
	float SToF(const FString& string);
	int SToI(const FString& string);

	void SetConfigPath(
		const FString& FilePath);	
	void SetConfigDefault();
	TArray<FString> GetConfigsFullPath();
	TArray<FString> GetConfigsFileName();

	const TMap<UClass*, FString> GetUClassTagMap();
	const TMap<UClass*, FString> GetUClassPrefixMap();

	const FConfigSection* GetSection(
		const TCHAR * SectionTag);

	const FName * GetSectionKey(
		const TCHAR* SectionTag,
		const FConfigValue& ValueTag);

	const FName* GetSectionKey(
		const FConfigSection* Section,
		const FConfigValue& ValueTag);

	const FConfigValue* GetSectionValue(
		const TCHAR* SectionTag,
		const FName & KeyTag);

	const FConfigValue* GetSectionValue(
		const FConfigSection* Section,
		const FName& KeyTag);

private:
	ConfigManager();
	virtual ~ConfigManager() = default;

	void RefreshConfig();

private:
	FString ConfigPath;
	FString DefaultConfigPath;

	TMap<UClass*, FString> UClassTagMap;
	TMap<UClass*, FString> UClassPrefixMap;
};