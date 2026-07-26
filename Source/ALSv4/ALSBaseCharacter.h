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
	// 等效于OnLanded，只是OnLanded是BlueprintImplementableEvent，这里为了兼容不同的引擎，就用Landed了
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
	void CacheValues();
	void DrawDebugShapes();
	void UpdateCharacterMovement();
	void UpdateDynamicMovementSettings(EALSGait InGait);
	void UpdateGroundedRotation();
	void UpdateInAirRotation();
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
	void OnStanceTriggered(const FInputActionValue& Value);
	void OnWalkTriggered(const FInputActionValue& Value);
	void OnWalkCompleted(const FInputActionValue& Value);
	void OnSprintTriggered(const FInputActionValue& Value);
	void OnSprintCompleted(const FInputActionValue& Value);
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

	EALSGait GetAllowedGait();
	EALSGait GetActualGait(EALSGait InGait);
	bool CanSprint();
	FALSMovementSettings GetTargetMovementSettings() const;
	float GetMappedSpeed() const;
	bool CanUpdateMovingORotation() const;
	float CalcGroundedRotationRate() const;

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
	FVector PreviousVelocity;

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
	bool bBreakFall;
	FTimerHandle BreakFallTimerHandle;
	FTimerHandle ResetMovementBrakingFrictionFactorTimerHandle;
};
