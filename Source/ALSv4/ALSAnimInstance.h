// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSBaseCharacter.h"
#include "Animation/AnimInstance.h"
#include "ALSAnimInstance.generated.h"

UCLASS()
class ALSV4_API UALSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

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
	void SetFootLocking(FName EnableFootIKCurve, FName FootLockCurve, FName IKFootBone, float& CurrentFootLockAlpha,
	                    FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation);
	void SetFootLockOffset(FVector& LocalLocation, FRotator& LocalRotation);
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
	UCurveVector* YawOffset_FB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveVector* YawOffset_LR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Curve")
	UCurveFloat* LandPredictionCurve;

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
	float DeltaTime;
	// CharacterInfo
	FVector Velocity;
	FVector Acceleration;
	FVector MovementInput;
	bool bIsMoving;
	bool bHasMovementInput;
	float Speed;
	float MovementInputAmount;
	FRotator AimingRotation;
	float AimYawRate;
	EALSMovementState MovementState;
	EALSMovementState PrevMovementState;
	EALSMovementAction MovementAction;
	EALSRotationMode RotationMode;
	EALSGait Gait;
	EALSStance Stance;
	EALSViewMode ViewMode;
	EALSOverlayState OverlayState;
	// MovementValues
	bool bShouldMove;
	FALSVelocityBlend VelocityBlend;
	float DiagonalScaleAmount;
	FVector RelativeAccelerationAmount;
	FALSLeanAmount LeanAmount;
	float WalkRunBlend;
	float StrideBlend;
	float StandingPlayRate;
	float CrouchingPlayRate;
	EALSMovementDirection MovementDirection;
	// RotationValues
	float FYaw;
	float BYaw;
	float LYaw;
	float RYaw;
	bool Rotate_L;
	bool Rotate_R;
	float RotateRate;
	float ElapsedDelayTime;
	float RotationScale;
	// InAirValues
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
	float FootLock_L_Alpha;
	float FootLock_R_Alpha;
	FVector FootLock_L_Location;
	FVector FootLock_R_Location;
	FRotator FootLock_L_Rotation;
	FRotator FootLock_R_Rotation;
	FVector FootOffset_L_Location;
	FVector FootOffset_R_Location;
	FRotator FootOffset_L_Rotation;
	FRotator FootOffset_R_Rotation;
	FVector PelvisOffset;
	float PelvisAlpha;
	// LayerValues
	int OverlayOverrideState;
	float Enable_AimOffset;
	float BasePose_N;
	float BasePose_CLF;
	float Arm_L;
	float Arm_L_Add;
	float Arm_L_LS;
	float Arm_L_MS;
	float Arm_R;
	float Arm_R_Add;
	float Arm_R_LS;
	float Arm_R_MS;
	float Hand_L;
	float Hand_R;
	float Legs;
	float Legs_Add;
	float Pelvis;
	float Pelvis_Add;
	float Spine;
	float Spine_Add;
	float Head;
	float Head_Add;
	float Enable_HandIK_L;
	float Enable_HandIK_R;
};
