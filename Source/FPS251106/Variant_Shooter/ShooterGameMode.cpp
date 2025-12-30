// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/ShooterGameMode.h"
#include "Variant_Shooter/UI/ShooterUI.h"
#include "Variant_Shooter/UI/GameOverUI.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	// record match start time
	if (UWorld* World = GetWorld())
	{
		MatchStartTime = World->GetTimeSeconds();
	}

	// create the UI if ShooterUIClass is set
	if (ShooterUIClass)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			ShooterUI = CreateWidget<UShooterUI>(PC, ShooterUIClass);
			if (ShooterUI)
			{
				ShooterUI->AddToViewport(0);
			}
		}
	}
}

void AShooterGameMode::IncrementTeamScore(uint8 TeamByte)
{
	// retrieve the team score if any
	int32 Score = 0;
	if (int32* FoundScore = TeamScores.Find(TeamByte))
	{
		Score = *FoundScore;
	}

	// increment the score for the given team
	++Score;
	TeamScores.Add(TeamByte, Score);

	// update the UI if it exists
	if (ShooterUI)
	{
		ShooterUI->BP_UpdateScore(TeamByte, Score);
	}
}

void AShooterGameMode::AddScore(int32 Delta)
{
	// update the player score and clamp to zero
	PlayerScore = FMath::Max(0, PlayerScore + Delta);

	// keep team 0 in sync with the player score so existing UI can still use it
	TeamScores.Add(0, PlayerScore);

	// update the UI if it exists
	if (ShooterUI)
	{
		// team byte 0 is treated as the player score
		ShooterUI->BP_UpdateScore(0, PlayerScore);
	}
}

float AShooterGameMode::GetElapsedTime() const
{
	if (const UWorld* World = GetWorld())
	{
		return FMath::Max(0.0f, World->GetTimeSeconds() - MatchStartTime);
	}

	return 0.0f;
}

void AShooterGameMode::HandlePlayerDeath(AShooterCharacter* DeadPlayer)
{
	// already showing game over?
	if (GameOverUI || !GameOverUIClass)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			GameOverUI = CreateWidget<UGameOverUI>(PC, GameOverUIClass);
			if (GameOverUI)
			{
				// let Blueprint fill the UI
				const float SurvivedTime = GetElapsedTime();
				GameOverUI->BP_SetupGameOver(PlayerScore, SurvivedTime);

				GameOverUI->AddToViewport(10);

				// switch to UI-only input so the player can interact with the buttons
				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputMode.SetWidgetToFocus(GameOverUI->TakeWidget());
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
			}
		}
	}
}

