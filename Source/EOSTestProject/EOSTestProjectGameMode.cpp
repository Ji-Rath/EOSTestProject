// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSTestProjectGameMode.h"
#include "EOSTestProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <OnlineSubsystemUtils.h>

AEOSTestProjectGameMode::AEOSTestProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AEOSTestProjectGameMode::RegisterExistingPlayers()
{
    for (auto It = GetWorld()->GetPlayerControllerIterator(); It; --It)
    {
        APlayerController* PlayerController = It->Get();

        // Get the unique player ID.
        TSharedPtr<const FUniqueNetId> UniqueNetId = GetNetId(PlayerController);
        check(UniqueNetId != nullptr);

        // Get the online session interface.
        IOnlineSubsystem* Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
        IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

        // Register the player with the "MyLocalSessionName" session; this name should match the name you provided in CreateSession.
        if (!Session->RegisterPlayer(FName(TEXT("MyLocalSessionName")), *UniqueNetId, false))
        {
            // The player could not be registered; typically you will want to kick the player from the server in this situation.
        }
    }
}

TSharedPtr<const FUniqueNetId> AEOSTestProjectGameMode::GetNetId(APlayerController* PlayerController)
{
    FUniqueNetIdRepl UniqueNetIdRepl;
    if (PlayerController->IsLocalPlayerController())
    {
        ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
        if (IsValid(LocalPlayer))
        {
            UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
        }
        else
        {
            UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
            check(IsValid(RemoteNetConnection));
            UniqueNetIdRepl = RemoteNetConnection->PlayerId;
        }
    }
    else
    {
        UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
        check(IsValid(RemoteNetConnection));
        UniqueNetIdRepl = RemoteNetConnection->PlayerId;
    }

    // Get the unique player ID.
    return UniqueNetIdRepl.GetUniqueNetId();
}

void AEOSTestProjectGameMode::PostLogin(APlayerController* PlayerController)
{
    check(IsValid(PlayerController));

    // Get the unique player ID.
    TSharedPtr<const FUniqueNetId> UniqueNetId = GetNetId(PlayerController);
    check(UniqueNetId != nullptr);

    // Get the online session interface.
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    // Register the player with the "MyLocalSessionName" session; this name should match the name you provided in CreateSession.
    if (!Session->RegisterPlayer(FName(TEXT("Test Session")), *UniqueNetId, false))
    {
        // The player could not be registered; typically you will want to kick the player from the server in this situation.
    }

    Super::PostLogin(PlayerController);
}

void AEOSTestProjectGameMode::PreLogout(APlayerController* PlayerController)
{
    check(IsValid(PlayerController));

    // Get the unique player ID.
    TSharedPtr<const FUniqueNetId> UniqueNetId = GetNetId(PlayerController);
    check(UniqueNetId != nullptr);

    // Get the online session interface.
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    // Register the player with the "MyLocalSessionName" session; this name should match the name you provided in CreateSession.
    if (!Session->UnregisterPlayer(FName(TEXT("Test Session")), *UniqueNetId))
    {
        // The player could not be registered; typically you will want to kick the player from the server in this situation.
    }
}
