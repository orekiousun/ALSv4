// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBaseCameraAnimInstance.h"

#include "ALSv4/Character/ALSCharacterInterface.h"

void UALSBaseCameraAnimInstance::SetControlledPawn(APawn* InPawn)
{
	ControlledPawn = InPawn;
}

void UALSBaseCameraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 更新State参数供状态机调用
	if (IALSCharacterInterface* CharacterInterface = Cast<IALSCharacterInterface>(ControlledPawn))
	{
		EMovementMode MovementMode;
		EALSMovementState PrevMomentumState;
		CharacterInterface->GetCurrentStates(MovementMode, MovementState, PrevMomentumState, MovementAction,
		                                     RotationMode, Gait, Stance, ViewMode);
	}
}
