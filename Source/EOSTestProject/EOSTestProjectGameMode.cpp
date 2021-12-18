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
