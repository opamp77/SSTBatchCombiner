// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SSTBatchCombinerPrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "SSTBatchCombinerStyle.h"
#include "SSTBatchCombinerCommands.h"

#include "LevelEditor.h"


static const FName SSTBatchCombinerTabName("SSTBatchCombiner");

#define LOCTEXT_NAMESPACE "FSSTBatchCombinerModule"

void FSSTBatchCombinerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSSTBatchCombinerStyle::Initialize();
	FSSTBatchCombinerStyle::ReloadTextures();

	FSSTBatchCombinerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSSTBatchCombinerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSSTBatchCombinerModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FSSTBatchCombinerModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FSSTBatchCombinerModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FSSTBatchCombinerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FSSTBatchCombinerStyle::Shutdown();

	FSSTBatchCombinerCommands::Unregister();
}

void FSSTBatchCombinerModule::PluginButtonClicked()
{
	TArray<FString> OutFileNames;
	TArray<FString> OutFileNames2;
	FDesktopPlatformModule::Get()->OpenFileDialog(nullptr, "select First Image Files", "", "", "Image Files (*.png)|*.png", 1, OutFileNames);
	FDesktopPlatformModule::Get()->OpenFileDialog(nullptr, "select Second Image Files", "", "", "Image Files (*.png)|*.png", 1, OutFileNames2);
	
	if (OutFileNames.Num() && OutFileNames2.Num())
	{
		if (OutFileNames.Num() != OutFileNames2.Num())
		{
			FString DialogText = "Error! first set quantity does not match second set!";
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(DialogText));
			return;
		}

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

		for (int32 i = 0; i < OutFileNames.Num(); i++)
		{
			TArray<uint8> RawFileData;
			TArray<uint8> RawFileData2;

			if (FFileHelper::LoadFileToArray(RawFileData, *OutFileNames[i]) && FFileHelper::LoadFileToArray(RawFileData2, *OutFileNames2[i]))
			{
				IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
				IImageWrapperPtr ImageWrapper2 = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num())
					&& ImageWrapper2.IsValid() && ImageWrapper2->SetCompressed(RawFileData2.GetData(), RawFileData2.Num()))
				{
					const TArray<uint8>* RawData = nullptr;
					const TArray<uint8>* RawData2 = nullptr;
					if (ImageWrapper->GetRaw(ERGBFormat::BGRA, ImageWrapper->GetBitDepth(), RawData)
						&& ImageWrapper2->GetRaw(ERGBFormat::BGRA, ImageWrapper2->GetBitDepth(), RawData2))
					{
						
						uint32 ImageWidth = ImageWrapper->GetWidth();
						uint32 ImageHeight = ImageWrapper->GetHeight();
						uint32 ImageWidth2 = ImageWrapper2->GetWidth();
						uint32 ImageHeight2 = ImageWrapper2->GetHeight();

						if ((ImageWidth != ImageWidth2) || (ImageHeight != ImageHeight2))
						{
							FString DialogText = "Error! Image dimensions do not match for frame " + FString::FromInt(i);
							FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(DialogText));
							return;
						}

						TArray<uint8> newdata;
						newdata = *RawData;
						
						for (int32 a = 0; a < RawData->Num() - 4; a = a + 4)
						{
							newdata[a + 1] = (*RawData2)[a];
						}
						//save
						ImageWrapper->SetRaw(newdata.GetData(), newdata.GetAllocatedSize(), ImageWidth, ImageHeight, ERGBFormat::BGRA, 8);
						const TArray<uint8>& PNGData = ImageWrapper->GetCompressed(100);
						FFileHelper::SaveArrayToFile(PNGData, *OutFileNames[i]);
					} //if imagewrapper.getraw
				} //if imagewrapper.valid
			} //if load file
		}// for files loop
	} //if files
}

void FSSTBatchCombinerModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FSSTBatchCombinerCommands::Get().PluginAction);
}

void FSSTBatchCombinerModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FSSTBatchCombinerCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSSTBatchCombinerModule, SSTBatchCombiner)