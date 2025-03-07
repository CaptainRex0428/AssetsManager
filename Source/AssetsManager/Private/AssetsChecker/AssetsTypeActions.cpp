// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsChecker/AssetsTypeActions.h"
#include "StandardAsset/FCustomStandardAsset.h"
#include "TextureEditor.h"
#include "Interfaces/ITextureEditorModule.h"
#include "Engine/Texture2D.h"
#include "Modules/ModuleManager.h"
#include "ManagerLogger.h"

void UCustomTexture2DAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	// NtfyMsgLog(L"Opening Assets.");

	for (UObject* Object : InObjects)
	{
		UCustomStandardObject standardObj(Object);

		if (standardObj.IsPrefixNonstandarized())
		{
			MsgLog(FString::Printf(L"[Prefix Error]%s",*standardObj.GetClassValidObjectName()));
		}

		UTexture2D* Texture = Cast<UTexture2D>(Object);
		if (Texture)
		{
			ITextureEditorModule * TextureEditorModule = FModuleManager::Get().LoadModulePtr<ITextureEditorModule>("TextureEditor");
			
			if (TextureEditorModule)
			{
				// 使用 AssetEditorManager 打开纹理编辑器
				TextureEditorModule->CreateTextureEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Texture);
			}
			
		}
	}
}
