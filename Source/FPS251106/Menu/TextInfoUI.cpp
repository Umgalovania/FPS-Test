// Copyright Epic Games, Inc. All Rights Reserved.

#include "Menu/TextInfoUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Menu/MainMenuUI.h"

void UTextInfoUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UTextInfoUI::OnBackClicked);
	}
}

void UTextInfoUI::SetMainMenuUI(UMainMenuUI* InMainMenuUI)
{
	MainMenuUI = InMainMenuUI;
}

void UTextInfoUI::OnBackClicked()
{
	// Show the main menu UI again if we have a reference
	if (MainMenuUI)
	{
		MainMenuUI->SetVisibility(ESlateVisibility::Visible);
	}

	// Remove this widget from viewport
	RemoveFromParent();
}

