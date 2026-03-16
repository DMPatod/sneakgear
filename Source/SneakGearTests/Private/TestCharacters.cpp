#include "TestCharacters.h"

#include "AbilitySystemComponent.h"
#include "Components/SceneComponent.h"
#include "Game/GAS/AmmoAttributeSet.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "TestInventory.h"

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

void ATestGuardCharacter::BeginPlay()
{
}
