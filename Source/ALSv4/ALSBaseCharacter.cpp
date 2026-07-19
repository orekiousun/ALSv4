// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

class UEnhancedInputLocalPlayerSubsystem;

AALSBaseCharacter::AALSBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AALSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnBeginPlay();
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
		EnhancedInputComponent->BindAction(InputActions.SprintAction, ETriggerEvent::Triggered, this,
		                                   &AALSBaseCharacter::OnSprintTriggered);
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
}

void AALSBaseCharacter::OnMoveLeftRightTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnLookUpDownTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnLookLeftRightTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnJumpTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnJumpCompleted(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnStanceTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnWalkTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnSprintTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnSelectRotationMode1Triggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnSelectRotationMode2Triggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnAimTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnAimCompleted(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnCameraTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::OnRagdollTriggered(const FInputActionValue& Value)
{
}

void AALSBaseCharacter::RagdollStart()
{
}
