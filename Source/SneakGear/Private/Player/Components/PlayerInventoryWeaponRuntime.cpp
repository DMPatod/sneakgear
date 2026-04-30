#include "Player/Components/PlayerInventoryWeaponRuntime.h"

#include "GameFramework/Character.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Weapon/WeaponBase.h"

void FPlayerInventoryWeaponRuntime::Initialize(UPlayerInventoryComponent& Inventory)
{
	Inventory.PrimaryWeaponRuntime.WeaponActor = SpawnWeapon(Inventory, Inventory.PrimaryWeaponClass);
	Inventory.SecondaryWeaponRuntime.WeaponActor = SpawnWeapon(Inventory, Inventory.SecondaryWeaponClass);
	Inventory.UnarmedWeapon = SpawnWeapon(Inventory, Inventory.UnarmedWeaponClass);
	Inventory.PrimaryWeaponRuntime.InClip = Inventory.PrimaryWeaponRuntime.WeaponActor
		? FMath::Max(Inventory.PrimaryWeaponRuntime.WeaponActor->GetClipSize(), 0)
		: -1;
	Inventory.PrimaryWeaponRuntime.State = EPlayerInventoryWeaponState::Idle;
	Inventory.PrimaryWeaponRuntime.bTriggerHeld = false;
	Inventory.SecondaryWeaponRuntime.InClip = Inventory.SecondaryWeaponRuntime.WeaponActor
		? FMath::Max(Inventory.SecondaryWeaponRuntime.WeaponActor->GetClipSize(), 0)
		: -1;
	Inventory.SecondaryWeaponRuntime.State = EPlayerInventoryWeaponState::Idle;
	Inventory.SecondaryWeaponRuntime.bTriggerHeld = false;

	if (Inventory.PrimaryWeaponRuntime.WeaponActor)
	{
		BindRuntimeWeaponDelegates(Inventory, EPlayerItemSlot::PrimaryWeapon, Inventory.PrimaryWeaponRuntime.WeaponActor);
	}

	if (Inventory.SecondaryWeaponRuntime.WeaponActor)
	{
		BindRuntimeWeaponDelegates(Inventory, EPlayerItemSlot::SecondaryWeapon, Inventory.SecondaryWeaponRuntime.WeaponActor);
	}

	SyncAttachments(Inventory);
}

void FPlayerInventoryWeaponRuntime::Shutdown(UPlayerInventoryComponent& Inventory)
{
	if (Inventory.PrimaryWeaponRuntime.WeaponActor)
	{
		ClearRuntimeWeapon(Inventory, Inventory.PrimaryWeaponRuntime);
	}

	if (Inventory.SecondaryWeaponRuntime.WeaponActor)
	{
		ClearRuntimeWeapon(Inventory, Inventory.SecondaryWeaponRuntime);
	}

	if (Inventory.UnarmedWeapon)
	{
		Inventory.UnarmedWeapon->Destroy();
		Inventory.UnarmedWeapon = nullptr;
	}
}

bool FPlayerInventoryWeaponRuntime::SetWeaponClassForSlot(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot,
                                                          TSubclassOf<AWeaponBase> WeaponClass)
{
	FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntimeMutable(Slot);
	if (!Runtime)
	{
		return false;
	}

	if (Slot == EPlayerItemSlot::PrimaryWeapon)
	{
		Inventory.PrimaryWeaponClass = WeaponClass;
	}
	else if (Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		Inventory.SecondaryWeaponClass = WeaponClass;
	}

	ClearRuntimeWeapon(Inventory, *Runtime);
	Runtime->WeaponActor = SpawnWeapon(Inventory, WeaponClass);
	Runtime->InClip = Runtime->WeaponActor ? FMath::Max(Runtime->WeaponActor->GetClipSize(), 0) : -1;
	Runtime->State = EPlayerInventoryWeaponState::Idle;
	Runtime->bTriggerHeld = false;

	if (Runtime->WeaponActor)
	{
		BindRuntimeWeaponDelegates(Inventory, Slot, Runtime->WeaponActor);
	}

	SyncAttachments(Inventory);
	Inventory.OnItemSlotUpdated.Broadcast(Slot);
	Inventory.OnInventoryStateChanged.Broadcast();
	return Runtime->WeaponActor != nullptr || !WeaponClass;
}

void FPlayerInventoryWeaponRuntime::SyncAttachments(const UPlayerInventoryComponent& Inventory)
{
	const FName PrimaryHolsterSocket = GetHolsterSocketForSlot(Inventory, EPlayerItemSlot::PrimaryWeapon);
	const FName SecondaryHolsterSocket = GetHolsterSocketForSlot(Inventory, EPlayerItemSlot::SecondaryWeapon);
	const bool bHasPrimaryItem = HasValidWeaponItem(Inventory, EPlayerItemSlot::PrimaryWeapon);
	const bool bHasSecondaryItem = HasValidWeaponItem(Inventory, EPlayerItemSlot::SecondaryWeapon);
	const bool bHasActiveSlottedWeapon = HasValidWeaponSelection(Inventory, Inventory.ActiveWeaponSlot);

	auto SyncWeaponPresentation = [](AWeaponBase* Weapon, bool bShouldShow)
	{
		if (!Weapon)
		{
			return;
		}

		Weapon->SetActorHiddenInGame(!bShouldShow);
		Weapon->SetActorEnableCollision(bShouldShow);
		if (!bShouldShow)
		{
			Weapon->StopFire();
		}
	};

	if (!Inventory.bWeaponEquipped)
	{
		SyncWeaponPresentation(Inventory.PrimaryWeaponRuntime.WeaponActor, bHasPrimaryItem);
		SyncWeaponPresentation(Inventory.SecondaryWeaponRuntime.WeaponActor, bHasSecondaryItem);
		SyncWeaponPresentation(Inventory.UnarmedWeapon, false);
		if (bHasPrimaryItem)
		{
			AttachWeapon(Inventory, Inventory.PrimaryWeaponRuntime.WeaponActor, PrimaryHolsterSocket, true);
		}
		if (bHasSecondaryItem)
		{
			AttachWeapon(Inventory, Inventory.SecondaryWeaponRuntime.WeaponActor, SecondaryHolsterSocket, true);
		}
		return;
	}

	const bool bPrimaryActive = Inventory.ActiveWeaponSlot == EPlayerItemSlot::PrimaryWeapon;
	SyncWeaponPresentation(Inventory.PrimaryWeaponRuntime.WeaponActor, bHasPrimaryItem);
	SyncWeaponPresentation(Inventory.SecondaryWeaponRuntime.WeaponActor, bHasSecondaryItem);
	SyncWeaponPresentation(Inventory.UnarmedWeapon, !bHasActiveSlottedWeapon);
	if (bHasPrimaryItem)
	{
		AttachWeapon(Inventory, Inventory.PrimaryWeaponRuntime.WeaponActor,
			bPrimaryActive ? Inventory.WeaponHandSocketName : PrimaryHolsterSocket,
			!bPrimaryActive);
	}
	if (bHasSecondaryItem)
	{
		AttachWeapon(Inventory, Inventory.SecondaryWeaponRuntime.WeaponActor,
			bPrimaryActive ? SecondaryHolsterSocket : Inventory.WeaponHandSocketName,
			bPrimaryActive);
	}
	if (!bHasActiveSlottedWeapon && Inventory.UnarmedWeapon)
	{
		AttachWeapon(Inventory, Inventory.UnarmedWeapon, Inventory.WeaponHandSocketName, false);
	}
}

bool FPlayerInventoryWeaponRuntime::SetActiveWeaponSlot(UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot,
                                                        bool bEquipInHand)
{
	if (WeaponSlot != EPlayerItemSlot::PrimaryWeapon && WeaponSlot != EPlayerItemSlot::SecondaryWeapon)
	{
		return false;
	}

	if (!HasValidWeaponSelection(Inventory, WeaponSlot))
	{
		return false;
	}

	if (FPlayerInventoryWeaponSlotRuntime* PreviousRuntime = Inventory.ResolveWeaponRuntimeMutable(Inventory.ActiveWeaponSlot))
	{
		PreviousRuntime->bTriggerHeld = false;
		if (!IsReloading(*PreviousRuntime))
		{
			SetState(Inventory, *PreviousRuntime, EPlayerInventoryWeaponState::Idle, false);
		}
	}

	Inventory.ActiveWeaponSlot = WeaponSlot;
	Inventory.bWeaponEquipped = bEquipInHand;
	SyncAttachments(Inventory);
	Inventory.OnItemSlotUpdated.Broadcast(WeaponSlot);
	Inventory.OnInventoryStateChanged.Broadcast();
	return true;
}

bool FPlayerInventoryWeaponRuntime::SetWeaponEquipped(UPlayerInventoryComponent& Inventory, bool bNewEquipped)
{
	if (!GetActiveWeapon(Inventory))
	{
		return false;
	}

	if (Inventory.bWeaponEquipped == bNewEquipped)
	{
		return true;
	}

	Inventory.bWeaponEquipped = bNewEquipped;
	if (!bNewEquipped)
	{
		if (FPlayerInventoryWeaponSlotRuntime* ActiveRuntime = Inventory.ResolveWeaponRuntimeMutable(Inventory.ActiveWeaponSlot))
		{
			ActiveRuntime->bTriggerHeld = false;
			if (!IsReloading(*ActiveRuntime))
			{
				SetState(Inventory, *ActiveRuntime, EPlayerInventoryWeaponState::Idle, false);
			}
		}
	}
	SyncAttachments(Inventory);
	Inventory.OnItemSlotUpdated.Broadcast(Inventory.ActiveWeaponSlot);
	Inventory.OnInventoryStateChanged.Broadcast();
	return true;
}

AWeaponBase* FPlayerInventoryWeaponRuntime::GetWeaponInSlot(const UPlayerInventoryComponent& Inventory,
                                                            EPlayerItemSlot WeaponSlot)
{
	if (!HasValidWeaponItem(Inventory, WeaponSlot))
	{
		return nullptr;
	}

	const FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->WeaponActor.Get() : nullptr;
}

AWeaponBase* FPlayerInventoryWeaponRuntime::GetActiveWeapon(const UPlayerInventoryComponent& Inventory)
{
	if (AWeaponBase* ActiveWeapon = GetWeaponInSlot(Inventory, Inventory.ActiveWeaponSlot))
	{
		return ActiveWeapon;
	}

	return Inventory.UnarmedWeapon;
}

EPlayerInventoryWeaponState FPlayerInventoryWeaponRuntime::GetWeaponState(const UPlayerInventoryComponent& Inventory,
                                                                          EPlayerItemSlot WeaponSlot)
{
	const FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->State : EPlayerInventoryWeaponState::Idle;
}

EPlayerInventoryWeaponState FPlayerInventoryWeaponRuntime::GetActiveWeaponState(
	const UPlayerInventoryComponent& Inventory)
{
	return GetWeaponState(Inventory, Inventory.ActiveWeaponSlot);
}

void FPlayerInventoryWeaponRuntime::StartFire(UPlayerInventoryComponent& Inventory)
{
	FPlayerInventoryWeaponSlotRuntime* ActiveRuntime = Inventory.ResolveWeaponRuntimeMutable(Inventory.ActiveWeaponSlot);
	if (ActiveRuntime)
	{
		ActiveRuntime->bTriggerHeld = true;
	}

	if (!Inventory.bWeaponEquipped)
	{
		return;
	}

	if (!HasValidWeaponSelection(Inventory, Inventory.ActiveWeaponSlot))
	{
		if (Inventory.UnarmedWeapon)
		{
			Inventory.UnarmedWeapon->StartFire();
		}
		return;
	}

	if (!ActiveRuntime || IsReloading(*ActiveRuntime) || ActiveRuntime->InClip <= 0 || !ActiveRuntime->WeaponActor)
	{
		return;
	}

	ActiveRuntime->WeaponActor->StartFire();
}

void FPlayerInventoryWeaponRuntime::StopFire(UPlayerInventoryComponent& Inventory)
{
	FPlayerInventoryWeaponSlotRuntime* ActiveRuntime = Inventory.ResolveWeaponRuntimeMutable(Inventory.ActiveWeaponSlot);
	if (ActiveRuntime)
	{
		ActiveRuntime->bTriggerHeld = false;
	}

	AWeaponBase* Weapon = GetActiveWeapon(Inventory);
	if (Weapon)
	{
		Weapon->StopFire();
	}

	if (ActiveRuntime && !IsReloading(*ActiveRuntime) && (!Weapon || !Weapon->IsFireNotifyPending()))
	{
		SetState(Inventory, *ActiveRuntime, EPlayerInventoryWeaponState::Idle);
	}
}

bool FPlayerInventoryWeaponRuntime::Reload(UPlayerInventoryComponent& Inventory)
{
	if (!HasValidWeaponSelection(Inventory, Inventory.ActiveWeaponSlot))
	{
		return false;
	}

	FPlayerInventoryWeaponSlotRuntime* ActiveRuntime = Inventory.ResolveWeaponRuntimeMutable(Inventory.ActiveWeaponSlot);
	if (!ActiveRuntime || !ActiveRuntime->WeaponActor)
	{
		return false;
	}

	if (IsReloading(*ActiveRuntime))
	{
		return false;
	}

	const int32 ClipSize = FMath::Max(ActiveRuntime->WeaponActor->GetClipSize(), 0);
	const int32 MissingAmmo = FMath::Max(ClipSize - ActiveRuntime->InClip, 0);
	if (MissingAmmo <= 0)
	{
		return false;
	}

	const EAmmoType AmmoType = GetAmmoTypeForSlot(Inventory, Inventory.ActiveWeaponSlot);
	const int32 AvailableReserve = Inventory.GetReserveAmmoCountForType(AmmoType);
	const int32 AmmoToLoad = FMath::Min(MissingAmmo, AvailableReserve);
	if (AmmoToLoad <= 0)
	{
		return false;
	}

	ActiveRuntime->WeaponActor->StopFire();
	SetState(Inventory, *ActiveRuntime, EPlayerInventoryWeaponState::Reloading);
	ActiveRuntime->WeaponActor->Reload();
	return true;
}

bool FPlayerInventoryWeaponRuntime::NotifyFireAnimation(UPlayerInventoryComponent& Inventory)
{
	AWeaponBase* Weapon = GetActiveWeapon(Inventory);
	return Weapon ? Weapon->NotifyFireAnimation() : false;
}

bool FPlayerInventoryWeaponRuntime::NotifyReloadAnimationFinished(UPlayerInventoryComponent& Inventory)
{
	AWeaponBase* Weapon = GetActiveWeapon(Inventory);
	return Weapon ? Weapon->NotifyReloadAnimationFinished() : false;
}

int32 FPlayerInventoryWeaponRuntime::GetInClip(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot)
{
	const FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->InClip : -1;
}

int32 FPlayerInventoryWeaponRuntime::GetClipSize(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot)
{
	const FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntime(WeaponSlot);
	return (Runtime && Runtime->WeaponActor) ? FMath::Max(Runtime->WeaponActor->GetClipSize(), 0) : -1;
}

int32 FPlayerInventoryWeaponRuntime::GetActiveWeaponInClip(const UPlayerInventoryComponent& Inventory)
{
	if (!HasValidWeaponSelection(Inventory, Inventory.ActiveWeaponSlot))
	{
		return 0;
	}

	return FMath::Max(GetInClip(Inventory, Inventory.ActiveWeaponSlot), 0);
}

int32 FPlayerInventoryWeaponRuntime::GetActiveWeaponClipSize(const UPlayerInventoryComponent& Inventory)
{
	if (!HasValidWeaponSelection(Inventory, Inventory.ActiveWeaponSlot))
	{
		return 0;
	}

	return FMath::Max(GetClipSize(Inventory, Inventory.ActiveWeaponSlot), 0);
}

bool FPlayerInventoryWeaponRuntime::IsActiveWeaponReloading(const UPlayerInventoryComponent& Inventory)
{
	const FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntime(Inventory.ActiveWeaponSlot);
	return Runtime ? IsReloading(*Runtime) : false;
}

bool FPlayerInventoryWeaponRuntime::WasFireRequestedRecently(const UPlayerInventoryComponent& Inventory,
                                                             float WindowSeconds)
{
	const UWorld* World = Inventory.GetWorld();
	if (!World)
	{
		return false;
	}

	const float SafeWindow = FMath::Max(WindowSeconds, 0.01f);
	return (World->GetTimeSeconds() - Inventory.LastActiveWeaponFireRequestTimestamp) <= SafeWindow;
}

bool FPlayerInventoryWeaponRuntime::IsFireNotifyPending(const UPlayerInventoryComponent& Inventory)
{
	const FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntime(Inventory.ActiveWeaponSlot);
	if (Runtime && Runtime->State == EPlayerInventoryWeaponState::FireRequested)
	{
		return true;
	}

	const AWeaponBase* Weapon = GetActiveWeapon(Inventory);
	return Weapon ? Weapon->IsFireNotifyPending() : false;
}

bool FPlayerInventoryWeaponRuntime::WasFiredRecently(const UPlayerInventoryComponent& Inventory, float WindowSeconds)
{
	const UWorld* World = Inventory.GetWorld();
	if (!World)
	{
		return false;
	}

	const float SafeWindow = FMath::Max(WindowSeconds, 0.01f);
	return (World->GetTimeSeconds() - Inventory.LastActiveWeaponFireTimestamp) <= SafeWindow;
}

void FPlayerInventoryWeaponRuntime::HandleFireRequested(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot)
{
	if (Inventory.ActiveWeaponSlot != Slot)
	{
		return;
	}

	if (FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntimeMutable(Slot))
	{
		SetState(Inventory, *Runtime, EPlayerInventoryWeaponState::FireRequested, false);
	}

	Inventory.LastActiveWeaponFireRequestTimestamp = Inventory.GetWorld()
		? Inventory.GetWorld()->GetTimeSeconds()
		: Inventory.LastActiveWeaponFireRequestTimestamp;
	Inventory.OnActiveWeaponFireRequested.Broadcast(Slot);
	Inventory.OnInventoryStateChanged.Broadcast();
}

void FPlayerInventoryWeaponRuntime::HandleFired(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot)
{
	FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntimeMutable(Slot);
	if (!Runtime)
	{
		return;
	}

	SetState(Inventory, *Runtime, EPlayerInventoryWeaponState::Firing, false);
	Runtime->InClip = FMath::Max(Runtime->InClip - 1, 0);
	if (Runtime->InClip <= 0 && Runtime->WeaponActor)
	{
		Runtime->WeaponActor->StopFire();
	}

	if (!Runtime->WeaponActor || Runtime->InClip <= 0 || !Runtime->bTriggerHeld)
	{
		SetState(Inventory, *Runtime, EPlayerInventoryWeaponState::Idle, false);
	}

	if (Inventory.ActiveWeaponSlot == Slot)
	{
		Inventory.LastActiveWeaponFireTimestamp = Inventory.GetWorld()
			? Inventory.GetWorld()->GetTimeSeconds()
			: Inventory.LastActiveWeaponFireTimestamp;
		Inventory.OnActiveWeaponFired.Broadcast(Slot);
	}

	Inventory.OnInventoryStateChanged.Broadcast();
}

void FPlayerInventoryWeaponRuntime::HandleReloaded(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot)
{
	FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntimeMutable(Slot);
	if (!Runtime || !Runtime->WeaponActor || !IsReloading(*Runtime))
	{
		return;
	}

	SetState(Inventory, *Runtime, EPlayerInventoryWeaponState::Idle, false);

	const int32 ClipSize = FMath::Max(Runtime->WeaponActor->GetClipSize(), 0);
	const int32 MissingAmmo = FMath::Max(ClipSize - Runtime->InClip, 0);
	if (MissingAmmo <= 0)
	{
		Inventory.OnInventoryStateChanged.Broadcast();
		return;
	}

	const EAmmoType AmmoType = GetAmmoTypeForSlot(Inventory, Slot);
	const int32 AvailableReserve = Inventory.GetReserveAmmoCountForType(AmmoType);
	const int32 AmmoToLoad = FMath::Min(MissingAmmo, AvailableReserve);
	if (AmmoToLoad <= 0)
	{
		Inventory.OnInventoryStateChanged.Broadcast();
		return;
	}

	const int32 LoadedAmmo = FMath::Clamp(Inventory.ConsumeReserveAmmo(AmmoType, AmmoToLoad), 0, AmmoToLoad);
	Runtime->InClip = FMath::Clamp(Runtime->InClip + LoadedAmmo, 0, ClipSize);
	if (LoadedAmmo > 0)
	{
		Inventory.OnInventoryStateChanged.Broadcast();
	}

	if (LoadedAmmo > 0 && Inventory.ActiveWeaponSlot == Slot && Runtime->bTriggerHeld && Inventory.bWeaponEquipped)
	{
		Runtime->WeaponActor->StartFire();
	}
}

AWeaponBase* FPlayerInventoryWeaponRuntime::SpawnWeapon(const UPlayerInventoryComponent& Inventory,
                                                        TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!WeaponClass)
	{
		return nullptr;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(Inventory.GetOwner());
	UWorld* World = Inventory.GetWorld();
	if (!OwnerCharacter || !World)
	{
		return nullptr;
	}

	AWeaponBase* SpawnedWeapon = World->SpawnActor<AWeaponBase>(WeaponClass);
	if (!SpawnedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInventoryComponent '%s' failed to spawn weapon class '%s'."),
			*Inventory.GetName(), *GetNameSafe(WeaponClass));
		return nullptr;
	}

	SpawnedWeapon->SetOwner(OwnerCharacter);
	return SpawnedWeapon;
}

bool FPlayerInventoryWeaponRuntime::HasValidWeaponItem(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot)
{
	const FPlayerInventoryItem* Item = Inventory.ResolveSlot(Slot);
	return Item && Item->IsValid() &&
		(Slot == EPlayerItemSlot::PrimaryWeapon || Slot == EPlayerItemSlot::SecondaryWeapon);
}

bool FPlayerInventoryWeaponRuntime::HasValidWeaponSelection(const UPlayerInventoryComponent& Inventory,
                                                            EPlayerItemSlot Slot)
{
	return HasValidWeaponItem(Inventory, Slot) && GetWeaponInSlot(Inventory, Slot) != nullptr;
}

EAmmoType FPlayerInventoryWeaponRuntime::GetAmmoTypeForSlot(const UPlayerInventoryComponent& Inventory,
                                                            EPlayerItemSlot Slot)
{
	const AWeaponBase* Weapon = GetWeaponInSlot(Inventory, Slot);
	return Weapon ? Weapon->GetAmmoType() : EAmmoType::None;
}

void FPlayerInventoryWeaponRuntime::ClearRuntimeWeapon(UPlayerInventoryComponent& Inventory,
                                                       FPlayerInventoryWeaponSlotRuntime& Runtime)
{
	if (!Runtime.WeaponActor)
	{
		Runtime.InClip = -1;
		Runtime.State = EPlayerInventoryWeaponState::Idle;
		Runtime.bTriggerHeld = false;
		return;
	}

	Runtime.WeaponActor->OnWeaponFireRequestedEvent().RemoveAll(&Inventory);
	Runtime.WeaponActor->OnWeaponFiredEvent().RemoveAll(&Inventory);
	Runtime.WeaponActor->OnWeaponReloadedEvent().RemoveAll(&Inventory);
	Runtime.WeaponActor->Destroy();
	Runtime.WeaponActor = nullptr;
	Runtime.InClip = -1;
	Runtime.State = EPlayerInventoryWeaponState::Idle;
	Runtime.bTriggerHeld = false;
}

void FPlayerInventoryWeaponRuntime::BindRuntimeWeaponDelegates(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot,
                                                               AWeaponBase* WeaponActor)
{
	if (!WeaponActor)
	{
		return;
	}

	WeaponActor->OnWeaponFireRequestedEvent().RemoveAll(&Inventory);
	WeaponActor->OnWeaponFiredEvent().RemoveAll(&Inventory);
	WeaponActor->OnWeaponReloadedEvent().RemoveAll(&Inventory);
	if (Slot == EPlayerItemSlot::PrimaryWeapon)
	{
		WeaponActor->OnWeaponFireRequestedEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnPrimaryWeaponFireRequested);
		WeaponActor->OnWeaponFiredEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnPrimaryWeaponFired);
		WeaponActor->OnWeaponReloadedEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnPrimaryWeaponReloaded);
	}
	else if (Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		WeaponActor->OnWeaponFireRequestedEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnSecondaryWeaponFireRequested);
		WeaponActor->OnWeaponFiredEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnSecondaryWeaponFired);
		WeaponActor->OnWeaponReloadedEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnSecondaryWeaponReloaded);
	}
}

void FPlayerInventoryWeaponRuntime::AttachWeapon(const UPlayerInventoryComponent& Inventory, AWeaponBase* Weapon,
                                                 FName SocketName, bool bUseHolsterOffset)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(Inventory.GetOwner());
	if (!Weapon || !OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	Weapon->AttachToComponent(OwnerCharacter->GetMesh(), Rules, SocketName);
	Weapon->SetActorRelativeTransform(Weapon->GetAttachmentOffset(bUseHolsterOffset));
}

FName FPlayerInventoryWeaponRuntime::GetHolsterSocketForSlot(const UPlayerInventoryComponent& Inventory,
                                                             EPlayerItemSlot WeaponSlot)
{
	switch (WeaponSlot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		return Inventory.PrimaryWeaponHolsterSocketName;
	case EPlayerItemSlot::SecondaryWeapon:
		return Inventory.SecondaryWeaponHolsterSocketName;
	default:
		return NAME_None;
	}
}

bool FPlayerInventoryWeaponRuntime::IsReloading(const FPlayerInventoryWeaponSlotRuntime& Runtime)
{
	return Runtime.State == EPlayerInventoryWeaponState::Reloading;
}

void FPlayerInventoryWeaponRuntime::SetState(UPlayerInventoryComponent& Inventory,
                                             FPlayerInventoryWeaponSlotRuntime& Runtime,
                                             EPlayerInventoryWeaponState NewState,
                                             bool bBroadcastChange)
{
	if (Runtime.State == NewState)
	{
		return;
	}

	Runtime.State = NewState;
	if (bBroadcastChange)
	{
		Inventory.OnInventoryStateChanged.Broadcast();
	}
}
