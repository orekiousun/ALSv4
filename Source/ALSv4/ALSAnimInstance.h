// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IALSAnimInterface.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ALSAnimInstance.generated.h"

class AALSBaseCharacter;

UCLASS()
class ALSV4_API UALSAnimInstance : public UAnimInstance, public IIALSAnimInterface
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void OnJumped() override;
	virtual void SetGroundedEntryState(EALSGroundedEntryState InGroundedEntryState) override;

protected:
	void UpdateCharacterInfo();
	void UpdateAimingValues();
	void UpdateLayerValue();
	// Rotation
	void UpdateRotationValues();
	// RotateInPlace
	bool CanRotateInPlace();
	void RotateInPlaceCheck();
	// TurnInPlace
	bool CanTurnInPlace();
	void TurnInPlaceCheck();
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent);
	// DynamicTransition
	bool CanDynamicTransition();
	void DynamicTransitionCheck();
	void PlayDynamicTransition(float ReTriggerDelay, FALSDynamicMontageParams Params);
	// Movement
	void UpdateMovementValues();
	bool ShouldMoveCheck();
	FALSVelocityBlend CalcVelocityBlend();
	float CalcDiagonalScaleAmount();
	FVector CalcRelativeAccelerationAmount();
	float CalcWalkRunBlend();
	float CalcStrideBlend();
	float CalcStandingPlayRate();
	float CalcCrouchingPlayRate();
	EALSMovementDirection CalcMovementDirection();
	// InAir
	void UpdateInAirValues();
	float CalcLandPrediction();
	FALSLeanAmount CalcInAirLeanAmount();
	// FootIK
	void UpdateFootIK();
	void SetFootLocking(FName EnableFootIKCurve, FName FootLockCurve, FName IKFootBone, float& CurFootLockAlpha,
	                    FVector& CurFootLockLocation, FRotator& CurFootLockRotation);
	void SetFootLockOffset(FVector& LocalLocation, FRotator& LocalRotation);
	void SetFootOffset(FName EnableFootIKCurve, FName IKFootBone, FName RootBone, FVector& CurLocationTarget,
	                   FVector& CurLocationOffset, FRotator& CurRotationOffset);
	void SetPelvisIKOffset(FVector FootOffsetLTarget, FVector FootOffsetRTarget);
	void ResetIKOffset();
	// Ragdoll
	void UpdateRagdollValues();

	EDrawDebugTrace::Type GetTraceDebugType(EDrawDebugTrace::Type TraceType) const;

	// Config
	// InterpSpeed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|InterpSpeed")
	float SmoothedAimingRotationInterpSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|InterpSpeed")
	float InputYawOffsetTimeInterpSpeed = 8.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|InterpSpeed")
	float VelocityBlendInterpSpeed = 12.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|InterpSpeed")
	float GroundedLeanInterpSpeed = 4.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|InterpSpeed")
	float InAirLeanInterpSpeed;
	// Curve
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* DiagonalScaleAmountCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* StrideBlend_N_Walk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* StrideBlend_N_Run;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* StrideBlend_C_Walk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* LandPredictionCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* LeanInAirCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveVector* YawOffset_FB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveVector* YawOffset_LR;
	// FootIK
	FALSFootIKSettings FootIKSettings;

	// AnimatedSpeed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FALSAnimatedSpeed AnimatedSpeed;
	// RotateInPlace
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FALSRotateInPlaceSettings RotateInPlaceSettings;
	// TurnInPlace
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FALSTurnInPlaceSettings TurnInPlaceSettings;
	// DynamicTransition
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FALSDynamicMontageParams DynamicTransitionParams_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FALSDynamicMontageParams DynamicTransitionParams_R;

	TObjectPtr<AALSBaseCharacter> Character;
	FTimerHandle JumpTimerHandle;

	float DeltaTime;
	// CharacterInfo
	FVector Velocity;
	FVector Acceleration;
	FVector MovementInput;
	bool bIsMoving;
	UPROPERTY(BlueprintReadOnly)
	bool bHasMovementInput;
	UPROPERTY(BlueprintReadOnly)
	float Speed;
	float MovementInputAmount;
	FRotator AimingRotation;
	float AimYawRate;
	UPROPERTY(BlueprintReadOnly)
	EALSMovementState MovementState;
	EALSMovementState PrevMovementState;
	UPROPERTY(BlueprintReadOnly)
	EALSMovementAction MovementAction;
	UPROPERTY(BlueprintReadOnly)
	EALSRotationMode RotationMode;
	UPROPERTY(BlueprintReadOnly)
	EALSGait Gait;
	UPROPERTY(BlueprintReadOnly)
	EALSStance Stance;
	EALSViewMode ViewMode;
	// MovementValues
	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove;
	UPROPERTY(BlueprintReadOnly)
	FALSVelocityBlend VelocityBlend;
	UPROPERTY(BlueprintReadOnly)
	float DiagonalScaleAmount;
	UPROPERTY(BlueprintReadOnly)
	FVector RelativeAccelerationAmount;
	UPROPERTY(BlueprintReadOnly)
	FALSLeanAmount LeanAmount;
	UPROPERTY(BlueprintReadOnly)
	float WalkRunBlend;
	UPROPERTY(BlueprintReadOnly)
	float StrideBlend;
	UPROPERTY(BlueprintReadOnly)
	float StandingPlayRate;
	float CrouchingPlayRate;
	UPROPERTY(BlueprintReadOnly)
	EALSMovementDirection MovementDirection;
	UPROPERTY(BlueprintReadOnly)
	EALSGroundedEntryState GroundedEntryState;
	// RotationValues
	UPROPERTY(BlueprintReadOnly)
	float FYaw;
	UPROPERTY(BlueprintReadOnly)
	float BYaw;
	UPROPERTY(BlueprintReadOnly)
	float LYaw;
	UPROPERTY(BlueprintReadOnly)
	float RYaw;
	UPROPERTY(BlueprintReadOnly)
	bool Rotate_L;
	UPROPERTY(BlueprintReadOnly)
	bool Rotate_R;
	UPROPERTY(BlueprintReadOnly)
	float RotateRate;
	float ElapsedDelayTime;
	UPROPERTY(BlueprintReadOnly)
	float RotationScale;
	// InAirValues
	bool bJumped;
	float JumpPlayRate;
	float FallSpeed;
	float LandPrediction;
	// RagdollValues
	float FlailRate;
	// AimingValues
	FRotator SmoothedAimingRotation;
	FRotator SpineRotation;
	FVector2D AimingAngle;
	FVector2D SmoothedAimingAngle;
	float AimSweepTime;
	float InputYawOffsetTime;
	float ForwardYawTime;
	float LeftYawTime;
	float RightYawTime;
	//Foot IK
	UPROPERTY(BlueprintReadOnly)
	float FootLock_L_Alpha;
	UPROPERTY(BlueprintReadOnly)
	float FootLock_R_Alpha;
	UPROPERTY(BlueprintReadOnly)
	FVector FootLock_L_Location;
	UPROPERTY(BlueprintReadOnly)
	FVector FootLock_R_Location;
	UPROPERTY(BlueprintReadOnly)
	FRotator FootLock_L_Rotation;
	UPROPERTY(BlueprintReadOnly)
	FRotator FootLock_R_Rotation;
	UPROPERTY(BlueprintReadOnly)
	FVector FootOffset_L_Location;
	UPROPERTY(BlueprintReadOnly)
	FVector FootOffset_R_Location;
	UPROPERTY(BlueprintReadOnly)
	FRotator FootOffset_L_Rotation;
	UPROPERTY(BlueprintReadOnly)
	FRotator FootOffset_R_Rotation;
	UPROPERTY(BlueprintReadOnly)
	FVector PelvisOffset;
	UPROPERTY(BlueprintReadOnly)
	float PelvisAlpha;
	// LayerValues
	float Enable_AimOffset;
	UPROPERTY(BlueprintReadOnly)
	float BasePose_N;
	UPROPERTY(BlueprintReadOnly)
	float BasePose_CLF;
	float Enable_HandIK_L;
	float Enable_HandIK_R;
};
