// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetsChecker/AssetsChecker.h"
#include "ManagerLogger.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define PATHLOOPIGNORE(Path) if (##Path.Contains("Developers") || \
							##Path.Contains("Collections")) \
						{continue;};
#define DIRPATHNOTEXISTIGNORE(Path) if (!UEditorAssetLibrary::DoesDirectoryExist(##Path)){continue;};
#define ASSETPATHNOTEXISTIGNORE(Path) if (!UEditorAssetLibrary::DoesAssetExist(##Path)){continue;};


int UAssetsChecker::EDuplicateAssets(const TArray<FAssetData>& AssetsDataSelected, int NumOfDupicates, bool forced)
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

void UAssetsChecker::DuplicateAssets(int NumOfDupicates,bool forced)
{
	
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	
	int Counter = EDuplicateAssets(SelectedAssetsData, NumOfDupicates, forced);

	if (Counter <= 0)
	{
		EAppReturnType::Type ReturnType = DlgMsgLog(EAppMsgType::Ok, "Number input is invalid.");
		return;
		
	}

	FString Msg = FString::FromInt(Counter) + " files copied.";
	NtfyMsg(Msg);
}

void UAssetsChecker::EAddPrefixes(const TArray<UObject*>& AssetsSelected)
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
			ScreenMsgLog(OldName + " alreay has prefix added", FColor::Red);
			++AlreadyCounter;
			continue;
		}

		// clear the predix & subfix for material instance created by default editor.
		if (selectedObj->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart("M_");
			OldName.RemoveFromEnd("_Inst");
		}

		const FString NewName = *prefix + "_" + OldName;

		UEditorUtilityLibrary::RenameAsset(selectedObj, NewName);
		++SuccessCounter;
	}


	if (SuccessCounter > 0)	NtfMsgLog("Successfully renamed " + FString::FromInt(SuccessCounter) + " asset" + (SuccessCounter > 1 ? "s" : ""));
	if (AlreadyCounter > 0) NtfMsgLog(FString::FromInt(AlreadyCounter) + " asset" + (AlreadyCounter > 1 ? "s" : "") + " already ha" + (AlreadyCounter > 1 ? "ve" : "s") + " prefix");
}

void UAssetsChecker::AddPrefixes()
{
	TArray<UObject *> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();

	EAddPrefixes(SelectedObjects);
}

TArray<FString> UAssetsChecker::EGetAssetReferencesPath(const FString& AssetPath)
{
	return UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath, true);
}

TArray<FString> UAssetsChecker::EGetAssetReferencesPath(const FAssetData& AssetData)
{
	return EGetAssetReferencesPath(AssetData.GetObjectPathString());
}

TArray<FString> UAssetsChecker::EGetAssetReferencesPath(const TSharedPtr<FAssetData>& AssetData)
{
	return EGetAssetReferencesPath(AssetData->GetObjectPathString());
}

uint32 UAssetsChecker::EDeleteAssets(const TArray<FAssetData>& AssetsData)
{
	return ObjectTools::DeleteAssets(AssetsData);
}

uint32 UAssetsChecker::EDeleteAsset(const FAssetData& AssetData)
{
	TArray<FAssetData> AssetsData;
	AssetsData.Add(AssetData);

	return ObjectTools::DeleteAssets(AssetsData);
}

void UAssetsChecker::ERemoveUnusedAssets(const TArray<FAssetData>& AssetsDataSelected)
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
		DlgMsg(EAppMsgType::Ok, "No unused asset found among selected assets.", false);
		return;
	}

	const uint32 NumOfAssetsDeleted = EDeleteAssets(UnusedAssetsData);

	if (NumOfAssetsDeleted == 0) return;

	NtfMsgLog(FString::FromInt(NumOfAssetsDeleted) + " assets have been deleted");
}

void UAssetsChecker::ERemoveUnusedAssets(const TArray<FString>& FolderPathSelected)
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

void UAssetsChecker::ERemoveEmptyFolder(const FString FolderPathSelected)
{
	TArray<FString> path;
	path.Add(FolderPathSelected);

	ERemoveEmptyFolder(path);
}

void UAssetsChecker::ERemoveEmptyFolder(const TArray<FString>& FolderPathSelected)
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
		DlgMsgLog(EAppMsgType::Ok, "No empty folder was found under selected folders.");
		return;
	}

	// empty folder found.
	EAppReturnType::Type ConfirmResult = DlgMsgLog(EAppMsgType::OkCancel, "Empty folder was found:\n"
									+ EmptyFolderPathNames
									+ "\n proceed to dele them?");

	if (ConfirmResult != EAppReturnType::Ok)
	{
		NtfyMsg("Canceled deleting empty folder.");
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
			NtfMsgLog("Failed to delete empty folder:" + FolderPath);
		}
	}

	NtfMsgLog("Successfully deleted " + FString::FromInt(Counter) + " folders.");
}

TArray<TSharedPtr<FAssetData>> UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(const FString& FolderPathSelected)
{
	TArray<FString> path;
	path.Add(FolderPathSelected);

	return EListAssetsDataPtrUnderSelectedFolder(path);
}

TArray<TSharedPtr<FAssetData>> UAssetsChecker::EListAssetsDataPtrUnderSelectedFolder(const TArray<FString>& FolderPathSelected)
{
	if (FolderPathSelected.Num() <= 0)
	{
		return TArray<TSharedPtr<FAssetData>>();
	}

	TArray<TSharedPtr<FAssetData>> AssetsDataArray;

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

int UAssetsChecker::EReplaceName(const TArray<UObject*>& AssetsSelected, const FString& OriginStr, const FString& ReplaceStr)
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

void UAssetsChecker::ReplaceName(const FString& OriginStr, const FString& ReplaceStr)
{
	TArray<UObject*> AssetsSelected = UEditorUtilityLibrary::GetSelectedAssets();
	int Counter = EReplaceName(AssetsSelected, OriginStr, ReplaceStr);

	if (Counter == -1)
	{
		EAppReturnType::Type ReturnType = DlgMsgLog(EAppMsgType::Ok, "Origin string pattern should not be empty.");
		return;
	}

	if (Counter >= 0)
	{
		NtfMsgLog(FString::FromInt(Counter) + " asset" + (Counter > 1 ? "s'" : "'s") + " name has been replaced.");
		return;
	}
}

void UAssetsChecker::EFixUpRedirectors(const FString & Path)
{
	TArray<FString> PathArray;
	PathArray.Add(Path);
	EFixUpRedirectors(PathArray);
}

void UAssetsChecker::EFixUpRedirectors(const TArray<FString>& Path)
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
		NtfMsgLog("No redirector was found");
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

void UAssetsChecker::ECheckerCheck(const TArray<FString>& Path)
{

	DlgMsg(EAppMsgType::Ok, "ReayToTest");
}

const TMap<UClass*, FString>& UAssetsChecker::EGetPrefixMap()
{
	return PrefixMap;
}
