// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "EOSGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PingInMs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString OwningUserName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NumOpenPublicConnections;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NumPublicConnections;

	FOnlineSessionSearchResult SearchResult;

	FSessionInfo() {};

	FSessionInfo(FOnlineSessionSearchResult Result)
	{
		PingInMs = Result.PingInMs;
		OwningUserName = Result.Session.OwningUserName;
		NumOpenPublicConnections = Result.Session.NumOpenPublicConnections;
		NumPublicConnections = Result.Session.SessionSettings.NumPublicConnections;
		SearchResult = Result;
	}
};

class IOnlineSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFindSessions, bool, bSuccess, TArray<FSessionInfo>, Sessions);

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

	TSharedRef<FOnlineSessionSearch> SearchSettings = MakeShared<FOnlineSessionSearch>();

	IOnlineSubsystem* Subsystem;

	IOnlineSessionPtr SessionPtr;

	IOnlineIdentityPtr Identity;

	void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	bool JoinSession(FSessionInfo SearchResult);

	UPROPERTY(BlueprintAssignable)
	FOnFindSessions OnFindSessions;
};
