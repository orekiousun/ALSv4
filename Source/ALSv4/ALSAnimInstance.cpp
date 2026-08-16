// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSAnimInstance.h"

#include "Kismet/KismetMathLibrary.h"

void UALSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AALSBaseCharacter>(TryGetPawnOwner());
}

void UALSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	DeltaTime = DeltaSeconds;
	if (DeltaTime == 0.f)
	{
		return;
	}
	if (!IsValid(Character))
	{
		return;
	}

	UpdateCharacterInfo();
	UpdateAimingValues();
	UpdateLayerValue();
	UpdateFootIK();
}

void UALSAnimInstance::UpdateCharacterInfo()
{
	IALSCharacterInterface* CharacterInterface = Cast<IALSCharacterInterface>(Character);
	if (!CharacterInterface)
	{
		return;
	}

	CharacterInterface->GetEssentialValues(Velocity, Acceleration, MovementInput, bIsMoving, bHasMovementInput, Speed,
	                                       MovementInputAmount, AimingRotation, AimYawRate);
	EMovementMode PawnMovementMode;
	CharacterInterface->GetCurrentStates(PawnMovementMode, MovementState, PrevMovementState, MovementAction,
	                                     RotationMode, Gait, Stance, ViewMode, OverlayState);
}

void UALSAnimInstance::UpdateAimingValues()
{
	if (IsValid(Character))
	{
		return;
	}

	FRotator ActorRotation = Character->GetActorRotation();
	// 插值SmoothedAimingRotation
	UKismetMathLibrary::RInterpTo(SmoothedAimingRotation, AimingRotation, DeltaTime, SmoothedAimingRotationInterpSpeed);

	// 计算控制器旋转和当前角色选装的插值，计算AimingAngle和SmoothedAimingAngle
	FRotator AimingDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimingRotation, ActorRotation);
	AimingAngle = FVector2D(AimingDeltaRotation.Yaw, AimingDeltaRotation.Pitch);
	FRotator SmoothedAimingDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(
		SmoothedAimingRotation, ActorRotation);
	SmoothedAimingAngle = FVector2D(SmoothedAimingDeltaRotation.Yaw, SmoothedAimingDeltaRotation.Pitch);

	switch (RotationMode)
	{
	case EALSRotationMode::LookingDirection:
	case EALSRotationMode::Aiming:
		AimSweepTime = UKismetMathLibrary::MapRangeClamped(AimingAngle.Y, -90.f, 90.f, 1.f, 0.f);
		SpineRotation = FRotator(0.f, 0.f, AimingAngle.X / 4);
	case EALSRotationMode::VelocityDirection:
		if (bHasMovementInput)
		{
			// 计算InputYawOffsetTime
			FRotator MovementDirection = UKismetMathLibrary::Conv_VectorToRotator(MovementInput);
			float DeltaYaw = MovementDirection.Yaw - ActorRotation.Yaw;
			float TargetYawOffsetTime = UKismetMathLibrary::MapRangeClamped(DeltaYaw, -180.f, 180.f, 0.f, 1.f);
			InputYawOffsetTime = UKismetMathLibrary::FInterpTo(InputYawOffsetTime, TargetYawOffsetTime, DeltaTime,
			                                                   InputYawOffsetTimeInterpSpeed);
		}
	}

	// 计算LeftYawTime、RightYawTime、ForwardYawTime
	LeftYawTime = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::Abs(SmoothedAimingAngle.X), 0.f, 180.f, 0.5f,
	                                                  0.f);
	RightYawTime = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::Abs(SmoothedAimingAngle.X), 0.f, 180.f, 0.5f,
	                                                   1.f);
	ForwardYawTime = UKismetMathLibrary::MapRangeClamped(SmoothedAimingAngle.X, -180.f, 180.f, 0.f, 1.f);
}

void UALSAnimInstance::UpdateLayerValue()
{
}

void UALSAnimInstance::UpdateFootIK()
{
}

void UALSAnimInstance::UpdateMovementValues()
{
}

void UALSAnimInstance::UpdateRotationValues()
{
}

void UALSAnimInstance::UpdateInAirValues()
{
}

void UALSAnimInstance::UpdateRagdollValues()
{
}
