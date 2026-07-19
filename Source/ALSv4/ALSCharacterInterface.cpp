// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSCharacterInterface.h"


// Add default functionality here for any IALSCharacterInterface functions that are not pure virtual.
void IALSCharacterInterface::GetCurrentStates(EMovementMode& PawnMovementMode, EALSMovementState MovementState,
	EALSMovementState& PrevMovementState, EALSMovementAction& MovementAction, EALSRotationMode& RotationMode,
	EALSGait& Gait, EALSStance& Stance, EALSViewMode ViewMode, EALSOverlayState& OverlayState)
{
}

void IALSCharacterInterface::GetEssentialValues(FVector& Velocity, FVector& Acceleration, FVector& MovementInput,
	bool bIsMoving, bool bHasMovementInput, float Speed, float MovementInputAmount, FRotator& AimingRotation,
	float AimYawRate)
{
}

void IALSCharacterInterface::SetMovementState(EALSMovementState NewMovementState)
{
}

void IALSCharacterInterface::SetMovementAction(EALSMovementAction NewMovementAction)
{
}

void IALSCharacterInterface::SetRotationMode(EALSRotationMode NewRotationMode)
{
}

void IALSCharacterInterface::SetGait(EALSGait NewGait)
{
}

void IALSCharacterInterface::SetViewMode(EALSViewMode NewViewMode)
{
}

void IALSCharacterInterface::SetOverlayState(EALSOverlayState NewOverlayState)
{
}
