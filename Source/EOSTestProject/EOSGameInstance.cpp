// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include <EOSTestProject\EOSTestProjectGameMode.h>

void UEOSGameInstance::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		SessionPtr = Subsystem->GetSessionInterface();
		Identity = Subsystem->GetIdentityInterface();

		if (Identity && SessionPtr)
		{
			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::CreateSessionComplete);
			Identity->AddOnLoginCompleteDelegate_Handle(0, FOnLoginComplete::FDelegate::CreateUObject(this, &UEOSGameInstance::HandleLoginComplete));
			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::FindSessionComplete);
			SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::JoinSessionComplete);
		}
	}

	AutoLogin();
}

void UEOSGameInstance::HandleLoginComplete(int32 LocalUserNum,bool bWasSuccessful,const FUniqueNetId& UserId,const FString& Error)
{
	// TODO: Check bWasSuccessful to see if the login was completed.

}

void UEOSGameInstance::AutoLogin()
{
	if (Identity)
	{
		if (!Identity->AutoLogin(0 /* LocalUserNum */))
		{
			// Call didn't start, return error.
			UE_LOG(LogTemp, Error, TEXT("FAILED TO AUTO LOGIN!"));

			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");
			Identity->Login(0, Credentials);
		}
	}
}

void UEOSGameInstance::CreateSession()
{
	if (SessionPtr)
	{
		FOnlineSessionSettings Settings;
		Settings.bIsDedicated = false;
		Settings.bShouldAdvertise = true;
		Settings.bIsLANMatch = false;
		Settings.NumPublicConnections = 10;
		Settings.bAllowJoinInProgress = true;
		Settings.bAllowJoinViaPresence = true;
		Settings.bUsesPresence = true;

		SessionPtr->CreateSession(0, FName("Test Session"), Settings);
		SearchSettings->SearchResults.Empty();
	}
}

void UEOSGameInstance::FindSessionComplete(bool bSuccess)
{
	if (bSuccess)
	{
		TArray<FSessionInfo> Sessions;
		Sessions.Reserve(SearchSettings->SearchResults.Num());
		for (FOnlineSessionSearchResult LobbyResult : SearchSettings->SearchResults)
		{
			Sessions.Emplace(FSessionInfo(LobbyResult));
		}
		OnFindSessions.Broadcast(bSuccess, Sessions);
	}
}

void UEOSGameInstance::JoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("JOINED SESSION: %s"), *(Name.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("RESULT: %s"), LexToString(Result));

	FString ConnectInfo;
	SessionPtr->GetResolvedConnectString(Name, ConnectInfo);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientTravel(ConnectInfo, TRAVEL_Absolute);
}

void UEOSGameInstance::CreateSessionComplete(FName Name, bool bSuccess)
{
	if (bSuccess)
	{
		if (AEOSTestProjectGameMode* GameMode = Cast<AEOSTestProjectGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->RegisterExistingPlayers();
		}
		UGameplayStatics::OpenLevel(GetWorld(), FName("ThirdPersonExampleMap"), true, FString("listen"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED TO CREATE SESSION!"));
	}
}

void UEOSGameInstance::FindSession()
{
	if (Subsystem && SessionPtr)
	{
		SearchSettings->bIsLanQuery = false;
		SearchSettings->MaxSearchResults = 50;

		SessionPtr->FindSessions(0, SearchSettings);
	}
}

void UEOSGameInstance::BeginDestroy()
{
	Super::BeginDestroy();

	if (SessionPtr)
	{
		SessionPtr->DestroySession(FName("Test Session"));
	}
}

bool UEOSGameInstance::JoinSession(FSessionInfo SearchResult)
{
	if (SessionPtr)
		return SessionPtr->JoinSession(0, FName("Test Session"), SearchResult.SearchResult);

	return false;
}
