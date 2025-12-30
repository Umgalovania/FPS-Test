// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class UShooterUI;
class UGameOverUI;

/**
 *  Simple GameMode for a first person shooter game
 *  Manages game UI
 *  Keeps track of team scores / player score
 */
UCLASS(abstract)
class FPS251106_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Shooter")
	TSubclassOf<UShooterUI> ShooterUIClass;

	/** Pointer to the UI widget */
	TObjectPtr<UShooterUI> ShooterUI;

	/** Map of scores by team ID */
	TMap<uint8, int32> TeamScores;

	/** Player score used by the shooter variant (hit / damage / kill) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Shooter|Score")
	int32 PlayerScore = 0;

	/** Time at which the match started (in seconds) */
	float MatchStartTime = 0.0f;

	/** Game over widget class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shooter|UI")
	TSubclassOf<UGameOverUI> GameOverUIClass;

	/** Instance of the game over widget */
	UPROPERTY()
	TObjectPtr<UGameOverUI> GameOverUI;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

public:

	/** Increases the score for the given team (legacy team scoreboard logic) */
	virtual void IncrementTeamScore(uint8 TeamByte);

	/** Adds score to the player (positive or negative) and updates the UI */
	void AddScore(int32 Delta);

	/** Returns the current player score */
	UFUNCTION(BlueprintPure, Category="Shooter|Score")
	int32 GetPlayerScore() const { return PlayerScore; }

	/** Returns the elapsed time since match start (in seconds) */
	UFUNCTION(BlueprintPure, Category="Shooter|Score")
	float GetElapsedTime() const;

	/** Called when the local player dies – shows the game over screen */
	void HandlePlayerDeath(class AShooterCharacter* DeadPlayer);
};
