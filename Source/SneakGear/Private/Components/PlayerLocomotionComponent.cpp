#include "Components/PlayerLocomotionComponent.h"

#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
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
	MoveComponent->NavAgentProps.bCanCrouch = true;
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

	PlayerCharacter->Stance = NewStance;

	UCharacterMovementComponent* Move = PlayerCharacter->GetCharacterMovement();
	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
	if (!Move || !Capsule)
	{
		return;
	}

	switch (PlayerCharacter->Stance)
	{
	case EStance::Standing:
		PlayerCharacter->UnCrouch();
		if (CanResizeCapsuleTo(StandingHalfHeight))
		{
			Capsule->SetCapsuleHalfHeight(StandingHalfHeight, true);
		}
		break;
	case EStance::Crouching:
		PlayerCharacter->Crouch();
		if (CanResizeCapsuleTo(CrouchHalfHeight))
		{
			Capsule->SetCapsuleHalfHeight(CrouchHalfHeight, true);
		}
		break;
	case EStance::Prone:
		PlayerCharacter->UnCrouch();
		if (CanResizeCapsuleTo(ProneHalfHeight))
		{
			Capsule->SetCapsuleHalfHeight(ProneHalfHeight, true);
		}
		break;
	}

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

	PlayerCharacter->GetWorldTimerManager().SetTimer(StanceHoldTimer, this, &UPlayerLocomotionComponent::HandleStanceHold,
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
		SetStance(EStance::Prone);
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

	if (PlayerCharacter->Stance != EStance::Prone)
	{
		if (PlayerCharacter->GetCharacterMovement() && PlayerCharacter->GetCharacterMovement()->IsMovingOnGround())
		{
			SetStance(EStance::Prone);
		}
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
	Move->MaxWalkSpeed = bIsSprinting ? BaseSpeed * SprintSpeedMultiplier : BaseSpeed;
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
	const float CurrentHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float Grow = TargetHalfHeight - CurrentHalfHeight;
	if (Grow <= 0.f)
	{
		return true;
	}

	const FVector Start = PlayerCharacter->GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, Grow * 2.f);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(StanceStandCheck), false, PlayerCharacter);
	FHitResult Hit;
	return !World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn,
	                                    FCollisionShape::MakeCapsule(Radius, TargetHalfHeight), Params);
}

AThirdPersonPlayerCharacter* UPlayerLocomotionComponent::GetPlayerCharacter() const
{
	return Cast<AThirdPersonPlayerCharacter>(GetOwner());
}
