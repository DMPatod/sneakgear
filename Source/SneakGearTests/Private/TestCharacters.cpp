#include "TestCharacters.h"

#include "AbilitySystemComponent.h"
#include "Components/CharacterWeaponComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Game/GAS/AmmoAttributeSet.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "TestInventory.h"
#include "TestWeapons.h"

namespace
{
void EnsureTestGuardWeaponRuntime(AGuardCharacter* GuardCharacter, UCharacterWeaponComponent* WeaponComponent, TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!GuardCharacter || !WeaponComponent || WeaponComponent->GetCurrentWeapon() || !WeaponClass)
	{
		return;
	}

	UWorld* World = GuardCharacter->GetWorld();
	if (!World)
	{
		return;
	}

	AWeaponBase* SpawnedWeapon = World->SpawnActor<AWeaponBase>(WeaponClass);
	if (!SpawnedWeapon)
	{
		return;
	}

	SpawnedWeapon->SetOwner(GuardCharacter);
	SpawnedWeapon->DispatchBeginPlay();

	FObjectProperty* CurrentWeaponProperty = FindFProperty<FObjectProperty>(UCharacterWeaponComponent::StaticClass(), TEXT("CurrentWeapon"));
	FIntProperty* InClipProperty = FindFProperty<FIntProperty>(UCharacterWeaponComponent::StaticClass(), TEXT("InClip"));
	if (!CurrentWeaponProperty || !InClipProperty)
	{
		return;
	}

	CurrentWeaponProperty->SetObjectPropertyValue_InContainer(WeaponComponent, SpawnedWeapon);
	InClipProperty->SetPropertyValue_InContainer(WeaponComponent, FMath::Max(SpawnedWeapon->GetClipSize(), 0));
}
}

void ATestDeathCharacter::InitializeAbilitySystemForTest(float InitialHealth)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	AbilitySystemComponent->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), InitialHealth);
	AbilitySystemComponent->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), InitialHealth);
}

void ATestDeathCharacter::BindHealthDeathForTest()
{
	BindHealthDeath();
}

void ATestDeathCharacter::ApplyHealthDeltaForTest(float Delta)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		AbilitySystemComponent->ApplyModToAttribute(UHealthAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, Delta);
	}
}

void ATestDeathCharacter::OnCharacterDeath()
{
	++DeathCount;
}

ATestInventoryCharacter::ATestInventoryCharacter()
{
	TestItemComponent = CreateDefaultSubobject<UTestPlayerInventoryComponent>(TEXT("TestItemComponent"));
}

UTestPlayerInventoryComponent* ATestInventoryCharacter::GetTestItemComponent() const
{
	return TestItemComponent;
}

void ATestInventoryCharacter::InitializeAbilitySystemForTest(float InitialAmmo, float MaxAmmo)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetMaxAmmoAttribute(), MaxAmmo);
	AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetAmmoAttribute(), FMath::Clamp(InitialAmmo, 0.f, MaxAmmo));
}

AWeaponBase* ATestInventoryCharacter::GetCurrentWeapon() const
{
	return TestItemComponent ? TestItemComponent->GetActiveWeapon() : Super::GetCurrentWeapon();
}

ATestAimPawn::ATestAimPawn()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);
}

bool ATestAimPawn::GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const
{
	if (!bProvideAimData)
	{
		return false;
	}

	OutAimOrigin = AimOrigin;
	OutAimDirection = AimDirection.GetSafeNormal();
	return !OutAimDirection.IsNearlyZero();
}

ATestPatrolPawn::ATestPatrolPawn()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);
	PatrolComponent = CreateDefaultSubobject<UGuardPatrolComponent>(TEXT("PatrolComponent"));
}

UGuardPatrolComponent* ATestPatrolPawn::GetPatrolComponent() const
{
	return PatrolComponent;
}

ATestGuardCharacter::ATestGuardCharacter()
{
	SetActorTickEnabled(true);
	if (UCharacterWeaponComponent* WeaponComponent = Cast<UCharacterWeaponComponent>(GetDefaultSubobjectByName(TEXT("WeaponComponent"))))
	{
		WeaponComponent->SetStartedWeaponClassForTesting(ATestDamageWeapon::StaticClass());
	}
}

void ATestGuardCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterWeaponComponent* WeaponComponent = FindComponentByClass<UCharacterWeaponComponent>();
	if (WeaponComponent)
	{
		WeaponComponent->SetStartedWeaponClassForTesting(ATestDamageWeapon::StaticClass());
		WeaponComponent->InitializeWeaponForTesting();
	}
	if (WeaponComponent && !WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->InitializeWeaponForTesting();
	}

	EnsureTestGuardWeaponRuntime(this, WeaponComponent, ATestDamageWeapon::StaticClass());
	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->ToggleEquip();
	}
}

ATestCoverObstacle::ATestCoverObstacle()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionObjectType(ECC_WorldStatic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Block);
	BoxComponent->SetGenerateOverlapEvents(false);
}

void ATestCoverObstacle::SetBoxExtent(const FVector& NewExtent)
{
	if (BoxComponent)
	{
		BoxComponent->SetBoxExtent(NewExtent);
	}
}
