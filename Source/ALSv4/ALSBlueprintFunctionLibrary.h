// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ALSBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ALSV4_API UALSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FALSVelocityBlend InterpolateVelocityBlend(const FALSVelocityBlend& Current, const FALSVelocityBlend& Target,
	                                                  float InterpSpeed,
	                                                  float DeltaTime);
	UFUNCTION(BlueprintCallable)
	static FALSLeanAmount InterpLeanAmount(const FALSLeanAmount& Current, const FALSLeanAmount& Target,
	                                       float InterpSpeed, float DeltaTime);

	UFUNCTION(BlueprintCallable)
	static float GetCurveFloatValue(const UCurveFloat* CurveFloat, const float InTime);

	UFUNCTION(BlueprintCallable)
	static FVector GetCurveVectorValue(const UCurveVector* CurveVector, const float InTime);

	UFUNCTION(BlueprintCallable)
	static EALSMovementDirection CalcQuadrant(EALSMovementDirection Current, float FRThreshold, float FLThreshold,
	                                          float BRThreshold, float BLThreshold, float Buffer, float Angle);

	UFUNCTION(BlueprintCallable)
	static bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer);

};
