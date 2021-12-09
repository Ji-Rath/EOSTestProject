// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SharedPointer.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"

class IOnlineSubsystem;

/**
 * 
 */
UCLASS()
class EOSTESTPROJECT_API UEOSGameInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void AutoLogin();

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION()
	void FindSessionComplete(bool bSuccess);

	void JoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void CreateSessionComplete(FName Name, bool bSuccess);

	UFUNCTION(BlueprintCallable)
	void FindSession();

	UFUNCTION(BlueprintCallable)
	void StartMatchmaking();

	TSharedRef<FOnlineSessionSearch> SearchSettings = MakeShared<FOnlineSessionSearch>();

	IOnlineSubsystem* Subsystem;

	void BeginDestroy() override;

};
