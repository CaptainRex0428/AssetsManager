// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"
#include "Misc/ConfigCacheIni.h"

ConfigManager& ConfigManager::Get()
{
	static ConfigManager instance;
	return instance;
}

void ConfigManager::SetConfigPath(
	const FString& FilePath)
{
	this->ConfigPath = FilePath;
	RefreshConfig();
}

void ConfigManager::SetConfigDefault()
{
	SetConfigPath(DefaultConfigPath);
}

TArray<FString> ConfigManager::GetConfigsFullPath()
{
	TArray<FString> PathArray;
	
	FString FolderPath = ASSETSMANAGER_CONFIGFOLDER;
	FPaths::NormalizeDirectoryName(FolderPath);
	IFileManager& FileManager = IFileManager::Get();
	FString FindPattern = FolderPath / TEXT("*");
	FileManager.FindFiles(PathArray,*FindPattern,true,true);

	return PathArray;
}

TArray<FString> ConfigManager::GetConfigsFileName()
{
	TArray<FString> FileNameArray;
	TArray<FString> FilePathArray = GetConfigsFullPath();

	for (FString & path : FilePathArray)
	{
		FileNameArray.Add (FPaths::GetCleanFilename(path));
	}

	return FileNameArray;
}

const TMap<UClass*, FString> ConfigManager::GetUClassTagMap()
{
	return this->UClassTagMap;
}

const TMap<UClass*, FString> ConfigManager::GetUClassPrefixMap()
{
	return this->UClassPrefixMap;
}

ConfigManager::ConfigManager()
	:ConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini"),
	DefaultConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini")
{
	RefreshConfig();
}

void ConfigManager::RefreshConfig()
{
	
}
