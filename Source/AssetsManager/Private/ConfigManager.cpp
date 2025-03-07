// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"

#include "ManagerLogger.h"

UManagerConfig& UManagerConfig::Get()
{
	static UManagerConfig instance;
	return instance;
}

double UManagerConfig::SToD(const FString& string)
{
	return FCString::Atod(*string);
}

float UManagerConfig::SToF(const FString& string)
{
	return FCString::Atof(*string);
}

int UManagerConfig::SToI(const FString& string)
{
	return FCString::Atoi(*string);
}

void UManagerConfig::SetConfigPath(
	const FString& FilePath)
{
	this->ConfigPath = FilePath;
	ConfigPath = FConfigCacheIni::NormalizeConfigIniPath(ConfigPath);
}

void UManagerConfig::SetConfigDefault()
{
	SetConfigPath(DefaultConfigPath);
}

TArray<FString> UManagerConfig::GetConfigsFullPath()
{
	TArray<FString> PathArray;
	
	FString FolderPath = ASSETSMANAGER_CONFIGFOLDER;
	FPaths::NormalizeDirectoryName(FolderPath);
	IFileManager& FileManager = IFileManager::Get();
	FString FindPattern = FolderPath / TEXT("*");
	FileManager.FindFiles(PathArray,*FindPattern,true,true);

	return PathArray;
}

TArray<FString> UManagerConfig::GetConfigsFileName()
{
	TArray<FString> FileNameArray;
	TArray<FString> FilePathArray = GetConfigsFullPath();

	for (FString & path : FilePathArray)
	{
		FileNameArray.Add (FPaths::GetCleanFilename(path));
	}

	return FileNameArray;
}

const FConfigSection* UManagerConfig::GetSection(
	const TCHAR* SectionTag)
{ 
	GConfig->LoadFile(ConfigPath);

	const FConfigSection* section =
		GConfig->GetSection(SectionTag, false, ConfigPath);

	return section;
}

const FName * UManagerConfig::GetSectionKey(
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

const FConfigValue* UManagerConfig::GetSectionValue(
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

TArray<FConfigValue> UManagerConfig::GetSectionValuesArray(
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

TArray<FString> UManagerConfig::GenerateStructKeyValueArray(
	const TCHAR* SectionTag,
	const FName& ArrayKeyTag,
	const FString& SearchKey)
{
	TArray<FConfigValue> StructLikeValueArray =
		GetSectionValuesArray(SectionTag, ArrayKeyTag);

	if (StructLikeValueArray.Num() == 0)
	{
		return TArray<FString>();
	}

	TArray<FString> OutArr;

	for (FConfigValue& value : StructLikeValueArray)
	{
		FStructLikeString JudgeString(value.GetValue());

		if(JudgeString.IsStructLike())
		{
			const FString * FoundValue 
				= JudgeString.GetMap().Find(SearchKey);

			if (FoundValue && !FoundValue->IsEmpty())
			{
				OutArr.Add(*FoundValue);
			}
		}
	}

	return OutArr;
}
	

TSharedPtr<FString> UManagerConfig::FindInSectionStructArray(
	const TCHAR* SectionTag, 
	const FName& ArrayKeyTag, 
	const FString& SearchKey, 
	const FString& ConditionKey, 
	const FString& ToFindKey)
{
	TArray<FConfigValue> StructLikeValueArray = 
		GetSectionValuesArray(SectionTag, ArrayKeyTag);

	if (StructLikeValueArray.Num() == 0)
	{
		return nullptr;
	}
	
	for (FConfigValue& value : StructLikeValueArray)
	{
		FStructLikeString JudgeString(value.GetValue());
		
		if(!JudgeString.IsStructLike())
		{
			continue;
		}

		TMap<FString, FString> JudgeMap = JudgeString.GetMap();

		const FString * KeyFound = JudgeMap.Find(SearchKey);
		const FString * ValueFound = JudgeMap.Find(ToFindKey);

		if(!KeyFound || !ValueFound)
		{
			continue;
		}

		if(ConditionKey == *KeyFound)
		{
			
			return MakeShared<FString>(*ValueFound);
		}
	}

	return nullptr;
}

UManagerConfig::UManagerConfig()
	:ConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini"),
	DefaultConfigPath(ASSETSMANAGER_CONFIGFOLDER + "AssetsManager.ini")
{
	GConfig->LoadFile(ConfigPath);
	GConfig->NormalizeConfigIniPath(ConfigPath);
	ConfigPath = FConfigCacheIni::NormalizeConfigIniPath(ConfigPath);
}
