// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/MainMenuPlayerController.h"
#include "Menu/MainMenuUI.h"
#include "Engine/Engine.h"
#include "FPS251106.h"

AMainMenuPlayerController::AMainMenuPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Only create UI for local player controller
	if (!IsLocalPlayerController())
	{
		UE_LOG(LogFPS251106, Warning, TEXT("MainMenuPlayerController: Not a local player controller, skipping UI creation"));
		return;
	}

	// Set input mode to UI only
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	// Create and display the main menu UI
	if (MainMenuUIClass)
	{
		MainMenuUI = CreateWidget<UMainMenuUI>(this, MainMenuUIClass);
		if (MainMenuUI)
		{
			MainMenuUI->AddToViewport(0);
			UE_LOG(LogFPS251106, Log, TEXT("MainMenuPlayerController: Successfully created and added main menu UI"));
		}
		else
		{
			UE_LOG(LogFPS251106, Error, TEXT("MainMenuPlayerController: Failed to create main menu UI widget"));
		}
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("MainMenuPlayerController: MainMenuUIClass is not set! Please set it in the Blueprint."));
	}
}

void AMainMenuPlayerController::RemoveMainMenuUI()
{
	if (MainMenuUI)
	{
		MainMenuUI->RemoveFromParent();
		MainMenuUI = nullptr;
		UE_LOG(LogFPS251106, Log, TEXT("MainMenuPlayerController: Removed main menu UI"));
	}
}

