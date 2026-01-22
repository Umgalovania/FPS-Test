// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerMenuUI.generated.h"

class UButton;
class UMainMenuUI;
class UJoinGameDialog;

/**
 * Multiplayer menu UI widget
 * Contains options for hosting and joining games
 */
UCLASS(abstract)
class FPS251106_API UMultiplayerMenuUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Set the reference to the main menu UI (to show it again when going back) */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void SetMainMenuUI(class UMainMenuUI* InMainMenuUI);

protected:
	/** Button to host a multiplayer game */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostGameButton;

	/** Button to find and join a game */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinGameButton;

	/** Button to go back to main menu */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	/** Called when Host Game button is clicked */
	UFUNCTION()
	void OnHostGameClicked();

	/** Called when Join Game button is clicked */
	UFUNCTION()
	void OnJoinGameClicked();

	/** Called when Back button is clicked */
	UFUNCTION()
	void OnBackClicked();

	/** Reference to the main menu UI (to show it again when going back) */
	UPROPERTY()
	TObjectPtr<UMainMenuUI> MainMenuUI;

	/** Called when session search completes */
	UFUNCTION()
	void OnSessionSearchComplete();

	/** Called when session join completes */
	UFUNCTION()
	void OnSessionJoined(bool bWasSuccessful);

	/** Called when user wants to join with a room code */
	UFUNCTION()
	void OnJoinWithRoomCode(FString RoomCode);

	/** Called when join is cancelled */
	UFUNCTION()
	void OnJoinCancelled();

	/** Show join error message */
	void ShowJoinError(const FString& ErrorMessage);

	/** Remove all menu UIs (main menu and multiplayer menu) */
	void RemoveAllMenuUIs();

	/** Join game dialog widget class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu")
	TSubclassOf<UJoinGameDialog> JoinGameDialogClass;

	/** Room code to search for (stored when user enters it) */
	FString PendingRoomCode;
};

