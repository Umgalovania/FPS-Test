// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS251106GameInstance.h"
#include "Menu/NetworkSessionManager.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "FPS251106.h"

UFPS251106GameInstance::UFPS251106GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create network session manager
	NetworkSessionManager = CreateDefaultSubobject<UNetworkSessionManager>(TEXT("NetworkSessionManager"));
}

void UFPS251106GameInstance::Init()
{
	Super::Init();

	// Debug: Check PVPGameModeClass value at initialization
	if (PVPGameModeClass)
	{
		UE_LOG(LogFPS251106, Log, TEXT("GameInstance Init: PVPGameModeClass is set to: %s"), *PVPGameModeClass->GetPathName());
	}
	else
	{
		UE_LOG(LogFPS251106, Warning, TEXT("GameInstance Init: PVPGameModeClass is NULL! Please set it in BP_FPS251106GameInstance blueprint."));
	}

	// Initialize network session manager
	if (NetworkSessionManager)
	{
		NetworkSessionManager->Initialize();
	}
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

void UFPS251106GameInstance::HostGame(int32 MaxPlayers)
{
	// Debug: Check PVPGameModeClass value
	if (PVPGameModeClass)
	{
		UE_LOG(LogFPS251106, Log, TEXT("HostGame: PVPGameModeClass is set to: %s"), *PVPGameModeClass->GetPathName());
	}
	else
	{
		UE_LOG(LogFPS251106, Warning, TEXT("HostGame: PVPGameModeClass is NULL! Please check blueprint settings."));
	}
	
	if (NetworkSessionManager)
	{
		// Remove any existing bindings first to avoid duplicate bindings
		NetworkSessionManager->OnSessionCreated.RemoveDynamic(this, &UFPS251106GameInstance::OnSessionCreated);
		// Bind to session created delegate
		NetworkSessionManager->OnSessionCreated.AddDynamic(this, &UFPS251106GameInstance::OnSessionCreated);
		NetworkSessionManager->CreateSession(MaxPlayers);
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("NetworkSessionManager is null!"));
		// Fallback to single player
		LoadGameLevel();
	}
}

void UFPS251106GameInstance::OnSessionCreated(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// Get the room code from the session
		FString RoomCode;
		if (NetworkSessionManager)
		{
			// Get room code from current session
			if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
			{
				if (IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
				{
					if (FOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession))
					{
						Session->SessionSettings.Get(FName("ROOMCODE"), RoomCode);
					}
				}
			}
		}

		// Store room code for later use
		SetPendingRoomCode(RoomCode);

		// Travel to the PVP level as a listen server with PVP GameMode
		if (UWorld* World = GetWorld())
		{
			// Use PVP level name if specified, otherwise fall back to GameLevelName
			FName LevelToLoad = !PVPLevelName.IsNone() ? PVPLevelName : GameLevelName;
			
			// Check if PVP GameMode class is set
			if (!PVPGameModeClass)
			{
				UE_LOG(LogFPS251106, Error, TEXT("PVPGameModeClass is not set in GameInstance! Please set it in the blueprint."));
			}
			
			// Build travel URL with game mode and listen parameters
			// URL format: LevelName?game=GameModePath&listen
			FString LevelPath = LevelToLoad.ToString();
			FString TravelURL = LevelPath;
			
			bool bFirstParam = true;
			
			// Add game mode parameter if specified
			if (PVPGameModeClass)
			{
				FString GameModePath = PVPGameModeClass->GetPathName();
				UE_LOG(LogFPS251106, Log, TEXT("GameInstance: Using PVP GameMode: %s"), *GameModePath);
				// URL encode: replace / with + (Unreal Engine URL encoding)
				GameModePath.ReplaceInline(TEXT("/"), TEXT("+"));
				TravelURL += FString::Printf(TEXT("%sgame=%s"), bFirstParam ? TEXT("?") : TEXT("&"), *GameModePath);
				bFirstParam = false;
			}
			else
			{
				UE_LOG(LogFPS251106, Warning, TEXT("GameInstance: PVPGameModeClass is null, game mode will use map default!"));
			}
			
			// Add listen parameter for listen server
			TravelURL += FString::Printf(TEXT("%slisten"), bFirstParam ? TEXT("?") : TEXT("&"));
			
			UE_LOG(LogFPS251106, Log, TEXT("GameInstance: Hosting PVP game, traveling to: %s (Room Code: %s)"), *TravelURL, *RoomCode);
			
			// When creating a session from menu, use ClientTravel to travel as listen server
			// This will properly set up the listen server and load the game mode
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				// Fallback: try ServerTravel if we're already a server
				UE_LOG(LogFPS251106, Warning, TEXT("No PlayerController found, trying ServerTravel"));
				World->ServerTravel(TravelURL);
			}
		}
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("Failed to create session, falling back to single player"));
		LoadGameLevel();
	}
}

