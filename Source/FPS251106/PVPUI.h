// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PVPUI.generated.h"

class UTextBlock;
class UImage;

/**
 * PVP UI widget that displays room code, match timer, scores, and player join notifications
 */
UCLASS(abstract)
class FPS251106_API UPVPUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Set the room code to display */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void SetRoomCode(const FString& InRoomCode);

	/** Update a player's score */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void UpdatePlayerScore(class APlayerController* PC, int32 Score);

	/** Show player joined notification */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void ShowPlayerJoinedNotification();

	/** Update match timer display */
	UFUNCTION(BlueprintCallable, Category="PVP")
	void UpdateMatchTimer(float RemainingTime);

protected:
	/** Room code text block (top left) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomCodeText;

	/** Match timer text block */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MatchTimerText;

	/** Player joined notification text (top right) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerJoinedNotificationText;

	/** Player joined notification image/panel (optional) */
	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UImage> PlayerJoinedNotificationPanel;

	/** Local player score text */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LocalPlayerScoreText;

	/** Opponent score text */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> OpponentScoreText;

	/** Timer handle for hiding player joined notification */
	FTimerHandle NotificationTimerHandle;

	/** Hide the player joined notification after a delay */
	void HidePlayerJoinedNotification();
};

