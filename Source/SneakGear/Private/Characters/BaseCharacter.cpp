#include "Characters/BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "GameplayEffectTypes.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "UI/EventLogSubsystem.h"

ABaseCharacter::ABaseCharacter()
{
	SetCanBeDamaged(true);

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	HealthSet = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthSet"));

	// GAS requires attribute sets to be registered on the ASC before attribute base values are mutated.
	AbilitySystem->AddAttributeSetSubobject(HealthSet.Get());
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

AWeaponBase* ABaseCharacter::GetCurrentWeapon() const
{
	return nullptr;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	bHasDied = false;
	InitGAS();
	BindHealthDeath();
}

void ABaseCharacter::InitGAS()
{
	if (!AbilitySystem)
	{
		AbilitySystem = FindComponentByClass<UAbilitySystemComponent>();
		if (!AbilitySystem)
		{
			UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent is not set on %s"), *GetName());
			return;
		}
	}

	AbilitySystem->InitAbilityActorInfo(this, this);

	auto ApplyGE = [&](TSubclassOf<UGameplayEffect> EffectClass)
	{
		if (!EffectClass)
		{
			return;
		}

		auto Context = AbilitySystem->MakeEffectContext();
		Context.AddSourceObject(this);

		auto Spec = AbilitySystem->MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (Spec.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	};

	ApplyGE(GE_DefaultHealth);
}

void ABaseCharacter::BindHealthDeath()
{
	if (!AbilitySystem)
	{
		return;
	}

	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (!bHasDied && Data.NewValue <= 0.f)
			{
				bHasDied = true;
				OnCharacterDeath();
			}
		});
}

void ABaseCharacter::OnCharacterDeath()
{
	if (!IsActorBeingDestroyed())
	{
		Destroy();
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                 AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	const float AppliedDamage = FMath::Max(DamageAmount, 0.f);
	if (AppliedDamage <= 0.f)
	{
		return AppliedDamage;
	}

	if (AbilitySystem)
	{
		AbilitySystem->ApplyModToAttribute(UHealthAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive,
										   -AppliedDamage);
	}

	const float CurrentHealth = HealthSet ? HealthSet->GetHealth() : 0.f;
	UE_LOG(LogTemp, Warning, TEXT("%s took %.0f damage from %s. Current health: %.0f"),
		*GetName(), AppliedDamage, *GetNameSafe(DamageCauser), CurrentHealth);

	if (auto* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
	{
		EventLog->ReportDamageTaken(this, AppliedDamage, CurrentHealth, DamageCauser);
	}

	return AppliedDamage;
}

float ABaseCharacter::InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	return Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);
}
