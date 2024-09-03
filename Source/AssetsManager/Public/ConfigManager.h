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
	static ConfigManager& Get();

	ConfigManager(ConfigManager&) = delete;

	float Test();
private:
	ConfigManager();
	virtual ~ConfigManager() = default;

private:
	FString ConfigPath;
};