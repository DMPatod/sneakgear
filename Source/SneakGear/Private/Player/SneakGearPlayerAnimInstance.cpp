#include "Player/SneakGearPlayerAnimInstance.h"

#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Weapon/WeaponBase.h"

void USneakGearPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	StealthCharacter = Cast<ASneakGearPlayerCharacter>(TryGetPawnOwner());
}

void USneakGearPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!StealthCharacter)
	{
		NativeInitializeAnimation();
		if (!StealthCharacter)
		{
			return;
		}
	}

	bIsAiming = StealthCharacter->IsAiming();
	bIsRunning = Speed > 300.f;
	Stance = StealthCharacter->Stance;
	bIsProne = Stance == EStance::Prone;
	bIsCrouching = Stance == EStance::Crouching;
	bIsVaulting = StealthCharacter->IsVaulting();
	bInCover = StealthCharacter->IsInCover();
	CoverMove = StealthCharacter->GetCoverMoveAxis();
	AimPitch = FRotator::NormalizeAxis(StealthCharacter->GetBaseAimRotation().Pitch);

	if (AWeaponBase* CurrentWeapon = StealthCharacter->GetCurrentWeapon())
	{
		WeaponSetAnimationBP = CurrentWeapon->AnimationSetBP ? CurrentWeapon->AnimationSetBP : DefaultSetAnimationBP;
	}
	else
	{
		WeaponSetAnimationBP = DefaultSetAnimationBP;
	}

	if (WeaponSetAnimationBP && WeaponSetAnimationBP != LinkedWeaponSet)
	{
		LinkAnimClassLayers(WeaponSetAnimationBP);
		LinkedWeaponSet = WeaponSetAnimationBP;
	}

	if (const UPlayerInventoryComponent* ItemComponent = StealthCharacter->GetItemComponent())
	{
		ActiveWeaponSlot = ItemComponent->GetActiveWeaponSlot();
		bWeaponFiredRecently = ItemComponent->WasActiveWeaponFiredRecently();
	}
}
