// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(BreakFallTimerHandle);
		TimerManager.ClearTimer(ResetMovementBrakingFrictionFactorTimerHandle);
	}
}

void AALSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetEssentialValues();
	switch (MovementState)
	{
	case EALSMovementState::Grounded:
		{
			UpdateCharacterMovement();
			UpdateGroundedRotation();
		}
	case EALSMovementState::InAir:
		{
			UpdateInAirRotation();
			if (bHasMovementInput)
			{
				MantleCheck(FallingTraceSettings, EDrawDebugTrace::Type::ForOneFrame);
			}
		}
	case EALSMovementState::Ragdoll:
		RagdollUpdate();
	}
	CacheValues();
	DrawDebugShapes();
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

void AALSBaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	InAirRotation = Speed > 100.f ? LastVelocityRotation : GetActorRotation();
	if (MainAnimInstance)
	{
		// TODO：调用MainAnimInstance的Jumped
	}
}

void AALSBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (bBreakFall)
	{
		BreakFall();
	}
	else
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->BrakingFrictionFactor = bHasMovementInput ? 0.5 : 3.0;
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(ResetMovementBrakingFrictionFactorTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(ResetMovementBrakingFrictionFactorTimerHandle, [this]()
				{
					if (GetCharacterMovement())
					{
						GetCharacterMovement()->BrakingFrictionFactor = 0.f;
					}
				}, 0.5, false);
			}
		}
	}
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
	// 缓存MainAnimInstance
	if (USkeletalMeshComponent* TempMesh = GetMesh())
	{
		TempMesh->AddTickPrerequisiteActor(this);
		MainAnimInstance = TempMesh->GetAnimInstance();
	}

	// 初始化状态值，下面的几个值都有默认值
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

void AALSBaseCharacter::SetEssentialValues()
{
	// 1.设置加速度
	FVector Velocity = GetVelocity();
	float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);
	Acceleration = (Velocity - PreviousVelocity) / DeltaSeconds;

	// 2.计算Speed，bIsMoving，LastVelocityRotation
	FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.f);
	Speed = UKismetMathLibrary::VSize(HorizontalVelocity);
	bIsMoving = Speed > 1.0f;
	if (bIsMoving)
	{
		LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(Velocity);
	}

	// 3.计算MovementInputAmount、bHasMovementInput、LastMovementInputRotation
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		FVector CurAcceleration = MoveComp->GetCurrentAcceleration();
		MovementInputAmount = UKismetMathLibrary::VSize(CurAcceleration) / MoveComp->GetMaxAcceleration();
		bHasMovementInput = MovementInputAmount > 0;
		if (bHasMovementInput)
		{
			LastMovementInputRotation = UKismetMathLibrary::Conv_VectorToRotator(CurAcceleration);
		}
	}

	// 4.计算AimYawRate（用当前的Yaw和上一帧Yaw的差值除以时间，相当于计算Yaw的速度）
	AimYawRate = UKismetMathLibrary::Abs((GetControlRotation().Yaw - PreviousAimYaw) / DeltaSeconds);
}

void AALSBaseCharacter::CacheValues()
{
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
}

void AALSBaseCharacter::DrawDebugShapes()
{
}

void AALSBaseCharacter::UpdateCharacterMovement()
{
	AllowedGait = GetAllowedGait();
	ActualGait = GetActualGait(AllowedGait);
	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}
	UpdateDynamicMovementSettings(AllowedGait);
}

void AALSBaseCharacter::UpdateDynamicMovementSettings(EALSGait InGait)
{
	CurMovementSettings = GetTargetMovementSettings();
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		switch (AllowedGait)
		{
		case EALSGait::Walking:
			{
				MoveComp->MaxWalkSpeed = CurMovementSettings.WalkSpeed;
				MoveComp->MaxWalkSpeedCrouched = CurMovementSettings.WalkSpeed;
			}
		case EALSGait::Running:
			{
				MoveComp->MaxWalkSpeed = CurMovementSettings.RunSpeed;
				MoveComp->MaxWalkSpeedCrouched = CurMovementSettings.RunSpeed;
			}
		case EALSGait::Sprinting:
			{
				MoveComp->MaxWalkSpeed = CurMovementSettings.SprintSpeed;
				MoveComp->MaxWalkSpeedCrouched = CurMovementSettings.SprintSpeed;
			}
		}

		// 根据当前速度决定步态，根据步态写入MovementComp参数
		if (CurMovementSettings.MovementCurve)
		{
			FVector CurveValue = CurMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed());
			MoveComp->MaxAcceleration = CurveValue.X;
			MoveComp->BrakingDecelerationWalking = CurveValue.Y;
			MoveComp->GroundFriction = CurveValue.Z;
		}
	}
}

void AALSBaseCharacter::UpdateGroundedRotation()
{
	switch (MovementAction)
	{
	case EALSMovementAction::None:
		if (CanUpdateMovingRotation())
		{
			switch (RotationMode)
			{
			case EALSRotationMode::VelocityDirection:
				// 插值到速度方向，只保留Yaw
				SmoothCharacterRotation(FRotator(0.f, LastVelocityRotation.Yaw, 0.f), 800.f,
				                        CalcGroundedRotationRate());
			case EALSRotationMode::LookingDirection:
				switch (Gait)
				{
				case EALSGait::Walking:
				case EALSGait::Running:
					{
						// 插值到Controller方向
						float AnimYawOffset = GetAnimCurveValue(TEXT("YawOffset"));
						SmoothCharacterRotation(FRotator(0.f, GetControlRotation().Yaw + AnimYawOffset, 0.f),
						                        500.f,
						                        CalcGroundedRotationRate());
					}
				case EALSGait::Sprinting:
					// 冲刺还是插值到速度方向
					SmoothCharacterRotation(FRotator(0.f, LastVelocityRotation.Yaw, 0.f), 500.f,
					                        CalcGroundedRotationRate());
				}
			case EALSRotationMode::Aiming:
				// 插值到Controller方向
				SmoothCharacterRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f), 1000.f, 20.f);
			}
		}
		else
		{
			// 第一人称或者瞄准模式下，如控制器的Yaw和角色自身的Yaw相差过大，需要纠正在合理范围内
			if (ViewMode == EALSViewMode::FirstPerson || RotationMode == EALSRotationMode::Aiming)
			{
				LimitRotation(-100.f, 100.f, 20.f);
			}
			// 应用动画曲线中的旋转值
			float TargetYaw = UGameplayStatics::GetWorldDeltaSeconds(this) * 30 * GetAnimCurveValue(
				TEXT("RotationAmount"));
			if (UKismetMathLibrary::Abs(TargetYaw) > 0.001)
			{
				AddActorWorldRotation(FRotator(0.f, TargetYaw, 0.f));
				TargetRotation = GetActorRotation();
			}
		}
	case EALSMovementAction::Rolling:
		if (bHasMovementInput)
		{
			SmoothCharacterRotation(FRotator(0.f, LastMovementInputRotation.Yaw, 0.f), 0.f, 2.f);
		}
	}
}

void AALSBaseCharacter::UpdateInAirRotation()
{
	switch (RotationMode)
	{
	case EALSRotationMode::VelocityDirection:
	case EALSRotationMode::LookingDirection:
		SmoothCharacterRotation(FRotator(0.f, InAirRotation.Yaw, 0.f), 0.f, 5.f);
	case EALSRotationMode::Aiming:
		{
			FRotator ControlRotation = GetControlRotation();
			SmoothCharacterRotation(FRotator(0.f, ControlRotation.Yaw, 0.f), 0.f, 5.f);
			InAirRotation = GetActorRotation();
		}
	}
}

void AALSBaseCharacter::SmoothCharacterRotation(FRotator InRotation, float TargetInterpSpeed, float ActorInterpSpeed)
{
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	// 先将当前的目标旋转插值到新的目标旋转，刷新目标旋转的值
	TargetRotation = UKismetMathLibrary::RInterpTo_Constant(TargetRotation, InRotation, DeltaTime, TargetInterpSpeed);
	// 再将角色当前的旋转向目标旋转插值
	FRotator ActorRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime,
	                                                       ActorInterpSpeed);
	SetActorRotation(ActorRotation);
}

void AALSBaseCharacter::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed)
{
	FRotator ControlRotation = GetControlRotation();
	FRotator ActorRotation = GetActorRotation();
	// 计算ControlRotation和ActorRotation的差值
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);
	// 如果当前ControlRotation和ActorRotation的Yaw不在限制的范围里面，就要旋转到范围中去
	if (!UKismetMathLibrary::InRange_FloatFloat(DeltaRotation.Yaw, AimYawMin, AimYawMax))
	{
		float TargetYaw = DeltaRotation.Yaw > 0 ? ControlRotation.Yaw + AimYawMin : ControlRotation.Yaw + AimYawMax;
		SmoothCharacterRotation(FRotator(0.f, TargetYaw, 0.f), 0.f, InterpSpeed);
	}
}

void AALSBaseCharacter::RagdollUpdate()
{
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
			{
				InAirRotation = GetActorRotation();
				if (Stance == EALSStance::Crouching)
				{
					UnCrouch();
				}
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
		if (RotationMode == EALSRotationMode::VelocityDirection || RotationMode ==
			EALSRotationMode::LookingDirection)
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
		FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(Rot), Value.GetMagnitude());
	}
}

void AALSBaseCharacter::OnMoveLeftRightTriggered(const FInputActionValue& Value)
{
	if (MovementState == EALSMovementState::Grounded || MovementState == EALSMovementState::InAir)
	{
		FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
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
	// 只有在MovementAction为None时才能跳起
	if (MovementAction != EALSMovementAction::None)
	{
		return;
	}

	switch (MovementState)
	{
	case EALSMovementState::Grounded:
		// 不能攀爬，则跳起
		if (bHasMovementInput && !MantleCheck(GroundTraceSettings, EDrawDebugTrace::Type::ForDuration))
		{
			// 如果是站立就跳起，如果是蹲下就回到站立
			switch (Stance)
			{
			case EALSStance::Standing:
				Jump();
			case EALSStance::Crouching:
				UnCrouch();
			}
		}
	case EALSMovementState::InAir:
		// 如果是在空中按跳跃，进行攀爬检查
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
				{
					DesiredStance = EALSStance::Crouching;
					Crouch();
				}
			case EALSStance::Crouching:
				{
					DesiredStance = EALSStance::Standing;
					UnCrouch();
				}
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
	// TODO: 待实现，播放蒙太奇
	if (MainAnimInstance)
	{
	}
}

void AALSBaseCharacter::BreakFall()
{
	// TODO: 待实现，播放蒙太奇
	if (MainAnimInstance)
	{
	}
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

void AALSBaseCharacter::MantleStart(float MantleHeight, FTransform MantleLedgeTransform,
                                    UPrimitiveComponent* MantleLedgeComponent, EALSMantleType MantleType)
{
}

void AALSBaseCharacter::MantleEnd()
{
}

void AALSBaseCharacter::MantleUpdate()
{
}

void AALSBaseCharacter::CapsuleHasRoomCheck()
{
}

FALSMantleAsset AALSBaseCharacter::GetMantleAsset(EALSMantleType MantleType)
{
	switch (MantleType)
	{
	case EALSMantleType::HighMantle:
		return MantleSettings.HighMantleAsset;
	case EALSMantleType::LowMantle:
		return MantleSettings.LowMantleAsset;
	case EALSMantleType::FallingCatch:
		return MantleSettings.FallingCatchMantleAsset;
	}

	return FALSMantleAsset();
}

float AALSBaseCharacter::GetAnimCurveValue(FName CurveName) const
{
	if (MainAnimInstance)
	{
		return MainAnimInstance->GetCurveValue(CurveName);
	}
	return 0.f;
}

EALSGait AALSBaseCharacter::GetAllowedGait() const
{
	if (Stance == EALSStance::Standing && (RotationMode == EALSRotationMode::VelocityDirection || RotationMode ==
		EALSRotationMode::LookingDirection))
	{
		switch (DesiredGait)
		{
		case EALSGait::Walking:
			return EALSGait::Walking;
		case EALSGait::Running:
			return EALSGait::Running;
		case EALSGait::Sprinting:
			return CanSprint() ? EALSGait::Sprinting : EALSGait::Running;
		}
	}
	else if (Stance == EALSStance::Crouching && RotationMode == EALSRotationMode::Aiming)
	{
		return (DesiredGait == EALSGait::Walking || DesiredGait == EALSGait::Running)
			       ? EALSGait::Walking
			       : EALSGait::Running;
	}
	return EALSGait::Walking;
}

EALSGait AALSBaseCharacter::GetActualGait(EALSGait InGait) const
{
	float WalkSpeed = CurMovementSettings.WalkSpeed;
	float RunSpeed = CurMovementSettings.RunSpeed;
	if (Speed >= RunSpeed + 10)
	{
		return InGait == EALSGait::Sprinting ? EALSGait::Sprinting : EALSGait::Running;
	}
	else if (Speed >= WalkSpeed + 10)
	{
		return EALSGait::Running;
	}
	else
	{
		return EALSGait::Walking;
	}
}

bool AALSBaseCharacter::CanSprint() const
{
	if (!bHasMovementInput)
	{
		return false;
	}

	switch (RotationMode)
	{
	case EALSRotationMode::VelocityDirection:
		return MovementInputAmount > 0.9;
	case EALSRotationMode::LookingDirection:
		if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
		{
			FRotator VelocityRotation =
				UKismetMathLibrary::Conv_VectorToRotator(MovementComp->GetCurrentAcceleration());
			FRotator ControlRotation = GetControlRotation();
			FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRotation, ControlRotation);
			// 需满足速度方向与控制器方向的Yaw差值小于一定值才能冲刺
			return MovementInputAmount > 0.9 && UKismetMathLibrary::Abs(DeltaRotation.Yaw) < 50.f;
		}
	case EALSRotationMode::Aiming:
		return false;
	}
	return false;
}

FALSMovementSettings AALSBaseCharacter::GetTargetMovementSettings() const
{
	switch (RotationMode)
	{
	case EALSRotationMode::VelocityDirection:
		return Stance == EALSStance::Standing
			       ? MovementData.VelocityDirection.Standing
			       : MovementData.VelocityDirection.Crouching;
	case EALSRotationMode::LookingDirection:
		return Stance == EALSStance::Standing
			       ? MovementData.LookingDirection.Standing
			       : MovementData.LookingDirection.Crouching;
	case EALSRotationMode::Aiming:
		return Stance == EALSStance::Standing
			       ? MovementData.Aiming.Standing
			       : MovementData.Aiming.Crouching;
	}
	return FALSMovementSettings();
}

float AALSBaseCharacter::GetMappedSpeed() const
{
	float WalkSpeed = CurMovementSettings.WalkSpeed;
	float RunSpeed = CurMovementSettings.RunSpeed;
	float SprintSpeed = CurMovementSettings.SprintSpeed;
	if (Speed > RunSpeed)
	{
		return UKismetMathLibrary::MapRangeClamped(Speed, RunSpeed, SprintSpeed, 2, 3);
	}
	else if (Speed > WalkSpeed)
	{
		return UKismetMathLibrary::MapRangeClamped(Speed, WalkSpeed, RunSpeed, 1, 2);
	}
	else
	{
		return UKismetMathLibrary::MapRangeClamped(Speed, 0, WalkSpeed, 0, 1);
	}
}

bool AALSBaseCharacter::CanUpdateMovingRotation() const
{
	return ((bIsMoving && bHasMovementInput) || Speed > 150.f) && !HasAnyRootMotion();
}

float AALSBaseCharacter::CalcGroundedRotationRate() const
{
	if (CurMovementSettings.RotationRateCurve)
	{
		float ClampedAimYawRate = UKismetMathLibrary::MapRangeClamped(AimYawRate, 0, 300, 1, 3);
		return ClampedAimYawRate * CurMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed());
	}
	return 0.f;
}
