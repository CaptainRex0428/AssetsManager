// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"

#include "ManagerLogger.h"

FConfigManager& FConfigManager::Get()
{
	static FConfigManager instance;
	return instance;
}

double FConfigManager::SToD(const FString& string)
{
	return FCString::Atod(*string);
}

float FConfigManager::SToF(const FString& string)
{
	return FCString::Atof(*string);
}

int FConfigManager::SToI(const FString& string)
{
	return FCString::Atoi(*string);
}

void FConfigManager::SetConfigPath(
	const FString& FilePath)
{
	this->ConfigPath = FilePath;
	RefreshConfig();
}

void FConfigManager::SetConfigDefault()
{
	SetConfigPath(DefaultConfigPath);
}

TArray<FString> FConfigManager::GetConfigsFullPath()
{
	TArray<FString> PathArray;
	
	FString FolderPath = ASSETSMANAGER_CONFIGFOLDER;
	FPaths::NormalizeDirectoryName(FolderPath);
	IFileManager& FileManager = IFileManager::Get();
	FString FindPattern = FolderPath / TEXT("*");
	FileManager.FindFiles(PathArray,*FindPattern,true,true);

	return PathArray;
}

TArray<FString> FConfigManager::GetConfigsFileName()
{
	TArray<FString> FileNameArray;
	TArray<FString> FilePathArray = GetConfigsFullPath();

	for (FString & path : FilePathArray)
	{
		FileNameArray.Add (FPaths::GetCleanFilename(path));
	}

	return FileNameArray;
}

const TMap<UClass*, FString> FConfigManager::GetUClassTagMap()
{
	return this->UClassTagMap;
}

const TMap<UClass*, FString> FConfigManager::GetUClassPrefixMap()
{
	return this->UClassPrefixMap;
}

const FConfigSection* FConfigManager::GetSection(
	const TCHAR* SectionTag)
{ 
	GConfig->LoadFile(ConfigPath);

	const FConfigSection* section =
		GConfig->GetSection(SectionTag, false, ConfigPath);

	return section;
}

const FName * FConfigManager::GetSectionKey(
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

const FConfigValue* FConfigManager::GetSectionValue(
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

TArray<FString> FConfigManager::GetArrayValue(
	const TCHAR* SectionTag, 
	const TCHAR* KeyTag)
{
	TArray<FString> OutArr;

	NtfyMsg(FString::FromInt(GConfig->GetArray(SectionTag, KeyTag, OutArr, ConfigPath)));

	return OutArr;
}

FConfigManager::FConfigManager()
	:ConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini"),
	DefaultConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini")
{
	RefreshConfig();
}

void FConfigManager::RefreshConfig()
{
	
}
