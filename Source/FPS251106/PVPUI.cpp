// Copyright Epic Games, Inc. All Rights Reserved.

#include "PVPUI.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

void UPVPUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide player joined notification initially
	if (PlayerJoinedNotificationText)
	{
		PlayerJoinedNotificationText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PlayerJoinedNotificationPanel)
	{
		PlayerJoinedNotificationPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPVPUI::SetRoomCode(const FString& InRoomCode)
{
	if (RoomCodeText)
	{
		RoomCodeText->SetText(FText::FromString(FString::Printf(TEXT("房间号: %s"), *InRoomCode)));
	}
}

void UPVPUI::UpdatePlayerScore(APlayerController* PC, int32 Score)
{
	if (!PC)
	{
		return;
	}

	// Get local player controller
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* LocalPC = World->GetFirstPlayerController())
		{
			if (PC == LocalPC)
			{
				// Update local player score
				if (LocalPlayerScoreText)
				{
					LocalPlayerScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Score)));
				}
			}
			else
			{
				// Update opponent score
				if (OpponentScoreText)
				{
					OpponentScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Score)));
				}
			}
		}
	}
}

void UPVPUI::ShowPlayerJoinedNotification()
{
	if (PlayerJoinedNotificationText)
	{
		PlayerJoinedNotificationText->SetText(FText::FromString(TEXT("有其他玩家加入游戏")));
		PlayerJoinedNotificationText->SetVisibility(ESlateVisibility::Visible);
	}
	if (PlayerJoinedNotificationPanel)
	{
		PlayerJoinedNotificationPanel->SetVisibility(ESlateVisibility::Visible);
	}

	// Hide notification after 3 seconds
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(NotificationTimerHandle, this, &UPVPUI::HidePlayerJoinedNotification, 3.0f, false);
	}
}

void UPVPUI::UpdateMatchTimer(float RemainingTime)
{
	if (MatchTimerText)
	{
		int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(RemainingTime) % 60;
		MatchTimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
	}
}

void UPVPUI::HidePlayerJoinedNotification()
{
	if (PlayerJoinedNotificationText)
	{
		PlayerJoinedNotificationText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PlayerJoinedNotificationPanel)
	{
		PlayerJoinedNotificationPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

