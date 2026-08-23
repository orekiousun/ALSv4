// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Curves/CurveVector.h"
#include "ALSTypes.generated.h"

// 步态
UENUM(BlueprintType)
enum class EALSGait: uint8
{
	Walking = 0,
	Running = 1,
	Sprinting = 2,
};

// 姿势
UENUM(BlueprintType)
enum class EALSStance: uint8
{
	Standing = 0,
	Crouching = 1,
};

// 旋转模式
UENUM(BlueprintType)
enum class EALSRotationMode: uint8
{
	VelocityDirection = 0,
	LookingDirection = 1,
	Aiming = 2,
};

// 观察模式
UENUM(BlueprintType)
enum class EALSViewMode: uint8
{
	ThirdPerson = 0,
	FirstPerson = 1,
};

// 移动行为
UENUM(BlueprintType)
enum class EALSMovementAction: uint8
{
	None = 0,
	LowMantle = 1,
	HighMantle = 2,
	Rolling = 3,
	GettingUp = 4,
};

// 移动状态
UENUM(BlueprintType)
enum class EALSMovementState: uint8
{
	None = 0,
	Grounded = 1,
	InAir = 2,
	Mantling = 3,
	Ragdoll = 4,
};

UENUM(BlueprintType)
enum class EALSOverlayState: uint8
{
	Default = 0,
	Masculine = 1,
	Feminine = 2,
	Injured = 3,
	HandsTied = 4,
	Rifle = 5,
	Pistol1H = 6,
	Pistol2H = 6,
	Bow = 7,
	Torch = 8,
	Binoculars = 9,
	Box = 10,
	Barrel = 11,
};

UENUM(BlueprintType)
enum class EALSMantleType: uint8
{
	HighMantle = 0,
	LowMantle = 1,
	FallingCatch = 2,
};

UENUM()
enum class EALSMovementDirection
{
	Forward = 0,
	Right = 1,
	Left = 2,
	Backward = 3,
};

USTRUCT(BlueprintType)
struct FALSComponentAndTransform
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform Transform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPrimitiveComponent* Component;
};

USTRUCT(BlueprintType)
struct FALSMovementSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RunSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SprintSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* MovementCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* RotationRateCurve;
};

USTRUCT(BlueprintType)
struct FALSMovementStanceSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMovementSettings Standing;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FALSMovementStateSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMovementStanceSettings VelocityDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMovementStanceSettings LookingDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMovementStanceSettings Aiming;
};

USTRUCT(BlueprintType)
struct FALSInputActions
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* MoveForwardBackwardAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* MoveLeftRightAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* LookUpDownAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* LookLeftRightAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* StanceAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* WalkAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* SelectionRotationMode1Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* SelectionRotationMode2Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* AimAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* CameraAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* RagdollAction;
};

USTRUCT(BlueprintType)
struct FALSMantleTraceSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxLedgeHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinLedgeHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReachDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ForwardTraceRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DownwardTraceRadius;
};

USTRUCT(BlueprintType)
struct FALSMantleAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* AnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* PositionCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector StartingOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LowHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LowPlayRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LowStartPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HighHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HighPlayRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HighStartPosition;
};

USTRUCT(BlueprintType)
struct FALSMantleSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMantleAsset HighMantleAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMantleAsset LowMantleAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSMantleAsset FallingCatchMantleAsset;
};

USTRUCT(BlueprintType)
struct FALSMantleParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* AnimMontage;
	UCurveVector* Curve;
	float StartingPosition;
	float PlayRate;
	FVector StartingOffset;
};

USTRUCT(BlueprintType)
struct FALSVelocityBlend
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float F;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float B;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float R;
};

USTRUCT(BlueprintType)
struct FALSLeanAmount
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FB;
};

USTRUCT(BlueprintType)
struct FALSAnimatedSpeed
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimatedWalkSpeed = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimatedRunSpeed = 350.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimatedSprintSpeed = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimatedCrouchSpeed = 150.f;
};

USTRUCT(BlueprintType)
struct FALSRotateInPlaceSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotateMinThreshold = -50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotateMaxThreshold = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimYawRateMinRange = 90.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimYawRateMaxRange = 270.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinPlayRate = 1.15f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxPlayRate = 3.f;
};

USTRUCT(BlueprintType)
struct FALSTurnInPlaceAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimateAngle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SlotName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bScaleTurnAngle;
};

USTRUCT(BlueprintType)
struct FALSTurnInPlaceSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TurnCheckMinAngle = 45.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TurnCheckMaxAngle = 180.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Turn180Threshold = 130.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimYawRateLimit = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinAngleDelay = 0.75f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxAngleDelay = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset N_TurnIP_L90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset N_TurnIP_R90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset N_TurnIP_L180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset N_TurnIP_R180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset CLF_TurnIP_L90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset CLF_TurnIP_R90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset CLF_TurnIP_L180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FALSTurnInPlaceAsset CLF_TurnIP_R180;
};
