// Fill out your copyright notice in the Description page of Project Settings.

#pragma warning(disable: 5103)

#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"
#include "StandardAsset/StandardAsset.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "AssetsManagerConfig.h"

#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"


#define PATHLOOPIGNORE(Path) if (##Path.Contains("Developers") || ##Path.Contains("Collections")) continue;
#define DIRPATHNOTEXISTIGNORE(Path) if (!UEditorAssetLibrary::DoesDirectoryExist(##Path))continue;
#define ASSETPATHNOTEXISTIGNORE(Path) if (!UEditorAssetLibrary::DoesAssetExist(##Path))continue;


bool UAssetsChecker::bIsPowerOfTwo(const int num)
{
	return (num & (num - 1)) == 0;
}

bool UAssetsChecker::bIsPowerOfTwo(const uint32 num)
{
	return (num & (num - 1)) == 0;
}

bool UAssetsChecker::bIsPowerOfTwo(const double num)
{
	if((num - (int)num) == 0)
	{
		return ((int)num & ((int)num - 1)) == 0;
	}

	return false;
}

bool UAssetsChecker::bIsPowerOfTwo(const float num)
{
	return bIsPowerOfTwo((double)num);
}



int UAssetsChecker::EDuplicateAssets(
	const TArray<FAssetData>& AssetsDataSelected, 
	int NumOfDupicates, 
	bool forced)
{
	if (NumOfDupicates <= 0)
	{
		return 0;
	}

	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : AssetsDataSelected)
	{
		int Subfix = 0;

		for (int32 i = 0; i < NumOfDupicates; i++)
		{
			const FString SourcrAssetPath = SelectedAssetData.GetObjectPathString();
			const FString DuplicateAssetPath = SelectedAssetData.PackagePath.ToString();
			const FString DuplicateAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_");

			if (!forced) {

				if (UEditorAssetLibrary::DuplicateAsset(SourcrAssetPath, FPaths::Combine(DuplicateAssetPath, DuplicateAssetName + FString::FromInt(++Subfix))))
				{
					UEditorAssetLibrary::SaveAsset(FPaths::Combine(DuplicateAssetPath, DuplicateAssetName + FString::FromInt(Subfix)), false);
					++Counter;
				}
			}
			else
			{
				while (!UEditorAssetLibrary::DuplicateAsset(SourcrAssetPath, FPaths::Combine(DuplicateAssetPath, DuplicateAssetName + FString::FromInt(++Subfix)))) {}

				UEditorAssetLibrary::SaveAsset(FPaths::Combine(DuplicateAssetPath, DuplicateAssetName + FString::FromInt(Subfix)), false);
				++Counter;
			}
		}
	}

	return Counter;
}

void UAssetsChecker::DuplicateAssets(
	int NumOfDupicates,bool forced)
{
	
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	
	int Counter = EDuplicateAssets(SelectedAssetsData, NumOfDupicates, forced);

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

bool UAssetsChecker::EConfirmPrefixes(
	TArray< TSharedPtr<FAssetData>>& AssetsSelected,
	TArray< TSharedPtr<FAssetData>>& ReadyToFixAssets)
{
	ReadyToFixAssets.Empty();
	uint32 ShouldRenameCounter = 0;
	FString NewAssetsName;
	
	for (TSharedPtr<FAssetData>& selectedObj : AssetsSelected)
	{
		if (!selectedObj->GetAsset()) continue;

		const FString* prefix = EGetPrefixMap().Find(selectedObj->GetAsset()->GetClass());

		if (!prefix || prefix->IsEmpty())
		{
			continue;
		}

		FString OldName = selectedObj->GetAsset()->GetName();
		if (OldName.StartsWith(*prefix))
		{
			continue;
		}

		ReadyToFixAssets.Add(selectedObj);

		// clear the prefix & subfix for material instance created by default editor.
		if (selectedObj->GetAsset()->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *prefix + "_" + OldName;

		NewAssetsName.Append(NewName + "\n");

		++ShouldRenameCounter;
	}

#ifdef ZH_CN
	EAppReturnType::Type result =  DlgMsgLog(EAppMsgType::YesNo, 
		TEXT("待重命名资产数: ") + FString::FromInt(ShouldRenameCounter)
		+ TEXT("\n重命名资产预览:\n") + NewAssetsName
		+ TEXT("\n是否确定继续？"));
#else
	EAppReturnType::Type result = DlgMsgLog(EAppMsgType::YesNo, 
		TEXT("Assets to rename count: ") + FString::FromInt(ShouldRenameCounter)
		+ TEXT("\nRename result preview:\n") + NewAssetsName
		+ TEXT("\nConfirm to Rename these assets?"));
#endif

	if (result == EAppReturnType::Yes)
	{
		return true;
	}

	return false;
};

void UAssetsChecker::EAddPrefixes(
	const TArray<FAssetData>& AssetsSelected)
{
	uint32 SuccessCounter = 0;
	uint32 AlreadyCounter = 0;

	for (FAssetData selectedAsset : AssetsSelected)
	{
		if (!selectedAsset.GetAsset()) continue;

		const FString* prefix = EGetPrefixMap().Find(selectedAsset.GetClass());

		if (!prefix || prefix->IsEmpty())
		{
#ifdef ZH_CN
			NtfyMsgLog(TEXT("找不到资产[") + selectedAsset.GetClass()->GetName()+ TEXT("]对应的前缀"));
#else
			NtfyMsgLog(TEXT("Can not find prefix for class ") + selectedAsset.GetClass()->GetName());
#endif
			continue;
		}

		FString OldName = selectedAsset.AssetName.ToString();
		if (OldName.StartsWith(*prefix))
		{
#ifdef ZH_CN
			NtfyMsgLog(OldName + TEXT("已有正确的命名前缀"));
#else
			NtfyMsgLog(OldName + TEXT(" alreay has prefix added"));
#endif
			++AlreadyCounter;
			continue;
		}

		// clear the prefix & subfix for material instance created by default editor.
		if (selectedAsset.GetAsset()->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *prefix + "_" + OldName;

		UEditorUtilityLibrary::RenameAsset(selectedAsset.GetAsset(), NewName);

		++SuccessCounter;
	}

#ifdef ZH_CN
	if (SuccessCounter > 0)	NtfyMsgLog(TEXT("成功重命名") + FString::FromInt(SuccessCounter) + TEXT("个资产"));
	if (AlreadyCounter > 0) NtfyMsgLog(FString::FromInt(AlreadyCounter) + TEXT("个资产已有正确命名前缀"));
#else
	if (SuccessCounter > 0)	NtfyMsgLog(TEXT("Successfully renamed ") + FString::FromInt(SuccessCounter) + TEXT(" asset") + (SuccessCounter > 1 ? TEXT("s") : ""));
	if (AlreadyCounter > 0) NtfyMsgLog(FString::FromInt(AlreadyCounter) + TEXT(" asset") + (AlreadyCounter > 1 ? TEXT("s") : "") + TEXT(" already ha") + (AlreadyCounter > 1 ? TEXT("ve") : TEXT("s")) + TEXT(" prefix"));
#endif
}

void UAssetsChecker::EAddPrefixes(
	const TArray<UObject*>& AssetsSelected)
{
	uint32 SuccessCounter = 0;
	uint32 AlreadyCounter = 0;

	for (UObject* selectedObj : AssetsSelected)
	{
		if (!selectedObj) continue;

		const FString* prefix = EGetPrefixMap().Find(selectedObj->GetClass());

		if (!prefix || prefix->IsEmpty())
		{
			ScreenMsgLog("Can not find prefix for class " + selectedObj->GetClass()->GetName(), FColor::Red);
			continue;
		}

		FString OldName = selectedObj->GetName();
		if (OldName.StartsWith(*prefix))
		{
			ScreenMsgLog(OldName + " already has prefix added", FColor::Red);
			++AlreadyCounter;
			continue;
		}

		// clear the prefix & subfix for material instance created by default editor.
		if (selectedObj->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *prefix + "_" + OldName;

		UEditorUtilityLibrary::RenameAsset(selectedObj, NewName);
		++SuccessCounter;
	}


	if (SuccessCounter > 0)	NtfyMsgLog("Successfully renamed " + FString::FromInt(SuccessCounter) + " asset" + (SuccessCounter > 1 ? "s" : ""));
	if (AlreadyCounter > 0) NtfyMsgLog(FString::FromInt(AlreadyCounter) + " asset" + (AlreadyCounter > 1 ? "s" : "") + " already ha" + (AlreadyCounter > 1 ? "ve" : "s") + " prefix");
}

bool UAssetsChecker::EFixTextureMaxSizeInGame(
	FAssetData& ClickedAssetData, 
	double maxSize, 
	bool forced)
{
	if (!ClickedAssetData.GetAsset()->IsA<UTexture2D>())
	{
		return false;
	}

	FVector2D MaxInGameSize = EGetTextureAssetMaxInGameSize(ClickedAssetData);
	double GameSize = MaxInGameSize.X > MaxInGameSize.Y ? MaxInGameSize.X : MaxInGameSize.Y;

	// NtfyMsg("MaxSize:" + FString::FromInt(MaxSize));

	if(forced && GameSize != maxSize)
	{
		return ESetTextureSize(ClickedAssetData, maxSize);
	}
		
	if( GameSize > maxSize &&
		bIsPowerOfTwo(MaxInGameSize.X) && 
		bIsPowerOfTwo(MaxInGameSize.Y))
	{
		return ESetTextureSize(ClickedAssetData, maxSize);
	}

	return false;
}

bool UAssetsChecker::ESetTextureSize(
	FAssetData& ClickedAssetData,
	double maxSize)
{
	UObject* AssetObj = ClickedAssetData.GetAsset();

	if (AssetObj->IsA<UTexture2D>())
	{
		UTexture2D * texture = Cast<UTexture2D>(AssetObj);
		if (texture)
		{
			texture->MaxTextureSize = maxSize;
			return UEditorAssetLibrary::SaveAsset(ClickedAssetData.GetObjectPathString(), false);
		}
	}

	return false;
}

bool UAssetsChecker::ESetTextureStandardSettings(FAssetData& ClickedAssetData)
{
	StandardAsset SAsset(ClickedAssetData);

	TSharedPtr<FString> subfix = SAsset.GetAssetSubfix();

	if (!subfix.IsValid())
	{
#ifdef ZH_CN
		NtfyMsgLog(TEXT("资产后缀错误\n") + ClickedAssetData.AssetName.ToString());
#else
		NtfyMsgLog(TEXT("Asset's subfix error\n") + ClickedAssetData->AssetName.ToString());
#endif
		return false;
	}

	const TextureCompressionSettings* StandardCompressionSettings
		= TextureSubfixCompressionSettingsMap.Find(*subfix);

	const bool* StandardSRGBSettings
		= TextureSubfixSRGBSettingsMap.Find(*subfix);

	if (!StandardCompressionSettings || !StandardSRGBSettings)
	{
#ifdef ZH_CN
		NtfyMsgLog(TEXT("找不到资产对应的正确配置\n") + ClickedAssetData.AssetName.ToString());
#else
		NtfyMsgLog(TEXT("Cannot find suitable fixing config\n") + ClickedAssetData->AssetName.ToString());
#endif
		return false;
	}

	bool StandardResult_Compression = ESetTextureAssetCompressionSettings(ClickedAssetData, *StandardCompressionSettings);
	bool StandardResult_sRGB = ESetTextureSRGBSettings(ClickedAssetData, *StandardSRGBSettings);

	return  StandardResult_Compression && StandardResult_sRGB;
}

TArray<FString> UAssetsChecker::EGetAssetReferencesPath(
	const FString& AssetPath)
{
	return UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath, true);
}

TArray<FString> UAssetsChecker::EGetAssetReferencesPath(
	const FAssetData& AssetData)
{
	return EGetAssetReferencesPath(AssetData.GetObjectPathString());
}

TArray<FString> UAssetsChecker::EGetAssetReferencesPath(
	const TSharedPtr<FAssetData>& AssetData)
{
	return EGetAssetReferencesPath(AssetData->GetObjectPathString());
}

FVector2D UAssetsChecker::EGetTextureAssetSourceSize(
	const FAssetData& AssetData)
{
	FVector2D size(0, 0);

	UObject* AssetOBJ = AssetData.GetAsset();

	if (!AssetOBJ->IsA<UTexture>())
	{
		return size;
	}

	UTexture2D* AssetAsT = Cast<UTexture2D>(AssetOBJ);
	
	if (AssetAsT)
	{
		size.X = AssetAsT->GetImportedSize().X;
		size.Y = AssetAsT->GetImportedSize().Y;
	}

	return size;
}

FVector2D UAssetsChecker::EGetTextureAssetMaxInGameSize(
	const FAssetData& AssetData)
{
	FVector2D size(0, 0);

	UObject* AssetOBJ = AssetData.GetAsset();                       

	if (!AssetOBJ)
	{
		return size;
	}

	if (!AssetOBJ->IsA<UTexture>())
	{
		return size;
	}

	TObjectPtr<UTexture2D> AssetAsT = Cast<UTexture2D>(AssetOBJ);

	if (!AssetAsT)
	{
		return size;
	}

	size.X = AssetAsT->GetImportedSize().X;
	size.Y = AssetAsT->GetImportedSize().Y;
	
	uint32 bias = AssetAsT->GetCachedLODBias();

	int32 MaximumTextureSize = AssetAsT->MaxTextureSize;
	
	if (MaximumTextureSize == 0 && bias == 0)
	{
		return size;
	}

	float rate;
	
	if (MaximumTextureSize == 0)
	{
		rate = 1;
	}
	else
	{
		rate = (MaximumTextureSize / (size.X > size.Y ? size.X : size.Y));
	}
	
	size.X *= (rate>1? 1:rate);
	size.Y *= (rate>1? 1:rate);

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


TSharedPtr<TextureCompressionSettings> UAssetsChecker::EGetTextureAssetCompressionSettings(
	const FAssetData& AssetData)
{
	UObject* AssetOBJ = AssetData.GetAsset();

	if (!AssetOBJ)
	{
		return nullptr;
	}

	if (!AssetOBJ->IsA<UTexture>())
	{
		return nullptr;
	}

	UTexture2D * AssetT = Cast<UTexture2D>(AssetOBJ);

	if(AssetT)
	{
		return MakeShared<TextureCompressionSettings>(AssetT->CompressionSettings);
	}

	return nullptr;
}

TSharedPtr<TextureGroup> UAssetsChecker::EGetTextureAssetTextureGroup(const FAssetData& AssetData)
{
	UObject* AssetOBJ = AssetData.GetAsset();

	if (!AssetOBJ)
	{
		return nullptr;
	}

	if (!AssetOBJ->IsA<UTexture>())
	{
		return nullptr;
	}

	UTexture2D* AssetT = Cast<UTexture2D>(AssetOBJ);

	if (AssetT)
	{
		return MakeShared<TextureGroup>(AssetT->LODGroup);
	}

	return nullptr;
}

bool UAssetsChecker::ESetTextureAssetCompressionSettings(
	const FAssetData& AssetData,
	const TEnumAsByte<TextureCompressionSettings>& CompressionSetting)
{
	UObject* AssetOBJ = AssetData.GetAsset();

	if (!AssetOBJ)
	{
		return false;
	}

	if (!AssetOBJ->IsA<UTexture>())
	{
		return false;
	}

	UTexture2D* AssetT = Cast<UTexture2D>(AssetOBJ);

	if (AssetT && (*EGetTextureAssetCompressionSettings(AssetData) != CompressionSetting))
	{
		
		AssetT->CompressionSettings = CompressionSetting;
		AssetT->UpdateResource();

		

#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功设置贴图压缩格式为\n") 
			+ *TextureCompressionMap.Find(CompressionSetting) + "\n"
			+ AssetData.AssetName.ToString());
#else
		NtfyMsgLog(TEXT("Successfully set the compression settings as\n")
			+ *TextureCompressionMap.Find(CompressionSetting) + "\n"
			+ AssetData.AssetName.ToString());
#endif

		return UEditorAssetLibrary::SaveAsset(AssetData.GetObjectPathString(), false);
	}

	return true;
}

TSharedPtr<bool> UAssetsChecker::EGetTextureAssetSRGBSettings(const FAssetData& AssetData)
{
	UObject* AssetOBJ = AssetData.GetAsset();

	if (!AssetOBJ)
	{
		return nullptr;
	}

	if (!AssetOBJ->IsA<UTexture>())
	{
		return nullptr;
	}

	UTexture2D* AssetT = Cast<UTexture2D>(AssetOBJ);

	if (AssetT)
	{
		return MakeShared<bool>(AssetT->SRGB == 0? false : true);
	}

	return nullptr;
}

bool UAssetsChecker::ESetTextureSRGBSettings(
	const FAssetData& AssetData, 
	const bool& sRGB)
{
	UObject* AssetOBJ = AssetData.GetAsset();

	if (!AssetOBJ)
	{
		return false;
	}

	if (!AssetOBJ->IsA<UTexture>())
	{
		return false;
	}

	UTexture2D* AssetT = Cast<UTexture2D>(AssetOBJ);

	if (AssetT && (*EGetTextureAssetSRGBSettings(AssetData) != sRGB))
	{

		AssetT->SRGB = sRGB;
		AssetT->UpdateResource();

		FString result = sRGB ? TEXT("true") : TEXT("false");

#ifdef ZH_CN
		NtfyMsgLog(TEXT("成功设置贴图sRGB为\n")
			+  result + "\n"
			+ AssetData.AssetName.ToString());
#else
		NtfyMsgLog(TEXT("Successfully set the sRGB settings as\n")
			+ result + "\n"
			+ AssetData.AssetName.ToString());
#endif

		return UEditorAssetLibrary::SaveAsset(AssetData.GetObjectPathString(), false);
	}

	return true;
}

void UAssetsChecker::EListUnusedAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode) 
	{
		OutList.Empty();
	}
	
	FSlowTask FindingProgress(FindInList.Num(), FText::FromString(TEXT("Checking References...")));
	FindingProgress.Initialize();
	FindingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData> & DataSPTR: FindInList)
	{
		TArray<FString> AssetReference = EGetAssetReferencesPath(DataSPTR);

		FindingProgress.EnterProgressFrame();

		if(AssetReference.Num() == 0)
		{
			OutList.Add(DataSPTR);
		}
	}

	FindingProgress.Destroy();
}

void UAssetsChecker::EListPrefixErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	for(TSharedPtr<FAssetData> AssetD : FindInList)
	{
		const FString* prefix = EGetPrefixMap().Find(AssetD->GetClass());

		if (!prefix || prefix->IsEmpty())
		{
			continue;
		}

		if (!AssetD->AssetName.ToString().StartsWith(*prefix))
		{
			OutList.Add(AssetD);
		};
	}
}

void UAssetsChecker::EListSameNameErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	TMultiMap<FString, TSharedPtr<FAssetData>> AssetsMultiInfoMap;

	for (const TSharedPtr<FAssetData> DataSharedPtr : FindInList)
	{
		AssetsMultiInfoMap.Emplace(DataSharedPtr->AssetName.ToString(), DataSharedPtr);
	}

	for (const TSharedPtr<FAssetData>& DataSharedPtr : FindInList)
	{
		TArray<TSharedPtr<FAssetData>> OutAssetData;
		AssetsMultiInfoMap.MultiFind(DataSharedPtr->AssetName.ToString(), OutAssetData);

		if (OutAssetData.Num() <= 1) continue;

		for (const TSharedPtr<FAssetData> & SamaNameData : OutAssetData)
		{
			if (SamaNameData.IsValid())
			{
				OutList.AddUnique(SamaNameData);
			}
		}
	}

}

void UAssetsChecker::EListMaxInGameSizeErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	for (TSharedPtr<FAssetData> AssetD : FindInList)
	{
		FVector2D size = EGetTextureAssetMaxInGameSize(*AssetD);

		if (size.X >2048 || 
			size.Y > 2048 || 
			!bIsPowerOfTwo(size.X) || 
			!bIsPowerOfTwo(size.Y))
		{
			OutList.Add(AssetD);
		}
	}
}

void UAssetsChecker::EListSourceSizeErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	for (TSharedPtr<FAssetData> AssetD : FindInList)
	{
		FVector2D size = EGetTextureAssetSourceSize(*AssetD);

		if (size.X > 2048 ||
			size.Y > 2048 ||
			!bIsPowerOfTwo(size.X) ||
			!bIsPowerOfTwo(size.Y))
		{
			OutList.Add(AssetD);
		}
	}
}

void UAssetsChecker::EListTextureSubfixErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	for (const TSharedPtr<FAssetData> & AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		StandardAsset Asset(*AssetDPtr);

		if (!Asset.GetAssetSubfix().IsValid())
		{
			OutList.Add(AssetDPtr);
			continue;
		}

		const TextureCompressionSettings * result = 
			TextureSubfixCompressionSettingsMap.Find(*Asset.GetAssetSubfix());

		if (!result)
		{
			OutList.Add(AssetDPtr);
		}
	}
}

void UAssetsChecker::EListTextureSettingsErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	for (const TSharedPtr<FAssetData>& AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		TSharedPtr<TextureCompressionSettings> CompressionS =
			EGetTextureAssetCompressionSettings(*AssetDPtr);

		TSharedPtr<bool> SRGBS =
			EGetTextureAssetSRGBSettings(*AssetDPtr);

		StandardAsset Asset(*AssetDPtr);

		TSharedPtr<FString> subfix = Asset.GetAssetSubfix();

		if (!subfix.IsValid())
		{
			continue;
		}

		const TextureCompressionSettings * standardCompressionSettings
			= TextureSubfixCompressionSettingsMap.Find(*subfix);

		const bool * standardSRGB
			= TextureSubfixSRGBSettingsMap.Find(*subfix);


		if (!(standardCompressionSettings && standardSRGB))
		{
			continue;
		}
		
		if(*CompressionS == *standardCompressionSettings &&  *SRGBS == *standardSRGB)
		{
			continue;
		}

		OutList.Add(AssetDPtr);
		
	}
}

void UAssetsChecker::EListTextureLODGroupErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList, 
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	for (const TSharedPtr<FAssetData>& AssetToJudge : FindInList)
	{
		OutList.Add(AssetToJudge);
	}
}

uint32 UAssetsChecker::EDeleteAssets(
	const TArray<FAssetData>& AssetsData)
{
	return ObjectTools::DeleteAssets(AssetsData);
}

uint32 UAssetsChecker::EDeleteAsset(
	const FAssetData& AssetData)
{
	TArray<FAssetData> AssetsData;
	AssetsData.Add(AssetData);

	return ObjectTools::DeleteAssets(AssetsData);
}

void UAssetsChecker::ERemoveUnusedAssets(
	const TArray<FAssetData>& AssetsDataSelected)
{
	EFixUpRedirectors();

	TArray<FAssetData> UnusedAssetsData;

	for (const FAssetData& SelectedAssetData : AssetsDataSelected)
	{
		TArray<FString> AssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString(), true);

		if (AssetReferences.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
#ifdef ZH_CN
		DlgMsg(EAppMsgType::Ok, TEXT("[恭喜]\n没有找到任何未被引用的资产"), false);
#else
		DlgMsg(EAppMsgType::Ok, TEXT("[Congratulations]\nNo unused asset found among selected assets."), false);
#endif
		return;
	}

	const uint32 NumOfAssetsDeleted = EDeleteAssets(UnusedAssetsData);

	if (NumOfAssetsDeleted == 0) return;

#ifdef ZH_CN
	NtfyMsgLog(TEXT("成功删除") + FString::FromInt(NumOfAssetsDeleted) + TEXT("个未使用资产"));
#else
	NtfyMsgLog(FString::FromInt(NumOfAssetsDeleted) + TEXT(" assets have been deleted"));
#endif
}

void UAssetsChecker::ERemoveUnusedAssets(
	const TArray<FString>& FolderPathSelected)
{
	if (FolderPathSelected.Num() < 1)
	{
		return;
	}

	for (const FString FolderPath : FolderPathSelected)
	{
		EFixUpRedirectors(FolderPath);

		TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(FolderPath);

		if (AssetPaths.Num() > 0)
		{
			TArray<FAssetData> UnusedAssetsData;

			for (const FString& assetPath : AssetPaths)
			{
				PATHLOOPIGNORE(assetPath);
				ASSETPATHNOTEXISTIGNORE(assetPath)

				if (EGetAssetReferencesPath(assetPath).Num() == 0)
				{
					const FAssetData AssetData = UEditorAssetLibrary::FindAssetData(assetPath);
					UnusedAssetsData.Add(AssetData);
				}
			}

			if (UnusedAssetsData.Num() > 0)
			{
				EDeleteAssets(UnusedAssetsData);
			};
		}
	}
}

void UAssetsChecker::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	ERemoveUnusedAssets(SelectedAssetsData);
}

void UAssetsChecker::ERemoveEmptyFolder(
	const FString FolderPathSelected)
{
	TArray<FString> path;
	path.Add(FolderPathSelected);

	ERemoveEmptyFolder(path);
}

void UAssetsChecker::ERemoveEmptyFolder(
	const TArray<FString>& FolderPathSelected)
{
	if (FolderPathSelected.Num() < 1)
	{
		return;
	}

	uint32 Counter = 0;

	FString EmptyFolderPathNames;
	TArray<FString> EmptyFolderPath;

	// loop to collect empty path
	for (const FString FolderPathCheck : FolderPathSelected)
	{
		EFixUpRedirectors(FolderPathCheck);

		PATHLOOPIGNORE(FolderPathCheck);
		if (UEditorAssetLibrary::ListAssets(FolderPathCheck).Num()==0) 
		{

			EmptyFolderPathNames.Append(FolderPathCheck);
			EmptyFolderPathNames.Append("\n");

			EmptyFolderPath.Add(FolderPathCheck);

			continue;
		};

		TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathCheck, true, true);

		for(const FString & FolderPath : FolderPathsArray)
		{
			PATHLOOPIGNORE(FolderPath);
			DIRPATHNOTEXISTIGNORE(FolderPath);

			if(UEditorAssetLibrary::ListAssets(FolderPath).Num() == 0)
			{
				EmptyFolderPathNames.Append(FolderPath);
				EmptyFolderPathNames.Append("\n");

				EmptyFolderPath.Add(FolderPath);
			}
		}
	}

	// no empty folder found.
	if (EmptyFolderPath.Num() == 0)
	{

		DlgMsgLog(EAppMsgType::Ok, 
#ifdef ZH_CN
			TEXT("[恭喜]\n此路径下未找到空文件夹")
#else
			TEXT("[Congratulations]\nNo empty folder was found under selected folders.")
#endif
			);		
		return;
	}

	// empty folder found.
#ifdef ZH_CN
	EAppReturnType::Type ConfirmResult = DlgMsgLog(EAppMsgType::OkCancel, 
		TEXT("找到如下空文件夹:\n")
		+ EmptyFolderPathNames
		+ TEXT("\n确定删除这些文件夹？"));
#else
	EAppReturnType::Type ConfirmResult = DlgMsgLog(EAppMsgType::OkCancel, 
		TEXT("Empty folder was found:\n")
		+ EmptyFolderPathNames
		+ TEXT("\n proceed to dele them?"));
#endif

	if (ConfirmResult != EAppReturnType::Ok)
	{

		NtfyMsg(
#ifdef ZH_CN
		TEXT("取消删除空文件夹")
#else
		TEXT("Canceled deleting empty folder.")
#endif
		);		
		
		return;
	};

	for(const FString& FolderPath : EmptyFolderPath)
	{
		if (UEditorAssetLibrary::DeleteDirectory(FolderPath))
		{
			++Counter;
		}
		else
		{
#ifdef ZH_CN
			NtfyMsgLog(TEXT("删除空文件夹[") + FolderPath + TEXT("]失败"));
#else
			NtfyMsgLog(TEXT("Failed to delete empty folder:") + FolderPath);
#endif
			;
		}
	}
#ifdef ZH_CN
	NtfyMsgLog(TEXT("成功删除空文件夹[") + FString::FromInt(Counter) + TEXT("]"));
#else
	NtfyMsgLog(TEXT("Successfully deleted ") + FString::FromInt(Counter) + TEXT(" folders."));
#endif
}

TArray<TSharedPtr<FAssetData>> 
UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(
	const FString& FolderPathSelected)
{
	TArray<FString> path;
	path.Add(FolderPathSelected);

	return EListAssetsDataPtrUnderSelectedFolder(path);
}

TArray<TSharedPtr<FAssetData>> 
UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(
	const TArray<FString>& FolderPathSelected)
{
	TArray<TSharedPtr<FAssetData>> AssetsDataArray;

	AssetsDataArray.Empty();


	if (FolderPathSelected.Num() <= 0)
	{
		return AssetsDataArray;
	}

	for (const FString FolderPath : FolderPathSelected) 
	{
		DIRPATHNOTEXISTIGNORE(FolderPath);

		const TArray<FString> AssetsPath =  UEditorAssetLibrary::ListAssets(FolderPath);

		for (const FString & AssetP : AssetsPath)
		{
			PATHLOOPIGNORE(AssetP);
			ASSETPATHNOTEXISTIGNORE(AssetP);

			const FAssetData AssetD = UEditorAssetLibrary::FindAssetData(AssetP);

			AssetsDataArray.Add(MakeShared<FAssetData>(AssetD));
		}
	}

	return AssetsDataArray;
}

int UAssetsChecker::EReplaceName(
	const TArray<UObject*>& AssetsSelected, 
	const FString& OriginStr, 
	const FString& ReplaceStr)
{
	if (OriginStr.IsEmpty())
	{
		return -1;
	}

	int Counter = 0;

	for (UObject * Asset:AssetsSelected)
	{
		FString OldName = Asset->GetName();
		
		if (!OldName.Contains(OriginStr))
		{
			continue;
		}

		const TCHAR* FromPtr = *OriginStr;
		const TCHAR* ToPtr = *ReplaceStr;
		FString NewName = OldName.Replace(FromPtr,ToPtr);

		UEditorUtilityLibrary::RenameAsset(Asset, NewName);

		++Counter;
	}

	return Counter;
}

bool UAssetsChecker::ERenameAsset(
	TSharedPtr<FAssetData> & AssetData, 
	FString NewName)
{
	if (!AssetData.IsValid() || !AssetData->IsValid())
	{
		return false;
	}

	FString OldName = AssetData->AssetName.ToString();

	if (OldName == NewName)
	{
		return false;
	}

	UEditorUtilityLibrary::RenameAsset(AssetData->GetAsset(), NewName);

	FString OldPath = AssetData->PackagePath.ToString();
	FString NewNameObject = NewName + "." + NewName;
	FString NewPath = FPaths::Combine(OldPath,NewNameObject);

	*AssetData.Get() = UEditorAssetLibrary::FindAssetData(NewPath);

	return true;
}

TSharedPtr<FString> UAssetsChecker::EGetAssetNameSubfix(const FAssetData& AssetSelected)
{
	StandardAsset AssetS(AssetSelected);

	return AssetS.GetAssetSubfix();
}

void UAssetsChecker::ReplaceName(
	const FString& OriginStr, 
	const FString& ReplaceStr)
{
	TArray<UObject*> AssetsSelected = UEditorUtilityLibrary::GetSelectedAssets();
	int Counter = EReplaceName(AssetsSelected, OriginStr, ReplaceStr);

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

void UAssetsChecker::EFixUpRedirectors(
	const FString & Path)
{
	TArray<FString> PathArray;
	PathArray.Add(Path);
	EFixUpRedirectors(PathArray);
}

void UAssetsChecker::EFixUpRedirectors(
	const TArray<FString>& Path)
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	// deal with content browser, the module should only have one instance.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// create filter
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Append(Path);
	Filter.ClassPaths.Emplace(UObjectRedirector::StaticClass());

	// output redirectors
	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	if (OutRedirectors.Num() == 0)
	{
#ifdef ZH_CN
		NtfyMsgLog(TEXT("[恭喜]\n未找到重定向资产(Redirector)"));
#else
		NtfyMsgLog(TEXT("[Congratulations]\nNo redirector was found"));
#endif
		return;
	}

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		};
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}

void UAssetsChecker::ECopyAssetsPtrList(
	TArray<TSharedPtr<FAssetData>>& ListToCopy, 
	TArray<TSharedPtr<FAssetData>>& ListToOutput)
{
	ListToOutput.Empty();

	for(TSharedPtr<FAssetData>& AssetD : ListToCopy)
	{
		ListToOutput.Add(AssetD);
	}
}

void UAssetsChecker::ECheckerCheck(
	const FAssetData& AssetData)
{
	UObject* Ast = AssetData.GetAsset();

	if (Ast->IsA<UTexture2D>())
	{
		UTexture2D* AsTexture = Cast<UTexture2D>(Ast);
		uint32 group = AsTexture->GetMaximumDimension();

		DlgMsgLog(EAppMsgType::Ok, FString::FromInt(group));
	}
}

void UAssetsChecker::CheckCheck()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

	TArray<TSharedPtr<FAssetData>> InList;
	TArray<TSharedPtr<FAssetData>> OutList;

	for (auto a : SelectedAssetsData) 
	{
		InList.Add(MakeShared<FAssetData>(a));
	}

	EListTextureSubfixErrorAssetsForAssetList(InList, OutList);

	for(auto a : OutList)
	{
		NtfyMsg(a->AssetName.ToString());
	}
	
}
	
const TMap<UClass*, FString>& UAssetsChecker::EGetPrefixMap()
{
	return PrefixMap;
}

