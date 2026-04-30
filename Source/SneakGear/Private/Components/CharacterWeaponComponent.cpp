#include "Components/CharacterWeaponComponent.h"

#include "GameFramework/Character.h"
#include "Misc/DataValidation.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Weapon/WeaponBase.h"

namespace
{
bool UsesInventoryDrivenWeapons(const UCharacterWeaponComponent* WeaponComponent)
{
	if (!WeaponComponent)
	{
		return false;
	}

	if (const AActor* OwnerActor = WeaponComponent->GetOwner())
	{
		return OwnerActor->IsA<ASneakGearPlayerCharacter>();
	}

	return WeaponComponent->GetTypedOuter<ASneakGearPlayerCharacter>() != nullptr;
}
}

UCharacterWeaponComponent::UCharacterWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!StartedWeaponClass)
	{
		if (!UsesInventoryDrivenWeapons(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("CharacterWeaponComponent '%s' has no StartedWeaponClass configured."), *GetName());
		}
		return;
	}

	auto* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	auto* World = GetWorld();
	if (!World)
	{
		return;
	}

	CurrentWeapon = World->SpawnActor<AWeaponBase>(StartedWeaponClass);
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterWeaponComponent '%s' failed to spawn weapon of class '%s'."),
			*GetName(), *GetNameSafe(StartedWeaponClass));
		return;
	}

	CurrentWeapon->SetOwner(OwnerCharacter);
	AttachWeaponToSocket(HolsterSocketName);
	CurrentWeapon->OnWeaponFireRequestedEvent().AddUObject(this, &UCharacterWeaponComponent::HandleWeaponFireRequested);
	CurrentWeapon->OnWeaponFiredEvent().AddUObject(this, &UCharacterWeaponComponent::HandleWeaponFired);
	CurrentWeapon->OnWeaponReloadedEvent().AddUObject(this, &UCharacterWeaponComponent::HandleWeaponReloaded);
	
	InClip = FMath::Max(GetClipSize(), 0);
}

void UCharacterWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->OnWeaponFireRequestedEvent().RemoveAll(this);
		CurrentWeapon->OnWeaponFiredEvent().RemoveAll(this);
		CurrentWeapon->OnWeaponReloadedEvent().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

EDataValidationResult UCharacterWeaponComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!StartedWeaponClass)
	{
		if (!UsesInventoryDrivenWeapons(this))
		{
			Context.AddWarning(FText::FromString(TEXT("StartedWeaponClass is not configured.")));
			if (Result == EDataValidationResult::NotValidated)
			{
				Result = EDataValidationResult::Valid;
			}
		}
	}

	if ((StartedWeaponClass != nullptr) && (HandSocketName.IsNone() || HolsterSocketName.IsNone()))
	{
		Context.AddError(FText::FromString(TEXT("Weapon socket names must be configured when StartedWeaponClass is set.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void UCharacterWeaponComponent::StartFire()
{
	if (CurrentWeapon && !bIsReloading && InClip > 0)
	{
		CurrentWeapon->StartFire();
	}
}

void UCharacterWeaponComponent::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void UCharacterWeaponComponent::ToggleEquip()
{
	if (!CurrentWeapon)
	{
		return;
	}

	const auto TargetSocket = CurrentWeapon->GetAttachParentSocketName() == HolsterSocketName
		                          ? HandSocketName
		                          : HolsterSocketName;

	AttachWeaponToSocket(TargetSocket);
}

void UCharacterWeaponComponent::AttachWeaponToSocket(FName SocketName) const
{
	auto* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetMesh() || !CurrentWeapon)
	{
		return;
	}

	const bool bUseHolsterOffset = SocketName == HolsterSocketName;
	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	CurrentWeapon->AttachToComponent(OwnerCharacter->GetMesh(), Rules, SocketName);
	CurrentWeapon->SetActorRelativeTransform(CurrentWeapon->GetAttachmentOffset(bUseHolsterOffset));
}

void UCharacterWeaponComponent::Reload()
{
	if (!CurrentWeapon || bIsReloading || !CanReload())
	{
		return;
	}

	CurrentWeapon->StopFire();
	bIsReloading = true;
	CurrentWeapon->Reload();
}

bool UCharacterWeaponComponent::NotifyCurrentWeaponFireAnimation()
{
	return CurrentWeapon ? CurrentWeapon->NotifyFireAnimation() : false;
}

bool UCharacterWeaponComponent::NotifyCurrentWeaponReloadAnimationFinished()
{
	return CurrentWeapon ? CurrentWeapon->NotifyReloadAnimationFinished() : false;
}

bool UCharacterWeaponComponent::WasCurrentWeaponFireRequestedRecently(float WindowSeconds) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float SafeWindow = FMath::Max(WindowSeconds, 0.01f);
	return (World->GetTimeSeconds() - LastWeaponFireRequestTimestamp) <= SafeWindow;
}

bool UCharacterWeaponComponent::IsCurrentWeaponFireNotifyPending() const
{
	return CurrentWeapon ? CurrentWeapon->IsFireNotifyPending() : false;
}

#if WITH_DEV_AUTOMATION_TESTS
void UCharacterWeaponComponent::SetStartedWeaponClassForTesting(TSubclassOf<AWeaponBase> InWeaponClass)
{
	StartedWeaponClass = InWeaponClass;
}

void UCharacterWeaponComponent::InitializeWeaponForTesting()
{
	if (!CurrentWeapon)
	{
		BeginPlay();
	}
}
#endif

int32 UCharacterWeaponComponent::GetClipSize() const
{
	if (!StartedWeaponClass || !CurrentWeapon)
	{
		return -1;
	}

	return CurrentWeapon->GetClipSize();
}

bool UCharacterWeaponComponent::CanReload() const
{
	return CurrentWeapon && InClip >= 0 && InClip < GetClipSize();
}

void UCharacterWeaponComponent::FinishReload()
{
	InClip = FMath::Max(GetClipSize(), 0);
}

void UCharacterWeaponComponent::HandleWeaponFireRequested()
{
	LastWeaponFireRequestTimestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : LastWeaponFireRequestTimestamp;
}

void UCharacterWeaponComponent::HandleWeaponFired()
{
	if (InClip <= 0)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
		return;
	}

	--InClip;
	if (InClip <= 0)
	{
		InClip = 0;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

void UCharacterWeaponComponent::HandleWeaponReloaded()
{
	if (!bIsReloading)
	{
		return;
	}

	bIsReloading = false;
	FinishReload();
}
