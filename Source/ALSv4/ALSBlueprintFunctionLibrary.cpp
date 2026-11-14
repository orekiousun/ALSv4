// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBlueprintFunctionLibrary.h"
#include "ALSTypes.h"
#include "Kismet/KismetMathLibrary.h"

FALSVelocityBlend UALSBlueprintFunctionLibrary::InterpolateVelocityBlend(const FALSVelocityBlend& Current,
                                                                         const FALSVelocityBlend& Target,
                                                                         float InterpSpeed, float DeltaTime)
{
	FALSVelocityBlend Ret;
	Ret.F = UKismetMathLibrary::FInterpTo(Current.F, Target.F, InterpSpeed, DeltaTime);
	Ret.B = UKismetMathLibrary::FInterpTo(Current.B, Target.B, InterpSpeed, DeltaTime);
	Ret.L = UKismetMathLibrary::FInterpTo(Current.L, Target.L, InterpSpeed, DeltaTime);
	Ret.R = UKismetMathLibrary::FInterpTo(Current.R, Target.R, InterpSpeed, DeltaTime);
	return Ret;
}

FALSLeanAmount UALSBlueprintFunctionLibrary::InterpLeanAmount(const FALSLeanAmount& Current,
                                                              const FALSLeanAmount& Target, float InterpSpeed,
                                                              float DeltaTime)
{
	FALSLeanAmount Ret;
	Ret.FB = UKismetMathLibrary::FInterpTo(Current.FB, Target.FB, InterpSpeed, DeltaTime);
	Ret.LR = UKismetMathLibrary::FInterpTo(Current.LR, Target.LR, InterpSpeed, DeltaTime);
	return Ret;
}

float UALSBlueprintFunctionLibrary::GetCurveFloatValue(const UCurveFloat* CurveFloat, const float InTime)
{
	if (CurveFloat)
	{
		return CurveFloat->GetFloatValue(InTime);
	}
	return 0.f;
}

FVector UALSBlueprintFunctionLibrary::GetCurveVectorValue(const UCurveVector* CurveVector, const float InTime)
{
	if (CurveVector)
	{
		return CurveVector->GetVectorValue(InTime);
	}
	return FVector::ZeroVector;
}

EALSMovementDirection UALSBlueprintFunctionLibrary::CalcQuadrant(EALSMovementDirection Current, float FRThreshold,
                                                                 float FLThreshold, float BRThreshold,
                                                                 float BLThreshold, float Buffer, float Angle)
{
	// 只有当"当前不在该轴的两个方向上"时才扩大判定范围，这样进入象限时用窄区间、离开时用宽区间，形成滞回
	bool bIncreaseBuffer_FB = Current != EALSMovementDirection::Forward && Current != EALSMovementDirection::Backward;
	bool bIncreaseBuffer_LR = Current != EALSMovementDirection::Left && Current != EALSMovementDirection::Right;
	if (AngleInRange(Angle, FLThreshold, FRThreshold, Buffer, bIncreaseBuffer_FB))
	{
		return EALSMovementDirection::Forward;
	}
	else if (AngleInRange(Angle, FRThreshold, BRThreshold, Buffer, bIncreaseBuffer_LR))
	{
		return EALSMovementDirection::Right;
	}
	else if (AngleInRange(Angle, BLThreshold, FLThreshold, Buffer, bIncreaseBuffer_LR))
	{
		return EALSMovementDirection::Left;
	}
	else
	{
		return EALSMovementDirection::Backward;
	}
}

bool UALSBlueprintFunctionLibrary::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer,
                                                bool bIncreaseBuffer)
{
	if (bIncreaseBuffer)
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle - Buffer, MaxAngle + Buffer);
	}
	else
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle + Buffer, MaxAngle - Buffer);
	}
}

FVector UALSBlueprintFunctionLibrary::CalcAxisIndependentLag(FVector CurLocation, FVector TargetLocation,
                                                             FRotator CameraRotation, FVector LagSpeed, float DeltaTime)
{
	FRotator CameraRotationYaw = FRotator(0.f, CameraRotation.Yaw, 0.f);
	FVector RelativeCurLocation = CameraRotationYaw.UnrotateVector(CurLocation);
	FVector RelativeTargetLocation = CameraRotationYaw.UnrotateVector(TargetLocation);
	FVector RelativeLerpLocation;
	RelativeLerpLocation.X = UKismetMathLibrary::FInterpTo(RelativeCurLocation.X, RelativeTargetLocation.X, DeltaTime,
	                                                       LagSpeed.X);
	RelativeLerpLocation.Y = UKismetMathLibrary::FInterpTo(RelativeCurLocation.Y, RelativeTargetLocation.Y, DeltaTime,
	                                                       LagSpeed.Y);
	RelativeLerpLocation.Z = UKismetMathLibrary::FInterpTo(RelativeCurLocation.Z, RelativeTargetLocation.Z, DeltaTime,
	                                                       LagSpeed.Z);

	return CameraRotationYaw.RotateVector(RelativeLerpLocation);
}
