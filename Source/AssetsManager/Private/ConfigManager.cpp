// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"
#include "Misc/ConfigCacheIni.h"

ConfigManager& ConfigManager::Get()
{
	static ConfigManager instance;
	return instance;
}

ConfigManager::ConfigManager()
	:ConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini")
{
}

float ConfigManager::Test()
{
	const FConfigSection* section =
		GConfig->GetSection(TEXT("/AssetsManager/AssetsStandatd"), false, ConfigPath);
	
	auto a = section->Find("testfloat");

	return FCString::Atof(*a->GetValue());
}