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
	/** Button to start the game (single player) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	/** Button to open multiplayer menu */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MultiplayerButton;

	/** Button to quit the game */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	/** Button to open text info menu */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InfoButton;

	/** Class of multiplayer menu UI widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu")
	TSubclassOf<class UMultiplayerMenuUI> MultiplayerMenuUIClass;

	/** Class of text info UI widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu")
	TSubclassOf<class UTextInfoUI> TextInfoUIClass;

	/** Called when Start Game button is clicked */
	UFUNCTION()
	void OnStartGameClicked();

	/** Called when Multiplayer button is clicked */
	UFUNCTION()
	void OnMultiplayerClicked();

	/** Called when Quit Game button is clicked */
	UFUNCTION()
	void OnQuitGameClicked();

	/** Called when Info button is clicked */
	UFUNCTION()
	void OnInfoClicked();
};





