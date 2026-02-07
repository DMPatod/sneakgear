#include "Weapon/WeaponBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/WeaponFireModeComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void AWeaponBase::AttachToCharacter(USkeletalMeshComponent* CharacterMesh, FName SocketName)
{
	if (!CharacterMesh)
	{
		return;
	}

	auto Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(CharacterMesh, Rules, SocketName);
}

void AWeaponBase::StartFire()
{
	auto Interval = FireRate > 0.f ? 1.f / FireRate : 0.1f;

	FireOnce();
	GetWorldTimerManager().SetTimer(FireTimer, this, &AWeaponBase::FireOnce, Interval, true);
}

void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (PrimaryFireModeClass)
	{
		PrimaryFireMode = NewObject<UWeaponFireModeComponent>(this, PrimaryFireModeClass);
		if (PrimaryFireMode)
		{
			PrimaryFireMode->RegisterComponent();
		}
	}
}

void AWeaponBase::FireOnce()
{
	if (!PrimaryFireMode || !WeaponMesh)
	{
		return;
	}

	auto OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	auto OwnerCharacter = Cast<ACharacter>(OwnerPawn);
	if (!OwnerCharacter)
	{
		return;
	}

	auto CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		return;
	}

	auto Context = FWeaponFireContext();
	Context.InstigatorPawn = OwnerPawn;
	Context.WeaponActor = this;
	Context.WeaponMesh = WeaponMesh;
	Context.CameraLocation = CameraComponent->GetComponentLocation();
	Context.CameraDirection = CameraComponent->GetForwardVector();
	Context.MuzzleSocket = MuzzleSocketName;

	PrimaryFireMode->FireOnce(Context);
}
