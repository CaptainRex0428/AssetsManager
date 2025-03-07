// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardTexture2D.h"
#include "ConfigManager.h"
#include "ManagerLogger.h"
#include "AssetsChecker/AssetsChecker.h"

FCustomStandardTexture2DData::FCustomStandardTexture2DData(
	const FAssetData& AssetData,
	bool StrictCheckMode)
	:FCustomStandardAssetData(AssetData, StrictCheckMode),
	TextureGlobalConfigSection(nullptr),
	TextureCategoryCommonConfigSection(nullptr),
	TextureCategoryStrictConfigSection(nullptr),
	bTexture2D(false),
	GlobalMaxSize(0), MaxSize(0),
	MaxInGameSizeX(0),MaxInGameSizeY(0),
	SourceSizeX(0),SourceSizeY(0),
	CompressionSettings(ValidCompressionConfig[0])
	
{

	/*
	* Get sections
	*/

	FString TGlobalSection = FPaths::Combine(ModuleConfigMaster, TEXT("Global"),TEXT("Texture"));

	if (UManagerConfig::Get().GetSection(*TGlobalSection))
	{
		this->TextureGlobalConfigSection = MakeShareable(new FString(TGlobalSection));
	}

	
	this->TextureCategoryStrictConfigSection
		= MakeShareable(new FString(FPaths::Combine(ModuleConfigMaster, *UAssetsChecker::GetCategoryTag(this->Get().GetStrictAssetCategory()), TEXT("Texture"))));
	
	this->TextureCategoryCommonConfigSection
		= MakeShareable(new FString(FPaths::Combine(ModuleConfigMaster, *UAssetsChecker::GetCategoryTag(this->Get().GetCommonAssetCategory()), TEXT("Texture"))));
	

	/*
	* Set global & strict max size
	*/

	this->GlobalMaxSize = GetStandardMaxSize();
	this->MaxSize = GetStandardMaxSizeStrict();

	/*
	* Judge texture validity
	*/

	UObject* AssetPtr = AssetData.GetAsset();

	if(AssetPtr && AssetPtr->IsA<UTexture2D>())
	{
		this->bTexture2D = true;
	}

	/*
	* Read compression settings
	*/

	TSharedPtr<TextureCompressionSettings> CompSettings = GetCompressionSettings();

	if (bTexture2D && CompSettings.IsValid())
	{	
		for (const CompressionSettingsInfo& CConfig :ValidCompressionConfig)
		{
			if (CConfig.Setting == *CompSettings)
			{
				this->CompressionSettings.ConfigName = CConfig.ConfigName;
				this->CompressionSettings.DisplayName = CConfig.DisplayName;
				this->CompressionSettings.Setting = CConfig.Setting;
				break;
			}
		}
	}

	/*
	* Get texture real size in game and source
	*/

	FVector2D size(0, 0);

	if(this->bTexture2D)
	{
		TObjectPtr<UTexture2D> AssetAsT = Cast<UTexture2D>(AssetData.GetAsset());

		this->SourceSizeX = AssetAsT->GetImportedSize().X;
		this->SourceSizeY = AssetAsT->GetImportedSize().Y;

		size.X = AssetAsT->GetImportedSize().X;
		size.Y = AssetAsT->GetImportedSize().Y;

		uint32 bias = AssetAsT->GetCachedLODBias();

		int32 MaximumTextureSize = AssetAsT->MaxTextureSize;

		if (MaximumTextureSize == 0 && bias == 0)
		{
			this->MaxInGameSizeX = this->SourceSizeX;
			this->MaxInGameSizeY = this->SourceSizeY;
		}
		else
		{
			float rate;

			if (MaximumTextureSize == 0)
			{
				rate = 1;
			}
			else
			{
				rate = (MaximumTextureSize / (size.X > size.Y ? size.X : size.X));
			}

			size.X *= (rate > 1 ? 1 : rate);
			size.Y *= (rate > 1 ? 1 : rate);

			if (bias > 0)
			{

				double scale = pow(2, bias);
				size.X /= scale;
				size.Y /= scale;

				size.X = size.X > 1 ? size.X : 1;
				size.Y = size.Y > 1 ? size.Y : 1;
			}

			this->MaxInGameSizeX = size.X;
			this->MaxInGameSizeY = size.Y;
		}
	}

}

FCustomStandardTexture2DData::~FCustomStandardTexture2DData()
{
}

bool FCustomStandardTexture2DData::IsTexture2D()
{
	return bTexture2D;
}

UTexture2D* FCustomStandardTexture2DData::GetTexture2D()
{
	if (!bTexture2D) 
	{
		return nullptr;
	}

	UTexture2D * AssetAsT = Cast<UTexture2D>(this->GetAsset());

	return AssetAsT;

}

bool FCustomStandardTexture2DData::IsTextureMaxInGameOverSize()
{
	if (this->bStrictCheckMode) { return this->MaxInGameSizeX > this->MaxSize || this->MaxInGameSizeY > this->MaxSize; };
	
	return this->MaxInGameSizeX > this->GlobalMaxSize || this->MaxInGameSizeY > this->GlobalMaxSize;
}

bool FCustomStandardTexture2DData::IsTextureSourceOverSize()
{
	if (bStrictCheckMode) { return this->SourceSizeX > this->MaxSize || this->SourceSizeY > this->MaxSize; };

	return this->SourceSizeX > this->GlobalMaxSize || this->SourceSizeY > this->GlobalMaxSize;
}

int64 FCustomStandardTexture2DData::GetMemorySize(bool bEstimatedTotal)
{
	if (!bTexture2D)
	{
		return FCustomStandardAssetData::Get().GetMemorySize(bEstimatedTotal);
	}

	UTexture2D* AssetT = Cast<UTexture2D>(this->GetAsset());

	if (!AssetT)
	{
		return 0;
	}

	AssetT->PostLoad();
	return AssetT->GetResourceSizeBytes(bEstimatedTotal ? EResourceSizeMode::EstimatedTotal : EResourceSizeMode::Exclusive);
}

int64 FCustomStandardTexture2DData::GetMemorySize(AssetsInfoDisplayLevel& DisplayLevel, bool bEstimatedTotal)
{
	int64 MemorySize = FCustomStandardAssetData::Get().GetMemorySize(bEstimatedTotal);

	FString AssetGlobalSection = "/AssetsManager/Global";

	TArray<FString> ValidLevels = UManagerConfig::Get().GenerateStructKeyValueArray(
		*AssetGlobalSection,
		L"AssetMemorySizeLevelDivide",
		L"Level");

	int32 LevelOut = 0;

	for (int32 levelIdx = 0; levelIdx < ValidLevels.Num(); ++levelIdx)
	{
		TSharedPtr<FString> LevelValue = UManagerConfig::Get().FindInSectionStructArray(
			*AssetGlobalSection,
			L"AssetMemorySizeLevelDivide",
			L"Level",
			FString::FromInt(levelIdx),
			L"Value");

		if (!LevelValue.IsValid())
		{
			continue;
		}

		int BorderSize = UManagerConfig::Get().SToI(*LevelValue);

		LevelOut = levelIdx;

		if (MemorySize < BorderSize * 1024 * 1024)
		{
			break;
		}
	}

	DisplayLevel = UAssetsChecker::IntToDisplayLevel(LevelOut);

	return MemorySize;


	return int64();
}

FVector2D FCustomStandardTexture2DData::GetSourceSize()
{
	return FVector2D(this->SourceSizeX,this->SourceSizeY);
}

FVector2D FCustomStandardTexture2DData::GetMaxInGameSize()
{
	return FVector2D(this->MaxInGameSizeX,this->MaxInGameSizeY);
}

TSharedPtr<TextureCompressionSettings> FCustomStandardTexture2DData::GetCompressionSettings()
{
	if (!bTexture2D)
	{
		return nullptr;
	}

	UTexture2D* AssetT = Cast<UTexture2D>(this->GetAsset());

	if (AssetT)
	{
		return MakeShared<TextureCompressionSettings>(AssetT->CompressionSettings);
	}

	return nullptr;
}

TSharedPtr<TextureCompressionSettings> FCustomStandardTexture2DData::GetStandardCompressionSettings(
	bool forced)
{
	if (!bTexture2D)
	{
		return nullptr;
	}

	TSharedPtr<FString> suffix = this->Get().GetAssetSuffix();

	if (suffix.IsValid())
	{
		FString ValidSection = GetTextureVaidSection();

		TSharedPtr<FString> DisplayCompression = UManagerConfig::Get().FindInSectionStructArray(
			*ValidSection,
			"SuffixStandard",
			"Suffix",
			*suffix,
			"CompressionSettings");

		if (DisplayCompression.IsValid())
		{
			for (CompressionSettingsInfo info : ValidCompressionConfig)
			{
				if (info.ConfigName == *DisplayCompression)
				{
					return MakeShared<TextureCompressionSettings>(info.Setting);
				}
			}
		}
	}


	if (forced)
	{
		if (*GetLODGroup() == TEXTUREGROUP_WorldNormalMap ||
			*GetLODGroup() == TEXTUREGROUP_CharacterNormalMap ||
			*GetLODGroup() == TEXTUREGROUP_WeaponNormalMap ||
			*GetLODGroup() == TEXTUREGROUP_VehicleNormalMap)
		{
			return MakeShared<TextureCompressionSettings>(TC_Normalmap);
		}

		return MakeShared<TextureCompressionSettings>(TC_Default);
	}

	return nullptr;
	
}

FCustomStandardTexture2DData::CompressionSettingsInfo 
FCustomStandardTexture2DData::GetCompressionSettingsInfo() const
{
	return this->CompressionSettings;
}

TSharedPtr<bool> FCustomStandardTexture2DData::GetsRGBSettings()
{
	if (!bTexture2D)
	{
		return nullptr;
	}

	UTexture2D* AssetT = Cast<UTexture2D>(this->GetAsset());

	if (AssetT)
	{
		if (AssetT->SRGB)
		{
			return MakeShared<bool>(true);
		}

		return MakeShared<bool>(false);
	}

	return nullptr;
}

TSharedPtr<bool> FCustomStandardTexture2DData::GetStandardsRGBSettings(
	bool forced)
{
	if (!bTexture2D)
	{
		return nullptr;
	}

	TSharedPtr<FString> suffix = this->Get().GetAssetSuffix();

	if (suffix.IsValid())
	{
		FString ValidSection = GetTextureVaidSection();

		TSharedPtr<FString> DisplaySRGB = UManagerConfig::Get().FindInSectionStructArray(
			*ValidSection,
			"SuffixStandard",
			"Suffix",
			*suffix,
			"sRGB");

		if (DisplaySRGB.IsValid())
		{
			if (*DisplaySRGB == "1" || DisplaySRGB->ToLower() == "true")
			{
				return MakeShared<bool>(true);
			}

			if (*DisplaySRGB == "0" || DisplaySRGB->ToLower() == "false")
			{
				return MakeShared<bool>(false);
			}
		}
	}

	if(forced)
	{
		return MakeShared<bool>(false);
	}

	return nullptr;
}

bool FCustomStandardTexture2DData::IsTextureSettingsStandarized()
{
	if (!bTexture2D)
	{
		return false;
	}

	TSharedPtr<TextureCompressionSettings> CurrentCS = GetCompressionSettings();
	TSharedPtr<TextureCompressionSettings> StandardCS = GetStandardCompressionSettings(true);

	TSharedPtr<bool> CurrentSRGB = GetsRGBSettings();
	TSharedPtr<bool> StandardSRGB = GetStandardsRGBSettings(true);

	return *CurrentCS == *StandardCS && *CurrentSRGB == *StandardSRGB;

}

TSharedPtr<TextureGroup> FCustomStandardTexture2DData::GetLODGroup()
{
	if (!bTexture2D)
	{
		return nullptr;
	}

	UTexture2D* TAsset = Cast<UTexture2D>(this->GetAsset());

	return MakeShared<TextureGroup>(TAsset->LODGroup);
}

TSharedPtr<TextureGroup> FCustomStandardTexture2DData::GetStandardLODGroup(
	bool forced)
{
	if (!bTexture2D)
	{
		return nullptr;
	}

	TSharedPtr<FString> suffix = this->Get().GetAssetSuffix();

	if (suffix.IsValid())
	{
		FString ValidSection = GetTextureVaidSection();

		TSharedPtr<FString> LODGroup = UManagerConfig::Get().FindInSectionStructArray(
			*ValidSection,
			"SuffixStandard",
			"Suffix",
			*suffix,
			"LODGroup");

		if (LODGroup.IsValid())
		{
			const TextureGroup* TGFound = TextureGroupNameMap.Find(*LODGroup);
			
			if (TGFound)
			{
				return MakeShared<TextureGroup>(*TGFound);
			}
		}

		const FConfigValue* CommonLODGroup = 
			UManagerConfig::Get().GetSectionValue(*ValidSection, 
				*GetCompressionSettings() == TC_Normalmap?"DefaultNormalLODGroup" : "DefaultLODGroup");

		if (CommonLODGroup)
		{
			const TextureGroup* TGFound = TextureGroupNameMap.Find(*CommonLODGroup->GetValue());

			if (TGFound)
			{
				return MakeShared<TextureGroup>(*TGFound);
			}
		}
	}

	return MakeShared<TextureGroup>(*GetCompressionSettings() == TC_Normalmap ? 
		TEXTUREGROUP_WorldNormalMap : TEXTUREGROUP_World);
	
}

bool FCustomStandardTexture2DData::IsTextureLODGroupStandarized()
{
	if (!bTexture2D)
	{
		return false;
	}

	TSharedPtr<TextureGroup> CurrentTG = GetLODGroup();
	TSharedPtr<TextureGroup> StandardTG = GetStandardLODGroup(true);

	return *CurrentTG == *StandardTG;
}

FString FCustomStandardTexture2DData::GetTextureVaidSection()
{
	FString ValidSection = "/AssetsManager/Global/Texture";

	if (TextureGlobalConfigSection.IsValid())
	{
		ValidSection = *TextureGlobalConfigSection;
	}

	if (TextureCategoryCommonConfigSection.IsValid())
	{
		ValidSection = *TextureCategoryCommonConfigSection;
	}

	if (TextureCategoryStrictConfigSection.IsValid())
	{
		ValidSection = *TextureCategoryStrictConfigSection;
	}

	return ValidSection;
}

double FCustomStandardTexture2DData::GetStandardMaxSize()
{
	FString ValidSection = GetTextureVaidSection();

	const FConfigValue* value = UManagerConfig::Get().GetSectionValue(
		*ValidSection, "MaxSize");

	if (value)
	{
		return UManagerConfig::Get().SToD(value->GetValue());
	}

	return 2048.f;
}

double FCustomStandardTexture2DData::GetStandardMaxSizeStrict()
{
	TSharedPtr<FString> suffixCurrent = this->Get().GetAssetSuffix();
	
	if(!suffixCurrent.IsValid() || suffixCurrent->IsEmpty())
	{
		return GetStandardMaxSize();
	}

	FString ValidSection = GetTextureVaidSection();

	TSharedPtr<FString> SizeInStr =
		UManagerConfig::Get().FindInSectionStructArray(
			*ValidSection,
			"SuffixStandard",
			"Suffix",
			*suffixCurrent,
			"MaxSize");

	if (SizeInStr)
	{
		return UManagerConfig::Get().SToD(*SizeInStr);
	}

	return GetStandardMaxSize();
}

FCustomStandardTexture2DData::CompressionSettingsInfo 
FCustomStandardTexture2DData::ConstructCompressionConfigPairs(
	FString ConfigName, 
	TextureCompressionSettings Setting, 
	FString DisplayName)
{
	FCustomStandardTexture2DData::CompressionSettingsInfo Info;

	Info.ConfigName = ConfigName;
	Info.Setting = Setting;
	Info.DisplayName = DisplayName;
	
	return Info;
}

bool FCustomStandardTexture2DData::IsSuffixStandarized()
{

	TSharedPtr<FString> suffix = this->Get().GetAssetSuffix();

	if (!suffix.IsValid())
	{
		return false;
	}

	FString ValidSection = GetTextureVaidSection();

	TArray<FString> ValidSuffixArray =
		UManagerConfig::Get().GenerateStructKeyValueArray(
			*ValidSection,
			"SuffixStandard",
			"Suffix");

	if (ValidSuffixArray.Contains(*suffix))
	{
		return true;
	}

	return false;
}
