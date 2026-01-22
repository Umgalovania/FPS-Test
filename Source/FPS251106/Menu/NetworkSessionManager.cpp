// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/NetworkSessionManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Menu/MainMenuPlayerController.h"
#include "Menu/MainMenuUI.h"
#include "Menu/MultiplayerMenuUI.h"
#include "Menu/JoinGameDialog.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "FPS251106.h"

UNetworkSessionManager::UNetworkSessionManager()
{
	SessionName = NAME_GameSession;
}

UWorld* UNetworkSessionManager::GetWorld() const
{
	// Get world from outer (GameInstance)
	if (UObject* Outer = GetOuter())
	{
		if (UGameInstance* GameInstance = Cast<UGameInstance>(Outer))
		{
			return GameInstance->GetWorld();
		}
	}
	return nullptr;
}

void UNetworkSessionManager::Initialize()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: OnlineSubsystem found: %s"), *OnlineSubsystem->GetSubsystemName().ToString());
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: SessionInterface is valid, binding delegates"));
			// Bind delegates
			OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UNetworkSessionManager::OnCreateSessionComplete);
			OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UNetworkSessionManager::OnStartSessionComplete);
			OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNetworkSessionManager::OnFindSessionsComplete);
			OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UNetworkSessionManager::OnJoinSessionComplete);
			OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UNetworkSessionManager::OnDestroySessionComplete);
			OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UNetworkSessionManager::OnEndSessionComplete);
		}
		else
		{
			UE_LOG(LogFPS251106, Error, TEXT("NetworkSessionManager: SessionInterface is null!"));
		}
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("NetworkSessionManager: OnlineSubsystem is null! Make sure OnlineSubsystem is configured in DefaultEngine.ini"));
	}
}

void UNetworkSessionManager::CreateSession(int32 MaxPlayers)
{
	UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: CreateSession called with MaxPlayers=%d"), MaxPlayers);
	
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogFPS251106, Error, TEXT("SessionInterface is not valid! Make sure Initialize() was called."));
		OnSessionCreated.Broadcast(false);
		return;
	}

	UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: Checking for existing session..."));
	// If a session already exists, destroy it first
	auto ExistingSession = SessionInterface->GetNamedSession(SessionName);
	if (ExistingSession != nullptr)
	{
		UE_LOG(LogFPS251106, Warning, TEXT("NetworkSessionManager: Existing session found, destroying it first..."));
		DestroySession();
	}

	// Create session settings
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bIsLANMatch = true; // Use LAN for P2P
	SessionSettings->bUseLobbiesIfAvailable = false;
	SessionSettings->BuildUniqueId = 1;

	// Add custom settings
	SessionSettings->Set(FName("MAPNAME"), FString("Lvl_Shooter"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// Generate and add room code
	FString RoomCode = GenerateRoomCode();
	SessionSettings->Set(FName("ROOMCODE"), RoomCode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: Generated room code: %s"), *RoomCode);

	// Create the session
	UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: Adding delegate handle and creating session..."));
	OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	
	UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: Calling CreateSession..."));
	if (!SessionInterface->CreateSession(0, SessionName, *SessionSettings))
	{
		UE_LOG(LogFPS251106, Error, TEXT("NetworkSessionManager: CreateSession returned false immediately!"));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		OnSessionCreated.Broadcast(false);
	}
	else
	{
		UE_LOG(LogFPS251106, Log, TEXT("NetworkSessionManager: CreateSession call succeeded, waiting for delegate..."));
	}
}

void UNetworkSessionManager::FindSessions()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogFPS251106, Error, TEXT("SessionInterface is not valid!"));
		OnSessionSearchComplete.Broadcast();
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->bIsLanQuery = true; // Search LAN sessions
	SessionSearch->QuerySettings.Set(FName("PRESENCE"), true, EOnlineComparisonOp::Equals);

	OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		UE_LOG(LogFPS251106, Error, TEXT("Failed to find sessions!"));
		OnSessionSearchComplete.Broadcast();
	}
}

void UNetworkSessionManager::JoinSession(int32 SessionIndex)
{
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
	{
		UE_LOG(LogFPS251106, Error, TEXT("SessionInterface or SessionSearch is not valid!"));
		OnSessionJoined.Broadcast(false);
		return;
	}

	if (SessionIndex < 0 || SessionIndex >= SessionSearch->SearchResults.Num())
	{
		UE_LOG(LogFPS251106, Error, TEXT("Invalid session index: %d"), SessionIndex);
		OnSessionJoined.Broadcast(false);
		return;
	}

	OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	if (!SessionInterface->JoinSession(0, SessionName, SessionSearch->SearchResults[SessionIndex]))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		UE_LOG(LogFPS251106, Error, TEXT("Failed to join session!"));
		OnSessionJoined.Broadcast(false);
	}
}

void UNetworkSessionManager::StartSession()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	OnStartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
	SessionInterface->StartSession(SessionName);
}

void UNetworkSessionManager::EndSession()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	OnEndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
	SessionInterface->EndSession(SessionName);
}

void UNetworkSessionManager::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	SessionInterface->DestroySession(SessionName);
}

FString UNetworkSessionManager::GetSessionInfo(int32 Index) const
{
	if (SessionSearch.IsValid() && Index >= 0 && Index < SessionSearch->SearchResults.Num())
	{
		const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[Index];
		return FString::Printf(TEXT("Session %d - Players: %d/%d"), 
			Index + 1, 
			Result.Session.NumOpenPublicConnections, 
			Result.Session.SessionSettings.NumPublicConnections);
	}
	return FString("Invalid Session");
}

FString UNetworkSessionManager::GetSessionRoomCode(int32 Index) const
{
	if (SessionSearch.IsValid() && Index >= 0 && Index < SessionSearch->SearchResults.Num())
	{
		const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[Index];
		FString RoomCode;
		if (Result.Session.SessionSettings.Get(FName("ROOMCODE"), RoomCode))
		{
			return RoomCode;
		}
	}
	return FString("");
}

int32 UNetworkSessionManager::FindSessionByRoomCode(const FString& RoomCode)
{
	if (!SessionSearch.IsValid())
	{
		return -1;
	}

	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); ++i)
	{
		FString SessionRoomCode = GetSessionRoomCode(i);
		if (SessionRoomCode == RoomCode)
		{
			return i;
		}
	}
	return -1;
}

FString UNetworkSessionManager::GenerateRoomCode()
{
	// Generate a random 4-digit number (1000-9999)
	int32 RoomNumber = FMath::RandRange(1000, 9999);
	return FString::Printf(TEXT("%04d"), RoomNumber);
}

void UNetworkSessionManager::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogFPS251106, Log, TEXT("Session created successfully!"));
		// Start the session
		StartSession();
	}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("Failed to create session!"));
	}

	OnSessionCreated.Broadcast(bWasSuccessful);
}

void UNetworkSessionManager::OnStartSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogFPS251106, Log, TEXT("Session started successfully!"));
	}
}

void UNetworkSessionManager::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		SessionSearchResults = SessionSearch->SearchResults;
		UE_LOG(LogFPS251106, Log, TEXT("Found %d sessions"), SessionSearchResults.Num());
	}
	else
	{
		SessionSearchResults.Empty();
		UE_LOG(LogFPS251106, Warning, TEXT("Session search failed or no sessions found"));
	}

	OnSessionSearchComplete.Broadcast();
}

void UNetworkSessionManager::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}

	bool bWasSuccessful = (Result == EOnJoinSessionCompleteResult::Success);
	
		if (bWasSuccessful)
		{
			UE_LOG(LogFPS251106, Log, TEXT("Joined session successfully!"));
			
			// Aggressively remove all menu UIs before traveling
			if (UWorld* World = GetWorld())
			{
				if (APlayerController* PlayerController = World->GetFirstPlayerController())
				{
					UE_LOG(LogFPS251106, Log, TEXT("OnJoinSessionComplete: Removing all menu UIs"));
					
					// Remove main menu UI from PlayerController
					if (AMainMenuPlayerController* MainMenuPC = Cast<AMainMenuPlayerController>(PlayerController))
					{
						MainMenuPC->RemoveMainMenuUI();
					}
					
					// Force remove all menu-related widgets by trying to find them
					// This is a more aggressive approach
					RemoveAllMenuWidgets(PlayerController);
					
					// Travel to the session
					FString TravelURL;
					if (SessionInterface->GetResolvedConnectString(InSessionName, TravelURL))
					{
						UE_LOG(LogFPS251106, Log, TEXT("OnJoinSessionComplete: Traveling to %s"), *TravelURL);
						PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
					}
				}
			}
		}
	else
	{
		UE_LOG(LogFPS251106, Error, TEXT("Failed to join session! Result: %d"), (int32)Result);
	}

	OnSessionJoined.Broadcast(bWasSuccessful);
}

void UNetworkSessionManager::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogFPS251106, Log, TEXT("Session destroyed successfully!"));
	}
}

void UNetworkSessionManager::RemoveAllMenuWidgets(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	UE_LOG(LogFPS251106, Log, TEXT("RemoveAllMenuWidgets: Starting aggressive widget removal"));

	// Remove main menu UI from PlayerController
	if (AMainMenuPlayerController* MainMenuPC = Cast<AMainMenuPlayerController>(PlayerController))
	{
		MainMenuPC->RemoveMainMenuUI();
	}

	// Get the world and game instance
	if (UWorld* World = PlayerController->GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			// The most aggressive approach: Set input mode to game only
			// This will effectively disable all UI interactions
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = false;
			
			UE_LOG(LogFPS251106, Log, TEXT("RemoveAllMenuWidgets: Set input mode to game only"));
			
			// Note: Widgets are still in viewport but input is disabled
			// The ClientTravel will eventually clear them when level loads
			// But we've done our best to remove them through references
		}
	}

	UE_LOG(LogFPS251106, Log, TEXT("RemoveAllMenuWidgets: Widget removal complete"));
}

void UNetworkSessionManager::OnEndSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogFPS251106, Log, TEXT("Session ended successfully!"));
	}
}

