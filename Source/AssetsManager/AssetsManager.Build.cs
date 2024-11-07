// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class AssetsManager : ModuleRules
{
	public AssetsManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Developer/SkeletalMeshUtilitiesCommon/Public",
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutility/Private",
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/UnrealEd/Private",
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/AssetRegistry/Private",
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Developer/AssetTools/Private",
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/Slate/Private",
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
                "Slate",
                "SlateCore",
                "Blutility",
				"EditorScriptingUtilities",
				"UMG",
				"Niagara",
                "NiagaraCore",
                "UnrealEd",
                "AssetRegistry",
                "AssetTools",
                "ContentBrowser",
                "ContentBrowserData",
				"InputCore",
				"Projects",
                "SkeletalMeshUtilitiesCommon",
                "UMGEditor",
				"AssetManagerEditor",
                "HairStrandsCore",
				"LevelEditor",
                "TextureEditor",
                "PhysicsCore",
                "InterchangeCore",
				"InterchangeEngine",
				"InterchangePipelines",
				"InterchangeNodes"

                
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
                "Projects",
                "InputCore",
                "EditorFramework",
                "ToolMenus"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
