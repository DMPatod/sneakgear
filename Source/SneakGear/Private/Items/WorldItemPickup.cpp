#include "Items/WorldItemPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AWorldItemPickup::AWorldItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTrigger"));
	PickupTrigger->SetupAttachment(PickupMesh);
	PickupTrigger->SetSphereRadius(80.f);
	PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AWorldItemPickup::ConsumePickup()
{
	Destroy();
}
