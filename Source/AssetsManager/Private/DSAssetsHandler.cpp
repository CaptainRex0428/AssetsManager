// Fill out your copyright notice in the Description page of Project Settings.


#include "DSAssetsHandler.h"

#include "ManagerLogger.h"
#include "StandardAsset/FCustomStandardAssetData.h"
#include "StandardAsset/FCustomStandardTexture2DData.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "FileHelpers.h"

#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UDSAssetsHandler::CreateIDCheckerMaterial(
	TextureChannel Channel, 
	int32 UVs)
{
	TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();

	
	FString MaterialPath = "/Script/Engine.Material'/AssetsManager/Material/M_IDChecker.M_IDChecker'";
	UMaterial* IDCheckerMaterial =Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(MaterialPath));

	if (IDCheckerMaterial)
	{
		for (FAssetData AssetData : SelectedAssets)
		{
			FCustomStandardTexture2DData TextureAsset(AssetData);

			if (TextureAsset.IsTexture2D())
			{
				FString PKGPath = FPaths::GetPath(TextureAsset.GetObjectPathString());
				FString AssetName = "MI_IDChecker_" + TextureAsset.GetAssetNameWithoutPrefix();

				UMaterialInstanceConstant * Instance =
					UAssetsCreator::CreateMaterialInstanceConstant(
						PKGPath, AssetName, IDCheckerMaterial);

				UPackage* Pkg = Instance->GetPackage();
				Pkg->FullyLoad();

				Instance->MarkPackageDirty();
				Instance->PreEditChange(nullptr);

				Instance->SetTextureParameterValueEditorOnly(FName("ID"), Cast<UTexture2D>(AssetData.GetAsset()));
				Instance->SetScalarParameterValueEditorOnly(FName("UVChannel"), (float)UVs);

				FLinearColor IDChannel(0, 0, 0, 0);

				switch (Channel)
				{
				case TextureChannel::Red:
				{
					IDChannel.R = 1;
					break;
				}	
				case TextureChannel::Green:
				{
					IDChannel.G = 1;
					break;
				}
				case TextureChannel::Blue:
				{
					IDChannel.B = 1;
					break;
				}
				case TextureChannel::Alpha:
				{
					IDChannel.A = 1;
					break;
				}
				default:
				{
					IDChannel.R = 1;
					break;
				}
				}

				Instance->SetVectorParameterValueEditorOnly(FName("IDChannel"), IDChannel);

				Instance->PostEditChange();
				Instance->CacheShaders(EMaterialShaderPrecompileMode::Background);

				TArray<UPackage*> PackageToSave;
				PackageToSave.Add(Pkg);
				UEditorLoadingAndSavingUtils::SavePackages(PackageToSave, true);
			}
		}

		return;
	}
	
#ifdef ZH_CN
	NtfyMsgLog(TEXT("基础材质(M_IDChecker)不可用"));
#else
	NtfyMsgLog(TEXT("Parent material (M_IDChecker) is not valid."));
#endif

}
