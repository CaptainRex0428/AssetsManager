#pragma once

#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"

#define ModuleConfigMaster TEXT("/AssetsManager")

#define ASSETSMANAGER_CONFIGFOLDER FPaths::ProjectDir() + "/Plugins/AssetsManager/Config/"

/**
 * 
 */
class ASSETSMANAGER_API UConfigManager
{

public:

	UConfigManager(UConfigManager&) = delete;
	
	static UConfigManager& Get();

	double SToD(const FString& string);
	float SToF(const FString& string);
	int SToI(const FString& string);

	void SetConfigPath(
		const FString& FilePath);	
	void SetConfigDefault();
	TArray<FString> GetConfigsFullPath();
	TArray<FString> GetConfigsFileName();

	const FConfigSection * GetSection(
		const TCHAR * SectionTag);

	const FName * GetSectionKey(
		const TCHAR* SectionTag,
		const FConfigValue& ValueTag);

	const FConfigValue * GetSectionValue(
		const TCHAR* SectionTag,
		const FName & KeyTag);

	TArray<FConfigValue> GetSectionValuesArray(
		const TCHAR* SectionTag,
		const FName& KeyTag);

private:
	UConfigManager();
	virtual ~UConfigManager() = default;

private:

	FString ConfigPath;
	FString DefaultConfigPath;
};