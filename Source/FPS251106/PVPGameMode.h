// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Variant_Shooter/ShooterGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PVPGameMode.generated.h"

class UPVPUI;

/**
 * Structure to hold player score information for replication
 */
USTRUCT(BlueprintType)
struct FPlayerScoreInfo
{
	GENERATED_BODY()

	/** Player controller (not replicated, only used on server) */
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	/** Player's score */
	UPROPERTY(BlueprintReadOnly)
	int32 Score = 0;

	FPlayerScoreInfo()
		: PlayerController(nullptr)
		, Score(0)
	{
	}

	FPlayerScoreInfo(APlayerController* PC, int32 InScore)
		: PlayerController(PC)
		, Score(InScore)
	{
	}
};

/**
 * PVP GameMode for player vs player combat
 * No AI enemies, 150 second match duration, kill-based scoring
 */
UCLASS(abstract)
class FPS251106_API APVPGameMode : public AShooterGameMode
{
	GENERATED_BODY()

public:
	APVPGameMode(const FObjectInitializer& ObjectInitializer);

protected:
	/** Match duration in seconds (150 seconds for PVP) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="PVP", meta = (ClampMin = 0))
	float MatchDuration = 150.0f;

	/** Timer handle for match duration */
	FTimerHandle MatchTimerHandle;

	/** True if the match has ended */
	UPROPERTY(Replicated)
	bool bMatchEnded = false;

	/** Room code for this match */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="PVP")
	FString RoomCode;

	/** PVP UI widget class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PVP|UI")
	TSubclassOf<UPVPUI> PVPUIClass;

	/** PVP UI widget instance */
	UPROPERTY()
	TObjectPtr<UPVPUI> PVPUI;

	/** Game over UI widget class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PVP|UI")
	TSubclassOf<class UGameOverUI> PVPGameOverUIClass;

	/** Game over UI widget instance */
	UPROPERTY()
	TObjectPtr<class UGameOverUI> PVPGameOverUI;

	/** Array of player scores (replicated) */
	UPROPERTY(Replicated)
	TArray<FPlayerScoreInfo> PlayerScoresArray;

	/** Map of player scores (server-side only, for quick lookup) */
	TMap<TObjectPtr<APlayerController>, int32> PlayerScores;

protected:
	virtual void BeginPlay() override;

	/** Network replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Called when a player kills another player */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void OnPlayerKill(APlayerController* Killer, APlayerController* Victim);

	/** Returns the remaining match time */
	UFUNCTION(BlueprintPure, Category="PVP")
	float GetRemainingMatchTime() const;

	/** Returns true if the match has ended */
	UFUNCTION(BlueprintPure, Category="PVP")
	bool IsMatchEnded() const { return bMatchEnded; }

	/** Returns the room code */
	UFUNCTION(BlueprintPure, Category="PVP")
	FString GetRoomCode() const { return RoomCode; }

	/** Sets the room code (called when session is created) */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void SetRoomCode(const FString& InRoomCode);

	/** Gets a specific player's score by PlayerController */
	UFUNCTION(BlueprintPure, Category="PVP")
	int32 GetPlayerScoreForPlayer(APlayerController* PC) const;

	/** Gets all player scores (for UI) */
	UFUNCTION(BlueprintPure, Category="PVP")
	TArray<FPlayerScoreInfo> GetAllPlayerScores() const { return PlayerScoresArray; }

	/** Called when a new player joins */
	UFUNCTION(BlueprintImplementableEvent, Category="PVP")
	void OnPlayerJoined();

	/** Called when match time expires */
	UFUNCTION(BlueprintImplementableEvent, Category="PVP")
	void OnMatchTimeExpired();

	/** Called to show game over screen */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void ShowGameOverScreen();

protected:
	/** Called when match duration expires */
	void OnMatchTimeExpired_Internal();

	/** Ends the match and prevents further gameplay */
	void EndMatch();

	/** Updates the UI with current scores */
	void UpdateScoreUI();
};

