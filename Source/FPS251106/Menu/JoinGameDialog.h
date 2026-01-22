// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinGameDialog.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;

/**
 * Dialog for entering room code to join a game
 */
UCLASS(abstract)
class FPS251106_API UJoinGameDialog : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Called when join button is clicked */
	UFUNCTION()
	void OnJoinClicked();

	/** Called when cancel button is clicked */
	UFUNCTION()
	void OnCancelClicked();

	/** Delegate called when user wants to join with a room code */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinWithRoomCode, FString, RoomCode);
	
	UPROPERTY(BlueprintAssignable, Category="Join Game")
	FOnJoinWithRoomCode OnJoinWithRoomCode;

	/** Delegate called when dialog is cancelled */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinCancelled);
	
	UPROPERTY(BlueprintAssignable, Category="Join Game")
	FOnJoinCancelled OnJoinCancelled;

protected:
	/** Room code input field */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> RoomCodeInput;

	/** Join button */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	/** Cancel button */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;

	/** Error message text */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ErrorMessageText;
};

