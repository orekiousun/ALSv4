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
	void UpdateFootIK();
	void UpdateMovementValues();
	void UpdateRotationValues();
	void UpdateInAirValues();
	void UpdateRagdollValues();
	void RotateInPlaceCheck();
	void TurnInPlaceCheck();
	void DynamicTransitionCheck();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	float SmoothedAimingRotationInterpSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	float InputYawOffsetTimeInterpSpeed = 8.f;

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
};
