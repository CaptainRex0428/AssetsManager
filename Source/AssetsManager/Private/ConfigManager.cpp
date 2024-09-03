// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"

ConfigManager& ConfigManager::Get()
{
	static ConfigManager instance;
	return instance;
}

ConfigManager::ConfigManager()
	:ConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini")
{
	const FConfigSection * section = 
		GConfig->GetSection(TEXT("AssetsManager.AssetsStandard"), false, ConfigPath);
}