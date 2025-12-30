// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Variant_Shooter/ShooterGameMode.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerGameMode.generated.h"

class UShooterUI;

/**
 *  Multiplayer GameMode for a first person shooter game
 *  Supports network replication and victory conditions
 */
UCLASS(abstract)
class FPS251106_API AMultiplayerGameMode : public AShooterGameMode
{
	GENERATED_BODY()

protected:

	/** Target score to win the match */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer", meta = (ClampMin = 1))
	int32 TargetScore = 10;

	/** Match duration in seconds (0 = no time limit) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer", meta = (ClampMin = 0))
	float MatchDuration = 300.0f;

	/** NPC class to spawn as enemies */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Enemies")
	TSubclassOf<class AShooterNPC> NPCClass;

	/** Number of enemies to spawn at match start */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Enemies", meta = (ClampMin = 0))
	int32 InitialEnemyCount = 1;

	/** Timer handle for match duration */
	FTimerHandle MatchTimerHandle;

	/** True if the match has ended */
	bool bMatchEnded = false;

	/** Minimum distance from player to spawn enemies (in cm, 3000 = 30 meters) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Enemies", meta = (ClampMin = 0))
	float MinDistanceFromPlayer = 3000.0f;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

public:

	/** Constructor */
	AMultiplayerGameMode();

	/** Increases the score for the given team and checks for victory */
	virtual void IncrementTeamScore(uint8 TeamByte) override;

	/** Spawns a single enemy at a random valid location in the map */
	UFUNCTION(BlueprintCallable, Category="Multiplayer|Enemies")
	class AShooterNPC* SpawnEnemyAtRandomLocation();

	/** Checks if any team has reached the target score */
	UFUNCTION(BlueprintCallable, Category="Multiplayer")
	void CheckVictoryCondition();

	/** Called when a team wins the match */
	UFUNCTION(BlueprintImplementableEvent, Category="Multiplayer")
	void OnTeamVictory(uint8 WinningTeam);

	/** Called when the match time runs out */
	UFUNCTION(BlueprintImplementableEvent, Category="Multiplayer")
	void OnMatchTimeExpired();

	/** Returns the target score */
	UFUNCTION(BlueprintPure, Category="Multiplayer")
	int32 GetTargetScore() const { return TargetScore; }

	/** Returns the remaining match time */
	UFUNCTION(BlueprintPure, Category="Multiplayer")
	float GetRemainingMatchTime() const;

	/** Returns true if the match has ended */
	UFUNCTION(BlueprintPure, Category="Multiplayer")
	bool IsMatchEnded() const { return bMatchEnded; }

protected:

	/** Called when match duration expires */
	void OnMatchTimeExpired_Internal();

	/** Ends the match and prevents further gameplay */
	void EndMatch();

	/** Spawns initial enemies in the world */
	void SpawnInitialEnemies();
};

