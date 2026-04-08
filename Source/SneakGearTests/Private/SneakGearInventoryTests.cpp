#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Guards/GuardManagerSubsystem.h"
#include "Items/MedkitItemDefinition.h"
#include "Items/ScannerItemDefinition.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "UI/EventLogSubsystem.h"

#include "SneakGearTestTypes.h"
#include "TestWorldHelpers.h"

namespace
{
void TickWorld(UWorld* World, int32 TickCount = 1, float DeltaSeconds = 1.f / 60.f)
{
	if (!World)
	{
		return;
	}

	for (int32 Index = 0; Index < TickCount; ++Index)
	{
		World->Tick(LEVELTICK_All, DeltaSeconds);
	}
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentCannotSelectEmptyWeaponSlotTest,
	"SneakGear.Inventory.PlayerInventoryComponent.CannotSelectEmptyWeaponSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentCannotSelectEmptyWeaponSlotTest::RunTest(const FString& Parameters)
{
	AActor* OwnerActor = NewObject<AActor>();
	UPlayerInventoryComponent* ItemComponent = NewObject<UPlayerInventoryComponent>(OwnerActor);

	TestNotNull(TEXT("Item component should be created"), ItemComponent);
	TestFalse(TEXT("Selecting an empty primary slot should fail"),
		ItemComponent->SetActiveWeaponSlot(EPlayerItemSlot::PrimaryWeapon));
	TestNull(TEXT("Empty primary slot should not expose a runtime weapon"),
		ItemComponent->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentReloadConsumesReserveAmmoTest,
	"SneakGear.Inventory.PlayerInventoryComponent.ReloadConsumesReserveAmmo",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentReloadConsumesReserveAmmoTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	TestNotNull(TEXT("Inventory test character should spawn"), Character);

	Character->InitializeAbilitySystemForTest(1.f, 10.f);

	UTestPlayerInventoryComponent* ItemComponent = Character->GetTestItemComponent();
	TestNotNull(TEXT("Test item component should exist"), ItemComponent);

	ItemComponent->ConfigureWeaponClasses(ATestWeapon::StaticClass());
	ItemComponent->RunBeginPlayForTest();
	ItemComponent->SetAmmoReserve(EAmmoType::Light, 1, 10);

	FPlayerInventoryItem PrimaryWeaponItem;
	PrimaryWeaponItem.ItemId = TEXT("PrimaryTestWeapon");
	PrimaryWeaponItem.DisplayName = FText::FromString(TEXT("Primary Test Weapon"));
	PrimaryWeaponItem.SlotType = EPlayerItemSlot::PrimaryWeapon;

	TestTrue(TEXT("Primary weapon item should be added"), ItemComponent->AddItem(PrimaryWeaponItem));

	AWeaponBase* PrimaryWeapon = ItemComponent->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon);
	TestNotNull(TEXT("Primary weapon runtime should exist after item assignment"), PrimaryWeapon);
	PrimaryWeapon->DispatchBeginPlay();

	TestTrue(TEXT("Primary slot should become active"), ItemComponent->SetActiveWeaponSlot(EPlayerItemSlot::PrimaryWeapon));

	ItemComponent->StartActiveWeaponFire();
	ItemComponent->StopActiveWeaponFire();

	TestEqual(TEXT("One shot should consume one round from the clip"), ItemComponent->GetActiveWeaponInClip(), 2);
	TestEqual(TEXT("Reserve ammo should stay unchanged before reload"), ItemComponent->GetReserveAmmoCount(), 1);

	TestTrue(TEXT("Reload should succeed when reserve ammo is available"), ItemComponent->ReloadActiveWeapon());
	TestEqual(TEXT("Reload should refill the missing round"), ItemComponent->GetActiveWeaponInClip(), 3);
	TestEqual(TEXT("Reload should consume the last reserve round"), ItemComponent->GetReserveAmmoCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentReloadWaitsForWeaponDelayTest,
	"SneakGear.Inventory.PlayerInventoryComponent.ReloadWaitsForWeaponDelay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentReloadWaitsForWeaponDelayTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	TestNotNull(TEXT("Inventory test character should spawn"), Character);

	UTestPlayerInventoryComponent* ItemComponent = Character->GetTestItemComponent();
	TestNotNull(TEXT("Test item component should exist"), ItemComponent);

	ItemComponent->ConfigureWeaponClasses(ATestDelayedReloadWeapon::StaticClass());
	ItemComponent->RunBeginPlayForTest();
	ItemComponent->SetAmmoReserve(EAmmoType::Light, 3, 10);

	FPlayerInventoryItem PrimaryWeaponItem;
	PrimaryWeaponItem.ItemId = TEXT("PrimaryDelayedReloadWeapon");
	PrimaryWeaponItem.DisplayName = FText::FromString(TEXT("Primary Delayed Reload Weapon"));
	PrimaryWeaponItem.SlotType = EPlayerItemSlot::PrimaryWeapon;

	TestTrue(TEXT("Primary weapon item should be added"), ItemComponent->AddItem(PrimaryWeaponItem));

	AWeaponBase* PrimaryWeapon = ItemComponent->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon);
	TestNotNull(TEXT("Primary weapon runtime should exist after item assignment"), PrimaryWeapon);
	PrimaryWeapon->DispatchBeginPlay();

	TestTrue(TEXT("Primary slot should become active"), ItemComponent->SetActiveWeaponSlot(EPlayerItemSlot::PrimaryWeapon));

	ItemComponent->StartActiveWeaponFire();
	ItemComponent->StopActiveWeaponFire();
	ItemComponent->StartActiveWeaponFire();
	ItemComponent->StopActiveWeaponFire();
	ItemComponent->StartActiveWeaponFire();
	ItemComponent->StopActiveWeaponFire();

	TestEqual(TEXT("Three shots should empty the clip"), ItemComponent->GetActiveWeaponInClip(), 0);
	TestEqual(TEXT("Reserve ammo should remain untouched before reload completes"), ItemComponent->GetReserveAmmoCount(), 3);

	TestTrue(TEXT("Reload should start when reserve ammo is available"), ItemComponent->ReloadActiveWeapon());
	TestEqual(TEXT("Reload should not refill the clip immediately"), ItemComponent->GetActiveWeaponInClip(), 0);
	TestEqual(TEXT("Reserve ammo should not be consumed immediately"), ItemComponent->GetReserveAmmoCount(), 3);

	for (int32 Index = 0; Index < 99; ++Index)
	{
		World->Tick(LEVELTICK_All, 0.1f);
		PrimaryWeapon->Tick(0.1f);
	}
	TestEqual(TEXT("Clip should stay empty before the 10 second reload completes"), ItemComponent->GetActiveWeaponInClip(), 0);
	TestEqual(TEXT("Reserve ammo should stay unchanged before the reload finishes"), ItemComponent->GetReserveAmmoCount(), 3);

	for (int32 Index = 0; Index < 2; ++Index)
	{
		World->Tick(LEVELTICK_All, 0.1f);
		PrimaryWeapon->Tick(0.1f);
	}
	TestEqual(TEXT("Clip should be refilled after the 10 second reload completes"), ItemComponent->GetActiveWeaponInClip(), 3);
	TestEqual(TEXT("Reload should consume reserve ammo after completion"), ItemComponent->GetReserveAmmoCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentPickupReportsEventAndStoresUtilityItemTest,
	"SneakGear.Inventory.PlayerInventoryComponent.PickupReportsEventAndStoresUtilityItem",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentPickupReportsEventAndStoresUtilityItemTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	ATestPickupActor* PickupActor = World->SpawnActor<ATestPickupActor>();
	UTestUtilityItemDefinition* UtilityDefinition = NewObject<UTestUtilityItemDefinition>(GetTransientPackage());
	UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>();

	TestNotNull(TEXT("Inventory character should spawn"), Character);
	TestNotNull(TEXT("Pickup actor should spawn"), PickupActor);
	TestNotNull(TEXT("Utility definition should be created"), UtilityDefinition);
	TestNotNull(TEXT("Event log subsystem should exist"), EventLog);

	UtilityDefinition->ItemId = TEXT("ScannerPickup");
	UtilityDefinition->DisplayName = FText::FromString(TEXT("Scanner Pickup"));
	PickupActor->GetPickupComponent()->SetItemDefinitionForTest(UtilityDefinition);

	TestTrue(TEXT("Picking up a utility item should succeed"), Character->GetTestItemComponent()->PickUpFromFloor(PickupActor));
	TestEqual(TEXT("Utility inventory count should increase"), Character->GetTestItemComponent()->GetItemCount(EPlayerItemSlot::Utility), 1);
	TestTrue(TEXT("Pickup event should be recorded"), EventLogContains(EventLog, TEXT("picked up Scanner Pickup")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentNearbyPickupRespectsRangeTest,
	"SneakGear.Inventory.PlayerInventoryComponent.NearbyPickupRespectsRange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentNearbyPickupRespectsRangeTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Inventory character should spawn"), Character);

	UTestUtilityItemDefinition* NearbyDefinition = NewObject<UTestUtilityItemDefinition>(GetTransientPackage());
	NearbyDefinition->ItemId = TEXT("NearbyScanner");
	NearbyDefinition->DisplayName = FText::FromString(TEXT("Nearby Scanner"));

	ATestPickupActor* NearbyPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	TestNotNull(TEXT("Nearby pickup should spawn"), NearbyPickup);
	NearbyPickup->GetPickupComponent()->SetItemDefinitionForTest(NearbyDefinition);

	TestTrue(TEXT("Nearby pickup should succeed when inside the search radius"),
		Character->GetTestItemComponent()->TryPickUpNearbyFloorItem(150.f));
	TestEqual(TEXT("Successful nearby pickup should add the item"), Character->GetTestItemComponent()->GetItemCount(EPlayerItemSlot::Utility), 1);

	UTestUtilityItemDefinition* FarDefinition = NewObject<UTestUtilityItemDefinition>(GetTransientPackage());
	FarDefinition->ItemId = TEXT("FarScanner");
	FarDefinition->DisplayName = FText::FromString(TEXT("Far Scanner"));

	ATestPickupActor* FarPickup = World->SpawnActor<ATestPickupActor>(FVector(600.f, 0.f, 0.f), FRotator::ZeroRotator);
	TestNotNull(TEXT("Far pickup should spawn"), FarPickup);
	FarPickup->GetPickupComponent()->SetItemDefinitionForTest(FarDefinition);

	TestFalse(TEXT("Far pickup should fail when outside the search radius"),
		Character->GetTestItemComponent()->TryPickUpNearbyFloorItem(150.f));
	TestEqual(TEXT("Failed far pickup should not add another item"), Character->GetTestItemComponent()->GetItemCount(EPlayerItemSlot::Utility), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentWeaponPickupSpawnsAndSelectsRuntimeWeaponTest,
	"SneakGear.Inventory.PlayerInventoryComponent.WeaponPickupSpawnsAndSelectsRuntimeWeapon",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentWeaponPickupSpawnsAndSelectsRuntimeWeaponTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	ATestPickupActor* PickupActor = World->SpawnActor<ATestPickupActor>();
	UPlayerItemDefinition* WeaponDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>();

	TestNotNull(TEXT("Inventory character should spawn"), Character);
	TestNotNull(TEXT("Pickup actor should spawn"), PickupActor);
	TestNotNull(TEXT("Weapon definition should be created"), WeaponDefinition);
	TestNotNull(TEXT("Event log subsystem should exist"), EventLog);

	WeaponDefinition->ItemId = TEXT("PrimaryTestRifle");
	WeaponDefinition->DisplayName = FText::FromString(TEXT("Primary Test Rifle"));
	WeaponDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	WeaponDefinition->WeaponClass = ATestWeapon::StaticClass();
	PickupActor->GetPickupComponent()->SetItemDefinitionForTest(WeaponDefinition);

	TestTrue(TEXT("Weapon pickup should succeed"), Character->GetTestItemComponent()->PickUpFromFloor(PickupActor));
	TestTrue(TEXT("Primary weapon slot should contain the picked-up item"),
		Character->GetTestItemComponent()->HasItem(EPlayerItemSlot::PrimaryWeapon));

	AWeaponBase* RuntimeWeapon = Character->GetTestItemComponent()->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon);
	TestNotNull(TEXT("Picking up the weapon should spawn a runtime weapon actor"), RuntimeWeapon);
	TestTrue(TEXT("Runtime weapon should use the weapon class from the item definition"),
		RuntimeWeapon->IsA(ATestWeapon::StaticClass()));

	TestTrue(TEXT("Selecting the picked-up primary weapon should succeed"),
		Character->GetTestItemComponent()->SetActiveWeaponSlot(EPlayerItemSlot::PrimaryWeapon));
	TestEqual(TEXT("Primary weapon slot should become active"),
		Character->GetTestItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);
	TestEqual(TEXT("Active weapon should be the spawned runtime weapon"),
		Character->GetTestItemComponent()->GetActiveWeapon(),
		RuntimeWeapon);
	TestTrue(TEXT("Pickup event should be recorded for the weapon"),
		EventLogContains(EventLog, TEXT("picked up Primary Test Rifle")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNearbyWeaponPickupRequiresHoldToSwapTest,
	"SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupRequiresHoldToSwap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNearbyWeaponPickupRequiresHoldToSwapTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), Character);

	ATestPickupActor* InitialPickup = World->SpawnActor<ATestPickupActor>(FVector(50.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestPickupActor* NearbySwapPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	TestNotNull(TEXT("Initial weapon pickup should spawn"), InitialPickup);
	TestNotNull(TEXT("Nearby swap pickup should spawn"), NearbySwapPickup);

	UPlayerItemDefinition* InitialDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	InitialDefinition->ItemId = TEXT("HeldRifle");
	InitialDefinition->DisplayName = FText::FromString(TEXT("Held Rifle"));
	InitialDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	InitialDefinition->WeaponClass = ATestWeapon::StaticClass();
	InitialPickup->GetPickupComponent()->SetItemDefinitionForTest(InitialDefinition);

	UPlayerItemDefinition* SwapDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	SwapDefinition->ItemId = TEXT("FloorRifle");
	SwapDefinition->DisplayName = FText::FromString(TEXT("Floor Rifle"));
	SwapDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	SwapDefinition->WeaponClass = ATestWeapon::StaticClass();
	NearbySwapPickup->GetPickupComponent()->SetItemDefinitionForTest(SwapDefinition);

	TestTrue(TEXT("Initial weapon pickup should succeed"), Character->GetItemComponent()->PickUpFromFloor(InitialPickup));
	TestTrue(TEXT("Nearby weapon pickup should require a hold when the slot is already filled"),
		Character->GetItemComponent()->RequiresHoldToSwapNearbyFloorItem(150.f));

	Character->TestTriggerNearbyPickupInput();

	const FPlayerInventoryItem EquippedItem = Character->GetItemComponent()->GetItem(EPlayerItemSlot::PrimaryWeapon);
	TestEqual(TEXT("Tap pickup should keep the currently equipped primary weapon"), EquippedItem.ItemId, InitialDefinition->ItemId);
	TestEqual(TEXT("Tap pickup should leave the floor pickup unchanged"),
		NearbySwapPickup->GetPickupComponent()->GetItemDefinition(),
		SwapDefinition);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNearbyWeaponPickupHoldSwapsWithFloorItemTest,
	"SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupHoldSwapsWithFloorItem",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNearbyWeaponPickupHoldSwapsWithFloorItemTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), Character);

	ATestPickupActor* InitialPickup = World->SpawnActor<ATestPickupActor>(FVector(50.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestPickupActor* NearbySwapPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	TestNotNull(TEXT("Initial weapon pickup should spawn"), InitialPickup);
	TestNotNull(TEXT("Nearby swap pickup should spawn"), NearbySwapPickup);

	UPlayerItemDefinition* InitialDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	InitialDefinition->ItemId = TEXT("HeldSMG");
	InitialDefinition->DisplayName = FText::FromString(TEXT("Held SMG"));
	InitialDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	InitialDefinition->WeaponClass = ATestWeapon::StaticClass();
	InitialPickup->GetPickupComponent()->SetItemDefinitionForTest(InitialDefinition);

	UPlayerItemDefinition* SwapDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	SwapDefinition->ItemId = TEXT("FloorSMG");
	SwapDefinition->DisplayName = FText::FromString(TEXT("Floor SMG"));
	SwapDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	SwapDefinition->WeaponClass = ATestWeapon::StaticClass();
	NearbySwapPickup->GetPickupComponent()->SetItemDefinitionForTest(SwapDefinition);

	TestTrue(TEXT("Initial weapon pickup should succeed"), Character->GetItemComponent()->PickUpFromFloor(InitialPickup));

	Character->TestTriggerNearbyPickupHoldInput();

	const FPlayerInventoryItem EquippedItem = Character->GetItemComponent()->GetItem(EPlayerItemSlot::PrimaryWeapon);
	TestEqual(TEXT("Hold pickup should swap in the floor weapon"), EquippedItem.ItemId, SwapDefinition->ItemId);
	TestEqual(TEXT("Hold pickup should leave the previous weapon on the floor"),
		NearbySwapPickup->GetPickupComponent()->GetItemDefinition(),
		InitialDefinition);
	TestTrue(TEXT("Swapped primary weapon should still expose a runtime weapon"),
		Character->GetItemComponent()->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon) != nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentEquippedItemSwitchLogsAndActivatesTest,
	"SneakGear.Inventory.PlayerInventoryComponent.EquippedItemSwitchLogsAndActivates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentEquippedItemSwitchLogsAndActivatesTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>();
	UTestEquipmentItemDefinition* FirstDefinition = NewObject<UTestEquipmentItemDefinition>(GetTransientPackage());
	UTestEquipmentItemDefinition* SecondDefinition = NewObject<UTestEquipmentItemDefinition>(GetTransientPackage());
	ATestPickupActor* FirstPickup = World->SpawnActor<ATestPickupActor>();
	ATestPickupActor* SecondPickup = World->SpawnActor<ATestPickupActor>();

	TestNotNull(TEXT("Inventory character should spawn"), Character);
	TestNotNull(TEXT("Event log subsystem should exist"), EventLog);

	FirstDefinition->ItemId = TEXT("NightVisor");
	FirstDefinition->DisplayName = FText::FromString(TEXT("Night Visor"));
	SecondDefinition->ItemId = TEXT("ThermalRig");
	SecondDefinition->DisplayName = FText::FromString(TEXT("Thermal Rig"));
	FirstPickup->GetPickupComponent()->SetItemDefinitionForTest(FirstDefinition);
	SecondPickup->GetPickupComponent()->SetItemDefinitionForTest(SecondDefinition);

	TestTrue(TEXT("First equipment pickup should succeed"), Character->GetTestItemComponent()->PickUpFromFloor(FirstPickup));
	TestTrue(TEXT("Second equipment pickup should succeed"), Character->GetTestItemComponent()->PickUpFromFloor(SecondPickup));
	TestTrue(TEXT("Switching active equipped item should succeed"), Character->GetTestItemComponent()->SetActiveItemIndex(EPlayerItemSlot::Equipped, 1));

	TestEqual(TEXT("First equipment should have been activated once"), FirstDefinition->ActivationCount, 1);
	TestEqual(TEXT("First equipment should have been deactivated once"), FirstDefinition->DeactivationCount, 1);
	TestEqual(TEXT("Second equipment should have been activated once"), SecondDefinition->ActivationCount, 1);
	TestTrue(TEXT("Equip event should be recorded"), EventLogContains(EventLog, TEXT("equipped Thermal Rig")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentConsumableUseRestoresHealthConsumesAndLogsTest,
	"SneakGear.Inventory.PlayerInventoryComponent.ConsumableUseRestoresHealthConsumesAndLogs",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentConsumableUseRestoresHealthConsumesAndLogsTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	ATestPickupActor* PickupActor = World->SpawnActor<ATestPickupActor>();
	UMedkitItemDefinition* MedkitDefinition = NewObject<UMedkitItemDefinition>(GetTransientPackage());
	UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>();

	TestNotNull(TEXT("Inventory character should spawn"), Character);
	TestNotNull(TEXT("Pickup actor should spawn"), PickupActor);
	TestNotNull(TEXT("Medkit definition should be created"), MedkitDefinition);
	TestNotNull(TEXT("Event log subsystem should exist"), EventLog);

	if (UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent())
	{
		AbilitySystem->InitAbilityActorInfo(Character, Character);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), 50.f);
	}

	MedkitDefinition->ItemId = TEXT("Medkit");
	MedkitDefinition->DisplayName = FText::FromString(TEXT("Medkit"));
	PickupActor->GetPickupComponent()->SetItemDefinitionForTest(MedkitDefinition);

	TestTrue(TEXT("Consumable pickup should succeed"), Character->GetTestItemComponent()->PickUpFromFloor(PickupActor));
	TestTrue(TEXT("Using the active support item should succeed"), Character->GetTestItemComponent()->UseActiveSupportItem());

	const float CurrentHealth = Character->GetAbilitySystemComponent()
		? Character->GetAbilitySystemComponent()->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute())
		: 0.f;

	TestEqual(TEXT("Medkit should restore health"), CurrentHealth, 85.f);
	TestEqual(TEXT("Consumed support item should be removed"), Character->GetTestItemComponent()->GetItemCount(EPlayerItemSlot::Support), 0);
	TestTrue(TEXT("Support use event should be recorded"), EventLogContains(EventLog, TEXT("used Medkit (Support)")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerInventoryComponentUtilityScannerReportsNearbyGuardsTest,
	"SneakGear.Inventory.PlayerInventoryComponent.UtilityScannerReportsNearbyGuards",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerInventoryComponentUtilityScannerReportsNearbyGuardsTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	ATestPickupActor* PickupActor = World->SpawnActor<ATestPickupActor>();
	ATestGuardCharacter* NearbyGuard = World->SpawnActor<ATestGuardCharacter>(FVector(200.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestGuardCharacter* FarGuard = World->SpawnActor<ATestGuardCharacter>(FVector(5000.f, 0.f, 0.f), FRotator::ZeroRotator);
	UScannerItemDefinition* ScannerDefinition = NewObject<UScannerItemDefinition>(GetTransientPackage());
	UGuardManagerSubsystem* GuardManager = World->GetSubsystem<UGuardManagerSubsystem>();
	UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>();

	TestNotNull(TEXT("Inventory character should spawn"), Character);
	TestNotNull(TEXT("Pickup actor should spawn"), PickupActor);
	TestNotNull(TEXT("Scanner definition should be created"), ScannerDefinition);
	TestNotNull(TEXT("Guard manager should exist"), GuardManager);
	TestNotNull(TEXT("Event log subsystem should exist"), EventLog);

	GuardManager->RegisterGuard(NearbyGuard);
	GuardManager->RegisterGuard(FarGuard);

	ScannerDefinition->ItemId = TEXT("Scanner");
	ScannerDefinition->DisplayName = FText::FromString(TEXT("Scanner"));
	PickupActor->GetPickupComponent()->SetItemDefinitionForTest(ScannerDefinition);

	TestTrue(TEXT("Utility pickup should succeed"), Character->GetTestItemComponent()->PickUpFromFloor(PickupActor));
	TestTrue(TEXT("Using the active utility item should succeed"), Character->GetTestItemComponent()->UseActiveUtilityItem());
	TestEqual(TEXT("Non-consumable scanner should remain in inventory"), Character->GetTestItemComponent()->GetItemCount(EPlayerItemSlot::Utility), 1);
	TestTrue(TEXT("Scanner should report the nearby guard"), EventLogContains(EventLog, TEXT("Scanner detected 1 guard")));
	TestTrue(TEXT("Utility use event should be recorded"), EventLogContains(EventLog, TEXT("used Scanner (Utility)")));
	return true;
}

#endif
