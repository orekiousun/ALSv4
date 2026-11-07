// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBasePlayerCameraManager.h"

#include "ALSBaseCameraAnimInstance.h"

AALSBasePlayerCameraManager::AALSBasePlayerCameraManager()
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>("CameraBehavior");
}

void AALSBasePlayerCameraManager::OnPossess(APawn* InPawn)
{
	ControlledPawn = InPawn;
	if (UALSBaseCameraAnimInstance* CameraAnimInstance = Cast<UALSBaseCameraAnimInstance>(CameraBehavior->GetAnimInstance()))
	{
		CameraAnimInstance->SetControlledPawn(InPawn);
	}
}

float AALSBasePlayerCameraManager::GetCameraBehaviorCurveValue(const FName CurveName) const
{
	if (CameraBehavior)
	{
		if (UAnimInstance* AnimInstance = CameraBehavior->GetAnimInstance())
		{
			return AnimInstance->GetCurveValue(CurveName);
		}
	}

	return 0.f;
}
