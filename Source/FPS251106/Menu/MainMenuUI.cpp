// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/MainMenuUI.h"
#include "Components/Button.h"
#include "FPS251106GameInstance.h"
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

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuUI::OnQuitGameClicked);
	}
}

void UMainMenuUI::OnStartGameClicked()
{
	if (UFPS251106GameInstance* GameInstance = Cast<UFPS251106GameInstance>(GetGameInstance()))
	{
		GameInstance->LoadGameLevel();
	}
}

void UMainMenuUI::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

