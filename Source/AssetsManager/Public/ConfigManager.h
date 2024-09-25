#pragma once

#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"
#include "StandardAsset/FStructLikeString.h"

#define ModuleLanguage_ZH 1

#if ModuleLanguage_ZH
#define ZH_CN
#else
#define EN
#endif

#define VNAME(value) (#value)

#define ModuleConfigMaster TEXT("/AssetsManager")

#define ASSETSMANAGER_LOGFOLDER FPaths::ProjectDir() + "/Saved/AssetsManager/"

#define ASSETSMANAGER_CONFIGFOLDER FPaths::ProjectDir() + "/Plugins/AssetsManager/Config/"

/**
 * 
 */
class ASSETSMANAGER_API FConfigManager
{

public:

	FConfigManager(FConfigManager&) = delete;
	
	static FConfigManager& Get();

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

	TArray<FString> GenerateStructKeyValueArray(
		const TCHAR* SectionTag,
		const FName& ArrayKeyTag,
		const FString& SearchKey);

	TSharedPtr<FString> FindInSectionStructArray(
		const TCHAR* SectionTag,
		const FName& ArrayKeyTag,
		const FString& SearchKey,
		const FString& ConditionKey,
		const FString& ToFindKey);

private:
	FConfigManager();
	virtual ~FConfigManager() = default;

private:

	FString ConfigPath;
	FString DefaultConfigPath;
};