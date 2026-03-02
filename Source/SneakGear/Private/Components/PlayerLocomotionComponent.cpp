#include "Components/PlayerLocomotionComponent.h"

#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

UPlayerLocomotionComponent::UPlayerLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerLocomotionComponent::Initialize(USpringArmComponent* InCameraBoom)
{
	CameraBoom = InCameraBoom;
	UpdateMovementSpeed();
}

void UPlayerLocomotionComponent::SetupMovementDefaults()
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MoveComponent = PlayerCharacter->GetCharacterMovement();
	if (!MoveComponent)
	{
		return;
	}

	MoveComponent->bOrientRotationToMovement = true;
	MoveComponent->RotationRate = FRotator(0.f, 540.f, 0.f);
	MoveComponent->JumpZVelocity = 600.f;
	MoveComponent->AirControl = 0.35f;
	MoveComponent->NavAgentProps.bCanCrouch = false;
}

void UPlayerLocomotionComponent::TickLocomotion(float DeltaSeconds)
{
	UpdateCameraSocketOffset(DeltaSeconds);
}

void UPlayerLocomotionComponent::Move(const FInputActionValue& Value)
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter || !PlayerCharacter->Controller)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	const FRotator ControlRot = PlayerCharacter->Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);

	PlayerCharacter->AddMovementInput(Forward, Axis.X);
	PlayerCharacter->AddMovementInput(Right, Axis.Y);
}

void UPlayerLocomotionComponent::SetStance(EStance NewStance)
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter || PlayerCharacter->Stance == NewStance)
	{
		return;
	}

	// Prone is not compatible with cover state; transition by exiting cover first.
	if (IsCoverActive() && NewStance == EStance::Prone)
	{
		if (UCoverStateComponent* CoverState = PlayerCharacter->FindComponentByClass<UCoverStateComponent>())
		{
			CoverState->RequestExitCover();
		}
	}

	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
	if (!Capsule)
	{
		return;
	}

	float TargetHalfHeight = StandingHalfHeight;
	switch (NewStance)
	{
	case EStance::Standing:
		TargetHalfHeight = StandingHalfHeight;
		break;
	case EStance::Crouching:
		TargetHalfHeight = CrouchHalfHeight;
		break;
	case EStance::Prone:
		TargetHalfHeight = ProneHalfHeight;
		break;
	}

	if (!CanResizeCapsuleTo(TargetHalfHeight))
	{
		return;
	}

	const float PreviousHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	Capsule->SetCapsuleHalfHeight(TargetHalfHeight, true);

	// Keep capsule base at same world height after center-based half-height resize.
	const float HeightDelta = TargetHalfHeight - PreviousHalfHeight;
	if (!FMath::IsNearlyZero(HeightDelta))
	{
		PlayerCharacter->AddActorWorldOffset(FVector(0.f, 0.f, HeightDelta), true);
	}

	PlayerCharacter->Stance = NewStance;
	UpdateMovementSpeed();
	UpdateRotationMode(PlayerCharacter->IsAiming());
}

void UPlayerLocomotionComponent::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
	UpdateMovementSpeed();
}

void UPlayerLocomotionComponent::OnStancePressed()
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	bStanceButtonDown = true;
	bStanceHoldTriggered = false;

	PlayerCharacter->GetWorldTimerManager().SetTimer(StanceHoldTimer, this,
	                                                 &UPlayerLocomotionComponent::HandleStanceHold,
	                                                 ProneHoldTime, false);
}

void UPlayerLocomotionComponent::OnStanceReleased()
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	bStanceButtonDown = false;
	const bool bWasTap = !bStanceHoldTriggered;
	PlayerCharacter->GetWorldTimerManager().ClearTimer(StanceHoldTimer);

	if (!bWasTap)
	{
		return;
	}

	if (PlayerCharacter->Stance == EStance::Prone)
	{
		SetStance(EStance::Crouching);
	}
	else if (PlayerCharacter->Stance == EStance::Crouching)
	{
		SetStance(EStance::Standing);
	}
	else
	{
		SetStance(EStance::Crouching);
	}
}

void UPlayerLocomotionComponent::UpdateRotationMode(bool bIsAiming)
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MoveComponent = PlayerCharacter->GetCharacterMovement();
	if (!MoveComponent)
	{
		return;
	}

	if (IsCoverActive())
	{
		MoveComponent->bOrientRotationToMovement = false;
		return;
	}

	MoveComponent->bOrientRotationToMovement = !bIsAiming && PlayerCharacter->Stance != EStance::Prone;
}

float UPlayerLocomotionComponent::GetMaxSpeed() const
{
	return WalkSpeed * SprintSpeedMultiplier;
}

void UPlayerLocomotionComponent::HandleStanceHold()
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter || !bStanceButtonDown || bStanceHoldTriggered)
	{
		return;
	}

	bStanceHoldTriggered = true;

	if (PlayerCharacter->Stance == EStance::Prone)
	{
		SetStance(EStance::Standing);
	}
	else
	{
		SetStance(EStance::Prone);
	}
}

void UPlayerLocomotionComponent::UpdateCameraSocketOffset(float DeltaSeconds)
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter || !CameraBoom)
	{
		return;
	}

	FVector TargetOffset(0.f, 50.f, 65.f);
	if (PlayerCharacter->Stance == EStance::Crouching)
	{
		TargetOffset.Z = 45.f;
	}
	else if (PlayerCharacter->Stance == EStance::Prone)
	{
		TargetOffset.Z = 25.f;
	}

	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, DeltaSeconds, 10.f);
}

void UPlayerLocomotionComponent::UpdateMovementSpeed()
{
	AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	UCharacterMovementComponent* Move = PlayerCharacter->GetCharacterMovement();
	if (!Move)
	{
		return;
	}

	const float BaseSpeed = GetStanceBaseSpeed();
	float TargetSpeed = bIsSprinting ? BaseSpeed * SprintSpeedMultiplier : BaseSpeed;

	if (const UCoverStateComponent* CoverState = PlayerCharacter->FindComponentByClass<UCoverStateComponent>())
	{
		if (CoverState->IsCoverActive())
		{
			TargetSpeed = FMath::Min(TargetSpeed, CoverState->GetCoverMaxSpeed());
		}
	}

	Move->MaxWalkSpeed = TargetSpeed;
}

float UPlayerLocomotionComponent::GetStanceBaseSpeed() const
{
	const AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return WalkSpeed;
	}

	switch (PlayerCharacter->Stance)
	{
	case EStance::Standing:
		return WalkSpeed;
	case EStance::Crouching:
		return CrouchSpeed;
	case EStance::Prone:
		return ProneSpeed;
	}

	return WalkSpeed;
}

void UPlayerLocomotionComponent::RefreshMovementState()
{
	const AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	UpdateMovementSpeed();
	UpdateRotationMode(PlayerCharacter->IsAiming());
}

bool UPlayerLocomotionComponent::IsCoverActive() const
{
	const AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return false;
	}

	const UCoverStateComponent* CoverState = PlayerCharacter->FindComponentByClass<UCoverStateComponent>();
	return CoverState && CoverState->IsCoverActive();
}

bool UPlayerLocomotionComponent::CanResizeCapsuleTo(float TargetHalfHeight) const
{
	const AThirdPersonPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return false;
	}

	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
	UWorld* World = PlayerCharacter->GetWorld();
	if (!Capsule || !World)
	{
		return false;
	}

	const float Radius = Capsule->GetUnscaledCapsuleRadius();
	const float CurrentHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	if (TargetHalfHeight <= CurrentHalfHeight)
	{
		return true;
	}

	// We keep capsule base fixed when resizing, so the capsule center rises by this amount.
	const float CenterOffsetZ = TargetHalfHeight - CurrentHalfHeight;
	const FVector TargetCenter = PlayerCharacter->GetActorLocation() + FVector(0.f, 0.f, CenterOffsetZ);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(StanceStandCheck), false, PlayerCharacter);
	return !World->OverlapBlockingTestByChannel(TargetCenter, FQuat::Identity, ECC_Pawn,
	                                            FCollisionShape::MakeCapsule(Radius, TargetHalfHeight), Params);
}

AThirdPersonPlayerCharacter* UPlayerLocomotionComponent::GetPlayerCharacter() const
{
	return Cast<AThirdPersonPlayerCharacter>(GetOwner());
}
