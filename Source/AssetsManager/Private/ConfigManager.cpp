// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"

#include "ManagerLogger.h"

UConfigManager& UConfigManager::Get()
{
	static UConfigManager instance;
	return instance;
}

double UConfigManager::SToD(const FString& string)
{
	return FCString::Atod(*string);
}

float UConfigManager::SToF(const FString& string)
{
	return FCString::Atof(*string);
}

int UConfigManager::SToI(const FString& string)
{
	return FCString::Atoi(*string);
}

void UConfigManager::SetConfigPath(
	const FString& FilePath)
{
	this->ConfigPath = FilePath;
	ConfigPath = FConfigCacheIni::NormalizeConfigIniPath(ConfigPath);
}

void UConfigManager::SetConfigDefault()
{
	SetConfigPath(DefaultConfigPath);
}

TArray<FString> UConfigManager::GetConfigsFullPath()
{
	TArray<FString> PathArray;
	
	FString FolderPath = ASSETSMANAGER_CONFIGFOLDER;
	FPaths::NormalizeDirectoryName(FolderPath);
	IFileManager& FileManager = IFileManager::Get();
	FString FindPattern = FolderPath / TEXT("*");
	FileManager.FindFiles(PathArray,*FindPattern,true,true);

	return PathArray;
}

TArray<FString> UConfigManager::GetConfigsFileName()
{
	TArray<FString> FileNameArray;
	TArray<FString> FilePathArray = GetConfigsFullPath();

	for (FString & path : FilePathArray)
	{
		FileNameArray.Add (FPaths::GetCleanFilename(path));
	}

	return FileNameArray;
}

const FConfigSection* UConfigManager::GetSection(
	const TCHAR* SectionTag)
{ 
	GConfig->LoadFile(ConfigPath);

	const FConfigSection* section =
		GConfig->GetSection(SectionTag, false, ConfigPath);

	return section;
}

const FName * UConfigManager::GetSectionKey(
	const TCHAR* SectionTag, 
	const FConfigValue& ValueTag)
{
	const FConfigSection* section = GetSection(SectionTag);

	if (!section)
	{
		return nullptr;
	}

	return section->FindKey(ValueTag);
}

const FConfigValue* UConfigManager::GetSectionValue(
	const TCHAR* SectionTag,
	const FName& KeyTag)
{
	const FConfigSection* section = GetSection(SectionTag);

	if (!section)
	{
		return nullptr;
	}

	return section->Find(KeyTag);
}

TArray<FConfigValue> UConfigManager::GetSectionValuesArray(
	const TCHAR* SectionTag, 
	const FName& KeyTag)
{
	const FConfigSection* section = GetSection(SectionTag);

	TArray<FConfigValue> OutArr;
	OutArr.Empty();

	FString FindKey = KeyTag.ToString();

	if(!FindKey.StartsWith("+"))
	{
		FindKey = "+" + FindKey;
	}

	section->MultiFind(*FindKey, OutArr, true);

	return OutArr;
}

UConfigManager::UConfigManager()
	:ConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini"),
	DefaultConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini")
{
	GConfig->LoadFile(ConfigPath);
	ConfigPath = FConfigCacheIni::NormalizeConfigIniPath(ConfigPath);
}
