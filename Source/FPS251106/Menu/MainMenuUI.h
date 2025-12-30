// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

class UButton;

/**
 * Main menu UI widget
 */
UCLASS(abstract)
class FPS251106_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	/** Button to start the game */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	/** Button to quit the game */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	/** Called when Start Game button is clicked */
	UFUNCTION()
	void OnStartGameClicked();

	/** Called when Quit Game button is clicked */
	UFUNCTION()
	void OnQuitGameClicked();
};


