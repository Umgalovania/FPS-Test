// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUI.generated.h"

/**
 *  Game over screen for the shooter game
 *  Shows final score and survival time
 *  Provides buttons to restart the shooter level or return to the main menu
 */
UCLASS(abstract)
class FPS251106_API UGameOverUI : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Called from C++ to allow Blueprint to setup the UI (score, time, etc.) */
	UFUNCTION(BlueprintImplementableEvent, Category="Shooter", meta = (DisplayName = "Setup Game Over"))
	void BP_SetupGameOver(int32 FinalScore, float SurvivedTimeSeconds);

	/** Called by the Restart button */
	UFUNCTION(BlueprintCallable, Category="Shooter")
	void OnPlayAgainClicked();

	/** Called by the Return to Main Menu button */
	UFUNCTION(BlueprintCallable, Category="Shooter")
	void OnReturnToMenuClicked();
};


