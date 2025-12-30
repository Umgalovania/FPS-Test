// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPS251106GameInstance.generated.h"

/**
 * Custom GameInstance for managing level transitions
 */
UCLASS()
class FPS251106_API UFPS251106GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFPS251106GameInstance(const FObjectInitializer& ObjectInitializer);

	/** Load the main menu level */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void LoadMainMenu();

	/** Load the game level */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void LoadGameLevel();

	/** Get the main menu level name */
	UFUNCTION(BlueprintPure, Category="Menu")
	FName GetMainMenuLevelName() const { return MainMenuLevelName; }

	/** Get the game level name */
	UFUNCTION(BlueprintPure, Category="Menu")
	FName GetGameLevelName() const { return GameLevelName; }

protected:
	/** Name of the main menu level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu")
	FName MainMenuLevelName = TEXT("/Game/Menu/MainMenu");

	/** Name of the game level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu")
	FName GameLevelName = TEXT("/Game/Variant_Shooter/Lvl_Shooter");
};

