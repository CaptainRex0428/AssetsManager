#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AssetsManagerStyle.h"

class FAssetsManagerCommands : public TCommands<FAssetsManagerCommands>
{
public:

	FAssetsManagerCommands()
		: TCommands<FAssetsManagerCommands>(TEXT("AssetsManager"), NSLOCTEXT("Contexts", "AssetsManager", "AssetsManager Plugin"), NAME_None, FAssetsMangerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction_DeleteEmptyFolders;
	TSharedPtr< FUICommandInfo > PluginAction_OpenAssetsManagerWindowWithSelectedPath;
	TSharedPtr< FUICommandInfo > PluginAction_OpenAssetsManagerWindowWithCurrentPath;
	TSharedPtr< FUICommandInfo > PluginAction_LookDev;
};