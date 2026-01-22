// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuUI;

/**
 * Player Controller for the main menu
 */
UCLASS()
class FPS251106_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainMenuPlayerController(const FObjectInitializer& ObjectInitializer);

	/** Remove the main menu UI from viewport */
	UFUNCTION(BlueprintCallable, Category="Menu")
	void RemoveMainMenuUI();

	/** Get the main menu UI reference */
	UFUNCTION(BlueprintCallable, Category="Menu")
	UMainMenuUI* GetMainMenuUI() const { return MainMenuUI; }

protected:
	virtual void BeginPlay() override;

	/** Type of main menu UI widget to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu")
	TSubclassOf<UMainMenuUI> MainMenuUIClass;

	/** Pointer to the main menu UI widget */
	UPROPERTY()
	TObjectPtr<UMainMenuUI> MainMenuUI;
};





