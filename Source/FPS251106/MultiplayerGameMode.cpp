// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "Variant_Shooter/UI/ShooterUI.h"
#include "Variant_Shooter/ShooterGameMode.h"
#include "Variant_Shooter/AI/ShooterNPC.h"
#include "Variant_Shooter/ShooterPlayerController.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerStart.h"
#include "FPS251106.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	// Enable replication
	bReplicates = true;
	
	// Note: PlayerControllerClass and DefaultPawnClass should be set in blueprint
	// AShooterPlayerController is abstract and cannot be directly instantiated
	// These will be inherited from ShooterGameMode or set in the blueprint
}

void AMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Start match timer if duration is set
	if (MatchDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &AMultiplayerGameMode::OnMatchTimeExpired_Internal, MatchDuration, false);
	}

	// Spawn initial enemies - Disabled, enemies should be placed manually in the level
	// if (NPCClass && InitialEnemyCount > 0)
	// {
	// 	SpawnInitialEnemies();
	// }
}

void AMultiplayerGameMode::IncrementTeamScore(uint8 TeamByte)
{
	// Don't increment score if match has ended
	if (bMatchEnded)
	{
		return;
	}

	// Call parent implementation to increment score
	Super::IncrementTeamScore(TeamByte);

	// Check for victory condition
	CheckVictoryCondition();
}

void AMultiplayerGameMode::CheckVictoryCondition()
{
	// Check if any team has reached the target score
	// Since TeamScores is protected in base class, we can access it directly
	for (const auto& TeamScorePair : TeamScores)
	{
		if (TeamScorePair.Value >= TargetScore)
		{
			// A team has won!
			OnTeamVictory(TeamScorePair.Key);
			EndMatch();
			return;
		}
	}
}

void AMultiplayerGameMode::OnMatchTimeExpired_Internal()
{
	if (!bMatchEnded)
	{
		OnMatchTimeExpired();
		EndMatch();
	}
}

void AMultiplayerGameMode::EndMatch()
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

float AMultiplayerGameMode::GetRemainingMatchTime() const
{
	if (MatchDuration <= 0.0f || bMatchEnded)
	{
		return 0.0f;
	}

	float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(MatchTimerHandle);
	return FMath::Max(0.0f, MatchDuration - ElapsedTime);
}

void AMultiplayerGameMode::SpawnInitialEnemies()
{
	if (!NPCClass)
	{
		return;
	}

	// Find all player starts in the world
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() == 0)
	{
		UE_LOG(LogFPS251106, Warning, TEXT("No PlayerStart found in the world. Cannot spawn enemies."));
		return;
	}

	// Get the player's starting location (if player exists)
	FVector PlayerStartLocation = FVector::ZeroVector;
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		PlayerStartLocation = PlayerPawn->GetActorLocation();
	}
	else
	{
		// If player hasn't spawned yet, use the first PlayerStart location
		if (PlayerStarts.Num() > 0)
		{
			PlayerStartLocation = PlayerStarts[0]->GetActorLocation();
		}
	}

	// Spawn enemies at player start locations that are far from the player
	TArray<int32> UsedIndices;
	for (int32 i = 0; i < InitialEnemyCount; ++i)
	{
		int32 Attempts = 0;
		const int32 MaxAttempts = 50; // Prevent infinite loop
		bool bSpawned = false;

		while (Attempts < MaxAttempts && !bSpawned)
		{
			// Select a random player start
			int32 StartIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
			
			// Skip if we've already used this index (unless we've tried too many times)
			if (UsedIndices.Contains(StartIndex) && UsedIndices.Num() < PlayerStarts.Num())
			{
				++Attempts;
				continue;
			}

			APlayerStart* SpawnPoint = Cast<APlayerStart>(PlayerStarts[StartIndex]);
			if (SpawnPoint)
			{
				FVector SpawnLocation = SpawnPoint->GetActorLocation();
				float DistanceToPlayer = FVector::Dist(SpawnLocation, PlayerStartLocation);

				// If this spawn point is far enough from player, or we've tried many times, use it
				if (DistanceToPlayer >= MinDistanceFromPlayer || Attempts >= MaxAttempts - 5)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

					AShooterNPC* SpawnedNPC = GetWorld()->SpawnActor<AShooterNPC>(NPCClass, SpawnPoint->GetActorTransform(), SpawnParams);
					if (SpawnedNPC)
					{
						UsedIndices.Add(StartIndex);
						UE_LOG(LogFPS251106, Log, TEXT("Spawned enemy NPC at PlayerStart %d (Distance from player: %.2f)"), StartIndex, DistanceToPlayer);
						bSpawned = true;
					}
				}
			}
			++Attempts;
		}

		// If we couldn't spawn an enemy after all attempts, break
		if (!bSpawned)
		{
			UE_LOG(LogFPS251106, Warning, TEXT("Could not find suitable spawn location for enemy %d after %d attempts"), i, MaxAttempts);
			break;
		}
	}
}

AShooterNPC* AMultiplayerGameMode::SpawnEnemyAtRandomLocation()
{
	if (!NPCClass)
	{
		return nullptr;
	}

	// Find all player starts in the world
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() == 0)
	{
		UE_LOG(LogFPS251106, Warning, TEXT("No PlayerStart found in the world. Cannot respawn enemy."));
		return nullptr;
	}

	// Get the player's location if available
	FVector PlayerLocation = FVector::ZeroVector;
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		PlayerLocation = PlayerPawn->GetActorLocation();
	}

	const int32 MaxAttempts = 50;
	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
	{
		const int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
		if (APlayerStart* SpawnPoint = Cast<APlayerStart>(PlayerStarts[Index]))
		{
			const FVector SpawnLocation = SpawnPoint->GetActorLocation();
			const float DistanceToPlayer = FVector::Dist(SpawnLocation, PlayerLocation);

			if (DistanceToPlayer >= MinDistanceFromPlayer || Attempt >= MaxAttempts - 5)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				AShooterNPC* SpawnedNPC = GetWorld()->SpawnActor<AShooterNPC>(NPCClass, SpawnPoint->GetActorTransform(), SpawnParams);
				if (SpawnedNPC)
				{
					UE_LOG(LogFPS251106, Log, TEXT("Respawned enemy NPC at PlayerStart %d (Distance from player: %.2f)"), Index, DistanceToPlayer);
					return SpawnedNPC;
				}
			}
		}
	}

	UE_LOG(LogFPS251106, Warning, TEXT("Failed to respawn enemy after %d attempts."), MaxAttempts);
	return nullptr;
}

