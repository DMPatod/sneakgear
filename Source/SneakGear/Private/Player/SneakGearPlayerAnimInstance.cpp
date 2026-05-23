#include "Player/SneakGearPlayerAnimInstance.h"

#include "Player/Components/PlayerWeaponComponent.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "UI/EventLogSubsystem.h"
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

	RefreshFromCharacter(DeltaSeconds);
}

void USneakGearPlayerAnimInstance::RefreshFromCharacter(float DeltaSeconds)
{
	if (!StealthCharacter)
	{
		return;
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

	if (const UPlayerWeaponComponent* WeaponComponent = StealthCharacter->GetWeaponComponent())
	{
		ActiveWeaponSlot = WeaponComponent->GetActiveWeaponSlot();
		WeaponState = WeaponComponent->GetActiveWeaponState();
		bWeaponFireRequestedRecently = WeaponComponent->WasActiveWeaponFireRequestedRecently();
		bWeaponFirePending = WeaponState == EPlayerInventoryWeaponState::FireRequested;
		bWeaponFiredRecently = WeaponComponent->WasActiveWeaponFiredRecently();
		bIsReloading = WeaponState == EPlayerInventoryWeaponState::Reloading;
	}
}

#if WITH_DEV_AUTOMATION_TESTS
void USneakGearPlayerAnimInstance::RefreshFromCharacterForTest(ASneakGearPlayerCharacter* InCharacter, float DeltaSeconds)
{
	StealthCharacter = InCharacter;
	RefreshFromCharacter(DeltaSeconds);
}
#endif

bool USneakGearPlayerAnimInstance::NotifyWeaponFireAnimation()
{
	if (!StealthCharacter)
	{
		NativeInitializeAnimation();
	}

	UPlayerWeaponComponent* WeaponComponent = StealthCharacter ? StealthCharacter->GetWeaponComponent() : nullptr;
	return WeaponComponent ? WeaponComponent->NotifyActiveWeaponFireAnimation() : false;
}

bool USneakGearPlayerAnimInstance::NotifyWeaponReloadAnimationFinished()
{
	if (!StealthCharacter)
	{
		NativeInitializeAnimation();
	}

	UPlayerWeaponComponent* WeaponComponent = StealthCharacter ? StealthCharacter->GetWeaponComponent() : nullptr;
	return WeaponComponent ? WeaponComponent->NotifyActiveWeaponReloadAnimationFinished() : false;
}
