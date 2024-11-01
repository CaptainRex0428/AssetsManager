// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"


#include "AssetTypeActions_Base.h" 
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"

#include "Factories/TextureFactory.h"


class ASSETSMANAGER_API UTextureAssetTypeActions: public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("ManagerTexture2D", "Texture2D", "Texture 2D"); }
	virtual FColor GetTypeColor() const override { return FColor(189,20,64); }
	virtual UClass* GetSupportedClass() const override { return UTexture2D::StaticClass(); } // 或者你想要支持的类
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Textures; } // 选择适合的分类
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;
};