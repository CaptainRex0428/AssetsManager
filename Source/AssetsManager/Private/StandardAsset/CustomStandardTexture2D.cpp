// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/CustomStandardTexture2D.h"
#include "ConfigManager.h"
#include "ManagerLogger.h"
#include "AssetsChecker/AssetsChecker.h"

#include "TextureAssetActions.h"
#include "TextureSourceDataUtils.h"
#include "TextureImportSettings.h"

#include "ConfigManager.h"


FCustomStandardTexture2DData::FCustomStandardTexture2DData(
	const FAssetData& AssetData,
	bool StrictCheckMode)
	:FCustomStandardAssetData(AssetData, StrictCheckMode),
	StandardTexture2DObject(AssetData.GetAsset(),StrictCheckMode)
{
}

FCustomStandardTexture2DData::~FCustomStandardTexture2DData()
{
}

UCustomStandardTexture2D& FCustomStandardTexture2DData::Get()
{
	return StandardTexture2DObject;
}

UTexture2D* FCustomStandardTexture2DData::GetTexture2D()
{
	if (!this->Get().IsTexture2D())
	{
		return nullptr;
	}

	UTexture2D * AssetAsT = Cast<UTexture2D>(this->GetAsset());

	return AssetAsT;

}

bool UCustomStandardTexture2D::IsTextureMaxInGameOverSize()
{
	if (this->bStrictCheckMode) { return this->GetMaxInGameSize().X > this->GetStandardMaxSizeStrict() || this->GetMaxInGameSize().Y > this->GetStandardMaxSizeStrict(); };
	
	return this->GetMaxInGameSize().X > this->GetStandardMaxSize() || this->GetMaxInGameSize().Y > this->GetStandardMaxSize();
}

bool UCustomStandardTexture2D::IsTextureSourceOverSize()
{
	if (bStrictCheckMode) { return this->GetSourceSize().X > this->GetStandardMaxSizeStrict() || this->GetSourceSize().Y > this->GetStandardMaxSizeStrict(); };

	return this->GetSourceSize().X > this->GetStandardMaxSize() || this->GetSourceSize().Y > this->GetStandardMaxSize();
}

bool UCustomStandardTexture2D::SetMaxInGameSize(double maxSize,bool forceSave)
{
	if(!this->IsTexture2D())
	{
		return false;
	}

	this->Get()->MaxTextureSize = maxSize;
	this->Get()->UpdateResource();

	if(forceSave)
	{
		return this->ForceSave();
	}
	
	return true;
}

bool UCustomStandardTexture2D::CustomMaxInGameSize(double size,bool forced, bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	FVector2D MaxInGameSize = this->GetMaxInGameSize();
	double GameSize = MaxInGameSize.X > MaxInGameSize.Y ? MaxInGameSize.X : MaxInGameSize.Y;

	if (forced && GameSize != size)
	{
		return SetMaxInGameSize(size, true);
	}

	if (GameSize > size &&
		UAssetsChecker::bIsPowerOfTwo(MaxInGameSize.X) &&
		UAssetsChecker::bIsPowerOfTwo(MaxInGameSize.Y))
	{
		return SetMaxInGameSize(size, true);
	}

	return false;
}

int64 UCustomStandardTexture2D::GetMemorySize(bool bEstimatedTotal)
{
	if (!this->IsTexture2D())
	{
		return UCustomStandardObject::GetMemorySize(bEstimatedTotal);
	}

	this->Get()->PostLoad();
	return this->Get()->GetResourceSizeBytes(bEstimatedTotal ? EResourceSizeMode::EstimatedTotal : EResourceSizeMode::Exclusive);
}

int64 UCustomStandardTexture2D::GetMemorySize(AssetsInfoDisplayLevel& DisplayLevel, bool bEstimatedTotal)
{
	int64 MemorySize = UCustomStandardObject::GetMemorySize(bEstimatedTotal);

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

TSharedPtr<bool> UCustomStandardTexture2D::GetsRGBSettings()
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	return MakeShared<bool>(this->Get().Get()->SRGB ? true : false);
}
		

TSharedPtr<bool> UCustomStandardTexture2D::GetStandardsRGBSettings(bool forced)
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	TSharedPtr<FString> suffix = this->GetAssetSuffix();

	if (suffix.IsValid())
	{
		FString ValidSection = this->GetTextureVaidSection();

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

bool UCustomStandardTexture2D::SetSRGBSettings(const bool& sRGB, bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	if (*this->GetsRGBSettings() != sRGB)
	{

		this->Get()->SRGB = sRGB;
		this->Get()->UpdateResource();

		FString result = sRGB ? TEXT("true") : TEXT("false");

#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功设置贴图sRGB为\n")
			+ result + "\n"
			+ this->Get()->GetFName().ToString());
#else
		NtfyMsgLog(TEXT("Successfully set the sRGB settings as\n")
			+ result + "\n"
			+ this->Get()->GetFName().ToString());
#endif
		
		if(forceSave)
		{
			return this->ForceSave();
		}
		
	}

	return true;
}

bool UCustomStandardTexture2D::IsSRGBStandarized()
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	TSharedPtr<bool> CurrentSRGB = GetsRGBSettings();
	TSharedPtr<bool> StandardSRGB = GetStandardsRGBSettings(true);

	return *CurrentSRGB == *StandardSRGB;
}

TSharedPtr<TextureGroup> UCustomStandardTexture2D::GetStandardLODGroup(bool forced)
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	TSharedPtr<FString> suffix = this->GetAssetSuffix();

	if (suffix.IsValid())
	{
		FString ValidSection = this->GetTextureVaidSection();

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
				*this->GetCompressionSettings() == TC_Normalmap?"DefaultNormalLODGroup" : "DefaultLODGroup");

		if (CommonLODGroup)
		{
			const TextureGroup* TGFound = TextureGroupNameMap.Find(*CommonLODGroup->GetValue());

			if (TGFound)
			{
				return MakeShared<TextureGroup>(*TGFound);
			}
		}
	}

	if(forced)
	{
		return MakeShared<TextureGroup>(*this->GetCompressionSettings() == TC_Normalmap ?
			TEXTUREGROUP_WorldNormalMap : TEXTUREGROUP_World);
	}
	
	return nullptr;
	
}

bool UCustomStandardTexture2D::SetLODGroup(TextureGroup InTextureGroup, bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	if (this->Get()->LODGroup != InTextureGroup)
	{
		this->Get()->LODGroup = InTextureGroup;

		if(forceSave)
		{
			return this->ForceSave();
		}

		return true;
	}

	return false;
}

bool UCustomStandardTexture2D::IsTextureLODGroupStandarized()
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	TSharedPtr<TextureGroup> CurrentTG = GetLODGroup();
	TSharedPtr<TextureGroup> StandardTG = GetStandardLODGroup(true);

	return *CurrentTG == *StandardTG;
}

uint8 UCustomStandardTexture2D::IsStandarized(uint8 settingsTag)
{
	if (!this->IsTexture2D())
	{
		return -1;
	}

	uint8 result = 0;

	if ((settingsTag & TextureSettingsTag::COMPRESSIONSETTINGS) && this->IsCompressoionSettingsStandarized())
	{
		result |= TextureSettingsTag::COMPRESSIONSETTINGS;
	};

	if ((settingsTag & TextureSettingsTag::SRGB) && this->IsSRGBStandarized())
	{
		result |= TextureSettingsTag::SRGB;
	};

	if ((settingsTag & TextureSettingsTag::SOURCESIZE) && !this->IsTextureSourceOverSize())
	{
		result |= TextureSettingsTag::SOURCESIZE;
	};

	if ((settingsTag & TextureSettingsTag::MAXINGAMESIZE) && !this->IsTextureMaxInGameOverSize())
	{
		result |= TextureSettingsTag::MAXINGAMESIZE;
	};

	if ((settingsTag & TextureSettingsTag::LODGROUP) && this->IsTextureLODGroupStandarized())
	{
		result |= TextureSettingsTag::LODGROUP;
	};

	if ((settingsTag & TextureSettingsTag::SUFFIX) && this->IsSuffixStandarized())
	{
		result |= TextureSettingsTag::SUFFIX;
	};

	return result;

}

uint8 UCustomStandardTexture2D::NotStandarized(uint8 settingsTag)
{
	if (!this->IsTexture2D())
	{
		return -1;
	}

	uint8 result = 0;

	if ((settingsTag & TextureSettingsTag::COMPRESSIONSETTINGS) && !this->IsCompressoionSettingsStandarized())
	{
		result |= TextureSettingsTag::COMPRESSIONSETTINGS;
	};

	if ((settingsTag & TextureSettingsTag::SRGB) && !this->IsSRGBStandarized())
	{
		result |= TextureSettingsTag::SRGB;
	};

	if ((settingsTag & TextureSettingsTag::SOURCESIZE) && this->IsTextureSourceOverSize())
	{
		result |= TextureSettingsTag::SOURCESIZE;
	};

	if ((settingsTag & TextureSettingsTag::MAXINGAMESIZE) && this->IsTextureMaxInGameOverSize())
	{
		result |= TextureSettingsTag::MAXINGAMESIZE;
	};

	if ((settingsTag & TextureSettingsTag::LODGROUP) && !this->IsTextureLODGroupStandarized())
	{
		result |= TextureSettingsTag::LODGROUP;
	};

	if ((settingsTag & TextureSettingsTag::SUFFIX) && !this->IsSuffixStandarized())
	{
		result |= TextureSettingsTag::SUFFIX;
	};

	return result;
}

uint8 UCustomStandardTexture2D::Fix(uint8 settingsTag, bool forceMode, bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return -1;
	}

	uint8 PreDeal = this->IsStandarized(settingsTag);

	uint8 result = 0;

	if ((TextureSettingsTag::COMPRESSIONSETTINGS & settingsTag) && ! (PreDeal & TextureSettingsTag::COMPRESSIONSETTINGS))
	{
		TSharedPtr<TextureCompressionSettings> CMPS = this->GetStandardCompressionSettings(forceMode);
		
		if (CMPS && this->SetCompressionSettings(*CMPS, forceSave))
		{
			result |= TextureSettingsTag::COMPRESSIONSETTINGS;
		}
	};

	if ((TextureSettingsTag::SRGB & settingsTag) && ! (PreDeal & TextureSettingsTag::SRGB))
	{
		TSharedPtr<bool> SRGBS = this->GetStandardsRGBSettings();

		if(SRGBS && this->SetSRGBSettings(*SRGBS, forceSave))
		{
			result |= TextureSettingsTag::SRGB;
		}
	};

	double maxsize = bStrictCheckMode ? this->GetStandardMaxSizeStrict() : this->GetStandardMaxSize();

	if ((TextureSettingsTag::SOURCESIZE & settingsTag) && ! (PreDeal & TextureSettingsTag::SOURCESIZE))
	{
		this->ResizeSource(maxsize, forceSave);
		result |= TextureSettingsTag::SOURCESIZE;
		
	};

	if ((TextureSettingsTag::MAXINGAMESIZE & settingsTag) && ! (PreDeal & TextureSettingsTag::MAXINGAMESIZE))
	{
		this->CustomMaxInGameSize(maxsize, false, forceSave);
		result |= TextureSettingsTag::MAXINGAMESIZE;
	};

	if ((TextureSettingsTag::LODGROUP & settingsTag) && ! (PreDeal & TextureSettingsTag::LODGROUP))
	{
		TSharedPtr<TextureGroup> txtg = this->GetStandardLODGroup(forceMode);
		
		if(txtg && this->SetLODGroup(*txtg, forceSave))
		{
			result |= TextureSettingsTag::LODGROUP;
		}
		
	};

	if ((TextureSettingsTag::SUFFIX & settingsTag) && !(PreDeal & TextureSettingsTag::SUFFIX))
	{
		// result |= TextureSettingsTag::SUFFIX;
	};

	return result;
}

double UCustomStandardTexture2D::GetStandardMaxSize()
{
	FString ValidSection = this->GetTextureVaidSection();

	const FConfigValue* value = UManagerConfig::Get().GetSectionValue(
		*ValidSection, "MaxSize");

	if (value)
	{
		return UManagerConfig::Get().SToD(value->GetValue());
	}

	return 2048.f;
}

double UCustomStandardTexture2D::GetStandardMaxSizeStrict()
{
	TSharedPtr<FString> suffixCurrent = this->GetAssetSuffix();
	
	if(!suffixCurrent.IsValid() || suffixCurrent->IsEmpty())
	{
		return GetStandardMaxSize();
	}

	FString ValidSection = this->GetTextureVaidSection();

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

CompressionSettingsInfo ConstructCompressionConfigPairs(
	FString ConfigName, 
	TextureCompressionSettings Setting, 
	FString DisplayName)
{
	CompressionSettingsInfo Info;

	Info.ConfigName = ConfigName;
	Info.Setting = Setting;
	Info.DisplayName = DisplayName;
	
	return Info;
}

bool UCustomStandardTexture2D::IsSuffixStandarized()
{

	TSharedPtr<FString> suffix = this->GetAssetSuffix();

	if (!suffix.IsValid())
	{
		return false;
	}

	FString ValidSection = this->GetTextureVaidSection();

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

UCustomStandardTexture2D::UCustomStandardTexture2D(UObject* InObj, bool StricCheckMode):
	UCustomStandardObject(InObj, StricCheckMode)
{
	FString TGlobalSection = FPaths::Combine(ModuleConfigMaster, TEXT("Global"), TEXT("Texture"));

	if (UManagerConfig::Get().GetSection(*TGlobalSection))
	{
		this->TextureGlobalConfigSection = MakeShareable(new FString(TGlobalSection));
	}


	this->TextureCategoryStrictConfigSection
		= MakeShareable(new FString(FPaths::Combine(ModuleConfigMaster, *UAssetsChecker::GetCategoryTag(this->GetStrictAssetCategory()), TEXT("Texture"))));

	this->TextureCategoryCommonConfigSection
		= MakeShareable(new FString(FPaths::Combine(ModuleConfigMaster, *UAssetsChecker::GetCategoryTag(this->GetCommonAssetCategory()), TEXT("Texture"))));
}

UCustomStandardTexture2D::UCustomStandardTexture2D(UTexture2D* InTexture2D, bool StricCheckMode):
	UCustomStandardObject(InTexture2D, StricCheckMode)
{
	FString TGlobalSection = FPaths::Combine(ModuleConfigMaster, TEXT("Global"), TEXT("Texture"));

	if (UManagerConfig::Get().GetSection(*TGlobalSection))
	{
		this->TextureGlobalConfigSection = MakeShareable(new FString(TGlobalSection));
	}


	this->TextureCategoryStrictConfigSection
		= MakeShareable(new FString(FPaths::Combine(ModuleConfigMaster, *UAssetsChecker::GetCategoryTag(this->GetStrictAssetCategory()), TEXT("Texture"))));

	this->TextureCategoryCommonConfigSection
		= MakeShareable(new FString(FPaths::Combine(ModuleConfigMaster, *UAssetsChecker::GetCategoryTag(this->GetCommonAssetCategory()), TEXT("Texture"))));
}

UCustomStandardTexture2D::~UCustomStandardTexture2D()
{
}

TWeakObjectPtr<UTexture2D> UCustomStandardTexture2D::Get()
{
	return Cast<UTexture2D>(UCustomStandardObject::Get());
}

bool UCustomStandardTexture2D::IsTexture2D()
{
	return this->Get().IsValid();
}

TIndirectArray<struct FTexture2DMipMap> * UCustomStandardTexture2D::GetTextureMipMaps()
{
	if(!this->IsTexture2D())
	{
		return nullptr;
	}

	UTexture2D* Texture2D = this->Get().Get();
	FTexturePlatformData* PlatformData = Texture2D->GetPlatformData();

	if (!PlatformData)
	{
		return nullptr;
	}

	return & PlatformData->Mips;
}

void UCustomStandardTexture2D::ResizeSource(int targetSize, bool forceSave)
{
	if(!this->IsTexture2D())
	{
		return;
	}

	// we do the resizing considering only Mip/LOD/build settings for the running Editor platform (Windows ...)
	const ITargetPlatform* RunningPlatform = GetTargetPlatformManagerRef().GetRunningTargetPlatform();

	int32 BeforeSizeX;
	int32 BeforeSizeY;
	this->Get()->GetBuiltTextureSize(RunningPlatform, BeforeSizeX, BeforeSizeY);

	const FIntPoint BeforeSourceSize = this->Get()->Source.GetLogicalSize();
	MsgLog(FString::Printf(TEXT("Texture (%s) Resizing to <= %d Source Size=%dx%d Built Size=%dx%d"), *this->Get()->GetName(), targetSize,
		BeforeSourceSize.X, BeforeSourceSize.Y, BeforeSizeX, BeforeSizeY));

	if (!UE::TextureUtilitiesCommon::Experimental::DownsizeTextureSourceData(this->Get().Get(), targetSize, RunningPlatform))
	{
		MsgLog(FString::Printf(TEXT("Texture (%s) did not resize."), *this->Get()->GetName()));

		// did not resize, but may have done PreEditChange
		return;
	}

	const FIntPoint AfterSourceSize = this->Get()->Source.GetLogicalSize();

	this->Get()->LODBias = 0;

	int32 AfterSizeX;
	int32 AfterSizeY;
	this->Get()->GetBuiltTextureSize(RunningPlatform, AfterSizeX, AfterSizeY);

	// if AfterSize > BeforeSize , kick up LODBias
	//	to try to preserve GetBuiltTextureSize
	this->Get()->LODBias = FMath::RoundToInt32(FMath::Log2((double)FMath::Max(AfterSizeX, AfterSizeY) / FMath::Max(BeforeSizeX, BeforeSizeY)));
	this->Get()->LODBias = FMath::Max(0, this->Get()->LODBias);

	// recompute AfterSize if we changed LODBias
	if (this->Get()->LODBias != 0)
	{
		this->Get()->GetBuiltTextureSize(RunningPlatform, AfterSizeX, AfterSizeY);
	}

	MsgLog(FString::Printf(TEXT("Texture (%s) did resize Source Size=%dx%d Built Size=%dx%d"), *this->Get()->GetName(),
		AfterSourceSize.X, AfterSourceSize.Y, AfterSizeX, AfterSizeY));

	if (BeforeSizeX != AfterSizeX || BeforeSizeY != AfterSizeY)
	{
		// not a warning, just FYI
		// changing built size is totally possible and expected to happen sometimes
		//	basically any time you resize smaller than the previous in-game size
		MsgLog(FString::Printf(TEXT("DoResizeTextureSource failed to preserve built size; was: %dx%d now: %dx%d on [%s]"),
			BeforeSizeX, BeforeSizeY,
			AfterSizeX, AfterSizeY,
			*this->Get()->GetFullName()));
	}

	// this counts as a reimport :
	UE::TextureUtilitiesCommon::ApplyDefaultsForNewlyImportedTextures(this->Get().Get(), true);

	// DownsizeTextureSourceData did the PreEditChange
	this->Get()->PostEditChange();

	if(forceSave)
	{
		this->ForceSave();
	}
}

void UCustomStandardTexture2D::ResizeSourcePowerOf2(bool forceSave)
{
	if(!this->IsTexture2D())
	{
		return;
	}

	check(!this->Get()->Source.AreAllBlocksPowerOfTwo()); // already filtered for

	const FIntPoint BeforeSourceSize = this->Get()->Source.GetLogicalSize();

	if (!UE::TextureUtilitiesCommon::Experimental::ResizeTextureSourceDataToNearestPowerOfTwo(this->Get().Get()))
	{
		MsgLog(FString::Printf(TEXT("Texture (%s) did not resize."), *this->Get()->GetName()));

		// did not resize, but may have done PreEditChange
		return;
	}

	const FIntPoint AfterSourceSize = this->Get()->Source.GetLogicalSize();

	MsgLog(FString::Printf(TEXT("Texture (%s) did resize Before Size=%dx%d After Size=%dx%d"), *this->Get()->GetName(),
		BeforeSourceSize.X, BeforeSourceSize.Y, AfterSourceSize.X, AfterSourceSize.Y));

	// ?? if Texture was set to stretch to Pow2, we could remove that now, but leaving it is harmless
	//Texture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None;

	// if Texture was set to MipGen = NoMipMaps, change to FromTextureGroup ?
	// assume that since we changed to Pow2, we want mips and streaming
	if (this->Get()->MipGenSettings == TMGS_NoMipmaps)
	{
		this->Get()->MipGenSettings = TMGS_FromTextureGroup;
	}
	this->Get()->NeverStream = false;

	// this counts as a reimport :
	UE::TextureUtilitiesCommon::ApplyDefaultsForNewlyImportedTextures(this->Get().Get(), true);

	// DownsizeTextureSourceData did the PreEditChange
	this->Get()->PostEditChange();

	if (forceSave)
	{
		this->ForceSave();
	}
}

void UCustomStandardTexture2D::ConvertSourceTo8bit(bool NormalMapsKeep16bits, bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return;
	}

	if (this->Get()->Source.GetNumLayers() > 1)
	{
		check(0);
		return;
	}

	ETextureSourceFormat InTSF = this->Get()->Source.GetFormat();
	ETextureSourceFormat OutTSF = GetReducedTextureSourceFormat(this->Get()->CompressionSettings, InTSF, NormalMapsKeep16bits);
	if (InTSF == OutTSF)
	{
		return;
	}

	MsgLog(FString::Printf(TEXT("Texture (%s) changing format from %s to %s, TC = %s"),
		*this->Get()->GetName(),
		*StaticEnum<ETextureSourceFormat>()->GetDisplayNameTextByValue(InTSF).ToString(), // these have TSF_ on the strings
		*StaticEnum<ETextureSourceFormat>()->GetDisplayNameTextByValue(OutTSF).ToString(),
		*StaticEnum<TextureCompressionSettings>()->GetDisplayNameTextByValue(this->Get()->CompressionSettings).ToString())
	);

	// calls Pre/PostEditChange :
	UE::TextureUtilitiesCommon::Experimental::ChangeTextureSourceFormat(this->Get().Get(), OutTSF);


	if (forceSave)
	{
		this->ForceSave();
	}
}

void UCustomStandardTexture2D::CompressWithJPEG(bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return;
	}

	// calls Pre/PostEditChange :
	bool bDid = UE::TextureUtilitiesCommon::Experimental::CompressTextureSourceWithJPEG(this->Get().Get());

	MsgLog(FString::Printf(TEXT("Texture (%s) %s"),
		*this->Get()->GetName(),
		bDid ? TEXT("was changed to JPEG compression.") : TEXT("was not changed."))
	);

	if (forceSave)
	{
		this->ForceSave();
	}
}

FVector2D UCustomStandardTexture2D::GetSourceSize()
{
	if (!this->IsTexture2D())
	{
		return FVector2D(-1,-1);
	}

	int SourceSizeX = this->Get()->GetImportedSize().X;
	int SourceSizeY = this->Get()->GetImportedSize().Y;

	return FVector2D(SourceSizeX, SourceSizeY);
}

FVector2D UCustomStandardTexture2D::GetMaxInGameSize()
{
	if (!this->IsTexture2D())
	{
		return FVector2D(-1, -1);
	}

	FVector2D SourceSize = GetSourceSize();
	
	FVector2D size;
	size.X = SourceSize.X;
	size.Y = SourceSize.Y;

	uint32 bias = this->Get()->GetCachedLODBias();

	int32 MaximumTextureSize = this->Get()->MaxTextureSize;

	if (MaximumTextureSize == 0 && bias == 0)
	{
		return SourceSize;
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

		return size;
	}
}

TSharedPtr<TextureCompressionSettings> UCustomStandardTexture2D::GetCompressionSettings()
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	return MakeShared<TextureCompressionSettings>(this->Get()->CompressionSettings.GetValue());
}

TSharedPtr<TextureCompressionSettings> UCustomStandardTexture2D::GetStandardCompressionSettings(bool forced)
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	TSharedPtr<FString> suffix = this->GetAssetSuffix();

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

TSharedPtr<CompressionSettingsInfo> UCustomStandardTexture2D::GetCompressionSettingsInfo()
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	TSharedPtr<TextureCompressionSettings> CompSettings = GetCompressionSettings();

	for (const CompressionSettingsInfo& CConfig : ValidCompressionConfig)
	{
		if (CConfig.Setting == *CompSettings)
		{
			CompressionSettingsInfo CSI;

			CSI.ConfigName = CConfig.ConfigName;
			CSI.DisplayName = CConfig.DisplayName;
			CSI.Setting = CConfig.Setting;

			return MakeShared<CompressionSettingsInfo>(CSI);
		}
	}

	return nullptr;
}

bool UCustomStandardTexture2D::SetCompressionSettings(const TEnumAsByte<TextureCompressionSettings>& CompressionSetting, bool forceSave)
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	this->GetCompressionSettingsInfo()->Setting;


	if (this->GetCompressionSettingsInfo()->Setting != CompressionSetting)
	{

		this->Get()->CompressionSettings = CompressionSetting;
		this->Get()->UpdateResource();

		FString DisplayInfo;

		for (CompressionSettingsInfo info : ValidCompressionConfig)
		{
			if (info.Setting == CompressionSetting)
			{
				DisplayInfo = info.DisplayName;
				break;
			}
		}

#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功设置贴图压缩格式为\n")
			+ DisplayInfo + "\n"
			+ this->Get()->GetName());
#else
		NtfyMsgLog(TEXT("Successfully set the compression settings as\n")
			+ *TextureCompressionMap.Find(CompressionSetting) + "\n"
			+ this->Get()->GetName());
#endif

		if (forceSave)
		{
			return this->ForceSave();
		}
	}

	return true;
}

bool UCustomStandardTexture2D::IsCompressoionSettingsStandarized()
{
	if (!this->IsTexture2D())
	{
		return false;
	}

	TSharedPtr<TextureCompressionSettings> CurrentCS = this->GetCompressionSettings();
	TSharedPtr<TextureCompressionSettings> StandardCS = this->GetStandardCompressionSettings(true);

	return *CurrentCS == *StandardCS;
}

TSharedPtr<TextureGroup> UCustomStandardTexture2D::GetLODGroup()
{
	if (!this->IsTexture2D())
	{
		return nullptr;
	}

	return MakeShared<TextureGroup>(this->Get()->LODGroup);
}

ETextureSourceFormat UCustomStandardTexture2D::GetReducedTextureSourceFormat(const TextureCompressionSettings TC, const ETextureSourceFormat InTSF, const bool NormalMapsKeep16bits)
{
	if (InTSF == TSF_BGRE8)
	{
		// don't change BGRE
		return InTSF;
	}

	bool Out8bit = false;
	bool OutBC45 = false;
	bool OutHDR = false;
	bool OutSingleChannel = false;

	switch (TC)
	{
	case TC_Grayscale: //"Grayscale (G8/16, RGB8 sRGB)"),
	case TC_Displacementmap: //"Displacementmap (G8/16)"),
		// Gray and Displacement pass through G16 ; note they do not do that for RGBA16 (see GetDefaultTextureFormatName)
		//	do this conditional on NormalMapsKeep16bits just so we have a way to toggle it, even though it's not really a normal map
		if (InTSF == TSF_G16 && NormalMapsKeep16bits) return InTSF;
		// otherwise we will convert to G8
		// [[fallthrough]];
	case TC_DistanceFieldFont: //"DistanceFieldFont (G8)"),
		Out8bit = true;
		OutSingleChannel = true;
		break;

	case TC_Default: //"Default (DXT1/5, BC1/3 on DX11)"),
	case TC_Masks: //"Masks (no sRGB)"),
	case TC_VectorDisplacementmap: //"VectorDisplacementmap (RGBA8)"),
	case TC_EditorIcon: //"UserInterface2D (RGBA)"),
	case TC_BC7: //"BC7 (DX11, optional A)"),
	case TC_LQ: // "Low Quality (BGR565/BGR555A1)", ToolTip = "BGR565/BGR555A1, fallback to DXT1/DXT5 on Mac platform"),
		Out8bit = true;
		break;

	case TC_Normalmap: //"Normalmap (DXT5, BC5 on DX11)"),
		OutBC45 = true;
		break;

	case TC_Alpha: //"Alpha (no sRGB, BC4 on DX11)"),
		OutBC45 = true;
		OutSingleChannel = true;
		break;

	case TC_HDR: //"HDR (RGBA16F, no sRGB)"),
	case TC_HDR_Compressed: //"HDR Compressed (RGB, BC6H, DX11)"),
		OutHDR = true;
		break;

	case TC_HalfFloat: //"Half Float (R16F)"),
	case TC_SingleFloat: //"Single Float (R32F)"),
		OutHDR = true;
		OutSingleChannel = true;
		break;

	case TC_EncodedReflectionCapture:
	case TC_HDR_F32: //"HDR High Precision (RGBA32F)"),
		// don't change :
		return InTSF;

	default:
		check(0);
		return InTSF;
	}

	if (OutBC45 && !NormalMapsKeep16bits)
	{
		// if NormalMaps don't keep 16 bit sources, then just treat them like 8 bit :
		Out8bit = true;
	}

	ETextureSourceFormat OutTSF = InTSF;

	if (Out8bit)
	{
		OutTSF = (OutSingleChannel) ? TSF_G8 : TSF_BGRA8;
	}
	else if (OutBC45)
	{
		check(NormalMapsKeep16bits);

		// just choose a 16 bit output format, even if source was 8 bit
		//	we will only convert if bytes per pixel goes down
		OutTSF = (OutSingleChannel) ? TSF_G16 : TSF_RGBA16;

		if (OutSingleChannel && InTSF == TSF_BGRA8)
		{
			// don't do BGRA8 -> G16 , use G8 instead
			OutTSF = TSF_G8;
		}
	}
	else
	{
		check(OutHDR);
		check(TC != TC_HDR_F32); // already handled

		if (TC == TC_SingleFloat &&
			(InTSF == TSF_RGBA32F || InTSF == TSF_R32F))
		{
			// 32 bit output, 32 bit input, keep it 32 bit
			OutTSF = TSF_R32F;
		}
		else
		{
			OutTSF = (OutSingleChannel) ? TSF_R16F : TSF_RGBA16F;
		}
	}

	int64 InBPP = FTextureSource::GetBytesPerPixel(InTSF);
	int64 OutBPP = FTextureSource::GetBytesPerPixel(OutTSF);

	if (InBPP <= OutBPP)
	{
		// if bytes per pixel didn't go down, don't change
		return InTSF;
	}
	else
	{
		// reducing
		return OutTSF;
	}
}

FString UCustomStandardTexture2D::GetTextureVaidSection()
{
	FString ValidSection = "/AssetsManager/Global/Texture";

	if (this->TextureGlobalConfigSection.IsValid())
	{
		ValidSection = *TextureGlobalConfigSection;
	}

	if (this->TextureCategoryCommonConfigSection.IsValid())
	{
		ValidSection = *TextureCategoryCommonConfigSection;
	}

	if (this->TextureCategoryStrictConfigSection.IsValid())
	{
		ValidSection = *TextureCategoryStrictConfigSection;
	}

	return ValidSection;
}

