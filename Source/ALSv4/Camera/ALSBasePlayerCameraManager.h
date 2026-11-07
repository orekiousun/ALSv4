// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ALSBasePlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ALSV4_API AALSBasePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AALSBasePlayerCameraManager();

	virtual void OnPossess(APawn* InPawn);
	float GetCameraBehaviorCurveValue(const FName CurveName) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> CameraBehavior;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APawn> ControlledPawn;
};
