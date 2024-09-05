// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/FCustomStandardTexture2DData.h"
#include "AssetsManagerConfig.h"
#include "ConfigManager.h"

FCustomStandardTexture2DData::FCustomStandardTexture2DData(const FAssetData& AssetData)
	:FCustomStandardAssetData(AssetData),
	bTexture2D(false),
	MaxInGameSizeX(0),MaxInGameSizeY(0),
	SourceSizeX(0),SourceSizeY(0)
{

	/*
	* Get sections
	*/

	this->GlobalMaxSize = 2048;
	this->MaxSize = 2048;

	/*
	* Judge texture validity
	*/

	UObject* AssetPtr = AssetData.GetAsset();

	if(AssetPtr && AssetPtr->IsA<UTexture2D>())
	{
		this->bTexture2D = true;
	}
	
	/*
	* Read strict max size
	*/

	TSharedPtr<FString> subfix = GetAssetSubfix();

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
