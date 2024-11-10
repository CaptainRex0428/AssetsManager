
#include "AssetsManagerCommand.h"

#define LOCTEXT_NAMESPACE "FAssetsManagerModule"

void FAssetsManagerCommands::RegisterCommands()
{

	UI_COMMAND(PluginAction_LookDev, "LookDev", "Open Standard LookDev", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PluginAction_OpenAssetsManagerWindowWithSelectedPath, "AssetsManager", "Open AssetsManager Window", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PluginAction_OpenAssetsManagerWindowWithCurrentPath, "AssetsManager", "Open AssetsManager Window", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PluginAction_DeleteEmptyFolders, "DeleteEmptyFolders", "Delete Empty Folders", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PluginAction_CharacterLookDev, "Standard Character Instance", "Open Standard Character Instance", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE