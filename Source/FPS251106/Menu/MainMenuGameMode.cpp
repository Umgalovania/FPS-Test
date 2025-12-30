// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/MainMenuGameMode.h"
#include "Menu/MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
}


