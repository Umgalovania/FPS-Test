// Copyright Epic Games, Inc. All Rights Reserved.

#include "PVPGameMode.h"
#include "Variant_Shooter/UI/ShooterUI.h"
#include "Variant_Shooter/UI/GameOverUI.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Variant_Shooter/ShooterPlayerController.h"
#include "Variant_Shooter/AI/ShooterNPC.h"
#include "PVPUI.h"
#include "FPS251106GameInstance.h"
#include "Menu/NetworkSessionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "FPS251106.h"

APVPGameMode::APVPGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Enable replication
	bReplicates = true;
	bMatchEnded = false;
	
	// Note: DefaultPawnClass and PlayerControllerClass should be set in blueprint
	// They will be inherited from ShooterGameMode blueprint or set explicitly in BP_PVPGameMode
}

void APVPGameMode::BeginPlay()
{
	// Call AGameModeBase::BeginPlay() instead of Super::BeginPlay()
	// to avoid creating ShooterUI from AShooterGameMode
	AGameModeBase::BeginPlay();

	// Get room code from GameInstance if available
	if (UFPS251106GameInstance* GI = Cast<UFPS251106GameInstance>(GetGameInstance()))
	{
		FString PendingCode = GI->GetPendingRoomCode();
		if (!PendingCode.IsEmpty())
		{
			RoomCode = PendingCode;
		}
	}

	// If room code is still empty, generate one (for testing/fallback)
	if (RoomCode.IsEmpty())
	{
		RoomCode = UNetworkSessionManager::GenerateRoomCode();
	}

	// Start match timer
	if (MatchDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &APVPGameMode::OnMatchTimeExpired_Internal, MatchDuration, false);
	}

	// Create PVP UI if class is set
	if (PVPUIClass)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PVPUI = CreateWidget<UPVPUI>(PC, PVPUIClass);
			if (PVPUI)
			{
				PVPUI->AddToViewport(0);
				// Set room code in UI
				PVPUI->SetRoomCode(RoomCode);
			}
		}
	}

	// Initialize player scores for all existing players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			PlayerScores.Add(PC, 0);
			PlayerScoresArray.Add(FPlayerScoreInfo(PC, 0));
		}
	}

	// Remove all AI enemies in PVP mode (server only)
	if (HasAuthority())
	{
		TArray<AActor*> NPCs;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterNPC::StaticClass(), NPCs);
		for (AActor* NPC : NPCs)
		{
			if (AShooterNPC* EnemyNPC = Cast<AShooterNPC>(NPC))
			{
				UE_LOG(LogFPS251106, Log, TEXT("PVPGameMode: Removing AI enemy %s"), *EnemyNPC->GetName());
				EnemyNPC->Destroy();
			}
		}
		UE_LOG(LogFPS251106, Log, TEXT("PVPGameMode: Removed %d AI enemies"), NPCs.Num());
	}

	UpdateScoreUI();
}

void APVPGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APVPGameMode, bMatchEnded);
	DOREPLIFETIME(APVPGameMode, RoomCode);
	DOREPLIFETIME(APVPGameMode, PlayerScoresArray);
}

void APVPGameMode::OnPlayerKill(APlayerController* Killer, APlayerController* Victim)
{
	if (bMatchEnded)
	{
		return;
	}

	if (Killer && Killer != Victim)
	{
		// Increment killer's score in map
		int32* ScorePtr = PlayerScores.Find(Killer);
		if (ScorePtr)
		{
			(*ScorePtr)++;
		}
		else
		{
			PlayerScores.Add(Killer, 1);
		}

		// Update replicated array
		bool bFound = false;
		for (FPlayerScoreInfo& ScoreInfo : PlayerScoresArray)
		{
			if (ScoreInfo.PlayerController == Killer)
			{
				ScoreInfo.Score = PlayerScores[Killer];
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			PlayerScoresArray.Add(FPlayerScoreInfo(Killer, PlayerScores[Killer]));
		}

		UpdateScoreUI();
	}
}

float APVPGameMode::GetRemainingMatchTime() const
{
	if (MatchDuration <= 0.0f || bMatchEnded)
	{
		return 0.0f;
	}

	float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(MatchTimerHandle);
	return FMath::Max(0.0f, MatchDuration - ElapsedTime);
}

void APVPGameMode::SetRoomCode(const FString& InRoomCode)
{
	RoomCode = InRoomCode;
	if (PVPUI)
	{
		PVPUI->SetRoomCode(RoomCode);
	}
}

int32 APVPGameMode::GetPlayerScoreForPlayer(APlayerController* PC) const
{
	if (const int32* ScorePtr = PlayerScores.Find(PC))
	{
		return *ScorePtr;
	}
	return 0;
}

void APVPGameMode::ShowGameOverScreen()
{
	if (PVPGameOverUIClass)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PVPGameOverUI = CreateWidget<class UGameOverUI>(PC, PVPGameOverUIClass);
			if (PVPGameOverUI)
			{
				// Find winner
				APlayerController* Winner = nullptr;
				int32 HighestScore = -1;

				for (const FPlayerScoreInfo& ScoreInfo : PlayerScoresArray)
				{
					if (ScoreInfo.Score > HighestScore)
					{
						HighestScore = ScoreInfo.Score;
						Winner = ScoreInfo.PlayerController;
					}
				}

				// Get local player's score
				int32 LocalPlayerScore = GetPlayerScoreForPlayer(PC);
				FString WinnerText;

				if (Winner == PC)
				{
					WinnerText = FString::Printf(TEXT("你获胜！得分: %d"), LocalPlayerScore);
				}
				else if (Winner)
				{
					WinnerText = FString::Printf(TEXT("对手获胜！得分: %d"), HighestScore);
				}
				else
				{
					WinnerText = FString::Printf(TEXT("平局！你的得分: %d"), LocalPlayerScore);
				}

				// Setup game over UI (assuming it has a method to set text)
				PVPGameOverUI->AddToViewport(100);

				// Set input mode to UI
				FInputModeUIOnly InputMode;
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
			}
		}
	}
}

void APVPGameMode::OnMatchTimeExpired_Internal()
{
	if (!bMatchEnded)
	{
		OnMatchTimeExpired();
		EndMatch();
		ShowGameOverScreen();
	}
}

void APVPGameMode::EndMatch()
{
	bMatchEnded = true;

	// Clear the match timer
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	// Disable player input for all players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			PC->SetPause(true);
		}
	}
}

void APVPGameMode::UpdateScoreUI()
{
	if (PVPUI)
	{
		// Update UI with all player scores
		for (const FPlayerScoreInfo& ScoreInfo : PlayerScoresArray)
		{
			if (ScoreInfo.PlayerController)
			{
				PVPUI->UpdatePlayerScore(ScoreInfo.PlayerController, ScoreInfo.Score);
			}
		}
	}
}

