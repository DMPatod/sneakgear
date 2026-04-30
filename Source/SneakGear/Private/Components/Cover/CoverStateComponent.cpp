#include "Components/Cover/CoverStateComponent.h"

#include "Player/PlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Components/PlayerLocomotionComponent.h"
#include "InputActionValue.h"

UCoverStateComponent::UCoverStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCoverStateComponent::BeginPlay()
{
	Super::BeginPlay();

	CoverComponent = GetOwner() ? GetOwner()->FindComponentByClass<UCoverComponent>() : nullptr;
}

void UCoverStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !CoverComponent)
	{
		return;
	}

	APlayerCharacterBase* ThirdPersonCharacter = Cast<APlayerCharacterBase>(OwnerCharacter);
	const bool bIsProne = ThirdPersonCharacter && ThirdPersonCharacter->Stance == EStance::Prone;

	if (bIsProne && CoverState != ECoverState::None)
	{
		ExitCover(OwnerCharacter);
		return;
	}

	if (CoverState == ECoverState::None)
	{
		if (bIsProne)
		{
			return;
		}

		auto Hit = CoverComponent->FindCover();
		if (Hit.bValid)
		{
			EnterCover(OwnerCharacter, Hit);
		}
	}
	else
	{
		if (CoverState == ECoverState::Approaching)
		{
			UpdateCoverApproach(OwnerCharacter, DeltaTime);
		}
		else if (CoverState == ECoverState::Locked)
		{
			OwnerCharacter->bUseControllerRotationYaw = false;
			UpdateCoverRotation(OwnerCharacter, DeltaTime);

			if (!CoverComponent->ValidateCover(CurrentCover.Normal, 90.f))
			{
				ExitCover(OwnerCharacter);
				return;
			}
		}
	}

	const bool bVaultNow = !bIsVaulting && CanVault(OwnerCharacter);
	if (bVaultNow != bVaultPromptActive)
	{
		bVaultPromptActive = bVaultNow;
		OnVaultPromptChanged.Broadcast(bVaultPromptActive);
	}
}

bool UCoverStateComponent::HandleMoveInput(ACharacter* OwnerCharacter, const FInputActionValue& Value)
{
	if (!OwnerCharacter || CoverState != ECoverState::Locked)
	{
		return false;
	}

	if (!OwnerCharacter->Controller)
	{
		return true;
	}

	const auto Axis = Value.Get<FVector2D>();
	auto ControlRot = OwnerCharacter->Controller->GetControlRotation();
	auto YawRot = FRotator(0.f, ControlRot.Yaw, 0.f);

	auto Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	auto Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);

	OwnerCharacter->AddMovementInput(Forward, Axis.X);
	OwnerCharacter->AddMovementInput(Right, Axis.Y);

	const FVector InputDir2D = (Forward * Axis.X + Right * Axis.Y).GetSafeNormal2D();
	const FVector CoverTangent2D = CurrentCover.Tangent.GetSafeNormal2D();
	const FVector CoverNormal2D = CurrentCover.Normal.GetSafeNormal2D();
	CoverMoveAxis = FVector::DotProduct(InputDir2D, CoverTangent2D);

	if (FMath::Abs(CoverMoveAxis) > 0.1f)
	{
		CoverFacingSign = CoverMoveAxis >= 0.f ? 1.f : -1.f;
	}

	const float AwayDot = FVector::DotProduct(InputDir2D, CoverNormal2D);

	if (!InputDir2D.IsNearlyZero() && AwayDot > CoverExitBackDotThreshold)
	{
		ExitCover(OwnerCharacter);
	}

	return true;
}

bool UCoverStateComponent::TryVault(ACharacter* OwnerCharacter)
{
	if (!CanVault(OwnerCharacter))
	{
		return false;
	}

	FVector VaultDir = -CurrentCover.Normal;
	VaultDir.Z = 0.f;
	VaultDir.Normalize();
	if (VaultDir.IsNearlyZero())
	{
		return false;
	}

	ExitCover(OwnerCharacter);
	OwnerCharacter->LaunchCharacter(VaultDir * VaultLaunchForward + FVector(0.f, 0.f, VaultLaunchUp), true, true);
	bIsVaulting = true;
	return true;
}

bool UCoverStateComponent::CanVault(const ACharacter* OwnerCharacter) const
{
	FVector LandingLocation = FVector::ZeroVector;
	return FindVaultLandingLocation(OwnerCharacter, LandingLocation);
}

float UCoverStateComponent::GetCurrentVaultMaxObstacleHeight(const ACharacter* OwnerCharacter) const
{
	const APlayerCharacterBase* ThirdPersonCharacter = Cast<APlayerCharacterBase>(OwnerCharacter);
	return GetVaultMaxObstacleHeightForStance(ThirdPersonCharacter);
}

void UCoverStateComponent::HandleLanded(ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter)
	{
		bIsVaulting = false;
		return;
	}

	APlayerCharacterBase* ThirdPersonCharacter = Cast<APlayerCharacterBase>(OwnerCharacter);
	if (bIsVaulting && ThirdPersonCharacter && ThirdPersonCharacter->Stance == EStance::Crouching)
	{
		ThirdPersonCharacter->SetStance(EStance::Standing);
	}

	bIsVaulting = false;
}

bool UCoverStateComponent::FindVaultLandingLocation(const ACharacter* OwnerCharacter, FVector& OutLandingLocation) const
{
	OutLandingLocation = FVector::ZeroVector;

	if (!OwnerCharacter || CoverState != ECoverState::Locked || !CurrentCover.bValid)
	{
		return false;
	}

	const APlayerCharacterBase* ThirdPersonCharacter = Cast<APlayerCharacterBase>(OwnerCharacter);
	if (!ThirdPersonCharacter)
	{
		return false;
	}

	const float MaxVaultObstacleHeight = GetVaultMaxObstacleHeightForStance(ThirdPersonCharacter);
	if (MaxVaultObstacleHeight <= 0.f || CurrentCover.ObstacleHeight <= 0.f || CurrentCover.ObstacleHeight > MaxVaultObstacleHeight)
	{
		return false;
	}

	const UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement();
	UWorld* World = GetWorld();
	const UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
	if (!Move || !World || !Capsule)
	{
		return false;
	}

	if (!Move->IsMovingOnGround())
	{
		return false;
	}

	FVector VaultDir = -CurrentCover.Normal;
	VaultDir.Z = 0.f;
	VaultDir.Normalize();
	if (VaultDir.IsNearlyZero())
	{
		return false;
	}

	const FVector LandingProbeXY = CurrentCover.ImpactPoint + VaultDir * VaultForwardDistance;
	const FVector LandingProbeStart = LandingProbeXY + FVector(0.f, 0.f, VaultLandingProbeHeight);
	const FVector LandingProbeEnd = LandingProbeXY - FVector(0.f, 0.f, VaultLandingProbeDepth);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(VaultLandingProbe), false);
	Params.AddIgnoredActor(OwnerCharacter);

	FHitResult LandingHit;
	const bool bFoundLanding = World->LineTraceSingleByChannel(LandingHit, LandingProbeStart, LandingProbeEnd,
		ECC_Visibility, Params);
	if (!bFoundLanding)
	{
		return false;
	}

	const float Radius = Capsule->GetScaledCapsuleRadius();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	OutLandingLocation = LandingHit.ImpactPoint + FVector(0.f, 0.f, HalfHeight + 2.f);

	const bool bLandingBlocked = World->OverlapBlockingTestByChannel(
		OutLandingLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Radius, HalfHeight),
		Params
	);
	if (bLandingBlocked)
	{
		OutLandingLocation = FVector::ZeroVector;
		return false;
	}

	return true;
}

float UCoverStateComponent::GetVaultMaxObstacleHeightForStance(const APlayerCharacterBase* PlayerCharacter) const
{
	if (!PlayerCharacter)
	{
		return 0.f;
	}

	switch (PlayerCharacter->Stance)
	{
	case EStance::Standing:
		return StandingVaultMaxObstacleHeight;
	case EStance::Crouching:
		return CrouchingVaultMaxObstacleHeight;
	case EStance::Prone:
	default:
		return 0.f;
	}
}

void UCoverStateComponent::EnterCover(ACharacter* OwnerCharacter, const FCoverHit& Hit)
{
	if (!OwnerCharacter || CoverState != ECoverState::None)
	{
		return;
	}

	CurrentCover = Hit;
	CoverState = ECoverState::Approaching;

	auto* Move = OwnerCharacter->GetCharacterMovement();
	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(CurrentCover.Normal);
	Move->SetPlaneConstraintOrigin(CurrentCover.SnapLocation);
	Move->bConstrainToPlane = true;

	OwnerCharacter->bUseControllerRotationYaw = false;

	if (auto* Locomotion = OwnerCharacter->FindComponentByClass<UPlayerLocomotionComponent>())
	{
		Locomotion->RefreshMovementState();
	}
}

void UCoverStateComponent::LockCover(ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter)
	{
		return;
	}

	CurrentCover.Tangent *= -1.f;
	CoverState = ECoverState::Locked;

	auto* Move = OwnerCharacter->GetCharacterMovement();
	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(CurrentCover.Normal);
	Move->SetPlaneConstraintOrigin(CurrentCover.SnapLocation);
	Move->bConstrainToPlane = true;

	OwnerCharacter->bUseControllerRotationYaw = false;

	if (auto* Locomotion = OwnerCharacter->FindComponentByClass<UPlayerLocomotionComponent>())
	{
		Locomotion->RefreshMovementState();
	}
}

void UCoverStateComponent::RequestExitCover()
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		ExitCover(OwnerCharacter);
	}
}

#if WITH_DEV_AUTOMATION_TESTS
void UCoverStateComponent::TestSetLockedCover(const FCoverHit& Hit)
{
	CurrentCover = Hit;
	CoverState = Hit.bValid ? ECoverState::Locked : ECoverState::None;
	CoverApproachTime = 0.f;
	CoverMoveAxis = 0.f;
	CoverFacingSign = 1.f;
}

void UCoverStateComponent::TestSetVaultMaxObstacleHeights(float InStandingMaxHeight, float InCrouchingMaxHeight)
{
	StandingVaultMaxObstacleHeight = InStandingMaxHeight;
	CrouchingVaultMaxObstacleHeight = InCrouchingMaxHeight;
}
#endif

void UCoverStateComponent::ExitCover(ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter || CoverState == ECoverState::None)
	{
		return;
	}

	CoverState = ECoverState::None;
	CoverApproachTime = 0.f;

	auto* Move = OwnerCharacter->GetCharacterMovement();
	Move->SetPlaneConstraintEnabled(false);
	Move->bConstrainToPlane = false;

	if (auto* Locomotion = OwnerCharacter->FindComponentByClass<UPlayerLocomotionComponent>())
	{
		Locomotion->RefreshMovementState();
	}
}

void UCoverStateComponent::UpdateCoverApproach(ACharacter* OwnerCharacter, float DeltaTime)
{
	if (!OwnerCharacter)
	{
		return;
	}

	CoverApproachTime += DeltaTime;

	if (CoverApproachTime > CoverApproachTimeout)
	{
		ExitCover(OwnerCharacter);
		return;
	}

	const auto CurrentLocation = OwnerCharacter->GetActorLocation();
	const auto TargetLocation = FVector(CurrentCover.SnapLocation.X, CurrentCover.SnapLocation.Y, CurrentLocation.Z);

	const auto NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, CoverAssistStrength);
	OwnerCharacter->SetActorLocation(NewLocation, true);

	const auto NewRotation = FMath::RInterpTo(OwnerCharacter->GetActorRotation(), CurrentCover.SnapRotation, DeltaTime,
	                                          CoverTurnSpeed);
	OwnerCharacter->SetActorRotation(NewRotation);

	if (FVector::Dist2D(NewLocation, TargetLocation) <= CoverLockDistance)
	{
		LockCover(OwnerCharacter);
	}
}

void UCoverStateComponent::UpdateCoverRotation(ACharacter* OwnerCharacter, float DeltaTime)
{
	if (!OwnerCharacter)
	{
		return;
	}

	auto FaceDir = CurrentCover.Tangent * CoverFacingSign;
	FaceDir.Z = 0.f;
	FaceDir.Normalize();

	const auto TargetRotation = FaceDir.Rotation();
	const auto NewRotation = FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime,
	                                          CoverFaceTurnSpeed);
	OwnerCharacter->SetActorRotation(NewRotation);
}

FVector UCoverStateComponent::GetCoverTangentAlignedToCamera(const ACharacter* OwnerCharacter) const
{
	auto Tangent = CurrentCover.Tangent;
	Tangent.Z = 0.f;
	Tangent.Normalize();

	if (!OwnerCharacter || !OwnerCharacter->Controller)
	{
		return Tangent;
	}

	FRotator YawRot(0.f, OwnerCharacter->Controller->GetControlRotation().Yaw, 0.f);
	auto CamRight = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	CamRight.Z = 0.f;
	CamRight.Normalize();

	if (FVector::DotProduct(CamRight, Tangent) < 0.f)
	{
		Tangent *= -1.f;
	}
	return Tangent;
}
