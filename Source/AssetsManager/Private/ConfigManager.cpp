// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"

ConfigManager& ConfigManager::Get()
{
	static ConfigManager instance;
	return instance;
}

double ConfigManager::SToD(const FString& string)
{
	return FCString::Atod(*string);
}

float ConfigManager::SToF(const FString& string)
{
	return FCString::Atof(*string);
}

int ConfigManager::SToI(const FString& string)
{
	return FCString::Atoi(*string);
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

const FConfigSection* ConfigManager::GetSection(
	const TCHAR* SectionTag)
{ 
	GConfig->LoadFile(ConfigPath);

	const FConfigSection* section =
		GConfig->GetSection(SectionTag, false, ConfigPath);

	return section;
}

const FName * ConfigManager::GetSectionKey(
	const TCHAR* SectionTag, 
	const FConfigValue& ValueTag)
{
	const FConfigSection* section = GetSection(SectionTag);

	return section->FindKey(ValueTag);
}

const FName* ConfigManager::GetSectionKey(
	const FConfigSection* Section, 
	const FConfigValue& ValueTag)
{
	GConfig->LoadFile(ConfigPath);
	return Section->FindKey(ValueTag);
}

const FConfigValue* ConfigManager::GetSectionValue(
	const TCHAR* SectionTag,
	const FName& KeyTag)
{
	const FConfigSection* section = GetSection(SectionTag);

	return section->Find(KeyTag);
}

const FConfigValue* ConfigManager::GetSectionValue(
	const FConfigSection* Section, 
	const FName& KeyTag)
{
	GConfig->LoadFile(ConfigPath);
	return Section->Find(KeyTag);
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
