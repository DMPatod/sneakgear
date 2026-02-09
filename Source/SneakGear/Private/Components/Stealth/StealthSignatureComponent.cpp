#include "Components/Stealth/StealthSignatureComponent.h"

UStealthSignatureComponent::UStealthSignatureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStealthSignatureComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStealthSignatureComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
