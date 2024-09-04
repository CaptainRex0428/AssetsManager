// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define ASSETSMANAGER_CONFIGFOLDER FPaths::ProjectDir() + "/Plugins/AssetsManager/Config/"

/**
 * 
 */
class ASSETSMANAGER_API ConfigManager
{
public:
	ConfigManager(ConfigManager&) = delete;
	
	static ConfigManager& Get();

	void SetConfigPath(
		const FString& FilePath);	
	void SetConfigDefault();
	TArray<FString> GetConfigsFullPath();
	TArray<FString> GetConfigsFileName();

	const TMap<UClass*, FString> GetUClassTagMap();
	const TMap<UClass*, FString> GetUClassPrefixMap();

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