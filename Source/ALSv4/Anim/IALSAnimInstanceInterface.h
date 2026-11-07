// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSv4/ALSTypes.h"
#include "UObject/Interface.h"
#include "IALSAnimInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIALSAnimInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSV4_API IIALSAnimInstanceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnJumped();
	virtual void SetGroundedEntryState(EALSGroundedEntryState InGroundedEntryState);
};
