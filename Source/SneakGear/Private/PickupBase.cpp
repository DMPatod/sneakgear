#include "PickupBase.h"

#include "Data/ItemDefinition.h"

APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	check(PickupMeshComponent != nullptr);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(SphereComponent != nullptr);

	SphereComponent->SetupAttachment(PickupMeshComponent);
	SphereComponent->SetSphereRadius(32.f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereBeginOverlap);
}

void APickupBase::InitializePickup()
{
	DataTable = GetDataTable();
	if (DataTable && !ItemID.IsNone())
	{
		const FItemData* ItemDataRow = DataTable->FindRow<FItemData>(ItemID, ItemID.ToString());

		auto* TempItemDefinition = ItemDataRow->ItemBase.Get();

		ReferenceItem = TempItemDefinition->CreateItemCopy();

		if (TempItemDefinition->WorldMesh.IsValid())
		{
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());
		}
		else
		{
			auto* WorldMesh = TempItemDefinition->WorldMesh.LoadSynchronous();
			PickupMeshComponent->SetStaticMesh(WorldMesh);
		}
		PickupMeshComponent->SetVisibility(true);

		PickupMeshComponent->SetVisibility(true);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup();
}

void APickupBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sphere Begin Overlap"));

	if (auto* Character = Cast<APlayerCharacterBase>(OtherActor))
	{
		Character->GiveItem(ReferenceItem);

		SphereComponent->OnComponentBeginOverlap.RemoveAll(this);

		PickupMeshComponent->SetVisibility(false);
		PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (bRespawnable)
		{
			GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase::InitializePickup, RespawnTime,
			                                false, 0);
		}
	}
}

#if WITH_EDITOR
void APickupBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property
		                                  ? PropertyChangedEvent.Property->GetFName()
		                                  : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase, ItemID) && DataTable)
	{
		if (const FItemData* ItemDataRow = DataTable->FindRow<FItemData>(ItemID, ItemID.ToString()))
		{
			UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase;

			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());

			SphereComponent->SetSphereRadius(32.f);
		}
	}
}
#endif

TSoftObjectPtr<UDataTable> APickupBase::GetDataTable()
{
	if (DataTable.IsNull())
	{
		return nullptr;
	}

	auto Table = DataTable.LoadSynchronous();
	checkf(Table, TEXT("Error Loading DataTable"));
	return Table;
}
