// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/MainMenuUI.h"
#include "Components/Button.h"
#include "FPS251106GameInstance.h"
#include "Menu/MultiplayerMenuUI.h"
#include "Menu/TextInfoUI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMainMenuUI::OnStartGameClicked);
	}

	if (MultiplayerButton)
	{
		MultiplayerButton->OnClicked.AddDynamic(this, &UMainMenuUI::OnMultiplayerClicked);
	}

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuUI::OnQuitGameClicked);
	}

	if (InfoButton)
	{
		InfoButton->OnClicked.AddDynamic(this, &UMainMenuUI::OnInfoClicked);
	}
}

void UMainMenuUI::OnStartGameClicked()
{
	if (UFPS251106GameInstance* GameInstance = Cast<UFPS251106GameInstance>(GetGameInstance()))
	{
		GameInstance->LoadGameLevel();
	}
}

void UMainMenuUI::OnMultiplayerClicked()
{
	// Create and show the multiplayer menu UI
	if (MultiplayerMenuUIClass)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = GetOwningPlayer())
			{
				UMultiplayerMenuUI* MultiplayerMenu = CreateWidget<UMultiplayerMenuUI>(PC, MultiplayerMenuUIClass);
				if (MultiplayerMenu)
				{
					// Store reference to main menu in multiplayer menu for back button
					MultiplayerMenu->SetMainMenuUI(this);
					
					// Hide the main menu
					SetVisibility(ESlateVisibility::Collapsed);
					
					// Show the multiplayer menu
					MultiplayerMenu->AddToViewport(0);
					UE_LOG(LogTemp, Log, TEXT("Opened multiplayer menu"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create multiplayer menu widget!"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerMenuUIClass is not set! Please set it in the Blueprint."));
	}
}

void UMainMenuUI::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMainMenuUI::OnInfoClicked()
{
	// Create and show the text info UI
	if (TextInfoUIClass)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = GetOwningPlayer())
			{
				UTextInfoUI* TextInfoMenu = CreateWidget<UTextInfoUI>(PC, TextInfoUIClass);
				if (TextInfoMenu)
				{
					// Store reference to main menu in text info menu for back button
					TextInfoMenu->SetMainMenuUI(this);
					
					// Hide the main menu
					SetVisibility(ESlateVisibility::Collapsed);
					
					// Show the text info menu
					TextInfoMenu->AddToViewport(0);
					UE_LOG(LogTemp, Log, TEXT("Opened text info menu"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create text info menu widget!"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TextInfoUIClass is not set! Please set it in the Blueprint."));
	}
}

