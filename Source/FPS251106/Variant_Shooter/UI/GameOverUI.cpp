// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/UI/GameOverUI.h"
#include "FPS251106GameInstance.h"

void UGameOverUI::OnPlayAgainClicked()
{
	if (UFPS251106GameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UFPS251106GameInstance>() : nullptr)
	{
		GI->LoadGameLevel();
	}
}

void UGameOverUI::OnReturnToMenuClicked()
{
	if (UFPS251106GameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UFPS251106GameInstance>() : nullptr)
	{
		GI->LoadMainMenu();
	}
}


