#include "EquipableToolBase.h"

#include "PlayerCharacterBase.h"

AEquipableToolBase::AEquipableToolBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ToolMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ToolMesh"));
	checkf(ToolMeshComponent, TEXT("ToolMeshComponent cannot be null!"));
}

void AEquipableToolBase::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Tool BeginPlay"));
}

void AEquipableToolBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEquipableToolBase::Use()
{
}

void AEquipableToolBase::BindInputAction(const UInputAction* ActionToBind)
{
}