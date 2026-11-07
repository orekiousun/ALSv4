// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSv4/ALSTypes.h"
#include "Animation/AnimInstance.h"
#include "ALSBaseCameraAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ALSV4_API UALSBaseCameraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetControlledPawn(APawn* InPawn);
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APawn> ControlledPawn;

	UPROPERTY(BlueprintReadOnly)
	EALSMovementState MovementState;
	UPROPERTY(BlueprintReadOnly)
	EALSMovementAction MovementAction;
	UPROPERTY(BlueprintReadOnly)
	EALSRotationMode RotationMode;
	UPROPERTY(BlueprintReadOnly)
	EALSGait Gait;
	UPROPERTY(BlueprintReadOnly)
	EALSStance Stance;
	UPROPERTY(BlueprintReadOnly)
	EALSViewMode ViewMode;
};
