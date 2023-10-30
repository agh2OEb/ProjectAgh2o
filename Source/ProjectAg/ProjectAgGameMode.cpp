// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectAgGameMode.h"
#include "ProjectAgCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectAgGameMode::AProjectAgGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
