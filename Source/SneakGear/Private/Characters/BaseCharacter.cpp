#include "Characters/BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "GameplayEffectTypes.h"
#include "GAS/HealthAttributeSet.h"
#include "GAS/StaminaAttributeSet.h"
#include "UI/EventLogSubsystem.h"

ABaseCharacter::ABaseCharacter()
{
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	HealthSet = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthSet"));
	StaminaSet = CreateDefaultSubobject<UStaminaAttributeSet>(TEXT("StaminaSet"));
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

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
	ApplyGE(GE_DefaultStamina);
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
			if (Data.NewValue <= 0.f)
			{
				auto TextLoc = GetActorLocation() + FVector(0.f, 0.f, 120.f);
				auto Txt = FString::Printf(TEXT("Player Died!"));
				DrawDebugString(GetWorld(), TextLoc, Txt, nullptr, FColor::Red, 0.f, false);
			}
		});
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                 AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (AppliedDamage <= 0.f || !AbilitySystem)
	{
		return AppliedDamage;
	}

	AbilitySystem->ApplyModToAttribute(UHealthAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive,
	                                   -AppliedDamage);

	if (auto* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
	{
		EventLog->ReportDamageTaken(this, AppliedDamage, DamageCauser);
	}

	return AppliedDamage;
}
