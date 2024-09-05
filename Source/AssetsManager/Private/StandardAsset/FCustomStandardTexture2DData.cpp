// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardTexture2DData.h"
#include "AssetsManagerConfig.h"
#include "ConfigManager.h"

FCustomStandardTexture2DData::FCustomStandardTexture2DData(const FAssetData& AssetData)
	:FCustomStandardAssetData(AssetData),
	TextureGlobalConfigSection(nullptr),
	TextureCategoryCommonConfigSection(nullptr),
	TextureCategoryStrictConfigSection(nullptr),
	bTexture2D(false),
	GlobalMaxSize(0), MaxSize(0),
	MaxInGameSizeX(0),MaxInGameSizeY(0),
	SourceSizeX(0),SourceSizeY(0)
	
{

	/*
	* Get sections
	*/

	FString TGlobalSection = FPaths::Combine(ModuleConfigMaster, TEXT("Global"),TEXT("Texture"));

	if (UConfigManager::Get().GetSection(*TGlobalSection))
	{
		AssetConfigGlobalSection = MakeShareable(new FString(TGlobalSection));
	}

	if(m_StrictAssetCategoryTag.IsValid())
	{
		TextureCategoryStrictConfigSection 
			= MakeShared<FString>(FPaths::Combine(ModuleConfigMaster, *m_StrictAssetCategoryTag, TEXT("Texture")));
	}

	if (m_CommonAssetCategoryTag.IsValid())
	{
		TextureCategoryCommonConfigSection
			= MakeShared<FString>(FPaths::Combine(ModuleConfigMaster, *m_CommonAssetCategoryTag, TEXT("Texture")));
	}

	/*
	* Set global max size
	*/

	this->GlobalMaxSize = GetStandardMaxSize();
	this->MaxSize = this->GlobalMaxSize;

	/*
	* Judge texture validity
	*/

	UObject* AssetPtr = AssetData.GetAsset();

	if(AssetPtr && AssetPtr->IsA<UTexture2D>())
	{
		this->bTexture2D = true;
	}
	
	/*
	* Set strict max size
	*/

	TSharedPtr<FString> subfix = GetAssetSuffix();

	if (subfix.IsValid())
	{
		const float* ValidMaxSize = TextureSubfixMaxSizeSettingsMap.Find(*subfix);

		if(ValidMaxSize)
		{
			this->MaxSize = *ValidMaxSize;
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

bool FCustomStandardTexture2DData::isTexture2D()
{
	return bTexture2D;
}

bool FCustomStandardTexture2DData::isTextureMaxInGameOverSize(bool bStrict)
{
	if (bStrict) { return this->MaxInGameSizeX > MaxSize || this->MaxInGameSizeY > MaxSize; };
	
	return this->MaxInGameSizeX > GlobalMaxSize || this->MaxInGameSizeY > GlobalMaxSize;
}

bool FCustomStandardTexture2DData::isTextureSourceOverSize(bool bStrict)
{
	if (bStrict) { return this->SourceSizeX > MaxSize || this->SourceSizeY > MaxSize; };

	return this->SourceSizeX > GlobalMaxSize || this->SourceSizeY > GlobalMaxSize;
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

double FCustomStandardTexture2DData::GetStandardMaxSize()
{
	if(TextureCategoryStrictConfigSection.IsValid())
	{
		const FConfigValue * value = UConfigManager::Get().GetSectionValue(**TextureCategoryStrictConfigSection, "MaxSize");

		if(value)
		{
			return UConfigManager::Get().SToD(value->GetValue());
		}
	}

	if (TextureCategoryCommonConfigSection.IsValid())
	{
		const FConfigValue* value = UConfigManager::Get().GetSectionValue(**TextureCategoryCommonConfigSection, "MaxSize");

		if (value)
		{
			return UConfigManager::Get().SToD(value->GetValue());
		}
	}

	if (TextureGlobalConfigSection.IsValid())
	{
		const FConfigValue* value = UConfigManager::Get().GetSectionValue(**TextureGlobalConfigSection, "MaxSize");

		if (value)
		{
			return UConfigManager::Get().SToD(value->GetValue());
		}
	}

	return 2048.f;
}

double FCustomStandardTexture2DData::GetStandardMaxSizeStrict()
{
	return 0.0;
}
