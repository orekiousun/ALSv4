// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSAnimInstance.h"

#include "ALSBlueprintFunctionLibrary.h"
#include "ALSControllerInterface.h"
#include "KismetAnimationLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

class IALSControllerInterface;
class AALSPlayerController;

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
				// 正在移动时
				UpdateMovementValues();
				UpdateRotationValues();
			}
			else if (!bOldShouldMove && !bShouldMove)
			{
				// 没有移动时
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
				// 开始移动时
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

bool UALSAnimInstance::ShouldMoveCheck()
{
	return (bIsMoving && bHasMovementInput) || Speed > 150.f;
}

bool UALSAnimInstance::CanRotateInPlace()
{
	return RotationMode == EALSRotationMode::Aiming || ViewMode == EALSViewMode::FirstPerson;
}

void UALSAnimInstance::RotateInPlaceCheck()
{
	// 超过对应角度阈值需要向左/向右旋转
	Rotate_L = AimingAngle.X < RotateInPlaceSettings.RotateMinThreshold;
	Rotate_R = AimingAngle.X > RotateInPlaceSettings.RotateMaxThreshold;

	// 更新RotateRate
	if (Rotate_L || Rotate_R)
	{
		RotateRate = UKismetMathLibrary::MapRangeClamped(AimYawRate,
		                                                 RotateInPlaceSettings.AimYawRateMinRange,
		                                                 RotateInPlaceSettings.AimYawRateMaxRange,
		                                                 RotateInPlaceSettings.MinPlayRate,
		                                                 RotateInPlaceSettings.MaxPlayRate
		);
	}
}

bool UALSAnimInstance::CanTurnInPlace()
{
	return RotationMode == EALSRotationMode::LookingDirection && ViewMode == EALSViewMode::ThirdPerson && GetCurveValue(
		TEXT("Enable_Transition")) > 0.99;
}

void UALSAnimInstance::TurnInPlaceCheck()
{
	if (UKismetMathLibrary::Abs(AimingAngle.X) > TurnInPlaceSettings.TurnCheckMinAngle && AimYawRate <
		TurnInPlaceSettings.AimYawRateLimit)
	{
		// 延迟一定时间再旋转
		ElapsedDelayTime += DeltaTime;
		float RangeClampedDelay = UKismetMathLibrary::MapRangeClamped(AimingAngle.X,
		                                                              TurnInPlaceSettings.TurnCheckMinAngle,
		                                                              TurnInPlaceSettings.TurnCheckMaxAngle,
		                                                              TurnInPlaceSettings.MinAngleDelay,
		                                                              TurnInPlaceSettings.MaxAngleDelay);
		if (ElapsedDelayTime > RangeClampedDelay)
		{
			TurnInPlace(
				FRotator(0.f, AimingRotation.Yaw, 0.f),
				1.f,
				0.f,
				false
			);
		}
	}
	else
	{
		ElapsedDelayTime = 0.f;
	}
}

void UALSAnimInstance::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent)
{
	float TurnAngle = TargetRotation.Yaw - Character->GetActorRotation().Yaw;
	FALSTurnInPlaceAsset TargetTurnAsset;
	if (UKismetMathLibrary::Abs(TurnAngle) < TurnInPlaceSettings.Turn180Threshold)
	{
		if (TurnAngle < 0)
		{
			TargetTurnAsset = Stance == EALSStance::Standing
				                  ? TurnInPlaceSettings.N_TurnIP_L90
				                  : TurnInPlaceSettings.CLF_TurnIP_L90;
		}
		else
		{
			TargetTurnAsset = Stance == EALSStance::Standing
				                  ? TurnInPlaceSettings.N_TurnIP_R90
				                  : TurnInPlaceSettings.CLF_TurnIP_R90;
		}
	}
	else
	{
		if (TurnAngle < 0)
		{
			TargetTurnAsset = Stance == EALSStance::Standing
				                  ? TurnInPlaceSettings.N_TurnIP_L180
				                  : TurnInPlaceSettings.CLF_TurnIP_L180;
		}
		else
		{
			TargetTurnAsset = Stance == EALSStance::Standing
				                  ? TurnInPlaceSettings.N_TurnIP_R180
				                  : TurnInPlaceSettings.CLF_TurnIP_R180;
		}
	}

	if (bOverrideCurrent || !IsPlayingSlotAnimation(TargetTurnAsset.Anim, TargetTurnAsset.SlotName))
	{
		// 播放转身的脚部蒙太奇
		PlaySlotAnimationAsDynamicMontage(
			TargetTurnAsset.Anim,
			TargetTurnAsset.SlotName,
			0.2f,
			0.2f,
			PlayRateScale * TargetTurnAsset.PlayRate,
			1,
			0.f,
			StartTime
		);
		RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
		if (TargetTurnAsset.bScaleTurnAngle)
		{
			RotationScale *= (TurnAngle / TargetTurnAsset.AnimateAngle);
		}
	}
}

bool UALSAnimInstance::CanDynamicTransition()
{
	return GetCurveValue(TEXT("Enable_Transition")) == 1.f;
}

void UALSAnimInstance::DynamicTransitionCheck()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponent();
	if (!SkeletalMeshComponent)
	{
		return;
	}

	float SocketDistance_L = UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange(
		SkeletalMeshComponent,
		TEXT("ik_foot_l"),
		ERelativeTransformSpace::RTS_Component,
		TEXT("VB foot_target_l"),
		ERelativeTransformSpace::RTS_Component,
		false,
		0.f,
		0.f,
		0.f,
		0.f
	);
	if (SocketDistance_L > 8.f)
	{
		PlayDynamicTransition(0.1, DynamicTransitionParams_L);
	}
	float SocketDistance_R = UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange(
		SkeletalMeshComponent,
		TEXT("ik_foot_r"),
		ERelativeTransformSpace::RTS_Component,
		TEXT("VB foot_target_r"),
		ERelativeTransformSpace::RTS_Component,
		false,
		0.f,
		0.f,
		0.f,
		0.f
	);
	if (SocketDistance_R > 8.f)
	{
		PlayDynamicTransition(0.1, DynamicTransitionParams_R);
	}
}

void UALSAnimInstance::PlayDynamicTransition(float ReTriggerDelay, FALSDynamicMontageParams Params)
{
	static float LastTime = 0.f;
	float NowTime = UGameplayStatics::GetTimeSeconds(this);
	if (NowTime > LastTime + ReTriggerDelay)
	{
		PlaySlotAnimationAsDynamicMontage(
			Params.Anim,
			TEXT("Grounded Slot"),
			Params.BlendInTime,
			Params.BlendOutTime,
			Params.PlayRate,
			1.f,
			0.f,
			Params.StartTime
		);
		LastTime = NowTime;
	}
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

void UALSAnimInstance::UpdateInAirValues()
{
	FallSpeed = Velocity.Z;
	LandPrediction = CalcLandPrediction();
	LeanAmount = UALSBlueprintFunctionLibrary::InterpLeanAmount(LeanAmount, CalcInAirLeanAmount(), InAirLeanInterpSpeed,
	                                                            DeltaTime);
}

float UALSAnimInstance::CalcLandPrediction()
{
	if (FallSpeed >= -200.f)
	{
		return 0.f;
	}

	UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!CapsuleComponent || !MoveComp)
	{
		return 0.f;
	}

	FVector TraceStart = CapsuleComponent->GetComponentLocation();
	FVector Direction = UKismetMathLibrary::Vector_NormalUnsafe(FVector(Velocity.X, Velocity.Y,
	                                                                    UKismetMathLibrary::Clamp(
		                                                                    Velocity.Z, -4000.f, -200.f)));

	FVector TraceEnd = TraceStart + Direction * UKismetMathLibrary::MapRangeClamped(
		Velocity.Z, 0.f, -4000.f, 50.f, 2000.f);
	TArray<AActor*> IgnoreActors;
	FHitResult Hit;
	UKismetSystemLibrary::CapsuleTraceSingleByProfile(
		this,
		TraceStart,
		TraceEnd,
		CapsuleComponent->GetScaledCapsuleRadius(),
		CapsuleComponent->GetScaledCapsuleHalfHeight(),
		TEXT("ALS_Character"),
		false,
		IgnoreActors,
		GetTraceDebugType(EDrawDebugTrace::ForOneFrame),
		Hit,
		true,
		FColor::Red,
		FColor::Green
	);
	if (MoveComp->IsWalkable(Hit) && Hit.bBlockingHit)
	{
		float CurveValue = UALSBlueprintFunctionLibrary::GetCurveFloatValue(LandPredictionCurve, Hit.Time);
		return UKismetMathLibrary::Lerp(CurveValue, 0.f, GetCurveValue("Mask_LandPrediction"));
	}
	return 0.f;;
}

FALSLeanAmount UALSAnimInstance::CalcInAirLeanAmount()
{
	return FALSLeanAmount();
}

void UALSAnimInstance::UpdateFootIK()
{
	SetFootLocking(TEXT("Enable_FootIK_L"), TEXT("FootLock_L"), TEXT("ik_foot_l"), FootLock_L_Alpha,
	               FootLock_L_Location, FootLock_L_Rotation);
	SetFootLocking(TEXT("Enable_FootIK_R"), TEXT("FootLock_R"), TEXT("ik_foot_r"), FootLock_R_Alpha,
	               FootLock_R_Location, FootLock_R_Rotation);

	switch (MovementState)
	{
	case EALSMovementState::None:
	case EALSMovementState::Grounded:
	case EALSMovementState::Mantling:
		{
			FVector FootOffset_R_Target;
			FVector FootOffset_L_Target;
			SetFootOffset(TEXT("Enable_FootIK_L"), TEXT("ik_foot_l"), TEXT("root"), FootOffset_L_Target,
			              FootLock_L_Location, FootOffset_L_Rotation);
			SetFootOffset(TEXT("Enable_FootIK_R"), TEXT("ik_foot_r"), TEXT("root"), FootOffset_R_Target,
			              FootLock_R_Location, FootOffset_R_Rotation);
			SetPelvisIKOffset(FootOffset_L_Target, FootOffset_R_Target);
		}
	case EALSMovementState::InAir:
		{
			SetPelvisIKOffset(FVector::ZeroVector, FVector::ZeroVector);
			ResetIKOffset();
		}
	}
}

void UALSAnimInstance::SetFootLocking(FName EnableFootIKCurve, FName FootLockCurve, FName IKFootBone,
                                      float& CurFootLockAlpha, FVector& CurFootLockLocation,
                                      FRotator& CurFootLockRotation)
{
	if (GetCurveValue(EnableFootIKCurve) <= 0.f)
	{
		return;
	}

	// 只在CurFootLockAlpha在变小或者等于1时才设置CurFootLockAlpha，是的这里只能混出，不能混入，除非lock到了一个新的位置
	float FootLockCurveValue = GetCurveValue(FootLockCurve);
	if (FootLockCurveValue >= 0.99f || FootLockCurveValue < CurFootLockAlpha)
	{
		CurFootLockAlpha = FootLockCurveValue;
	}

	if (CurFootLockAlpha >= 0.99f)
	{
		if (USkeletalMeshComponent* OwningComp = GetOwningComponent())
		{
			FTransform SocketTransform = OwningComp->GetSocketTransform(IKFootBone, RTS_Component);
			CurFootLockLocation = SocketTransform.GetLocation();
			CurFootLockRotation = SocketTransform.Rotator();
		}
	}

	if (CurFootLockAlpha > 0.f)
	{
		SetFootLockOffset(CurFootLockLocation, CurFootLockRotation);
	}
}

void UALSAnimInstance::SetFootLockOffset(FVector& LocalLocation, FRotator& LocalRotation)
{
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	FVector LocationDifference = FVector::ZeroVector;
	FRotator RotationDifference = FRotator::ZeroRotator;
	if (MoveComp->IsMovingOnGround())
	{
		// 从上一帧的Rotation和当前Rotation获取旋转差量
		RotationDifference = Character->GetActorRotation() - MoveComp->GetLastUpdateRotation();
	}

	if (USkeletalMeshComponent* OwingComp = GetOwningComponent())
	{
		// 计算位移差量
		LocationDifference = OwingComp->GetComponentRotation().UnrotateVector(
			Velocity * UGameplayStatics::GetWorldDeltaSeconds(this));
	}

	LocalLocation = UKismetMathLibrary::RotateAngleAxis(LocalLocation - LocationDifference, RotationDifference.Yaw,
	                                                    FVector::DownVector);
	LocalRotation = LocalRotation - RotationDifference;
}

void UALSAnimInstance::SetFootOffset(FName EnableFootIKCurve, FName IKFootBone, FName RootBone,
                                     FVector& CurLocationTarget, FVector& CurLocationOffset,
                                     FRotator& CurRotationOffset)
{
	if (GetCurveValue(EnableFootIKCurve) <= 0.f)
	{
		CurLocationOffset = FVector::ZeroVector;
		CurRotationOffset = FRotator::ZeroRotator;
		return;
	}

	USkeletalMeshComponent* OwningComp = GetOwningComponent();
	if (!OwningComp)
	{
		return;
	}

	FVector FootLocation = OwningComp->GetSocketLocation(IKFootBone);
	FVector RootLocation = OwningComp->GetSocketLocation(RootBone);
	FVector IKFootFloorLocation = FVector(FootLocation.X, FootLocation.Y, RootLocation.Z);
	FRotator TargetRotationOffset;
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;
	// TODO: TraceChannel待定
	UKismetSystemLibrary::LineTraceSingle(
		this,
		IKFootFloorLocation + FVector(0.f, 0.f, FootIKSettings.IK_TraceDistanceAboveFoot),
		IKFootFloorLocation + FVector(0.f, 0.f, FootIKSettings.IK_TraceDistanceBelowFoot),
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		GetTraceDebugType(EDrawDebugTrace::Type::ForOneFrame),
		Hit,
		true,
		FColor::Red,
		FColor::Green,
		5.f
	);

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (MoveComp && MoveComp->IsWalkable(Hit))
	{
		FVector ImpactPoint = Hit.ImpactPoint;
		FVector ImpactNormal = Hit.ImpactNormal;
		CurLocationTarget = ImpactPoint + ImpactNormal * FootIKSettings.FootHeight - (IKFootFloorLocation +
			FVector::UpVector * FootIKSettings.FootHeight);
		TargetRotationOffset = FRotator(
			-UKismetMathLibrary::DegAtan2(ImpactNormal.X, ImpactNormal.Z),
			UKismetMathLibrary::DegAtan2(ImpactNormal.Y, ImpactNormal.Z),
			0.f
		);
	}

	float InterpSpeed = CurLocationOffset.Z > CurLocationTarget.Z ? 30.f : 15.f;
	CurLocationOffset = UKismetMathLibrary::VInterpTo(CurLocationOffset, CurLocationTarget, DeltaTime, InterpSpeed);
	CurRotationOffset = UKismetMathLibrary::RInterpTo(CurRotationOffset, TargetRotationOffset, DeltaTime, 30.f);
}

void UALSAnimInstance::SetPelvisIKOffset(FVector FootOffsetLTarget, FVector FootOffsetRTarget)
{
	PelvisAlpha = GetCurveValue(TEXT("Enable_FootIK_L")) + GetCurveValue(TEXT("Enable_FootIK_R")) / 2;
	if (PelvisAlpha <= 0.f)
	{
		PelvisOffset = FVector::ZeroVector;
	}

	FVector PelvisTarget = FootOffsetLTarget.Z < FootOffsetRTarget.Z ? FootOffsetLTarget : FootOffsetRTarget;
	float InterpSpeed = PelvisTarget.Z > PelvisOffset.Z ? 10.f : 15.f;
	PelvisOffset = UKismetMathLibrary::VInterpTo(PelvisOffset, PelvisTarget, DeltaTime, InterpSpeed);
}

void UALSAnimInstance::ResetIKOffset()
{
	FootOffset_L_Location = UKismetMathLibrary::VInterpTo(FootLock_L_Location, FVector::ZeroVector, DeltaTime, 15.f);
	FootOffset_R_Location = UKismetMathLibrary::VInterpTo(FootLock_R_Location, FVector::ZeroVector, DeltaTime, 15.f);
	FootOffset_L_Rotation = UKismetMathLibrary::RInterpTo(FootLock_L_Rotation, FRotator::ZeroRotator, DeltaTime, 15.f);
	FootOffset_R_Rotation =
		UKismetMathLibrary::RInterpTo(FootOffset_R_Rotation, FRotator::ZeroRotator, DeltaTime, 15.f);
}

void UALSAnimInstance::UpdateRagdollValues()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponent();
	if (!SkeletalMeshComponent)
	{
		return;
	}

	FVector RootVelocity = SkeletalMeshComponent->GetPhysicsLinearVelocity(TEXT("root"));
	FlailRate = UKismetMathLibrary::MapRangeClamped(RootVelocity.Length(), 0.f, 1000.f, 0.f, 1.f);
}

EDrawDebugTrace::Type UALSAnimInstance::GetTraceDebugType(EDrawDebugTrace::Type TraceType) const
{
	if (!Character)
	{
		return EDrawDebugTrace::Type::None;
	}

	if (IALSControllerInterface* Interface = Character->GetController<IALSControllerInterface>())
	{
		if (Interface->GetShowTraces())
		{
			return TraceType;
		}
	}

	return EDrawDebugTrace::Type::None;
}
