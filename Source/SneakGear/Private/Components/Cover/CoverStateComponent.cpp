#include "Components/Cover/CoverStateComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	if (CoverState == ECoverState::None)
	{
		auto Hit = CoverComponent->FindCover();
		if (Hit.bValid)
		{
			EnterCover(OwnerCharacter, Hit);
		}
	}
	else
	{
		if (!CoverComponent->ValidateCover(CurrentCover.Normal, 90.f))
		{
			ExitCover(OwnerCharacter);
			return;
		}

		if (CoverState == ECoverState::Approaching)
		{
			UpdateCoverApproach(OwnerCharacter, DeltaTime);
		}
		else if (CoverState == ECoverState::Locked)
		{
			UpdateCoverRotation(OwnerCharacter, DeltaTime);
		}
	}

	const auto Velocity = OwnerCharacter->GetVelocity();
	FVector Velocity2D(Velocity.X, Velocity.Y, 0.f);
	if (Velocity2D.Size() > 10.f)
	{
		const FVector MoveDirection = Velocity2D.GetSafeNormal();
		const float AwayDot = FVector::DotProduct(MoveDirection, CurrentCover.Normal);
		if (AwayDot > CoverExitBackDotThreshold)
		{
			ExitCover(OwnerCharacter);
		}
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

	auto Tangent = CurrentCover.Tangent;
	Tangent.Z = 0.f;
	Tangent = Tangent.GetSafeNormal();

	CoverMoveAxis = Axis.Y;
	OwnerCharacter->AddMovementInput(Tangent, CoverMoveAxis);

	if (FMath::Abs(CoverMoveAxis) > 0.1f)
	{
		CoverFacingSign = CoverMoveAxis >= 0.f ? 1.f : -1.f;
	}

	if (Axis.X < -0.4f)
	{
		ExitCover(OwnerCharacter);
	}

	return true;
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
	Move->MaxWalkSpeed = CoverMaxSpeed;

	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(CurrentCover.Normal);
	Move->SetPlaneConstraintOrigin(CurrentCover.SnapLocation);
	Move->bConstrainToPlane = true;

	Move->bOrientRotationToMovement = false;
	OwnerCharacter->bUseControllerRotationYaw = false;

	Move->MaxWalkSpeed = 300.f;
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
	Move->MaxWalkSpeed = CoverMaxSpeed;

	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(CurrentCover.Normal);
	Move->SetPlaneConstraintOrigin(CurrentCover.SnapLocation);
	Move->bConstrainToPlane = true;

	Move->bOrientRotationToMovement = false;
	OwnerCharacter->bUseControllerRotationYaw = false;
}

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
	Move->bOrientRotationToMovement = true;
	Move->MaxWalkSpeed = 450.f;
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
	                                          CoverTurnSpeed);
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
