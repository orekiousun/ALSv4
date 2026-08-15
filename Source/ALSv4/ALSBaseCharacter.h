// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSCharacterInterface.h"
#include "ALSTypes.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ALSBaseCharacter.generated.h"

UCLASS()
class ALSV4_API AALSBaseCharacter : public ACharacter, public IALSCharacterInterface
{
	GENERATED_BODY()

public:
	AALSBaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnJumped_Implementation() override;
	// 等效于OnLanded，只是OnLanded是BlueprintImplementableEvent，这里为了兼容不同版本的引擎，就用Landed了
	virtual void Landed(const FHitResult& Hit) override;

	virtual void SetMovementState(EALSMovementState NewMovementState) override;
	virtual void SetMovementAction(EALSMovementAction NewMovementAction) override;
	virtual void SetRotationMode(EALSRotationMode NewRotationMode) override;
	virtual void SetGait(EALSGait NewGait) override;
	virtual void SetViewMode(EALSViewMode NewViewMode) override;
	virtual void SetOverlayState(EALSOverlayState NewOverlayState) override;

protected:
	void OnBeginPlay();

	// Tick相关
	void SetEssentialValues();
	// 计算并更新步态，根据步态调用UpdateDynamicMovementSettings
	void UpdateCharacterMovement();
	// 更新CharacterMovementComponent速度、加速度等参数
	void UpdateDynamicMovementSettings(EALSGait InGait);
	// 更新在地面上的旋转
	void UpdateGroundedRotation();
	// 更新在空中的旋转
	void UpdateInAirRotation();
	void SmoothCharacterRotation(FRotator InRotation, float TargetInterpSpeed, float ActorInterpSpeed);
	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed);
	void CacheValues();
	void DrawDebugShapes();
	void RagdollUpdate();

	// 状态改变
	void OnMovementStateChanged(EALSMovementState NewMovementState);
	void OnMovementActionChanged(EALSMovementAction NewMovementAction);
	void OnRotationModeChanged(EALSRotationMode NewRotationMode);
	void OnGaitChanged(EALSGait NewGait);
	void OnStanceChanged(EALSStance NewStance);
	void OnViewModeChanged(EALSViewMode NewViewMode);
	void OnOverlayStateChanged(EALSOverlayState NewOverlayState);

	// 输入
	void OnMoveForwardBackwardTriggered(const FInputActionValue& Value);
	void OnMoveLeftRightTriggered(const FInputActionValue& Value);
	void OnLookUpDownTriggered(const FInputActionValue& Value);
	void OnLookLeftRightTriggered(const FInputActionValue& Value);
	void OnJumpTriggered(const FInputActionValue& Value);
	void OnJumpCompleted(const FInputActionValue& Value);
	// 切换站立和下蹲
	void OnStanceTriggered(const FInputActionValue& Value);
	// 切换步态
	void OnWalkTriggered(const FInputActionValue& Value);
	void OnWalkCompleted(const FInputActionValue& Value);
	void OnSprintTriggered(const FInputActionValue& Value);
	void OnSprintCompleted(const FInputActionValue& Value);
	// 切换旋转模式
	void OnSelectRotationMode1Triggered(const FInputActionValue& Value);
	void OnSelectRotationMode2Triggered(const FInputActionValue& Value);
	void OnAimTriggered(const FInputActionValue& Value);
	void OnAimCompleted(const FInputActionValue& Value);
	void OnCameraTriggered(const FInputActionValue& Value);
	void OnRagdollTriggered(const FInputActionValue& Value);

	// 翻滚
	void Roll();
	// 下落时攀爬
	void BreakFall();

	// 布娃娃
	void RagdollStart();
	void RagdollEnd();

	// 攀爬
	bool MantleCheck(const FALSMantleTraceSettings& MantleTraceSettings, EDrawDebugTrace::Type DebugType);
	void MantleStart(float MantleHeight, FALSComponentAndTransform MantleLedgeWS, EALSMantleType MantleType);
	void MantleEnd();
	void MantleUpdate();
	bool CapsuleHasRoomCheck(UCapsuleComponent* Capsule, FVector TargetLocation, float HeightOffset, float RadiusOffset,
	                         EDrawDebugTrace::Type DebugType);
	FALSMantleAsset GetMantleAsset(EALSMantleType MantleType);

	// Utils
	float GetAnimCurveValue(FName CurveName) const;
	FVector GetPlayerMovementInput();
	EALSGait GetAllowedGait() const;
	EALSGait GetActualGait(EALSGait InGait) const;
	bool CanSprint() const;
	FALSMovementSettings GetTargetMovementSettings() const;
	float GetMappedSpeed() const;
	bool CanUpdateMovingRotation() const;
	float CalcGroundedRotationRate() const;
	// 获取胶囊体脚部位置
	FVector GetCapsuleBaseLocation(float ZOffset);
	// 输入胶囊体脚部位置，计算胶囊体中心位置
	FVector GetCapsuleLocationFormBase(FVector BaseLocation, float ZOffset);
	EDrawDebugTrace::Type GetTraceDebugType(EDrawDebugTrace::Type TraceType) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	FALSMovementStateSettings MovementData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	FALSMantleTraceSettings GroundTraceSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	FALSMantleTraceSettings FallingTraceSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	FALSInputActions InputActions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	FALSMantleSettings MantleSettings;

	UPROPERTY(Transient)
	TObjectPtr<UAnimInstance> MainAnimInstance;

	EALSMovementState MovementState;
	EALSMovementState PrevMovementState;
	EALSMovementAction MovementAction;
	EALSMovementAction PrevMovementAction;
	EALSRotationMode RotationMode;
	EALSRotationMode PrevRotationMode;
	EALSRotationMode DesiredRotationMode = EALSRotationMode::LookingDirection;
	EALSGait Gait;
	EALSGait AllowedGait;
	EALSGait ActualGait;
	EALSGait PrevGait;
	EALSGait DesiredGait = EALSGait::Running;
	EALSStance Stance;
	EALSStance PrevStance;
	EALSStance DesiredStance = EALSStance::Standing;
	EALSViewMode ViewMode = EALSViewMode::ThirdPerson;
	EALSViewMode PrevViewMode;
	EALSOverlayState OverlayState = EALSOverlayState::Default;
	EALSOverlayState PrevOverlayState;

	FVector Acceleration;
	FVector PrevVelocity;

	float Speed;
	bool bIsMoving;
	FRotator LastVelocityRotation;

	float MovementInputAmount;
	bool bHasMovementInput;
	FRotator LastMovementInputRotation;

	float AimYawRate;
	float PreviousAimYaw;

	FALSMovementSettings CurMovementSettings;

	FRotator TargetRotation;
	FRotator InAirRotation;
	// 标识需要在落地时翻滚
	bool bBreakFall;
	FTimerHandle BreakFallTimerHandle;
	FTimerHandle BrakingFrictionFactorTimerHandle;

	// Mantle
	FALSMantleParams MantleParams;
	FALSComponentAndTransform MantleLedgeLS;
	FTransform MantleTarget;
	FTransform MantleActualStartOffset;
	FTransform MantleAnimatedStartOffset;
};
