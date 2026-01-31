#include "Radar/RadarEmitterComponent.h"

URadarEmitterComponent::URadarEmitterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void URadarEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
}


void URadarEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
