// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS251106GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "FPS251106.h"

UFPS251106GameInstance::UFPS251106GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFPS251106GameInstance::LoadMainMenu()
{
	if (UWorld* World = GetWorld())
	{
		UE_LOG(LogFPS251106, Log, TEXT("GameInstance: Loading main menu level: %s"), *MainMenuLevelName.ToString());
		UGameplayStatics::OpenLevel(World, MainMenuLevelName);
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("GameInstance: World is null, cannot load main menu"));
	}
}

void UFPS251106GameInstance::LoadGameLevel()
{
	if (UWorld* World = GetWorld())
	{
		UE_LOG(LogFPS251106, Log, TEXT("GameInstance: Loading game level: %s"), *GameLevelName.ToString());
		UGameplayStatics::OpenLevel(World, GameLevelName);
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("GameInstance: World is null, cannot load game level"));
	}
}

