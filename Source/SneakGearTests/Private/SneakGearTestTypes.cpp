#include "SneakGearTestTypes.h"

#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SceneComponent.h"
#include "Game/GAS/AmmoAttributeSet.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "Game/GAS/StaminaAttributeSet.h"
#include "Components/TextBlock.h"

void ATestDeathCharacter::InitializeAbilitySystemForTest(float InitialHealth)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
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

void UTestPlayerItemComponent::ConfigureWeaponClasses(TSubclassOf<AWeaponBase> InPrimaryWeaponClass, TSubclassOf<AWeaponBase> InSecondaryWeaponClass)
{
	PrimaryWeaponClass = InPrimaryWeaponClass;
	SecondaryWeaponClass = InSecondaryWeaponClass;
}

void UTestPlayerItemComponent::RunBeginPlayForTest()
{
	BeginPlay();
}

void UTestWeaponFireModeComponent::FireOnce(const FWeaponFireContext& Context)
{
	++FireCount;
	LastContext = Context;
}

ATestWeapon::ATestWeapon()
{
	PrimaryFireModeClass = UTestWeaponFireModeComponent::StaticClass();
	ClipSize = 3;
	FireRate = 10.f;
}

UTestWeaponFireModeComponent* ATestWeapon::GetTestFireMode() const
{
	return Cast<UTestWeaponFireModeComponent>(PrimaryFireMode);
}

ATestInventoryCharacter::ATestInventoryCharacter()
{
	TestItemComponent = CreateDefaultSubobject<UTestPlayerItemComponent>(TEXT("TestItemComponent"));
}

UTestPlayerItemComponent* ATestInventoryCharacter::GetTestItemComponent() const
{
	return TestItemComponent;
}

void ATestInventoryCharacter::InitializeAbilitySystemForTest(float InitialAmmo, float MaxAmmo)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
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

void ATestGuardAIController::OnPossess(APawn* InPawn)
{
	AAIController::OnPossess(InPawn);
}

bool ATestGuardAIController::InitializeBlackboardForTest(UBlackboardData* BlackboardAsset)
{
	UBlackboardComponent* BlackboardComponent = nullptr;
	return BlackboardAsset && UseBlackboard(BlackboardAsset, BlackboardComponent);
}

void ATestGuardAIController::UpdateBlackboardForTest()
{
	UpdateBlackboardFromGuard();
}

void UTestBTTask_SetNextPatrolPoint::ConfigureBlackboardKeys(FName InPatrolLocationKeyName, FName InPatrolIndexKeyName)
{
	PatrolLocationKey.SelectedKeyName = InPatrolLocationKeyName;
	PatrolIndexKey.SelectedKeyName = InPatrolIndexKeyName;
}

void UTestBTTask_SetNextPatrolPoint::SetLoopingForTest(bool bInLoop)
{
	bLoop = bInLoop;
}

FText UTestPlayerVitalsWidget::GetHealthDisplayText() const
{
	return HealthText ? HealthText->GetText() : FText::GetEmpty();
}

FText UTestPlayerVitalsWidget::GetStaminaDisplayText() const
{
	return StaminaText ? StaminaText->GetText() : FText::GetEmpty();
}

void UTestStanceWidget::ConstructForTest()
{
	NativeConstruct();
}

FText UTestStanceWidget::GetStanceDisplayText() const
{
	return StanceText ? StanceText->GetText() : FText::GetEmpty();
}

FText UTestEventFeedWidget::GetEventFeedText() const
{
	return EventText ? EventText->GetText() : FText::GetEmpty();
}
