// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SSTBatchCombinerStyle.h"

class FSSTBatchCombinerCommands : public TCommands<FSSTBatchCombinerCommands>
{
public:

	FSSTBatchCombinerCommands()
		: TCommands<FSSTBatchCombinerCommands>(TEXT("SSTBatchCombiner"), NSLOCTEXT("Contexts", "SSTBatchCombiner", "SSTBatchCombiner Plugin"), NAME_None, FSSTBatchCombinerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
