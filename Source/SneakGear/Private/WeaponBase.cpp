#include "WeaponBase.h"

#include "EnhancedInputComponent.h"
#include "PlayerCharacterBase.h"
#include "PlayerControllerBase.h"

void AWeaponBase::Use()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Using Weapon"));
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::BindInputAction(const UInputAction* InputToBind)
{
	if (auto* PlayerController = Cast<APlayerControllerBase>(OwningCharacter->GetController()))
	{
		if (auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(InputToBind, ETriggerEvent::Triggered, this, &AWeaponBase::Use);
		}
	}
}
