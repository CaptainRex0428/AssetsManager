// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsHandler.h"

#include "ManagerLogger.h"
#include "StandardAsset/FCustomStandardAssetData.h"
#include "StandardAsset/FCustomStandardTexture2DData.h"
#include "StandardAsset/FCustomStandardSkeletalMeshData.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "FileHelpers.h"

#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UAssetsHandler::CreateIDCheckerMaterial(
	TextureChannel Channel, 
	int32 UVs,
	bool CheckNum)
{
	TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	
	FString ColorMaterialPath = "/Script/Engine.Material'/AssetsManager/AssetsTool/Material/M_IDChecker_Color.M_IDChecker_Color'";
	FString NumMaterialPath = "/Script/Engine.Material'/AssetsManager/AssetsTool/Material/M_IDChecker_Num.M_IDChecker_Num'";

	UMaterial* IDCheckerMaterial =Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(CheckNum? NumMaterialPath : ColorMaterialPath));

	if (IDCheckerMaterial)
	{
		for (FAssetData AssetData : SelectedAssets)
		{
			FCustomStandardTexture2DData TextureAsset(AssetData);

			if (TextureAsset.IsTexture2D())
			{
				FString PKGPath = FPaths::GetPath(TextureAsset.GetObjectPathString());
				FString AssetName = "MI_IDChecker_" + TextureAsset.GetAssetNameWithoutPrefix();

				UMaterialInstanceConstant* Instance;

				if (!FPaths::FileExists(FPaths::Combine(PKGPath, AssetName))) {
					Instance =
						UAssetsCreator::CreateMaterialInstanceConstant(
							PKGPath, AssetName, IDCheckerMaterial);
				}
				else
				{
					Instance =
						Cast<UMaterialInstanceConstant>(
							UEditorAssetLibrary::LoadAsset(
								FPaths::Combine(PKGPath, AssetName)));
				}

				if (Instance)
				{
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
		}

		return;
	}
	
#ifdef ZH_CN
	NtfyMsgLog(TEXT("基础材质(M_IDChecker)不可用"));
#else
	NtfyMsgLog(TEXT("Parent material (M_IDChecker) is not valid."));
#endif

}

void UAssetsHandler::DuplicateAssets(
	int NumOfDupicates,
	bool forced)
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

	int Counter = UAssetsChecker::EDuplicateAssets(SelectedAssetsData, NumOfDupicates, forced);

	if (Counter <= 0)
	{
#ifdef ZH_CN
		EAppReturnType::Type ReturnType = DlgMsgLog(EAppMsgType::Ok, TEXT("输入的复制数量不可以小于0"));
#else
		EAppReturnType::Type ReturnType = DlgMsgLog(EAppMsgType::Ok, TEXT("Number input is invalid."));
#endif
		return;

	}
#ifdef ZH_CN
	FString Msg = TEXT("成功复制") + FString::FromInt(Counter) + TEXT("个文件");
#else
	FString Msg = FString::FromInt(Counter) + TEXT(" files copied.");
#endif
	NtfyMsg(Msg);
}

void UAssetsHandler::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	UAssetsChecker::ERemoveUnusedAssets(SelectedAssetsData);
}

void UAssetsHandler::ReplaceName(const FString& OriginStr, const FString& ReplaceStr)
{
	
	TArray<UObject*> AssetsSelected = UEditorUtilityLibrary::GetSelectedAssets();
	int Counter = UAssetsChecker::EReplaceName(AssetsSelected, OriginStr, ReplaceStr);

	if (Counter == -1)
	{
#ifdef ZH_CN
		EAppReturnType::Type ReturnType = DlgMsgLog(EAppMsgType::Ok,
			TEXT("被替换的文本(Origin string)不能为空!!!"));
#else
		EAppReturnType::Type ReturnType = DlgMsgLog(EAppMsgType::Ok,
			TEXT("Origin string pattern should not be empty."));
#endif
		return;
	}

	if (Counter >= 0)
	{
#ifdef ZH_CN
		NtfyMsgLog(FString::FromInt(Counter) + TEXT("个资产重命名替换成功"));
#else
		NtfyMsgLog(FString::FromInt(Counter)
			+ TEXT(" asset")
			+ (Counter > 1 ? TEXT("s'") : TEXT("'s"))
			+ TEXT(" name has been replaced."));
#endif
		return;
	}
}

void UAssetsHandler::Check()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData AssetData : SelectedAssetsData)
	{
		FCustomStandardAssetData AssetSelect(AssetData);
		
		NtfyMsgLog(FString::SanitizeFloat(AssetSelect.GetDiskSize(AssetSizeDisplayUnit::MB,true)));
	}
}
