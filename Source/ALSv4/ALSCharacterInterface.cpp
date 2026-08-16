// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSCharacterInterface.h"

void IALSCharacterInterface::GetCurrentStates(EMovementMode& OutPawnMovementMode, EALSMovementState& OutMovementState,
	EALSMovementState& OutPrevMovementState, EALSMovementAction& OutMovementAction, EALSRotationMode& OutRotationMode,
	EALSGait& OutGait, EALSStance& OutStance, EALSViewMode& OutViewMode, EALSOverlayState& OutOverlayState)
{
}

void IALSCharacterInterface::GetEssentialValues(FVector& OutVelocity, FVector& OutAcceleration,
	FVector& OutMovementInput, bool& bOutIsMoving, bool& bOutHasMovementInput, float& OutSpeed,
	float& OutMovementInputAmount, FRotator& OutAimingRotation, float& OutAimYawRate)
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
