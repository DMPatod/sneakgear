#include "Guards/GuardCharacterAnimInstance.h"

#include "Guards/GuardCharacter.h"

void UGuardCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	GuardCharacter = Cast<AGuardCharacter>(TryGetPawnOwner());
}

void UGuardCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!GuardCharacter)
	{
		NativeInitializeAnimation();
		if (!GuardCharacter)
		{
			return;
		}
	}

	Awareness = GuardCharacter->GetAwareness();
	bHasLineOfSight = GuardCharacter->HasLineOfSight();
	AwarenessState = GuardCharacter->GetAwarenessState();
	bIsSuspicious = AwarenessState == EGuardAwarenessState::Suspicious;
	bIsAlerted = AwarenessState == EGuardAwarenessState::Alerted;
}
