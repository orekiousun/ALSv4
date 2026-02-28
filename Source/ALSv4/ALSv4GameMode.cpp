// Copyright Epic Games, Inc. All Rights Reserved.

#include "ALSv4GameMode.h"
#include "ALSv4Character.h"
#include "UObject/ConstructorHelpers.h"

AALSv4GameMode::AALSv4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
