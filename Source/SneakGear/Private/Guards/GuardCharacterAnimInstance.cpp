#include "Guards/GuardCharacterAnimInstance.h"

#include "Guards/GuardCharacter.h"
#include "Guards/Components/GuardAwarenessComponent.h"

void UGuardCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	GuardCharacter = Cast<AGuardCharacter>(TryGetPawnOwner());
	AwarenessComponent = GuardCharacter ? GuardCharacter->GetAwarenessComponent() : nullptr;
}

void UGuardCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!GuardCharacter || !AwarenessComponent)
	{
		NativeInitializeAnimation();
		if (!GuardCharacter || !AwarenessComponent)
		{
			return;
		}
	}

	Awareness = AwarenessComponent->GetAwareness();
	bHasLineOfSight = AwarenessComponent->HasLineOfSight();
	AwarenessState = AwarenessComponent->GetAwarenessState();
	bIsSuspicious = AwarenessState == EGuardAwarenessState::Suspicious;
	bIsAlerted = AwarenessState == EGuardAwarenessState::Alerted;
	bWeaponFireRequestedRecently = GuardCharacter->WasWeaponFireRequestedRecently();
	bWeaponFirePending = GuardCharacter->IsWeaponFireNotifyPending();
	bIsReloading = GuardCharacter->IsWeaponReloading();
}

bool UGuardCharacterAnimInstance::NotifyWeaponFireAnimation()
{
	if (!GuardCharacter)
	{
		NativeInitializeAnimation();
	}

	return GuardCharacter ? GuardCharacter->NotifyWeaponFireAnimation() : false;
}

bool UGuardCharacterAnimInstance::NotifyWeaponReloadAnimationFinished()
{
	if (!GuardCharacter)
	{
		NativeInitializeAnimation();
	}

	return GuardCharacter ? GuardCharacter->NotifyWeaponReloadAnimationFinished() : false;
}
