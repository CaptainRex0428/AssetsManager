// Fill out your copyright notice in the Description page of Project Settings.

#pragma warning(disable: 5103)

#include "AssetsChecker/AssetsChecker.h"

#include "ManagerLogger.h"
#include "StandardAsset/CustomStandardAsset.h"
#include "StandardAsset/CustomStandardTexture2D.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "UObject/SavePackage.h"

#include "ConfigManager.h"

#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"




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

int UAssetsChecker::DuplicateAssets(
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

TArray<FString> UAssetsChecker::GetValidCategoryTag(
	AssetCategory Category,
	FString ConfigSection)
{
	if (UManagerConfig::Get().GetSection(*ConfigSection))
	{
		TArray<FConfigValue> TagValue;
		TagValue.Empty();

		switch (Category)
		{

		case AssetCategory::Character:
		{
			TagValue = UManagerConfig::Get().GetSectionValuesArray(
				*ConfigSection,
				"CharacterCategoryTag");
			break;
		}
		case AssetCategory::Effect:
		{
			TagValue = UManagerConfig::Get().GetSectionValuesArray(
				*ConfigSection,
				"EffectCategoryTag");
			break;
		}
		case AssetCategory::Scene:
		{
			TagValue = UManagerConfig::Get().GetSectionValuesArray(
				*ConfigSection,
				"SceneCategoryTag");
			break;
		}
		case AssetCategory::UI:
		{
			TagValue = UManagerConfig::Get().GetSectionValuesArray(
				*ConfigSection,
				"UICategoryTag");
			break;
		}
		case AssetCategory::Hair:
		{
			TagValue = UManagerConfig::Get().GetSectionValuesArray(
				*ConfigSection,
				"HairCategoryTag");
			break;
		}
		case AssetCategory::Undefined:
		case AssetCategory::LastCatergory:
		default:
		{
			break;
		}
		}

		TArray<FString> Tags;
		Tags.Empty();

		for (FConfigValue& value : TagValue)
		{
			Tags.Add(value.GetValue());
		}

		return Tags;
	}

	return TArray<FString>();
}

FString UAssetsChecker::GetCategoryTag(AssetCategory Category)
{
	switch (Category)
	{
	case AssetCategory::Character:
		return L"Character";
	case AssetCategory::Effect:
		return L"Effect";
	case AssetCategory::Scene:
		return L"Scene";
	case AssetCategory::UI:
		return L"UI";
	case AssetCategory::Hair:
		return L"Hair";
	case AssetCategory::Undefined:
	case AssetCategory::LastCatergory:
	default:
		return L"Global";
	}
}

bool UAssetsChecker::ConfirmPrefixes(
	TArray< TSharedPtr<FAssetData>>& AssetsSelected,
	TArray< TSharedPtr<FAssetData>>& ReadyToFixAssets)
{
	ReadyToFixAssets.Empty();
	uint32 ShouldRenameCounter = 0;
	FString NewAssetsName;
	
	for (TSharedPtr<FAssetData>& selectedObj : AssetsSelected)
	{
		if (!selectedObj->GetAsset()) continue;

		FCustomStandardAssetData StandardAsset(*selectedObj);

		if (!StandardAsset.Get().IsPrefixNonstandarized() || !StandardAsset.Get().GetAssetStandardPrefix())
		{
			continue;
		};

		ReadyToFixAssets.Add(selectedObj);

		FString OldName = selectedObj->AssetName.ToString();

		// clear the prefix & subfix for material instance created by default editor.
		if (selectedObj->GetAsset()->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *StandardAsset.Get().GetAssetStandardPrefix() + "_" + OldName;

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

void UAssetsChecker::AddPrefixes(
	const TArray<FAssetData>& AssetsSelected)
{
	uint32 SuccessCounter = 0;
	uint32 AlreadyCounter = 0;

	for (FAssetData selectedAsset : AssetsSelected)
	{
		if (!selectedAsset.GetAsset()) continue;

		FCustomStandardAssetData StandardAsset(selectedAsset);

		if (!StandardAsset.Get().IsPrefixNonstandarized())
		{
			continue;
		};


		if (!StandardAsset.Get().GetAssetStandardPrefix())
		{
#ifdef ZH_CN
			NtfyMsgLog(TEXT("找不到资产[") + StandardAsset.GetClass()->GetName()+ TEXT("]对应的前缀"));
#else
			NtfyMsgLog(TEXT("Can not find prefix for class ") + selectedAsset.GetClass()->GetName());
#endif
			continue;
		}

		FString OldName = StandardAsset.AssetName.ToString();

		if (!StandardAsset.Get().IsPrefixNonstandarized())
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
		if (StandardAsset.GetAsset()->IsA<UMaterialInstanceConstant>())
		{	
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *StandardAsset.Get().GetAssetStandardPrefix() + "_" + OldName;

		UEditorUtilityLibrary::RenameAsset(StandardAsset.GetAsset(), NewName);

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

void UAssetsChecker::AddPrefixes(
	const TArray<UObject*>& AssetsSelected)
{
	uint32 SuccessCounter = 0;
	uint32 AlreadyCounter = 0;

	for (UObject* selectedObj : AssetsSelected)
	{
		if (!selectedObj) continue;
		
		FAssetData AssetDataIn(selectedObj);

		FCustomStandardAssetData StandardAsset(AssetDataIn);

		if (!StandardAsset.Get().IsPrefixNonstandarized())
		{
			continue;
		};


		if (!StandardAsset.Get().GetAssetStandardPrefix())
		{
#ifdef ZH_CN
			NtfyMsgLog(TEXT("找不到资产[") + StandardAsset.GetClass()->GetName() + TEXT("]对应的前缀"));
#else
			NtfyMsgLog(TEXT("Can not find prefix for class ") + selectedAsset.GetClass()->GetName());
#endif
			continue;
		}

		FString OldName = StandardAsset.AssetName.ToString();

		if (!StandardAsset.Get().IsPrefixNonstandarized())
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
		if (StandardAsset.GetAsset()->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *StandardAsset.Get().GetAssetStandardPrefix() + "_" + OldName;

		UEditorUtilityLibrary::RenameAsset(StandardAsset.GetAsset(), NewName);

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

TArray<FString> UAssetsChecker::GetAssetReferencesPath(
	const FString& AssetPath,
	bool bLoadToCheck)
{
	return UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath, bLoadToCheck);
}

TArray<FString> UAssetsChecker::GetAssetReferencesPath(
	const FAssetData& AssetData)
{
	return GetAssetReferencesPath(AssetData.GetObjectPathString());
}

TArray<FString> UAssetsChecker::GetAssetReferencesPath(
	const TSharedPtr<FAssetData>& AssetData)
{
	return GetAssetReferencesPath(AssetData->GetObjectPathString());
}

void UAssetsChecker::FilterUnusedAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	TArray<FString> StoredFolderPaths,
	bool bRecursive,
	bool isAdditiveMode)
{
	if (!isAdditiveMode) 
	{
		OutList.Empty();
	}

	TArray<FString> AssetPkgToRemove;
	TArray<FString> AssetPkgCannotRemove;
	TArray<TSharedPtr<FAssetData>> AssetsRest;

	AssetPkgToRemove.Empty();
	AssetPkgCannotRemove.Empty();
	AssetsRest.Empty();

	for (const TSharedPtr<FAssetData>& AssetDPtr : FindInList)
	{
		TArray<FString> AssetReference = GetAssetReferencesPath(AssetDPtr);

		if (AssetReference.Num() == 0)
		{
			if (isAdditiveMode)
			{
				if (OutList.Contains(AssetDPtr)) continue;
			}

			OutList.AddUnique(AssetDPtr);

			AssetPkgToRemove.AddUnique(AssetDPtr->PackageName.ToString());
		}
		else
		{
			AssetsRest.AddUnique(AssetDPtr);
		}
	}

	if (!bRecursive)
	{
		return;
	}

	bool CanEndLoop = false;

	while (!CanEndLoop)
	{
		bool EndCondition = true;

		TArray<TSharedPtr<FAssetData>> AssetsRestNew;
		AssetsRestNew.Empty();

		for (const TSharedPtr<FAssetData>& AssetDPtr : AssetsRest)
		{
			TArray<FString> AssetReferences = GetAssetReferencesPath(AssetDPtr);

			bool NeedToDelete = true;
			bool CannotDelete = false;

			for (FString& reference : AssetReferences)
			{
				if (StoredFolderPaths.Num()!=0)
				{
					bool ReferenceOutOfFolders = true;

					for (FString folderPath : StoredFolderPaths)
					{
						if (FPaths::IsUnderDirectory(reference, folderPath))
						{
							ReferenceOutOfFolders = false;
						};
					}

					if (ReferenceOutOfFolders)
					{
						AssetPkgCannotRemove.AddUnique(AssetDPtr->PackageName.ToString());
						NeedToDelete = false;
						CannotDelete = true;
						break;
					}
				}

				if (!AssetPkgToRemove.Contains(reference))
				{
					NeedToDelete = false;
					break;
				}
			}

			if (CannotDelete) continue;

			if (NeedToDelete)
			{
				EndCondition = false;

				if (isAdditiveMode)
				{
					if (OutList.Contains(AssetDPtr)) continue;
				}

				OutList.AddUnique(AssetDPtr);
				AssetPkgToRemove.AddUnique(AssetDPtr->PackageName.ToString());
				continue;
			}

			AssetsRestNew.AddUnique(AssetDPtr);
		}

		AssetsRest = AssetsRestNew;

		CanEndLoop = EndCondition;
	}
	
}

void UAssetsChecker::FilterPrefixErrorAssetsForAssetList(
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
		FCustomStandardAssetData StandardAsset(*AssetDPtr);

		if (StandardAsset.Get().IsPrefixNonstandarized())
		{
			if (isAdditiveMode)
			{
				if (OutList.Contains(AssetDPtr)) continue;
			}

			OutList.Add(AssetDPtr);
		}

	}

}

void UAssetsChecker::FilterSameNameErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	TMultiMap<FString, TSharedPtr<FAssetData>> AssetsMultiInfoMap;

	FSlowTask CollectingProgress(2*FindInList.Num(), FText::FromString(TEXT("Collecting Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData> & DataSharedPtr : FindInList)
	{
		AssetsMultiInfoMap.Emplace(DataSharedPtr->AssetName.ToString(), DataSharedPtr);

		CollectingProgress.EnterProgressFrame();
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

				// may get error
				if (isAdditiveMode)
				{
					if (OutList.Contains(SamaNameData)) continue;
				}

				if (!OutList.Contains(SamaNameData))
				{
					OutList.AddUnique(SamaNameData);
				}
			}
		}

		CollectingProgress.EnterProgressFrame();
	}

	CollectingProgress.Destroy();
}

void UAssetsChecker::FilterMaxInGameSizeErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool bStrictMode,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}
	
	FSlowTask CollectingProgress(FindInList.Num(), FText::FromString(TEXT("Scaning Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData> & AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		FCustomStandardTexture2DData CustomStandardAsset(*AssetDPtr, bStrictMode);

		uint8 checkResult = CustomStandardAsset.Get().IsStandarized(MAXINGAMESIZE);

		if (!(checkResult & MAXINGAMESIZE))
		{
			OutList.Add(AssetDPtr);
		}
		
		CollectingProgress.EnterProgressFrame();

	}

	CollectingProgress.Destroy();
}

void UAssetsChecker::FilterSourceSizeErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool bStrictMode,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	FSlowTask CollectingProgress(FindInList.Num(), FText::FromString(TEXT("Scaning Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData>& AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		FCustomStandardTexture2DData CustomStandardAsset(*AssetDPtr,bStrictMode);
		
		uint8 checkResult = CustomStandardAsset.Get().IsStandarized(SOURCESIZE);

		if (!(checkResult & SOURCESIZE))
		{
			OutList.Add(AssetDPtr);
		}

		CollectingProgress.EnterProgressFrame();
	}

	CollectingProgress.Destroy();
}

void UAssetsChecker::FilterTextureSubfixErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	FSlowTask CollectingProgress(FindInList.Num(), FText::FromString(TEXT("Collecting Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData> & AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		FCustomStandardTexture2DData Asset(*AssetDPtr);

		uint8 checkResult = Asset.Get().IsStandarized(SUFFIX);

		if (!(checkResult & SUFFIX))
		{
			if (isAdditiveMode)
			{
				if (OutList.Contains(AssetDPtr)) continue;
			}

			OutList.Add(AssetDPtr);
		}

		CollectingProgress.EnterProgressFrame();
	}

	CollectingProgress.Destroy();
}

void UAssetsChecker::FilterTextureSettingsErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList,
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	FSlowTask CollectingProgress(FindInList.Num(), FText::FromString(TEXT("Collecting Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData>& AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		FCustomStandardTexture2DData TextureAsset(*AssetDPtr);

		uint8 standarized = TextureAsset.Get().IsStandarized(COMPRESSIONSETTINGS | SRGB);

		if (!((standarized & COMPRESSIONSETTINGS) && (standarized & SRGB)))
		{
			if (isAdditiveMode)
			{
				if (OutList.Contains(AssetDPtr)) continue;
			}

			OutList.Add(AssetDPtr);
		};

		CollectingProgress.EnterProgressFrame();
	}

	CollectingProgress.Destroy();
}

void UAssetsChecker::FilterTextureLODGroupErrorAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList, 
	bool isAdditiveMode)
{
	if (!isAdditiveMode)
	{
		OutList.Empty();
	}

	FSlowTask CollectingProgress(FindInList.Num(), FText::FromString(TEXT("Collecting Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const TSharedPtr<FAssetData>& AssetDPtr : FindInList)
	{
		if (!AssetDPtr->GetAsset()->IsA<UTexture2D>())
		{
			continue;
		}

		FCustomStandardTexture2DData SAsset(*AssetDPtr);

		uint8 checkResult = SAsset.Get().IsStandarized(LODGROUP);

		if (!(checkResult & LODGROUP))
		{
			if (isAdditiveMode)
			{
				if (OutList.Contains(AssetDPtr)) continue;
			}

			OutList.Add(AssetDPtr);

		}

		CollectingProgress.EnterProgressFrame();
	}

	CollectingProgress.Destroy();
}

uint32 UAssetsChecker::DeleteAssets(
	const TArray<FAssetData>& AssetsData)
{
	return ObjectTools::DeleteAssets(AssetsData);
}

uint32 UAssetsChecker::DeleteAsset(
	const FAssetData& AssetData)
{
	TArray<FAssetData> AssetsData;
	AssetsData.Add(AssetData);

	return ObjectTools::DeleteAssets(AssetsData);
}

uint32 UAssetsChecker::DeleteObject(UObject* OBJ)
{
	if(!OBJ)
	{
		return -1;
	}

	// call AssetRegistry to mark the object as deleted
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().AssetDeleted(OBJ);

	// Update content browser
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(OBJ);

	UPackage* Package = OBJ->GetOutermost();
	if (Package)
	{
		Package->SetDirtyFlag(false);  // Marked as no need to save
	}

	// force GC
	OBJ->MarkAsGarbage();
	GEngine->ForceGarbageCollection(true);

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToAssets(TArray<FAssetData>());

	OBJ->ConditionalBeginDestroy();
	OBJ = nullptr;

	return 1;
}

void UAssetsChecker::RemoveUnusedAssets(
	const TArray<FAssetData>& AssetsDataSelected)
{
	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsData;

	FSlowTask CollectingProgress(AssetsDataSelected.Num(), FText::FromString(TEXT("Loading Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const FAssetData& SelectedAssetData : AssetsDataSelected)
	{
		TArray<FString> AssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString(), true);

		if (AssetReferences.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}

		CollectingProgress.EnterProgressFrame();
	}

	CollectingProgress.Destroy();

	if (UnusedAssetsData.Num() == 0)
	{
#ifdef ZH_CN
		DlgMsg(EAppMsgType::Ok, TEXT("[恭喜]\n没有找到任何未被引用的资产"), false);
#else
		DlgMsg(EAppMsgType::Ok, TEXT("[Congratulations]\nNo unused asset found among selected assets."), false);
#endif
		return;
	}

	const uint32 NumOfAssetsDeleted = DeleteAssets(UnusedAssetsData);

	if (NumOfAssetsDeleted == 0) return;

#ifdef ZH_CN
	NtfyMsgLog(TEXT("成功删除") + FString::FromInt(NumOfAssetsDeleted) + TEXT("个未使用资产"));
#else
	NtfyMsgLog(FString::FromInt(NumOfAssetsDeleted) + TEXT(" assets have been deleted"));
#endif
}

void UAssetsChecker::RemoveUnusedAssets(
	const TArray<FString>& FolderPathSelected)
{
	if (FolderPathSelected.Num() < 1)
	{
		return;
	}

	for (const FString FolderPath : FolderPathSelected)
	{
		FixUpRedirectors(FolderPath);

		TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(FolderPath);

		if (AssetPaths.Num() > 0)
		{
			TArray<FAssetData> UnusedAssetsData;

			for (const FString& assetPath : AssetPaths)
			{
				PATHLOOPIGNORE(assetPath);
				ASSETPATHNOTEXISTIGNORE(assetPath)

				if (GetAssetReferencesPath(assetPath).Num() == 0)
				{
					const FAssetData AssetData = UEditorAssetLibrary::FindAssetData(assetPath);
					UnusedAssetsData.Add(AssetData);
				}
			}

			if (UnusedAssetsData.Num() > 0)
			{
				DeleteAssets(UnusedAssetsData);
			};
		}
	}
}

void UAssetsChecker::RemoveEmptyFolder(
	const FString FolderPathSelected)
{
	TArray<FString> path;
	path.Add(FolderPathSelected);

	RemoveEmptyFolder(path);
}

void UAssetsChecker::RemoveEmptyFolder(
	const TArray<FString>& FolderPathSelected)
{
	if (FolderPathSelected.Num() < 1)
	{
		return;
	}

	uint32 Counter = 0;

	FString EmptyFolderPathNames;
	TArray<FString> EmptyFolderPath;

	int TaskCount = FolderPathSelected.Num();
	FSlowTask RemoveUnusedFolderTask(TaskCount,FText::FromString(TEXT("Iterating Folders...")));
	RemoveUnusedFolderTask.Initialize();
	RemoveUnusedFolderTask.MakeDialog();

	// loop to collect empty path
	for (const FString FolderPathCheck : FolderPathSelected)
	{
		FixUpRedirectors(FolderPathCheck);

		PATHLOOPIGNORE(FolderPathCheck);
		if (UEditorAssetLibrary::ListAssets(FolderPathCheck).Num()==0) 
		{

			EmptyFolderPathNames.Append(FolderPathCheck);
			EmptyFolderPathNames.Append("\n");

			EmptyFolderPath.Add(FolderPathCheck);

			continue;
		};

		TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathCheck, true, true);

		int subTaskCount = FolderPathsArray.Num();

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

			RemoveUnusedFolderTask.EnterProgressFrame((1/TaskCount)*(1/subTaskCount));
		}
	}

	RemoveUnusedFolderTask.Destroy();

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
UAssetsChecker::ListAssetsDataPtrUnderSelectedFolder(
	const FString& FolderPathSelected)
{
	TArray<FString> path;
	path.Add(FolderPathSelected);

	return ListAssetsDataPtrUnderSelectedFolder(path);
}

TArray<TSharedPtr<FAssetData>> 
UAssetsChecker::ListAssetsDataPtrUnderSelectedFolder(
	const TArray<FString>& FolderPathSelected)
{
	TArray<TSharedPtr<FAssetData>> AssetsDataArray;

	AssetsDataArray.Empty();


	if (FolderPathSelected.Num() <= 0)
	{
		return AssetsDataArray;
	}

	FSlowTask CollectingProgress(FolderPathSelected.Num(), FText::FromString(TEXT("Collecting Assets ...")));
	CollectingProgress.Initialize();
	CollectingProgress.MakeDialog();

	for (const FString FolderPath : FolderPathSelected) 
	{
		DIRPATHNOTEXISTIGNORE(FolderPath);

		const TArray<FString> AssetsPath =  UEditorAssetLibrary::ListAssets(FolderPath);

		for (const FString & AssetP : AssetsPath)
		{
			CollectingProgress.EnterProgressFrame((1 / FolderPathSelected.Num()) / AssetsPath.Num());

			PATHLOOPIGNORE(AssetP);
			ASSETPATHNOTEXISTIGNORE(AssetP);

			const FAssetData AssetD = UEditorAssetLibrary::FindAssetData(AssetP);

			AssetsDataArray.Add(MakeShared<FAssetData>(AssetD));
		}
	}
	
	CollectingProgress.Destroy();

	return AssetsDataArray;
}

void UAssetsChecker::FilterAssetsWithTagFromAssetList(
	const TArray<TSharedPtr<FAssetData>>& FindInList, 
	TArray<TSharedPtr<FAssetData>>& OutList, 
	FName TagName)
{
	OutList.Empty();

	if (FindInList.Num() < 1)
	{
		return;
	}

	for (TSharedPtr<FAssetData> AssetDataPtr : FindInList)
	{
		if (AssetDataPtr->AssetName.ToString().Contains(TagName.ToString()))
		{
			OutList.AddUnique(AssetDataPtr);
		};
	}
}

int UAssetsChecker::ReplaceName(
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

bool UAssetsChecker::RenameAsset(
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

TSharedPtr<FString> UAssetsChecker::GetAssetNameSubfix(const FAssetData& AssetSelected)
{
	FCustomStandardAssetData AssetS(AssetSelected);

	return AssetS.Get().GetAssetSuffix();
}

void UAssetsChecker::FixUpRedirectors(
	const FString & Path)
{
	TArray<FString> PathArray;
	PathArray.Add(Path);
	FixUpRedirectors(PathArray);
}

void UAssetsChecker::FixUpRedirectors(
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

bool UAssetsChecker::JudgeSort(FVector2D& A, FVector2D& B, bool bReverse)
{
	if (A.X == B.X)
	{
		return bReverse ? A.Y > B.Y : A.Y < B.Y;
	}

	return bReverse ? A.X > B.X : A.X < B.X;
}

bool UAssetsChecker::JudgeSort(FString& A, FString& B, bool bReverse)
{
	int AL = A.Len();
	int BL = B.Len();

	for (int Idx = 0; Idx < (AL<BL? AL:BL); ++Idx)
	{
		if(A[Idx] == B[Idx])
		{
			continue;
		}

		return bReverse ? A[Idx] > B[Idx] : A[Idx] < B[Idx];
	}

	return bReverse ? AL < BL : AL > BL;
}

TArray<FString> UAssetsChecker::SplitStringRecursive(
	const FString& InStr, 
	const FString& SpliteTag)
{
	TArray<FString> OutList;
	OutList.Empty();

	FString LStr;
	FString RStr;

	if (InStr.Split(SpliteTag, &LStr, &RStr))
	{
		OutList.Add(LStr);

		while (RStr.Split(SpliteTag, &LStr, &RStr))
		{
			OutList.Add(LStr);
		}

		OutList.Add(RStr);
	}
	else
	{
		OutList.Add(InStr);
	}

	return OutList;
}

bool UAssetsChecker::StringMatchPattern(
	const FString & Pattern, 
	FString& StringToMatch)
{
	TArray<FString> Subpatterns =  SplitStringRecursive(Pattern,L"!");

	for (int32 Idx = 0; Idx < Subpatterns.Num(); Idx++)
	{
		if(Subpatterns[Idx].TrimStartAndEnd().IsEmpty())
		{
			continue;
		}

		if (!Idx)
		{
			if (!StringMatchSubPattern(Subpatterns[Idx].TrimStartAndEnd(), StringToMatch))
			{
				return false;
			};

			continue;
		}

		if (StringMatchSubPattern(Subpatterns[Idx].TrimStartAndEnd(), StringToMatch))
		{
			return false;
		};
	}

	return true;
}

bool UAssetsChecker::StringMatchSubPattern(
	const FString& Pattern, 
	FString& StringToMatch)
{
	TArray<FString> Subpatterns = SplitStringRecursive(Pattern, L"&");

	for (FString subpattern : Subpatterns)
	{
		if (subpattern.TrimStartAndEnd().IsEmpty())
		{
			continue;
		}

		bool matched = false;

		TArray<FString> SubConditions = SplitStringRecursive(subpattern, L" ");

		for (FString Condition : SubConditions)
		{
			if(Condition.TrimStartAndEnd().IsEmpty())
			{
				continue;
			}

			if (StringToMatch.Contains(Condition.TrimStartAndEnd()))
			{
				matched = true;
				break;
			}
		}

		if (!matched)
		{
			return false;
		}
	}

	return true;
}


double UAssetsChecker::ByteConversion(
	uint64 ByteSize,
	AssetSizeDisplayUnit Target)
{
	if (Target == AssetSizeDisplayUnit::AssetSizeDisplayUnit_Max)
	{
		return ByteSize;
	}

	double scale = FMath::Pow(1024.f, uint8(Target) - 1);

	double result = ByteSize / scale;

	return result;
}

double UAssetsChecker::ByteConversion(
	uint64 ByteSize,
	FString& UnitOut,
	bool OnlyUnit,
	AssetSizeDisplayUnit Target)
{
	double SizeResult = ByteSize;
	AssetSizeDisplayUnit UnitType = Target;

	if (Target != AssetSizeDisplayUnit::AssetSizeDisplayUnit_Max)
	{
		SizeResult = ByteConversion(ByteSize, Target);
	}
	else
	{
		UnitType = AssetSizeDisplayUnit::Byte;

		for (enum AssetSizeDisplayUnit unitLoop = UnitType;
			unitLoop < AssetSizeDisplayUnit::AssetSizeDisplayUnit_Max;
			unitLoop = (AssetSizeDisplayUnit)(uint8(unitLoop) + 1))
		{
			double SizePreview = ByteConversion(ByteSize, unitLoop);

			if (SizePreview < 0.1)
			{
				break;
			}

			UnitType = unitLoop;
			SizeResult = SizePreview;
		}
	}

	FString UnitStr = "";

	switch (UnitType)
	{
	case AssetSizeDisplayUnit::bit:
	{ UnitStr = "bit"; break; }
	case AssetSizeDisplayUnit::Byte:
	{ UnitStr = "Byte"; break; }
	case AssetSizeDisplayUnit::KB:
	{ UnitStr = "KB"; break; }
	case AssetSizeDisplayUnit::MB:
	{ UnitStr = "MB"; break; }
	case AssetSizeDisplayUnit::GB:
	{ UnitStr = "GB"; break; }
	case AssetSizeDisplayUnit::TB:
	{ UnitStr = "TB"; break; }
	case AssetSizeDisplayUnit::AssetSizeDisplayUnit_Max:
	{ UnitStr = "Byte"; break; }
	default:
	{ UnitStr = "Byte"; break; }
	}

	if (OnlyUnit)
	{
		UnitOut = UnitStr;
	}
	else
	{
		UnitOut = FString::Printf(L"%.4f%s", SizeResult, *UnitStr);
	}

	return SizeResult;

}

AssetsInfoDisplayLevel UAssetsChecker::IntToDisplayLevel(int32 Idx)
{
	if (Idx < 0)
	{
		return AssetsInfoDisplayLevel::AssetsInfoDisplay_Display;
	}

	if (Idx < (int32)AssetsInfoDisplayLevel::AssetsInfoDisplayLevel_Max)
	{
		return (AssetsInfoDisplayLevel)Idx;
	}

	return AssetsInfoDisplayLevel::AssetsInfoDisplay_Exceed;
}

FColor UAssetsChecker::DisplayLevelToColor(AssetsInfoDisplayLevel & DisplayLevel)
{
	switch (DisplayLevel)
	{
	case AssetsInfoDisplayLevel::AssetsInfoDisplay_Display:
		return FColor::White;
	case AssetsInfoDisplayLevel::AssetsInfoDisplay_Tip:
		return FColor::Cyan;
	case AssetsInfoDisplayLevel::AssetsInfoDisplay_Attention:
		return FColor::Green;
	case AssetsInfoDisplayLevel::AssetsInfoDisplay_Warning:
		return FColor::Yellow;
	case AssetsInfoDisplayLevel::AssetsInfoDisplay_Risk:
		return FColor::Orange;
	case AssetsInfoDisplayLevel::AssetsInfoDisplay_Exceed:
		return FColor::Red;
	case AssetsInfoDisplayLevel::AssetsInfoDisplayLevel_Max:
	default:
		return FColor::White;
		break;
	}
}

FString UAssetsChecker::IntStrAddColumn(FString SourceStr)
{
	int Count = SourceStr.Len();
	int ColumnCount = (Count-1) / 3;
	int StartPos = Count % 3 ? Count % 3 : 3;

	for (int Idx = 0; Idx < ColumnCount; ++ Idx)
	{
		SourceStr.InsertAt(StartPos+Idx*3, ",");
	}

	return SourceStr;
}

bool UAssetsChecker::OpenLocalFolder(const FString& FolderPath)
{
	FString AbsPath = FPaths::ConvertRelativePathToFull(FolderPath);

	if (!FPaths::DirectoryExists(AbsPath))
	{
		return false;
	}

	FPlatformProcess::ExploreFolder(*AbsPath);

	return true;
}

bool UAssetsChecker::OpenSizeMapUI(TArray<TSharedPtr<FAssetData>> AssetsIn)
{
	TArray<FAssetIdentifier> list;

	for (TSharedPtr<FAssetData> AssetData : AssetsIn)
	{
		list.Add(FAssetIdentifier(AssetData->PackageName));
	}

	IAssetManagerEditorModule::Get().OpenSizeMapUI(list);
	
	return true;
}

bool UAssetsChecker::OpenSizeMapUI(TSharedPtr<FAssetData> AssetIn)
{
	TArray<TSharedPtr<FAssetData>> Assets;
	Assets.Add(AssetIn);

	return UAssetsChecker::OpenSizeMapUI(Assets);
}

bool UAssetsChecker::OpenEditorUI(TSharedPtr<FAssetData> AssetIn)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(AssetIn->GetObjectPathString());
	return true;
}

TArray<FString> UAssetsChecker::GetCurrentContentBrowserSelectedPaths()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	IContentBrowserSingleton& ContentBrowser = ContentBrowserModule.Get();

	TArray<FString> SelectedPaths;
	ContentBrowser.GetSelectedFolders(SelectedPaths);

	for (FString& path : SelectedPaths)
	{
		path.RemoveFromStart(L"/All");
	}

	return SelectedPaths;
}

FString UAssetsChecker::GetCurrentContentBrowserPath()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	IContentBrowserSingleton& ContentBrowser = ContentBrowserModule.Get();

	FContentBrowserItemPath Path = ContentBrowser.GetCurrentPath();
	FString PathStr = Path.GetVirtualPathString();
	PathStr.RemoveFromStart(L"/All");

	return PathStr;
}

bool UAssetsChecker::SaveAsset(const FString& AssetToSave, bool bOnlyIfIsDirty)
{
	return UEditorAssetLibrary::SaveAsset(AssetToSave, bOnlyIfIsDirty);
}

bool UAssetsChecker::SaveAsset(UObject* ObjectToSave)
{
	if (ObjectToSave && ObjectToSave->MarkPackageDirty())
    {
        UPackage* AssetPackage = ObjectToSave->GetOutermost();

        if (AssetPackage)
        {
            FString PackageFilePath = FPackageName::LongPackageNameToFilename(AssetPackage->GetName(), FPackageName::GetAssetPackageExtension());

            FSavePackageArgs SaveArgs;
            SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
            SaveArgs.Error = GError;
            SaveArgs.bForceByteSwapping = true;
            SaveArgs.SaveFlags = SAVE_None;

            return UPackage::SavePackage(AssetPackage, ObjectToSave, *PackageFilePath, SaveArgs);
        }

        return false;
    }

    return false;
}
