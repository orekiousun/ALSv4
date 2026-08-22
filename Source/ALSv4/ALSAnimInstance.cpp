// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSAnimInstance.h"

#include "ALSBlueprintFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	switch (MovementState)
	{
	case EALSMovementState::Grounded:
		{
			bool bOldShouldMove = bShouldMove;
			bShouldMove = ShouldMoveCheck();
			if (bOldShouldMove && bShouldMove)
			{
				UpdateMovementValues();
				UpdateRotationValues();
			}
			else if (!bOldShouldMove && !bShouldMove)
			{
				if (CanRotateInPlace())
				{
					RotateInPlaceCheck();
				}
				else
				{
					Rotate_L = false;
					Rotate_R = false;
				}

				if (CanTurnInPlace())
				{
					TurnInPlaceCheck();
				}
				else
				{
					ElapsedDelayTime = 0.0f;
				}

				if (CanDynamicTransition())
				{
					DynamicTransitionCheck();
				}
			}
			else if (!bOldShouldMove && bShouldMove)
			{
				ElapsedDelayTime = 0.0f;
				Rotate_L = false;
				Rotate_R = false;
			}
		}
	case EALSMovementState::InAir:
		UpdateInAirValues();
	case EALSMovementState::Ragdoll:
		UpdateRagdollValues();
	}
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
			FRotator MovementRotation = UKismetMathLibrary::Conv_VectorToRotator(MovementInput);
			float DeltaYaw = MovementRotation.Yaw - ActorRotation.Yaw;
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
	Enable_AimOffset = UKismetMathLibrary::Lerp(0, 1, GetCurveValue(TEXT("Mask_AimOffset")));
	BasePose_N = GetCurveValue(TEXT("BasePose_N"));
	BasePose_CLF = GetCurveValue(TEXT("BasePose_CLF"));
	Spine_Add = GetCurveValue(TEXT("Layering_Spine_Add"));
	Head_Add = GetCurveValue(TEXT("Layering_Head_Add"));
	Arm_L_Add = GetCurveValue(TEXT("Layering_Arm_L_Add"));
	Arm_R_Add = GetCurveValue(TEXT("Layering_Arm_R_Add"));
	Hand_L = GetCurveValue(TEXT("Layering_Hand_L"));
	Hand_R = GetCurveValue(TEXT("Layering_Hand_R"));
	Enable_HandIK_L = UKismetMathLibrary::Lerp(0.f, GetCurveValue(TEXT("Enable_HandIK_L")),
	                                           GetCurveValue(TEXT("Layering_Arm_L")));
	Enable_HandIK_R = UKismetMathLibrary::Lerp(0.f, GetCurveValue(TEXT("Enable_HandIK_R")),
	                                           GetCurveValue(TEXT("Layering_Arm_R")));
	Arm_L_LS = GetCurveValue(TEXT("Layering_Arm_L_LS"));
	Arm_R_LS = GetCurveValue(TEXT("Layering_Arm_R_LS"));
	Arm_L_MS = 1 - UKismetMathLibrary::FFloor(Arm_L_LS);
	Arm_R_MS = 1 - UKismetMathLibrary::FFloor(Arm_R_LS);
}

void UALSAnimInstance::UpdateFootIK()
{
}

void UALSAnimInstance::UpdateMovementValues()
{
	VelocityBlend = UALSBlueprintFunctionLibrary::InterpolateVelocityBlend(
		VelocityBlend, CalcVelocityBlend(), VelocityBlendInterpSpeed, DeltaTime);
	DiagonalScaleAmount = CalcDiagonalScaleAmount();
	RelativeAccelerationAmount = CalcRelativeAccelerationAmount();
	// 计算倾斜
	FALSLeanAmount TargetLeanAmount = FALSLeanAmount(RelativeAccelerationAmount.Y, RelativeAccelerationAmount.X);
	LeanAmount = UALSBlueprintFunctionLibrary::InterpLeanAmount(LeanAmount, TargetLeanAmount, GroundedLeanInterpSpeed,
	                                                            DeltaTime);

	// 计算Blend和PlayRate
	WalkRunBlend = CalcWalkRunBlend();
	StrideBlend = CalcStrideBlend();
	StandingPlayRate = CalcStandingPlayRate();
	CrouchingPlayRate = CalcCrouchingPlayRate();
}

void UALSAnimInstance::UpdateRotationValues()
{
	MovementDirection = CalcMovementDirection();
	float YawOffset = UKismetMathLibrary::Conv_VectorToRotator(Velocity).Yaw - Character->GetControlRotation().Yaw;
	FVector YawOffset_FB_Val = UALSBlueprintFunctionLibrary::GetCurveVectorValue(YawOffset_FB, YawOffset);
	FVector YawOffset_LR_Val = UALSBlueprintFunctionLibrary::GetCurveVectorValue(YawOffset_LR, YawOffset);
	FYaw = YawOffset_FB_Val.X;
	BYaw = YawOffset_FB_Val.Y;
	LYaw = YawOffset_LR_Val.X;
	RYaw = YawOffset_LR_Val.Y;
}

void UALSAnimInstance::UpdateInAirValues()
{
}

void UALSAnimInstance::UpdateRagdollValues()
{
}

bool UALSAnimInstance::ShouldMoveCheck()
{
	return (bIsMoving && bHasMovementInput) || Speed > 150.f;
}

bool UALSAnimInstance::CanRotateInPlace()
{
	return false;
}

void UALSAnimInstance::RotateInPlaceCheck()
{
}

bool UALSAnimInstance::CanTurnInPlace()
{
	return false;
}

void UALSAnimInstance::TurnInPlaceCheck()
{
}

bool UALSAnimInstance::CanDynamicTransition()
{
	return false;
}

void UALSAnimInstance::DynamicTransitionCheck()
{
}

FALSVelocityBlend UALSAnimInstance::CalcVelocityBlend()
{
	FALSVelocityBlend Ret;
	FVector VelocityDirection = UKismetMathLibrary::Normal(Velocity, 0.1f);
	FRotator ActorRotation = Character->GetActorRotation();
	FVector RelativeVelocityDir = ActorRotation.UnrotateVector(VelocityDirection);
	float Sum = UKismetMathLibrary::Abs(RelativeVelocityDir.X) + UKismetMathLibrary::Abs(RelativeVelocityDir.Y) +
		UKismetMathLibrary::Abs(RelativeVelocityDir.Z);
	FVector RelativeDirection = RelativeVelocityDir / Sum;
	Ret.F = UKismetMathLibrary::Clamp(RelativeDirection.X, 0.f, 1.f);
	Ret.B = UKismetMathLibrary::Clamp(RelativeDirection.X, -1.f, 0.f);
	Ret.L = UKismetMathLibrary::Clamp(RelativeDirection.Y, -1.f, 0.f);
	Ret.R = UKismetMathLibrary::Clamp(RelativeDirection.Y, 0.f, 1.f);
	return Ret;
}

float UALSAnimInstance::CalcDiagonalScaleAmount()
{
	float InTime = UKismetMathLibrary::Abs(VelocityBlend.F + VelocityBlend.B);
	return UALSBlueprintFunctionLibrary::GetCurveFloatValue(DiagonalScaleAmountCurve, InTime);
}

FVector UALSAnimInstance::CalcRelativeAccelerationAmount()
{
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		return FVector::ZeroVector;
	}

	// 速度和加速度夹角小于180度
	FRotator ActorRotation = Character->GetActorRotation();
	if (UKismetMathLibrary::Dot_VectorVector(Acceleration, Velocity) > 0.f)
	{
		float MaxAcceleration = MoveComp->GetMaxAcceleration();
		FVector AccelerationAmount = UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxAcceleration) /
			MaxAcceleration;
		return ActorRotation.UnrotateVector(AccelerationAmount);
	}
	else
	{
		float MaxBrakingDeceleration = MoveComp->GetMaxBrakingDeceleration();
		FVector AccelerationAmount = UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxBrakingDeceleration) /
			MaxBrakingDeceleration;
		return ActorRotation.UnrotateVector(AccelerationAmount);
	}
}

float UALSAnimInstance::CalcWalkRunBlend()
{
	return Gait == EALSGait::Walking ? 0.f : 1.f;
}

float UALSAnimInstance::CalcStrideBlend()
{
	// 获取当前步态
	float Weight_Gait = GetCurveValue(TEXT("Weight_Gait"));
	float Weight_Run = UKismetMathLibrary::Clamp(Weight_Gait - 1, 0.f, 1.f);

	// 混合步态对应的步幅
	float StrideBlendNWalk = UALSBlueprintFunctionLibrary::GetCurveFloatValue(StrideBlend_N_Walk, Speed);
	float StrideBlendNRun = UALSBlueprintFunctionLibrary::GetCurveFloatValue(StrideBlend_N_Run, Speed);
	float StrideBlendN = UKismetMathLibrary::Lerp(StrideBlendNWalk, StrideBlendNRun, Weight_Run);
	float StrideBlendCWalk = UALSBlueprintFunctionLibrary::GetCurveFloatValue(StrideBlend_C_Walk, Speed);

	// 混合下蹲
	float Weight_CLF = GetCurveValue(TEXT("BasePose_CLF"));
	return UKismetMathLibrary::Lerp(StrideBlendN, StrideBlendCWalk, Weight_CLF);
}

float UALSAnimInstance::CalcStandingPlayRate()
{
	if (!GetOwningComponent())
	{
		return 0.f;
	}

	// 从曲线获取当前的步态
	float Weight_Gait = GetCurveValue(TEXT("Weight_Gait"));
	float Weight_Run = UKismetMathLibrary::Clamp(Weight_Gait - 1, 0.f, 1.f);
	float Weight_Sprint = UKismetMathLibrary::Clamp(Weight_Gait - 2, 0.f, 1.f);

	// 混合播放速率
	float Run_Percent = UKismetMathLibrary::Lerp(Speed / AnimatedSpeed.AnimatedWalkSpeed,
	                                             Speed / AnimatedSpeed.AnimatedRunSpeed, Weight_Run);
	float Sprint_Percent = UKismetMathLibrary::Lerp(Run_Percent, Speed / AnimatedSpeed.AnimatedSprintSpeed,
	                                                Weight_Sprint);
	float PlayRate = Sprint_Percent / StrideBlend;
	PlayRate /= GetOwningComponent()->GetComponentScale().Z;
	return UKismetMathLibrary::Clamp(PlayRate, 0.f, 3.f);
}

float UALSAnimInstance::CalcCrouchingPlayRate()
{
	if (!GetOwningComponent())
	{
		return 0.f;
	}

	float PlayRate = Speed / AnimatedSpeed.AnimatedCrouchSpeed;
	PlayRate /= StrideBlend;
	PlayRate /= GetOwningComponent()->GetComponentScale().Z;
	return UKismetMathLibrary::Clamp(PlayRate, 0.f, 2.f);
}

EALSMovementDirection UALSAnimInstance::CalcMovementDirection()
{
	if (Gait == EALSGait::Sprinting || RotationMode == EALSRotationMode::VelocityDirection)
	{
		return EALSMovementDirection::Forward;
	}
	float YawOffset = UKismetMathLibrary::Conv_VectorToRotator(Velocity).Yaw - AimingRotation.Yaw;
	return UALSBlueprintFunctionLibrary::CalcQuadrant(
		MovementDirection, 70.f, -70.f, 110.f, -110.f, 5.f, YawOffset);
}
