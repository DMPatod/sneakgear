#include "Characters/Player/ThirdPersonPlayerAnimInstance.h"

#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

static float CalculateDirectionDegrees(const FVector& Velocity2D, const FTransform& ActorTransform)
{
	auto LocalVel = ActorTransform.InverseTransformVectorNoScale(Velocity2D);
	return FMath::RadiansToDegrees(FMath::Atan2(LocalVel.Y, LocalVel.X));
}

void UThirdPersonPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPawn = TryGetPawnOwner();
	ThirdPersonCharacter = Cast<AThirdPersonPlayerCharacter>(TryGetPawnOwner());
	MovementComponent = ThirdPersonCharacter ? ThirdPersonCharacter->GetCharacterMovement() : nullptr;
}

void UThirdPersonPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ThirdPersonCharacter || !MovementComponent)
	{
		NativeInitializeAnimation();
		if (!ThirdPersonCharacter || !MovementComponent)
		{
			return;
		}
	}

	auto Vel = ThirdPersonCharacter->GetVelocity();
	auto Vel2D = FVector(Vel.X, Vel.Y, 0.f);

	Speed = Vel2D.Size();
	bIsInAir = MovementComponent->IsFalling();
	bIsCrouching = ThirdPersonCharacter->bIsCrouched;

	bIsRunning = Speed > 300.f;

	if (Vel2D.SizeSquared() > 1.f)
	{
		Direction = CalculateDirectionDegrees(Vel2D, ThirdPersonCharacter->GetActorTransform());
	}
	else
	{
		Direction = 0.f;
	}
}
