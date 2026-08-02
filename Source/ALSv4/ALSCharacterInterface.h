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
	virtual void GetCurrentStates(OUT EMovementMode& PawnMovementMode, OUT EALSMovementState MovementState,
	                              OUT EALSMovementState& PrevMovementState, OUT EALSMovementAction& MovementAction,
	                              OUT EALSRotationMode& RotationMode, OUT EALSGait& Gait, OUT EALSStance& Stance,
	                              OUT EALSViewMode ViewMode,
	                              OUT EALSOverlayState& OverlayState);
	virtual void GetEssentialValues(OUT FVector& Velocity, OUT FVector& Acceleration, OUT FVector& MovementInput,
	                                OUT bool bIsMoving, OUT bool bHasMovementInput, OUT float Speed,
	                                OUT float MovementInputAmount, OUT FRotator& AimingRotation, OUT float AimYawRate);
	virtual void SetMovementState(EALSMovementState NewMovementState);
	virtual void SetMovementAction(EALSMovementAction NewMovementAction);
	virtual void SetRotationMode(EALSRotationMode NewRotationMode);
	virtual void SetGait(EALSGait NewGait);
	virtual void SetViewMode(EALSViewMode NewViewMode);
	virtual void SetOverlayState(EALSOverlayState NewOverlayState);
};
