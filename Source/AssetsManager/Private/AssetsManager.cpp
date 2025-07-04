﻿// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetsManagerCommand.h"
#include "ManagerLogger.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetsChecker/AssetsChecker.h"
#include "AssetsChecker/AssetsTypeActions.h"
#include "AssetsCreator/AssetsCreator.h"
#include "StandardAsset/CustomStandardTexture2D.h"
#include "SlateWidgets/ManagerSlate.h"
#include "SlateWidgets/BatchRenameSlate.h"
#include "SlateWidgets/Material/MaterialCreatorSlate.h"
#include "AssetToolsModule.h"
#include "AssetsManagerStyle.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "InterchangeManager.h"
#include "InterchangeSourceData.h"
#include "InterchangeFactoryBase.h"
#include "InterchangePipelineBase.h"
#include "InterchangeAssetImportData.h"

#include "ConfigManager.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

#include "AssetsImporter/CustomFBXImporterFactory.h"

#include "EditorLevelUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LevelEditorSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingAlwaysLoaded.h"
#include "Engine/World.h"
#include "EditorLevelUtils.h"

#include "FileHelpers.h"
#include "Interfaces/IMainFrameModule.h"
#include "Modules/ModuleManager.h"

#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FAssetsManagerModule"

void FAssetsManagerModule::StartupModule()
{

	FAssetsMangerStyle::Initialize();
	FAssetsMangerStyle::ReloadTextures();

	FAssetsManagerCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_LookDev,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnLookDevButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_CharacterLookDev,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnCharacterLookDevButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithCurrentPath,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsManagerWithCurrentPathButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithSelectedPath,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnAssetsManagerWithSelectedPathButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAssetsManagerCommands::Get().PluginAction_DeleteEmptyFolders,
		FExecuteAction::CreateRaw(this, &FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetsManagerModule::RegisterMenus));
	
	// Registry Code

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName(CONTENTFOLDER_MANAGERTAB_NAME),
		FOnSpawnTab::CreateRaw(this, &FAssetsManagerModule::OnSpawnManagerSlateTab))
		.SetDisplayName(FText::FromString(TEXT(CONTENTFOLDER_MANAGERTAB_NAME)))
		.SetIcon(FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);


	
	// OnAssetPre/PostImport
	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		if (GEditor)
		{
			UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>();
			if (ImportSubsystem)
			{
				ImportSubsystem->OnAssetPreImport.AddRaw(this, &FAssetsManagerModule::OnEditorDelegatePreImport);
				ImportSubsystem->OnAssetPostImport.AddRaw(this, &FAssetsManagerModule::OnEditorDelegatePostImport);
			}

			GEngine->Exec(nullptr, TEXT("Interchange.FeatureFlags.Import.FBX true"));
		}
	});

	FCoreDelegates::OnEnginePreExit.AddLambda([this]()
	{
		if (GEditor)
		{
			if (UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>())
			{
					ImportSubsystem->OnAssetPreImport.RemoveAll(this);
					ImportSubsystem->OnAssetPostImport.RemoveAll(this);
			}
		}
	});
	
	// 获取 IAssetTools 模块
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();
	
	// AssetTypeActions
	// TSharedRef<UCustomTexture2DAssetTypeActions> CustomAssetTypeAction = MakeShareable(new UCustomTexture2DAssetTypeActions());
	// AssetTools.RegisterAssetTypeActions(CustomAssetTypeAction);

	// 创建自定义 UFBXImporterFactory 的实例并注册
	UCustomFBXImporterFactory* CustomFactory = NewObject<UCustomFBXImporterFactory>(UCustomFBXImporterFactory::StaticClass());

	if (CustomFactory)
	{
		CustomFactory->AddToRoot(); // 确保 Factory 不被 GC 回收
		MsgLog(TEXT("FBX Importer Factory registered."));
	}
	else
	{
		MsgLog(TEXT("Failed to create FBX Importer Factory instance."));
	}


	UInterchangeManager& InterchangeManager = UInterchangeManager::GetInterchangeManager();

	// OnAssetPostImport -- Texture
	// InterchangeManager.OnAssetPostImport.AddRaw(this,&FAssetsManagerModule::OnInterchangeAssetPostImport);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "AssetsManager",
			NSLOCTEXT("AssetsManager", "AssetsManager", "Assets Manager"),
			NSLOCTEXT("AssetsManager", "AssetsManager", "Configure settings for AssetsManager"),
			GetMutableDefault<UManagerSettings>()
		);
	}
}

void FAssetsManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAssetsMangerStyle::ShutDown();

	FAssetsManagerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(CONTENTFOLDER_MANAGERTAB_NAME));
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(CONTENTFOLDER_MATERIALCREATORTAB_NAME));
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(TABNAME_BATCHRENAME));
	FAssetsMangerStyle::ShutDown();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "AssetsManager");
	}
}

#pragma region ContentBrowserMenuExtend

// Third bind. The actual function to execute.
void FAssetsManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	UAssetsChecker::RemoveUnusedAssets(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnDeleteEmptyFolderButtonClicked()
{
	UAssetsChecker::RemoveEmptyFolder(SelectedContentFolderPaths);
}

void FAssetsManagerModule::OnAssetsManagerWithSelectedPathButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MANAGERTAB_NAME));
}

void FAssetsManagerModule::OnAssetsManagerWithCurrentPathButtonClicked()
{
	SelectedContentFolderPaths.Empty();
	SelectedContentFolderPaths.Add(UAssetsChecker::GetCurrentContentBrowserPath());

	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MANAGERTAB_NAME));
}

void FAssetsManagerModule::OnMaterialCreatButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(CONTENTFOLDER_MATERIALCREATORTAB_NAME));
}

void FAssetsManagerModule::OnLookDevButtonClicked()
{
	const UManagerSettings* Settings = GetDefault<UManagerSettings>();
	if (!Settings)
	{
		MsgLog("Get Settings Error");
		return;
	}

	UWorld* TemplateLevel = Settings->LevelAsset.LoadSynchronous();
	if (!TemplateLevel)
	{
		MsgLog("Level asset not found or failed to load.");
		return;
	}
	
	if(!FEditorFileUtils::SaveDirtyPackages(true, true, false))
	{
		return;
	}

	UWorld* NewLevel = GEditor->NewMap(false);
	// FEditorFileUtils::ResetLevelFilenames();

	if (!NewLevel)
	{
		MsgLog("Creating new level failed.");
		return;
	}

	NewLevel->Rename(TEXT("LookDev"), nullptr);
	
	FString LevelPackageName = Settings->LevelAsset.GetLongPackageName();
	TSubclassOf<ULevelStreaming> LevelStreamingClass = ULevelStreamingAlwaysLoaded::StaticClass();
	FTransform LevelTransform = FTransform::Identity;
	ULevelStreaming* LevelStreaming = UEditorLevelUtils::AddLevelToWorld(NewLevel, *LevelPackageName, LevelStreamingClass, LevelTransform);
	
	if (LevelStreaming)
	{
		LevelStreaming->SetShouldBeVisible(true);
		LevelStreaming->SetShouldBeLoaded(true);
	}

	IMainFrameModule& MainFrameModule = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.SetLevelNameForWindowTitle(TEXT("LookDev *"));
}

void FAssetsManagerModule::OnCharacterLookDevButtonClicked()
{

	UWorld* World = GEditor->GetEditorWorldContext().World();
	
	if (!World)
	{
		MsgLog(L"No world context found");
		return;
	}

	const UManagerSettings* Settings = GetDefault<UManagerSettings>();

	if (!Settings)
	{
		MsgLog(L"Get Settings Error");
		return;
	}

	UBlueprint* Blueprint = Settings->StandardCharacterDisplay.LoadSynchronous();

	if (!Blueprint)
	{
		MsgLog(L"Blueprint asset not found or failed to load.");
		return;
	}

	// Viewport
	FEditorViewportClient* ViewportClient = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	if (!ViewportClient)
	{
		MsgLog("Lost viewport.");
		return;
	}

	FVector SpawnLocation = ViewportClient->GetViewLocation();
	FRotator SpawnRotation = ViewportClient->GetViewRotation();

	FVector ForwardVector = SpawnRotation.Vector();
	SpawnLocation += ForwardVector * 500.0f;

	// Spawn the actor
	UClass* ActorClass = Blueprint->GeneratedClass;
	if (!ActorClass)
	{
		MsgLog(L"Blueprint class initialize error.");
		return;
	}

	if (!ActorClass->IsChildOf<AAssetsDisplay>())
	{
		MsgLog(L"LookDev blueprint type is not allowed.");
		return;
	}
	
	AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, SpawnLocation, FRotator(0, 0, 0));
	if (SpawnedActor)
	{
		MsgLog("Actor spawned.");
	}
}

void FAssetsManagerModule::OnInterchangeAssetPostImport(UObject* OBJ)
{
	bool shouldDelete = false;
	UCustomStandardObject StandardObj(OBJ);

	if (StandardObj.IsPrefixNonstandarized())
	{
		DlgMsgLog(EAppMsgType::Ok,FString::Printf(L"[Prefix Error]%s",*StandardObj.GetClassValidObjectName()));

		shouldDelete = true;
	}

	if (shouldDelete)
	{
		UAssetsChecker::DeleteObject(OBJ);
	}
}

void FAssetsManagerModule::OnEditorDelegatePreImport(UFactory* Factory, UClass* InClass, UObject* Outer, const FName& Name, const TCHAR* Type)
{
	MsgLog(L"PreImport");
}

void FAssetsManagerModule::OnEditorDelegatePostImport(UFactory* Factory, UObject* CreatedObject)
{
	if (CreatedObject)
	{

		const UManagerSettings* Settings = GetDefault<UManagerSettings>();

		if (!Settings)
		{
			return;
		}

		if (Settings->ImportCheck)
		{
			if (CreatedObject->IsA<UTexture2D>())
			{
				UCustomStandardTexture2D StandardObj(CreatedObject, Settings->StrictMode);
				FString AssetName = StandardObj.GetClassValidObjectName();

				if (StandardObj.IsPrefixNonstandarized())
				{
					NtfyMsgLog(FString::Printf(L"[Prefix Error]%s", *AssetName), true);

					UAssetsChecker::DeleteObject(CreatedObject);
					return;
				}

				// 角色类
				if (Settings->CharacterImportLimit && (StandardObj.GetCommonAssetCategory() == AssetCategory::Character || StandardObj.GetCommonAssetCategory() == AssetCategory::Hair))
				{
					uint8 result = StandardObj.NotStandarized();

					if (result != 0)
					{
						if (Settings->SuffixCheck && (result & SUFFIX))
						{
							NtfyMsgLog(FString::Printf(L"[Suffix Error]%s", *AssetName), true);

							UAssetsChecker::DeleteObject(CreatedObject);
							return;
						};

						FString unstandardTag = "";

						if (result & SOURCESIZE) { unstandardTag += TEXT("->SourceSize Oversize\n"); };
						if (result & MAXINGAMESIZE) { unstandardTag += TEXT("->MaxInGameSize Oversize\n"); };
						if (result & COMPRESSIONSETTINGS) { unstandardTag += TEXT("->CompressionSettings Error\n"); };
						if (result & SRGB) { unstandardTag += TEXT("->SRGB Error\n"); };
						if (result & LODGROUP) { unstandardTag += TEXT("->LODGroup Error\n"); };

						FString tips = FString::Printf(L"[Asset Config Error]\n%s\n\n%s\nAuto Fix?", *AssetName, *unstandardTag);

						if (result & SOURCESIZE | result & MAXINGAMESIZE)
						{
							EAppReturnType::Type returntype = DlgMsgLog(EAppMsgType::YesNoCancel, tips);

							if (returntype == EAppReturnType::Yes)
							{
								StandardObj.Fix(SOURCESIZE | MAXINGAMESIZE);
							}

							if (returntype == EAppReturnType::Cancel || (returntype == EAppReturnType::No && Settings->ImportForcedLimit))
							{
								UAssetsChecker::DeleteObject(CreatedObject);
								return;
							}
						}

						StandardObj.Fix(COMPRESSIONSETTINGS | SRGB | LODGROUP);

					}
				}

				if (Settings->UIImportLimit && StandardObj.GetCommonAssetCategory() == AssetCategory::UI)
				{

				}

				if (Settings->SceneImportLimit && StandardObj.GetCommonAssetCategory() == AssetCategory::Scene)
				{

				}

				if (Settings->VFXImportLimit && StandardObj.GetCommonAssetCategory() == AssetCategory::Effect)
				{

				}
			}
		}



		if (CreatedObject->IsAsset())
		{
			MsgLog(FString::Printf(L"Verification completed: %s", *CreatedObject->GetName()));
		}
	}
	
}

void FAssetsManagerModule::RegisterMenus()
{
	// register LookDev Button

	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(
				FAssetsManagerCommands::Get().PluginAction_LookDev, 
				PluginCommands,
				TAttribute<FText>(),
				TAttribute<FText>(),
				FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.ManagerLookDev")
				);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("AssetsManager");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetsManagerCommands::Get().PluginAction_LookDev));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.ManagerLookDev");
			}
		}
	}

	// register CharacterLookDev Button

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(
				FAssetsManagerCommands::Get().PluginAction_CharacterLookDev,
				PluginCommands,
				TAttribute<FText>(),
				TAttribute<FText>(),
				FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.CharacterLookDev")
			);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("AssetsManager");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetsManagerCommands::Get().PluginAction_CharacterLookDev));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "LevelEditor.CharacterLookDev");
			}
		}
	}


	// Content Browser
	
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		
		// register ContentBrowser Paths Selected Menu
		{
			ContentBrowserModule.GetAllPathViewContextMenuExtenders().Add(
				FContentBrowserMenuExtender_SelectedPaths::CreateLambda([this](const TArray<FString>& SelectedPaths)
				{
						TSharedRef<FExtender> MenuExtender(new FExtender());

						SelectedContentFolderPaths = SelectedPaths;

						if (SelectedPaths.Num() > 0)
						{
							MenuExtender->AddMenuExtension(
								FName("PathViewFolderOptions"),
								EExtensionHook::After,
								PluginCommands,
								FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
									{
										MenuBuilder.AddMenuEntry(
											FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithSelectedPath,
											NAME_None,
											LOCTEXT("AssetsManager_Label", "AssetsManager"),
											TAttribute<FText>(),
											FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager"));

										MenuBuilder.AddMenuEntry(
											FAssetsManagerCommands::Get().PluginAction_DeleteEmptyFolders,
											NAME_None,
											LOCTEXT("DeleteEmptyFolder_Label", "DeleteEmptyFolder"),
											TAttribute<FText>(),
											FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedFolders"));
									}));
						}

						return MenuExtender;
				}));
		}

		// register ContentBrowser Current Path Menu
		{
			UToolMenu* ContentBrowserToolBar = UToolMenus::Get()->ExtendMenu("ContentBrowser.ToolBar");
			{
				FToolMenuSection& Section = ContentBrowserToolBar->AddSection(
					"AssetsManager",
					LOCTEXT("AssetsManager_Label", "AssetsManager"),
					FToolMenuInsert(FName("Save"), EToolMenuInsertType::After)
				);

				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetsManagerCommands::Get().PluginAction_OpenAssetsManagerWindowWithCurrentPath));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FAssetsMangerStyle::GetStyleSetName(), "ContentBrowser.AssetsManager");
			}
		}
		
		
	}
}

#pragma endregion

#pragma region CustomEditorTab

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnManagerSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{

	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SManagerSlateTab)
				//.TitleText(CONTENTFOLDER_MANAGERTAB_NAME)
				.TitleText("Assets Manager")
				.SelectedFolderPaths(SelectedContentFolderPaths)
		];
}

TSharedRef<SDockTab> FAssetsManagerModule::OnSpawnMaterialCreatorSlateTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
		SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SMaterialCreatorSlate)
				//.TitleText(CONTENTFOLDER_MANAGERTAB_NAME)
				.TitleText("Material Creator")
		];
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetsManagerModule, AssetsManager)