#pragma once

#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"
#include "StandardAsset/FStructLikeString.h"

#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetsChecker/AssetsDisplay.h"

#include "UObject/NoExportTypes.h"

#include "ConfigManager.generated.h"

#define ModuleLanguage_ZH 1

#if ModuleLanguage_ZH
#define ZH_CN
#else
#define EN
#endif

#define VNAME(value) (#value)

#define ModuleConfigMaster TEXT("/AssetsManager")

#define ASSETSMANAGER_LOGFOLDER FPaths::ProjectDir() + L"Saved/AssetsManager/"

#define ASSETSMANAGER_CONFIGFOLDER FPaths::ProjectDir() + L"Plugins/AssetsManager/Config/"

UENUM(BlueprintType)
enum class AssetSizeDisplayUnit : uint8
{
	bit = 0 UMETA(DisplayName="bit"),
	Byte = 1 UMETA(DisplayName="Byte"),
	KB = 2 UMETA(DisplayName="KB"),
	MB = 3 UMETA(DisplayName="MB"),
	GB = 4 UMETA(DisplayName="GB"),
	TB = 5 UMETA(DisplayName="TB"),
	AssetSizeDisplayUnit_Max
};

UENUM(BlueprintType)
enum class AssetsInfoDisplayLevel : uint8
{
	AssetsInfoDisplay_Display = 0 UMETA(DisplayName = "Display"),
	AssetsInfoDisplay_Tip = 1 UMETA(DisplayName = "Tip"),
	AssetsInfoDisplay_Attention = 2 UMETA(DisplayName = "Attention"),
	AssetsInfoDisplay_Warning = 3 UMETA(DisplayName = "Warning"),
	AssetsInfoDisplay_Risk = 4 UMETA(DisplayName = "Risk"),
	AssetsInfoDisplay_Exceed = 5 UMETA(DisplayNam = "Exceed"),
	AssetsInfoDisplayLevel_Max
};

/**
 * 
 */
class ASSETSMANAGER_API UManagerConfig
{
	 
public:

	UManagerConfig(UManagerConfig&) = delete;
	
	static UManagerConfig& Get();

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
	UManagerConfig();
	virtual ~UManagerConfig() = default;

private:

	FString ConfigPath;
	FString DefaultConfigPath;
};



UCLASS(config = Editor, defaultconfig)
class ASSETSMANAGER_API UManagerSettings : public UObject
{
	GENERATED_BODY()

public:
	//  æ¿˝≈‰÷√—°œÓ
	UPROPERTY(EditAnywhere, config, Category = "Global")
	bool bEnableFeatureX;

	UPROPERTY(EditAnywhere, config, Category = "CharacterLookDev")
	TSoftObjectPtr<UBlueprint> StandardCharacterDisplay;
};