// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSPlayerController.h"
#include <EOSTestProject\EOSTestProjectGameMode.h>

void AEOSPlayerController::OnNetCleanup(UNetConnection* Connection)
{
    if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
    {
        AEOSTestProjectGameMode* GameMode = Cast<AEOSTestProjectGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->PreLogout(this);
        }
    }

	Super::OnNetCleanup(Connection);
}