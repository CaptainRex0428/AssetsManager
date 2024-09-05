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
	enum ReadType
	{
		Config_Int, 
		Config_Float,
		Config_Double,
		Config_Bool,
		Config_Str,
		Config_Struct,
		Config_Array_Str,
		Config_Vector,
		Config_LinerColor,
		Config_Vector4,
		Config_Vector2D
	};

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

	void RefreshConfig();

private:

	FString ConfigPath;
	FString DefaultConfigPath;

	TMap<UClass*, FString> UClassTagMap;
	TMap<UClass*, FString> UClassPrefixMap;
};