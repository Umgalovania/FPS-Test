// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "NetworkSessionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionJoined, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionSearchComplete);

/**
 * Manages network sessions for P2P multiplayer
 */
UCLASS(BlueprintType)
class FPS251106_API UNetworkSessionManager : public UObject
{
	GENERATED_BODY()

public:
	UNetworkSessionManager();

	/** Initialize the session manager */
	void Initialize();

	/** Create a new session (host a game) */
	UFUNCTION(BlueprintCallable, Category="Network")
	void CreateSession(int32 MaxPlayers = 2);

	/** Find available sessions */
	UFUNCTION(BlueprintCallable, Category="Network")
	void FindSessions();

	/** Join a session by index (from search results) */
	UFUNCTION(BlueprintCallable, Category="Network")
	void JoinSession(int32 SessionIndex);

	/** Start the session (call after creating) */
	UFUNCTION(BlueprintCallable, Category="Network")
	void StartSession();

	/** End the current session */
	UFUNCTION(BlueprintCallable, Category="Network")
	void EndSession();

	/** Destroy the current session */
	UFUNCTION(BlueprintCallable, Category="Network")
	void DestroySession();

	/** Get the number of found sessions */
	UFUNCTION(BlueprintPure, Category="Network")
	int32 GetNumSearchResults() const { return SessionSearchResults.Num(); }

	/** Get session info at index */
	UFUNCTION(BlueprintPure, Category="Network")
	FString GetSessionInfo(int32 Index) const;

	/** Get room code from session at index */
	UFUNCTION(BlueprintPure, Category="Network")
	FString GetSessionRoomCode(int32 Index) const;

	/** Find session by room code */
	UFUNCTION(BlueprintCallable, Category="Network")
	int32 FindSessionByRoomCode(const FString& RoomCode);

	/** Generate a random 4-digit room code */
	UFUNCTION(BlueprintCallable, Category="Network")
	static FString GenerateRoomCode();

	/** Delegate called when session creation completes */
	UPROPERTY(BlueprintAssignable, Category="Network")
	FOnSessionCreated OnSessionCreated;

	/** Delegate called when session join completes */
	UPROPERTY(BlueprintAssignable, Category="Network")
	FOnSessionJoined OnSessionJoined;

	/** Delegate called when session search completes */
	UPROPERTY(BlueprintAssignable, Category="Network")
	FOnSessionSearchComplete OnSessionSearchComplete;

protected:
	/** Get the world context */
	UWorld* GetWorld() const override;

	/** Called when session creation completes */
	void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);

	/** Called when session start completes */
	void OnStartSessionComplete(FName InSessionName, bool bWasSuccessful);

	/** Called when session find completes */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/** Called when session join completes */
	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result);

	/** Called when session destroy completes */
	void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);

	/** Called when session end completes */
	void OnEndSessionComplete(FName InSessionName, bool bWasSuccessful);

	/** Aggressively remove all menu widgets from viewport */
	void RemoveAllMenuWidgets(class APlayerController* PlayerController);

private:
	/** Online session interface */
	IOnlineSessionPtr SessionInterface;

	/** Session search results */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	/** Cached search results */
	TArray<FOnlineSessionSearchResult> SessionSearchResults;

	/** Session name */
	FName SessionName;

	/** Delegate handles */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

	/** Delegate handles */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
};

