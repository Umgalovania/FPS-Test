// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/MultiplayerMenuUI.h"
#include "Components/Button.h"
#include "FPS251106GameInstance.h"
#include "Menu/NetworkSessionManager.h"
#include "Menu/MainMenuUI.h"
#include "Menu/MainMenuPlayerController.h"
#include "Menu/JoinGameDialog.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Slate/SGameLayerManager.h"
#include "Framework/Application/SlateApplication.h"

void UMultiplayerMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (HostGameButton)
	{
		HostGameButton->OnClicked.AddDynamic(this, &UMultiplayerMenuUI::OnHostGameClicked);
	}

	if (JoinGameButton)
	{
		JoinGameButton->OnClicked.AddDynamic(this, &UMultiplayerMenuUI::OnJoinGameClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UMultiplayerMenuUI::OnBackClicked);
	}
}

void UMultiplayerMenuUI::OnHostGameClicked()
{
	// Remove all menu UIs before hosting
	RemoveAllMenuUIs();
	
	if (UWorld* World = GetWorld())
	{
		if (UFPS251106GameInstance* GameInstance = Cast<UFPS251106GameInstance>(World->GetGameInstance()))
		{
			GameInstance->HostGame(2); // Host for 2 players
		}
	}
}

void UMultiplayerMenuUI::OnJoinGameClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OnJoinGameClicked: Hiding menu UIs before showing join dialog"));
	
	// Hide main menu UI instead of removing it, so we can restore it later
	if (MainMenuUI)
	{
		MainMenuUI->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("OnJoinGameClicked: Hid MainMenuUI"));
	}
	else
	{
		// Try to get main menu UI from PlayerController if we don't have a reference
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = GetOwningPlayer())
			{
				if (AMainMenuPlayerController* MainMenuPC = Cast<AMainMenuPlayerController>(PC))
				{
					MainMenuUI = MainMenuPC->GetMainMenuUI();
					if (MainMenuUI)
					{
						MainMenuUI->SetVisibility(ESlateVisibility::Collapsed);
						UE_LOG(LogTemp, Log, TEXT("OnJoinGameClicked: Got MainMenuUI from PlayerController and hid it"));
					}
				}
			}
		}
	}
	
	// Hide this widget (multiplayer menu) but don't remove it yet
	// We'll remove it when the dialog is cancelled or join succeeds
	SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogTemp, Log, TEXT("OnJoinGameClicked: Hid MultiplayerMenuUI"));
	
	// Show join game dialog
	if (JoinGameDialogClass)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = GetOwningPlayer())
			{
				UJoinGameDialog* Dialog = CreateWidget<UJoinGameDialog>(PC, JoinGameDialogClass);
				if (Dialog)
				{
					Dialog->OnJoinWithRoomCode.AddDynamic(this, &UMultiplayerMenuUI::OnJoinWithRoomCode);
					Dialog->OnJoinCancelled.AddDynamic(this, &UMultiplayerMenuUI::OnJoinCancelled);
					Dialog->AddToViewport(100);
					UE_LOG(LogTemp, Log, TEXT("OnJoinGameClicked: Showing join dialog"));
				}
			}
		}
	}
}

void UMultiplayerMenuUI::OnJoinWithRoomCode(FString RoomCode)
{
	// Store the room code to search for
	PendingRoomCode = RoomCode;

	if (UWorld* World = GetWorld())
	{
		if (UFPS251106GameInstance* GameInstance = Cast<UFPS251106GameInstance>(World->GetGameInstance()))
		{
			if (UNetworkSessionManager* SessionManager = GameInstance->GetNetworkSessionManager())
			{
				// Remove any existing bindings first to avoid duplicate bindings
				SessionManager->OnSessionSearchComplete.RemoveDynamic(this, &UMultiplayerMenuUI::OnSessionSearchComplete);
				// Bind to search complete delegate
				SessionManager->OnSessionSearchComplete.AddDynamic(this, &UMultiplayerMenuUI::OnSessionSearchComplete);
				SessionManager->FindSessions();
			}
		}
	}
}

void UMultiplayerMenuUI::OnJoinCancelled()
{
	UE_LOG(LogTemp, Log, TEXT("OnJoinCancelled: Restoring menu UIs"));
	
	// Restore main menu UI visibility if we have a reference
	if (MainMenuUI)
	{
		MainMenuUI->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("OnJoinCancelled: Restored MainMenuUI visibility"));
	}
	else
	{
		// Try to get main menu UI from PlayerController if we don't have a reference
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = GetOwningPlayer())
			{
				if (AMainMenuPlayerController* MainMenuPC = Cast<AMainMenuPlayerController>(PC))
				{
					MainMenuUI = MainMenuPC->GetMainMenuUI();
					if (MainMenuUI)
					{
						MainMenuUI->SetVisibility(ESlateVisibility::Visible);
						UE_LOG(LogTemp, Log, TEXT("OnJoinCancelled: Got MainMenuUI from PlayerController and restored visibility"));
					}
				}
			}
		}
	}
	
	// Show this widget (multiplayer menu) again
	SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogTemp, Log, TEXT("OnJoinCancelled: Restored MultiplayerMenuUI visibility"));
}

void UMultiplayerMenuUI::SetMainMenuUI(UMainMenuUI* InMainMenuUI)
{
	MainMenuUI = InMainMenuUI;
}

void UMultiplayerMenuUI::OnBackClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OnBackClicked: Returning to main menu"));
	
	// Show the main menu UI again if we have a reference
	if (MainMenuUI)
	{
		MainMenuUI->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("OnBackClicked: Restored MainMenuUI visibility via reference"));
	}
	else
	{
		// Try to get main menu UI from PlayerController if we don't have a reference
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = GetOwningPlayer())
			{
				if (AMainMenuPlayerController* MainMenuPC = Cast<AMainMenuPlayerController>(PC))
				{
					MainMenuUI = MainMenuPC->GetMainMenuUI();
					if (MainMenuUI)
					{
						MainMenuUI->SetVisibility(ESlateVisibility::Visible);
						UE_LOG(LogTemp, Log, TEXT("OnBackClicked: Got MainMenuUI from PlayerController and restored visibility"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("OnBackClicked: MainMenuUI is null in PlayerController!"));
					}
				}
			}
		}
	}

	// Remove this widget from viewport
	RemoveFromParent();
}

void UMultiplayerMenuUI::OnSessionSearchComplete()
{
	if (UWorld* World = GetWorld())
	{
		if (UFPS251106GameInstance* GameInstance = Cast<UFPS251106GameInstance>(World->GetGameInstance()))
		{
			if (UNetworkSessionManager* SessionManager = GameInstance->GetNetworkSessionManager())
			{
				// Find session by room code
				int32 SessionIndex = SessionManager->FindSessionByRoomCode(PendingRoomCode);
				
				if (SessionIndex >= 0)
				{
					// Found the session, remove all menu UIs before joining
					RemoveAllMenuUIs();
					
					// Found the session, join it
					// Remove any existing bindings first to avoid duplicate bindings
					SessionManager->OnSessionJoined.RemoveDynamic(this, &UMultiplayerMenuUI::OnSessionJoined);
					SessionManager->OnSessionJoined.AddDynamic(this, &UMultiplayerMenuUI::OnSessionJoined);
					SessionManager->JoinSession(SessionIndex);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Room code %s not found!"), *PendingRoomCode);
					// Show error message to user
					ShowJoinError(TEXT("未找到房间"));
				}
			}
		}
	}
}

void UMultiplayerMenuUI::ShowJoinError(const FString& ErrorMessage)
{
	// This would show an error message to the user
	// For now, just log it
	UE_LOG(LogTemp, Error, TEXT("Join error: %s"), *ErrorMessage);
}

void UMultiplayerMenuUI::RemoveAllMenuUIs()
{
	UE_LOG(LogTemp, Log, TEXT("RemoveAllMenuUIs: Starting aggressive UI cleanup"));
	
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			// Remove main menu UI if we have a reference
			if (MainMenuUI)
			{
				UE_LOG(LogTemp, Log, TEXT("RemoveAllMenuUIs: Removing MainMenuUI via reference"));
				MainMenuUI->RemoveFromParent();
				MainMenuUI = nullptr;
			}
			
			// Remove main menu UI from PlayerController
			if (AMainMenuPlayerController* MainMenuPC = Cast<AMainMenuPlayerController>(PC))
			{
				UE_LOG(LogTemp, Log, TEXT("RemoveAllMenuUIs: Removing MainMenuUI via PlayerController"));
				MainMenuPC->RemoveMainMenuUI();
			}
			
			// Aggressively remove all menu-related widgets by type
			// This will catch any widgets that weren't removed through references
			TArray<UUserWidget*> WidgetsToRemove;
			
			// Try to find and remove MainMenuUI
			if (UClass* MainMenuUIClass = UMainMenuUI::StaticClass())
			{
				// We can't directly enumerate widgets, but we can try to remove known types
				// The widgets should be removed through their references above
			}
			
			// Remove this widget (multiplayer menu) - do this last
			UE_LOG(LogTemp, Log, TEXT("RemoveAllMenuUIs: Removing MultiplayerMenuUI"));
			RemoveFromParent();
			
			// Set input mode to game only to disable UI interactions
			// This ensures UI won't interfere even if widgets are still in viewport
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("RemoveAllMenuUIs: UI cleanup complete"));
}

void UMultiplayerMenuUI::OnSessionJoined(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully joined session!"));
		
		// Remove all menu UIs after successfully joining
		RemoveAllMenuUIs();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to join session!"));
	}
}

