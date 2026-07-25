// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

class UEnhancedInputLocalPlayerSubsystem;

#define MULTI_TAP_INPUT(WorldContextObject, DelayTime, bDoubleClick) \
	bool bDoubleClick = false; \
	static double LastClickTime = 0.f; \
	if (WorldContextObject && WorldContextObject->GetWorld()) \
	{ \
		const double ThisClickTime = WorldContextObject->GetWorld()->GetTimeSeconds(); \
		bDoubleClick = (ThisClickTime - LastClickTime) < DelayTime ? true : false; \
		LastClickTime = ThisClickTime; \
	}

AALSBaseCharacter::AALSBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AALSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnBeginPlay();
}

void AALSBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BreakFallTimerHandle);
	}
}

void AALSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AALSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InputActions.MoveForwardBackwardAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnMoveForwardBackwardTriggered);
		EnhancedInputComponent->BindAction(InputActions.MoveLeftRightAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnMoveLeftRightTriggered);
		EnhancedInputComponent->BindAction(InputActions.LookUpDownAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnLookUpDownTriggered);
		EnhancedInputComponent->BindAction(InputActions.LookLeftRightAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnLookLeftRightTriggered);
		EnhancedInputComponent->BindAction(InputActions.JumpAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnJumpTriggered);
		EnhancedInputComponent->BindAction(InputActions.JumpAction, ETriggerEvent::Completed, this,
		                                   &AALSBaseCharacter::OnJumpCompleted);
		EnhancedInputComponent->BindAction(InputActions.StanceAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnStanceTriggered);
		EnhancedInputComponent->BindAction(InputActions.WalkAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnWalkTriggered);
		EnhancedInputComponent->BindAction(InputActions.WalkAction, ETriggerEvent::Completed, this,
		                                   &AALSBaseCharacter::OnWalkCompleted);
		EnhancedInputComponent->BindAction(InputActions.SprintAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnSprintTriggered);
		EnhancedInputComponent->BindAction(InputActions.WalkAction, ETriggerEvent::Completed, this,
		                                   &AALSBaseCharacter::OnSprintCompleted);
		EnhancedInputComponent->BindAction(InputActions.SelectionRotationMode1Action, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnSelectRotationMode1Triggered);
		EnhancedInputComponent->BindAction(InputActions.SelectionRotationMode2Action, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnSelectRotationMode2Triggered);
		EnhancedInputComponent->BindAction(InputActions.AimAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnAimTriggered);
		EnhancedInputComponent->BindAction(InputActions.AimAction, ETriggerEvent::Completed, this,
		                                   &AALSBaseCharacter::OnAimCompleted);
		EnhancedInputComponent->BindAction(InputActions.CameraAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnCameraTriggered);
		EnhancedInputComponent->BindAction(InputActions.RagdollAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnRagdollTriggered);
	}
}

void AALSBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		if (MovementComp->MovementMode == MOVE_Walking || MovementComp->MovementMode == MOVE_NavWalking)
		{
			SetMovementState(EALSMovementState::Grounded);
		}
		else if (MovementComp->MovementMode == MOVE_Falling)
		{
			SetMovementState(EALSMovementState::InAir);
		}
	}
}

void AALSBaseCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OnStanceChanged(EALSStance::Crouching);
}

void AALSBaseCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OnStanceChanged(EALSStance::Standing);
}

void AALSBaseCharacter::SetMovementState(EALSMovementState NewMovementState)
{
	IALSCharacterInterface::SetMovementState(NewMovementState);
	if (NewMovementState == MovementState)
	{
		return;
	}
	OnMovementStateChanged(NewMovementState);
}

void AALSBaseCharacter::SetMovementAction(EALSMovementAction NewMovementAction)
{
	IALSCharacterInterface::SetMovementAction(NewMovementAction);
	if (NewMovementAction == MovementAction)
	{
		return;
	}
	OnMovementActionChanged(NewMovementAction);
}

void AALSBaseCharacter::SetRotationMode(EALSRotationMode NewRotationMode)
{
	IALSCharacterInterface::SetRotationMode(NewRotationMode);
	if (NewRotationMode == RotationMode)
	{
		return;
	}
	OnRotationModeChanged(NewRotationMode);
}

void AALSBaseCharacter::SetGait(EALSGait NewGait)
{
	IALSCharacterInterface::SetGait(NewGait);
	if (NewGait == Gait)
	{
		return;
	}
	OnGaitChanged(NewGait);
}

void AALSBaseCharacter::SetViewMode(EALSViewMode NewViewMode)
{
	IALSCharacterInterface::SetViewMode(NewViewMode);
	if (NewViewMode == ViewMode)
	{
		return;
	}
	OnViewModeChanged(NewViewMode);
}

void AALSBaseCharacter::SetOverlayState(EALSOverlayState NewOverlayState)
{
	IALSCharacterInterface::SetOverlayState(NewOverlayState);
	if (NewOverlayState == OverlayState)
	{
		return;
	}
	OnOverlayStateChanged(NewOverlayState);
}

void AALSBaseCharacter::OnBeginPlay()
{
	if (USkeletalMeshComponent* TempMesh = GetMesh())
	{
		TempMesh->AddTickPrerequisiteActor(this);
		MainAnimInstance = TempMesh->GetAnimInstance();
	}

	// 初始化状态值
	OnGaitChanged(DesiredGait);
	OnRotationModeChanged(DesiredRotationMode);
	OnViewModeChanged(ViewMode);
	OnOverlayStateChanged(OverlayState);
	switch (DesiredStance)
	{
	case EALSStance::Standing:
		UnCrouch();
	case EALSStance::Crouching:
		Crouch();
	}

	// 更新旋转
	FRotator ActorRotation = GetActorRotation();
	TargetRotation = ActorRotation;
	LastVelocityRotation = ActorRotation;
	LastMovementInputRotation = ActorRotation;
}

void AALSBaseCharacter::OnMovementStateChanged(EALSMovementState NewMovementState)
{
	PrevMovementState = MovementState;
	MovementState = NewMovementState;
	switch (MovementState)
	{
	case EALSMovementState::InAir:
		switch (MovementAction)
		{
		case EALSMovementAction::None:
			InAirRotation = GetActorRotation();
			if (Stance == EALSStance::Crouching)
			{
				UnCrouch();
			}
		case EALSMovementAction::Rolling:
			RagdollStart();
		}
	case EALSMovementState::Ragdoll:
		if (PrevMovementState == EALSMovementState::Mantling)
		{
			// TODO:停止MantleTimeline
		}
	}
}

void AALSBaseCharacter::OnMovementActionChanged(EALSMovementAction NewMovementAction)
{
	PrevMovementAction = MovementAction;
	MovementAction = NewMovementAction;
	if (MovementAction == EALSMovementAction::Rolling)
	{
		// 翻滚时需要蹲下
		Crouch();
	}
	if (PrevMovementAction == EALSMovementAction::Rolling)
	{
		switch (DesiredStance)
		{
		case EALSStance::Standing:
			UnCrouch();
		case EALSStance::Crouching:
			Crouch();
		}
	}
}

void AALSBaseCharacter::OnRotationModeChanged(EALSRotationMode NewRotationMode)
{
	PrevRotationMode = RotationMode;
	RotationMode = NewRotationMode;
	// 第一人称不能用VelocityDirection，需要将第一人称修正为第三人称
	if (ViewMode == EALSViewMode::FirstPerson && RotationMode == EALSRotationMode::VelocityDirection)
	{
		SetViewMode(EALSViewMode::ThirdPerson);
	}
}

void AALSBaseCharacter::OnGaitChanged(EALSGait NewGait)
{
	PrevGait = Gait;
	Gait = NewGait;
}

void AALSBaseCharacter::OnStanceChanged(EALSStance NewStance)
{
	PrevStance = Stance;
	Stance = NewStance;
}

void AALSBaseCharacter::OnViewModeChanged(EALSViewMode NewViewMode)
{
	PrevViewMode = ViewMode;
	ViewMode = NewViewMode;
	switch (ViewMode)
	{
	case EALSViewMode::ThirdPerson:
		if (RotationMode == EALSRotationMode::VelocityDirection || RotationMode == EALSRotationMode::LookingDirection)
		{
			SetRotationMode(DesiredRotationMode);
		}
	case EALSViewMode::FirstPerson:
		if (RotationMode == EALSRotationMode::VelocityDirection)
		{
			SetRotationMode(EALSRotationMode::LookingDirection);
		}
	}
}

void AALSBaseCharacter::OnOverlayStateChanged(EALSOverlayState NewOverlayState)
{
	PrevOverlayState = OverlayState;
	OverlayState = NewOverlayState;
}

void AALSBaseCharacter::OnMoveForwardBackwardTriggered(const FInputActionValue& Value)
{
	if (MovementState == EALSMovementState::Grounded || MovementState == EALSMovementState::InAir)
	{
		FRotator Rot = GetControlRotation();
		Rot.Pitch = 0.0f;
		Rot.Roll = 0.0f;
		AddMovementInput(UKismetMathLibrary::GetForwardVector(Rot), Value.GetMagnitude());
	}
}

void AALSBaseCharacter::OnMoveLeftRightTriggered(const FInputActionValue& Value)
{
	if (MovementState == EALSMovementState::Grounded || MovementState == EALSMovementState::InAir)
	{
		FRotator Rot = GetControlRotation();
		Rot.Pitch = 0.0f;
		Rot.Roll = 0.0f;
		AddMovementInput(UKismetMathLibrary::GetRightVector(Rot), Value.GetMagnitude());
	}
}

void AALSBaseCharacter::OnLookUpDownTriggered(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.GetMagnitude());
}

void AALSBaseCharacter::OnLookLeftRightTriggered(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.GetMagnitude());
}

void AALSBaseCharacter::OnJumpTriggered(const FInputActionValue& Value)
{
	if (MovementAction != EALSMovementAction::None)
	{
		return;
	}

	switch (MovementState)
	{
	case EALSMovementState::Grounded:
		// 攀爬检测不通过，则跳起
		if (HasMovementInput && !MantleCheck(GroundTraceSettings, EDrawDebugTrace::Type::ForDuration))
		{
			switch (Stance)
			{
			case EALSStance::Standing:
				Jump();
			case EALSStance::Crouching:
				UnCrouch();
			}
		}
	case EALSMovementState::InAir:
		// 如果是在空中按跳跃，直接进行攀爬检查
		MantleCheck(FallingTraceSettings, EDrawDebugTrace::Type::ForDuration);
	case EALSMovementState::Ragdoll:
		RagdollStart();
	}
}

void AALSBaseCharacter::OnJumpCompleted(const FInputActionValue& Value)
{
	StopJumping();
}

void AALSBaseCharacter::OnStanceTriggered(const FInputActionValue& Value)
{
	MULTI_TAP_INPUT(this, 0.3, bDoubleClick);
	if (!bDoubleClick)
	{
		// 单击切换站立和下蹲
		switch (MovementState)
		{
		case EALSMovementState::Grounded:
			switch (Stance)
			{
			case EALSStance::Standing:
				DesiredStance = EALSStance::Crouching;
				Crouch();
			case EALSStance::Crouching:
				DesiredStance = EALSStance::Standing;
				UnCrouch();
			}
		case EALSMovementState::InAir:
			// 在空中单击了下蹲，需要在着地时翻滚
			if (GetWorld())
			{
				bBreakFall = true;
				// 这里延迟0.4s后将bBreakFall改回来，避免在空中只要按了下蹲过了很长时间后仍然翻滚
				FTimerManager& TimerManager = GetWorld()->GetTimerManager();
				TimerManager.ClearTimer(BreakFallTimerHandle);
				TimerManager.SetTimer(
					BreakFallTimerHandle, [this]() { bBreakFall = false; }, 0.4, false);
			}
		}
	}
	else
	{
		// 双击翻滚
		Roll();
		switch (Stance)
		{
		case EALSStance::Standing:
			DesiredStance = EALSStance::Crouching;
		case EALSStance::Crouching:
			DesiredStance = EALSStance::Standing;
		}
	}
}

void AALSBaseCharacter::OnWalkTriggered(const FInputActionValue& Value)
{
	// 切换移动模式
	DesiredGait = EALSGait::Running;
}

void AALSBaseCharacter::OnWalkCompleted(const FInputActionValue& Value)
{
	DesiredGait = EALSGait::Walking;
}

void AALSBaseCharacter::OnSprintTriggered(const FInputActionValue& Value)
{
	DesiredGait = EALSGait::Sprinting;
}

void AALSBaseCharacter::OnSprintCompleted(const FInputActionValue& Value)
{
	DesiredGait = EALSGait::Walking;
}

void AALSBaseCharacter::OnSelectRotationMode1Triggered(const FInputActionValue& Value)
{
	DesiredRotationMode = EALSRotationMode::VelocityDirection;
	SetRotationMode(DesiredRotationMode);
}

void AALSBaseCharacter::OnSelectRotationMode2Triggered(const FInputActionValue& Value)
{
	DesiredRotationMode = EALSRotationMode::LookingDirection;
	SetRotationMode(DesiredRotationMode);
}

void AALSBaseCharacter::OnAimTriggered(const FInputActionValue& Value)
{
	SetRotationMode(EALSRotationMode::Aiming);
}

void AALSBaseCharacter::OnAimCompleted(const FInputActionValue& Value)
{
	switch (ViewMode)
	{
	case EALSViewMode::ThirdPerson:
		SetRotationMode(DesiredRotationMode);
	case EALSViewMode::FirstPerson:
		SetRotationMode(EALSRotationMode::LookingDirection);
	}
}

void AALSBaseCharacter::OnCameraTriggered(const FInputActionValue& Value)
{
	// TODO: 切镜头后面再处理
}

void AALSBaseCharacter::OnRagdollTriggered(const FInputActionValue& Value)
{
	switch (MovementState)
	{
	case EALSMovementState::None:
	case EALSMovementState::Grounded:
	case EALSMovementState::InAir:
	case EALSMovementState::Mantling:
		RagdollStart();
	case EALSMovementState::Ragdoll:
		RagdollEnd();
	}
}

void AALSBaseCharacter::Roll()
{
	// TODO: 待实现
}

void AALSBaseCharacter::RagdollStart()
{
	// TODO: 待实现
}

void AALSBaseCharacter::RagdollEnd()
{
	// TODO: 待实现
}

bool AALSBaseCharacter::MantleCheck(const FALSMantleTraceSettings& MantleTraceSettings, EDrawDebugTrace::Type DebugType)
{
	// TODO: 待实现
	return true;
}
