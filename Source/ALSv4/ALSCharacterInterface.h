// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSTypes.h"
#include "UObject/Interface.h"
#include "ALSCharacterInterface.generated.h"

UINTERFACE()
class UALSCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSV4_API IALSCharacterInterface
{
	GENERATED_BODY()

public:
	virtual void GetCurrentStates(OUT EMovementMode& OutPawnMovementMode,
	                              OUT EALSMovementState& OutMovementState,
	                              OUT EALSMovementState& OutPrevMovementState,
	                              OUT EALSMovementAction& OutMovementAction,
	                              OUT EALSRotationMode& OutRotationMode,
	                              OUT EALSGait& OutGait,
	                              OUT EALSStance& OutStance,
	                              OUT EALSViewMode& OutViewMode,
	                              OUT EALSOverlayState& OutOverlayState);
	virtual void GetEssentialValues(OUT FVector& OutVelocity,
	                                OUT FVector& OutAcceleration,
	                                OUT FVector& OutMovementInput,
	                                OUT bool& bOutIsMoving,
	                                OUT bool& bOutHasMovementInput,
	                                OUT float& OutSpeed,
	                                OUT float& OutMovementInputAmount,
	                                OUT FRotator& OutAimingRotation,
	                                OUT float& OutAimYawRate);
	virtual void SetMovementState(EALSMovementState NewMovementState);
	virtual void SetMovementAction(EALSMovementAction NewMovementAction);
	virtual void SetRotationMode(EALSRotationMode NewRotationMode);
	virtual void SetGait(EALSGait NewGait);
	virtual void SetViewMode(EALSViewMode NewViewMode);
	virtual void SetOverlayState(EALSOverlayState NewOverlayState);
};
