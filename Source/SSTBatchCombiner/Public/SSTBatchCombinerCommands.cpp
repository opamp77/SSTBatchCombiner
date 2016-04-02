// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SSTBatchCombinerPrivatePCH.h"
#include "SSTBatchCombinerCommands.h"

#define LOCTEXT_NAMESPACE "FSSTBatchCombinerModule"

void FSSTBatchCombinerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SSTBatchCombiner", "Execute SSTBatchCombiner action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
