// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPS251106GameInstance.generated.h"

class UNetworkSessionManager;

/**
 * Custom GameInstance for managing level transitions and network sessions
 */
UCLASS()
class FPS251106_API UFPS251106GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFPS251106GameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;

	/** Load the main menu level */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void LoadMainMenu();

	/** Load the game level */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void LoadGameLevel();

	/** Load the game level as a network host */
	UFUNCTION(BlueprintCallable, Category="Network")
	void HostGame(int32 MaxPlayers = 2);

	/** Called when session is created */
	UFUNCTION()
	void OnSessionCreated(bool bWasSuccessful);

	/** Get the main menu level name */
	UFUNCTION(BlueprintPure, Category="Menu")
	FName GetMainMenuLevelName() const { return MainMenuLevelName; }

	/** Get the game level name */
	UFUNCTION(BlueprintPure, Category="Menu")
	FName GetGameLevelName() const { return GameLevelName; }

	/** Get the network session manager */
	UFUNCTION(BlueprintPure, Category="Network")
	UNetworkSessionManager* GetNetworkSessionManager() const { return NetworkSessionManager; }

	/** Get the pending room code */
	UFUNCTION(BlueprintPure, Category="Network")
	FString GetPendingRoomCode() const { return PendingRoomCode; }

	/** Set the pending room code */
	void SetPendingRoomCode(const FString& InRoomCode) { PendingRoomCode = InRoomCode; }

protected:
	/** Name of the main menu level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu")
	FName MainMenuLevelName = TEXT("/Game/Menu/MainMenu");

	/** Name of the game level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu")
	FName GameLevelName = TEXT("/Game/Variant_Shooter/Lvl_Shooter");

	/** Name of the PVP level (can be same as GameLevelName or different) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Network")
	FName PVPLevelName = TEXT("/Game/Variant_Shooter/Lvl_Shooter");

	/** PVP GameMode class to use for multiplayer matches */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Network")
	TSubclassOf<class AGameModeBase> PVPGameModeClass;

	/** Network session manager */
	UPROPERTY()
	TObjectPtr<UNetworkSessionManager> NetworkSessionManager;

	/** Pending room code (set when session is created) */
	FString PendingRoomCode;
};

