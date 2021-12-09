// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

void UEOSGameInstance::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

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
	if (Subsystem)
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

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
	if (Subsystem)
	{
		FOnlineSessionSettings Settings;
		Settings.bIsDedicated = false;
		Settings.bShouldAdvertise = true;
		Settings.bIsLANMatch = false;
		Settings.NumPublicConnections = 10;
		Settings.bAllowJoinInProgress = true;
		Settings.bAllowJoinViaPresence = true;
		Settings.bUsesPresence = true;

		IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();
		SessionPtr->CreateSession(0, FName("Test Session"), Settings);
		SearchSettings->SearchResults.Empty();
	}
}

void UEOSGameInstance::FindSessionComplete(bool bSuccess)
{
	if (bSuccess)
	{
		for (FOnlineSessionSearchResult LobbyResult : SearchSettings->SearchResults)
		{
			FString Name = LobbyResult.Session.OwningUserName;
			UE_LOG(LogTemp, Warning, TEXT("SERVER NAME: %s"), *Name);

			IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();
			SessionPtr->JoinSession(0, FName("Test Session"), LobbyResult);
		}
	}
}

void UEOSGameInstance::JoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("JOINED SESSION: %s"), *(Name.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("RESULT: %s"), LexToString(Result));

	IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();

	FString ConnectInfo;
	SessionPtr->GetResolvedConnectString(Name, ConnectInfo);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientTravel(ConnectInfo, TRAVEL_Absolute);
}

void UEOSGameInstance::CreateSessionComplete(FName Name, bool bSuccess)
{
	if (bSuccess)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("ThirdPersonExampleMap"), true, FString("listen"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED TO CREATE SESSION!"));
	}
}

void UEOSGameInstance::FindSession()
{
	if (Subsystem)
	{
		IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();

		SearchSettings->bIsLanQuery = false;
		SearchSettings->MaxSearchResults = 50;

		SessionPtr->FindSessions(0, SearchSettings);
	}
}

void UEOSGameInstance::StartMatchmaking()
{
	if (Subsystem)
	{
		IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();
		SessionPtr->StartMatchmaking()
		
	}
}

void UEOSGameInstance::BeginDestroy()
{
	Super::BeginDestroy();

	if (Subsystem)
	{
		IOnlineSessionPtr SessionPtr = Subsystem->GetSessionInterface();
		SessionPtr->DestroySession(FName("Test Session"));
	}
}
