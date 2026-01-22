// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/JoinGameDialog.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UJoinGameDialog::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UJoinGameDialog::OnJoinClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UJoinGameDialog::OnCancelClicked);
	}

	// Hide error message initially
	if (ErrorMessageText)
	{
		ErrorMessageText->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Set focus on room code input
	if (RoomCodeInput)
	{
		RoomCodeInput->SetKeyboardFocus();
	}
}

void UJoinGameDialog::OnJoinClicked()
{
	if (RoomCodeInput)
	{
		FString RoomCode = RoomCodeInput->GetText().ToString();
		
		// Validate room code (should be 4 digits)
		RoomCode = RoomCode.TrimStartAndEnd();
		
		if (RoomCode.Len() == 4 && RoomCode.IsNumeric())
		{
			// Hide error message
			if (ErrorMessageText)
			{
				ErrorMessageText->SetVisibility(ESlateVisibility::Collapsed);
			}

			// Broadcast join event
			OnJoinWithRoomCode.Broadcast(RoomCode);
		}
		else
		{
			// Show error message
			if (ErrorMessageText)
			{
				ErrorMessageText->SetText(FText::FromString(TEXT("请输入4位数字房间号")));
				ErrorMessageText->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UJoinGameDialog::OnCancelClicked()
{
	OnJoinCancelled.Broadcast();
	RemoveFromParent();
}

