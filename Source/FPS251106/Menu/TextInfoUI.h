// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextInfoUI.generated.h"

class UButton;
class UTextBlock;
class UMainMenuUI;

/**
 * Text info UI widget
 * Displays text information with a back button to return to main menu
 */
UCLASS(abstract)
class FPS251106_API UTextInfoUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Set the reference to the main menu UI (to show it again when going back) */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void SetMainMenuUI(class UMainMenuUI* InMainMenuUI);

protected:
	/** Button to go back to main menu */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	/** Text block to display information (optional) */
	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UTextBlock> InfoTextBlock;

	/** Called when Back button is clicked */
	UFUNCTION()
	void OnBackClicked();

	/** Reference to the main menu UI (to show it again when going back) */
	UPROPERTY()
	TObjectPtr<UMainMenuUI> MainMenuUI;
};

