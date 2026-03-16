#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Guards/GuardManagerSubsystem.h"
#include "Guards/Patrol/PatrolPath.h"
#include "Items/MedkitItemDefinition.h"
#include "Items/ScannerItemDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "Game/GAS/StaminaAttributeSet.h"
#include "Tests/AutomationEditorCommon.h"
#include "UI/EventLogSubsystem.h"

#include "SneakGearTestTypes.h"

namespace
{
UWorld* CreateTestWorld()
{
	UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
	if (World && !World->HasBegunPlay())
	{
		World->InitializeActorsForPlay(FURL());
		World->BeginPlay();
	}

	return World;
}

UBlackboardData* CreatePatrolBlackboard(UObject* Outer)
{
	UBlackboardData* BlackboardAsset = NewObject<UBlackboardData>(Outer);
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(TEXT("PatrolLocation"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(TEXT("PatrolIndex"));
	BlackboardAsset->UpdateKeyIDs();
	return BlackboardAsset;
}

UBlackboardData* CreateGuardBlackboard(UObject* Outer)
{
	UBlackboardData* BlackboardAsset = NewObject<UBlackboardData>(Outer);
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(TEXT("TargetActor"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Float>(TEXT("Awareness"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(TEXT("HasLineOfSight"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(TEXT("AwarenessState"));
	BlackboardAsset->UpdateKeyIDs();
	return BlackboardAsset;
}

APlayerController* EnsureTestPlayerController(UWorld* World)
{
	if (APlayerController* Existing = World ? World->GetFirstPlayerController() : nullptr)
	{
		return Existing;
	}

	if (!World)
	{
		return nullptr;
	}

	return World->SpawnActor<APlayerController>();
}

bool EventLogContains(UEventLogSubsystem* EventLog, const FString& Substring)
{
	if (!EventLog)
	{
		return false;
	}

	TArray<FGameEventEntry> Entries;
	EventLog->GetEventsAfter(0, Entries);
	for (const FGameEventEntry& Entry : Entries)
	{
		if (Entry.Message.ToString().Contains(Substring))
		{
			return true;
		}
	}

	return false;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBaseCharacterDeathOnlyRunsOnceTest,
	"SneakGear.Characters.BaseCharacter.DeathOnlyRunsOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBaseCharacterDeathOnlyRunsOnceTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestDeathCharacter* Character = World->SpawnActor<ATestDeathCharacter>();
	TestNotNull(TEXT("Test death character should be created"), Character);

	Character->InitializeAbilitySystemForTest(100.f);
	Character->BindHealthDeathForTest();
	Character->ApplyHealthDeltaForTest(-150.f);
	Character->ApplyHealthDeltaForTest(-10.f);

	TestEqual(TEXT("Death should only trigger once"), Character->DeathCount, 1);
	return true;
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTTaskSetNextPatrolPointAdvancesAndLoopsTest,
	"SneakGear.AI.BehaviorTree.BTTaskSetNextPatrolPoint.AdvancesAndLoops",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBTTaskSetNextPatrolPointAdvancesAndLoopsTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestPatrolPawn* Pawn = World->SpawnActor<ATestPatrolPawn>();
	APatrolPath* PatrolPath = World->SpawnActor<APatrolPath>();
	AAIController* Controller = World->SpawnActor<AAIController>();

	TestNotNull(TEXT("Patrol pawn should spawn"), Pawn);
	TestNotNull(TEXT("Patrol path should spawn"), PatrolPath);
	TestNotNull(TEXT("AI controller should spawn"), Controller);

	PatrolPath->Waypoints = {
		FVector(100.f, 0.f, 0.f),
		FVector(200.f, 0.f, 0.f),
		FVector(300.f, 0.f, 0.f)
	};
	Pawn->GetPatrolComponent()->SetPatrolPath(PatrolPath);
	Controller->Possess(Pawn);

	UBlackboardData* BlackboardAsset = CreatePatrolBlackboard(Controller);
	UBlackboardComponent* BlackboardComponent = NewObject<UBlackboardComponent>(Controller);
	UBehaviorTreeComponent* BehaviorTreeComponent = NewObject<UBehaviorTreeComponent>(Controller);
	TestNotNull(TEXT("Blackboard component should be created"), BlackboardComponent);
	TestNotNull(TEXT("Behavior tree component should be created"), BehaviorTreeComponent);

	BlackboardComponent->RegisterComponent();
	BehaviorTreeComponent->RegisterComponent();
	TestTrue(TEXT("Blackboard should initialize"), BlackboardComponent->InitializeBlackboard(*BlackboardAsset));
	BehaviorTreeComponent->CacheBlackboardComponent(BlackboardComponent);

	UTestBTTask_SetNextPatrolPoint* Task = NewObject<UTestBTTask_SetNextPatrolPoint>();
	Task->ConfigureBlackboardKeys(TEXT("PatrolLocation"), TEXT("PatrolIndex"));
	Task->SetLoopingForTest(true);

	BlackboardComponent->SetValueAsInt(TEXT("PatrolIndex"), 0);
	TestEqual(TEXT("Task should succeed from the first waypoint"),
		Task->ExecuteTask(*BehaviorTreeComponent, nullptr), EBTNodeResult::Succeeded);
	TestEqual(TEXT("Task should write the first waypoint location"),
		BlackboardComponent->GetValueAsVector(TEXT("PatrolLocation")), PatrolPath->GetWorldPoint(0));
	TestEqual(TEXT("Task should advance the patrol index"), BlackboardComponent->GetValueAsInt(TEXT("PatrolIndex")), 1);

	BlackboardComponent->SetValueAsInt(TEXT("PatrolIndex"), 2);
	TestEqual(TEXT("Task should succeed from the last waypoint"),
		Task->ExecuteTask(*BehaviorTreeComponent, nullptr), EBTNodeResult::Succeeded);
	TestEqual(TEXT("Task should write the last waypoint location"),
		BlackboardComponent->GetValueAsVector(TEXT("PatrolLocation")), PatrolPath->GetWorldPoint(2));
	TestEqual(TEXT("Looping should wrap the patrol index to the first waypoint"),
		BlackboardComponent->GetValueAsInt(TEXT("PatrolIndex")), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseUsesAimProviderWithoutCameraTest,
	"SneakGear.Weapon.WeaponBase.UsesAimProviderWithoutCamera",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseUsesAimProviderWithoutCameraTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestAimPawn* OwnerPawn = World->SpawnActor<ATestAimPawn>();
	ATestWeapon* Weapon = World->SpawnActor<ATestWeapon>();
	TestNotNull(TEXT("Aim provider pawn should spawn"), OwnerPawn);
	TestNotNull(TEXT("Test weapon should spawn"), Weapon);

	Weapon->SetOwner(OwnerPawn);
	Weapon->DispatchBeginPlay();

	int32 BroadcastCount = 0;
	Weapon->OnWeaponFiredEvent().AddLambda([&BroadcastCount]()
	{
		++BroadcastCount;
	});

	Weapon->StartFire();
	Weapon->StopFire();

	TestEqual(TEXT("Weapon should broadcast a fire event when aim data comes from the owner"), BroadcastCount, 1);

	UTestWeaponFireModeComponent* FireMode = Weapon->GetTestFireMode();
	TestNotNull(TEXT("Test fire mode should be initialized"), FireMode);
	TestEqual(TEXT("Fire mode should execute once"), FireMode->FireCount, 1);
	TestEqual(TEXT("Aim origin should come from the aim provider"), FireMode->LastContext.AimOrigin, OwnerPawn->AimOrigin);
	TestEqual(TEXT("Aim direction should come from the aim provider"),
		FireMode->LastContext.AimDirection, OwnerPawn->AimDirection.GetSafeNormal());

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterFullInputFlowTest,
	"SneakGear.Player.SneakGearPlayerCharacter.FullInputFlow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterFullInputFlowTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	APlayerController* PlayerController = EnsureTestPlayerController(World);
	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), Character);
	TestNotNull(TEXT("Player controller should be available"), PlayerController);
	PlayerController->Possess(Character);

	if (UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent())
	{
		AbilitySystem->InitAbilityActorInfo(Character, Character);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), 50.f);
	}

	int32 WeaponStateBroadcasts = 0;
	Character->OnPlayerUIWeaponStateChangedEvent().AddLambda([&WeaponStateBroadcasts]()
	{
		++WeaponStateBroadcasts;
	});

	ATestPickupActor* WeaponPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UPlayerItemDefinition* WeaponDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	WeaponDefinition->ItemId = TEXT("InputFlowRifle");
	WeaponDefinition->DisplayName = FText::FromString(TEXT("Input Flow Rifle"));
	WeaponDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	WeaponDefinition->WeaponClass = ATestWeapon::StaticClass();
	WeaponPickup->GetPickupComponent()->SetItemDefinitionForTest(WeaponDefinition);

	Character->TestTriggerNearbyPickupInput();
	TestTrue(TEXT("Primary weapon should be picked up through the player input handler"),
		Character->GetItemComponent()->HasItem(EPlayerItemSlot::PrimaryWeapon));

	ATestPickupActor* MedkitPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UMedkitItemDefinition* MedkitDefinition = NewObject<UMedkitItemDefinition>(GetTransientPackage());
	MedkitDefinition->ItemId = TEXT("InputFlowMedkit");
	MedkitDefinition->DisplayName = FText::FromString(TEXT("Input Flow Medkit"));
	MedkitPickup->GetPickupComponent()->SetItemDefinitionForTest(MedkitDefinition);

	Character->TestTriggerNearbyPickupInput();
	TestEqual(TEXT("Support inventory should contain the medkit after pickup"),
		Character->GetItemComponent()->GetItemCount(EPlayerItemSlot::Support), 1);

	ATestPickupActor* UtilityPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UTestUtilityItemDefinition* UtilityDefinition = NewObject<UTestUtilityItemDefinition>(GetTransientPackage());
	UtilityDefinition->ItemId = TEXT("InputFlowScanner");
	UtilityDefinition->DisplayName = FText::FromString(TEXT("Input Flow Scanner"));
	UtilityPickup->GetPickupComponent()->SetItemDefinitionForTest(UtilityDefinition);

	Character->TestTriggerNearbyPickupInput();
	TestEqual(TEXT("Utility inventory should contain the utility item after pickup"),
		Character->GetItemComponent()->GetItemCount(EPlayerItemSlot::Utility), 1);

	Character->TestTriggerUseSupportItemInput();
	const float CurrentHealth = Character->GetAbilitySystemComponent()
		? Character->GetAbilitySystemComponent()->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute())
		: 0.f;
	TestEqual(TEXT("Support item input should use the medkit and restore health"), CurrentHealth, 85.f);
	TestEqual(TEXT("Used medkit should be consumed"), Character->GetItemComponent()->GetItemCount(EPlayerItemSlot::Support), 0);

	Character->TestTriggerUseUtilityItemInput();
	TestEqual(TEXT("Utility input should use the active utility item once"), UtilityDefinition->UseCount, 1);

	Character->TestTriggerPrimaryWeaponInput();
	TestEqual(TEXT("Primary weapon input should select the primary weapon slot"),
		Character->GetItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);
	TestTrue(TEXT("Weapon-state delegate should have fired during the input flow"), WeaponStateBroadcasts > 0);
	TestNotNull(TEXT("Primary weapon input should leave an active runtime weapon"), Character->GetCurrentWeapon());

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGuardAIControllerUpdatesBlackboardFromGuardTargetTest,
	"SneakGear.AI.GuardAIController.UpdatesBlackboardFromGuardTarget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGuardAIControllerUpdatesBlackboardFromGuardTargetTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestGuardCharacter* Guard = World->SpawnActor<ATestGuardCharacter>();
	ATestGuardAIController* Controller = World->SpawnActor<ATestGuardAIController>();
	AActor* TargetActor = World->SpawnActor<AActor>();

	TestNotNull(TEXT("Test guard should spawn"), Guard);
	TestNotNull(TEXT("Test guard AI controller should spawn"), Controller);
	TestNotNull(TEXT("Target actor should spawn"), TargetActor);

	Controller->Possess(Guard);

	UBlackboardData* BlackboardAsset = CreateGuardBlackboard(Controller);
	TestNotNull(TEXT("Guard blackboard should be created"), BlackboardAsset);
	TestTrue(TEXT("Guard blackboard should initialize"), Controller->InitializeBlackboardForTest(BlackboardAsset));

	Guard->SetTargetActor(TargetActor);
	Controller->UpdateBlackboardForTest();

	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	TestNotNull(TEXT("Guard controller blackboard should exist"), BlackboardComponent);
	TestEqual(TEXT("Blackboard target should match the guard target actor"),
		BlackboardComponent->GetValueAsObject(TEXT("TargetActor")),
		static_cast<UObject*>(TargetActor));
	TestEqual(TEXT("Blackboard awareness should reflect the guard awareness"), BlackboardComponent->GetValueAsFloat(TEXT("Awareness")), Guard->GetAwareness());
	TestEqual(TEXT("Blackboard line-of-sight should reflect the guard state"), BlackboardComponent->GetValueAsBool(TEXT("HasLineOfSight")), Guard->HasLineOfSight());
	TestEqual(TEXT("Blackboard awareness state should reflect the guard state"),
		BlackboardComponent->GetValueAsInt(TEXT("AwarenessState")),
		static_cast<int32>(Guard->GetAwarenessState()));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerVitalsWidgetUpdatesFromAttributeDelegatesTest,
	"SneakGear.UI.PlayerVitalsWidget.UpdatesFromAttributeDelegates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerVitalsWidgetUpdatesFromAttributeDelegatesTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	APlayerController* PlayerController = EnsureTestPlayerController(World);
	TestNotNull(TEXT("Player controller should be available"), PlayerController);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	TestNotNull(TEXT("Inventory character should spawn"), Character);

	Character->InitializeAbilitySystemForTest(0.f, 10.f);
	if (UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent())
	{
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UStaminaAttributeSet::GetMaxStaminaAttribute(), 50.f);
		AbilitySystem->SetNumericAttributeBase(UStaminaAttributeSet::GetStaminaAttribute(), 50.f);
	}

	UTestPlayerVitalsWidget* Widget = CreateWidget<UTestPlayerVitalsWidget>(World, UTestPlayerVitalsWidget::StaticClass());
	TestNotNull(TEXT("Vitals widget should be created"), Widget);
	Widget->SetObservedPlayer(Character);
	Widget->ConstructForTest();

	if (UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent())
	{
		AbilitySystem->ApplyModToAttribute(UHealthAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -25.f);
		AbilitySystem->ApplyModToAttribute(UStaminaAttributeSet::GetStaminaAttribute(), EGameplayModOp::Additive, -10.f);
	}

	TestTrue(TEXT("Health text should update without widget tick"),
		Widget->GetHealthDisplayText().ToString().Contains(TEXT("75 / 100")));
	TestTrue(TEXT("Stamina text should update without widget tick"),
		Widget->GetStaminaDisplayText().ToString().Contains(TEXT("40 / 50")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStanceWidgetUpdatesFromStanceDelegateTest,
	"SneakGear.UI.StanceWidget.UpdatesFromStanceDelegate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStanceWidgetUpdatesFromStanceDelegateTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	APlayerController* PlayerController = EnsureTestPlayerController(World);
	TestNotNull(TEXT("Player controller should be available"), PlayerController);

	ATestInventoryCharacter* Character = World->SpawnActor<ATestInventoryCharacter>();
	TestNotNull(TEXT("Inventory character should spawn"), Character);

	UTestStanceWidget* Widget = CreateWidget<UTestStanceWidget>(World, UTestStanceWidget::StaticClass());
	TestNotNull(TEXT("Stance widget should be created"), Widget);
	Widget->SetObservedPlayer(Character);
	Widget->ConstructForTest();

	Character->SetStance(EStance::Crouching);
	TestEqual(TEXT("Stance widget should react to stance changes without widget tick"),
		Widget->GetStanceDisplayText().ToString(),
		StaticEnum<EStance>()->GetDisplayNameTextByValue(static_cast<int64>(EStance::Crouching)).ToString());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEventFeedWidgetUpdatesFromEventDelegateTest,
	"SneakGear.UI.EventFeedWidget.UpdatesFromEventDelegate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEventFeedWidgetUpdatesFromEventDelegateTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>();
	TestNotNull(TEXT("Event log subsystem should exist"), EventLog);

	UTestEventFeedWidget* Widget = CreateWidget<UTestEventFeedWidget>(World, UTestEventFeedWidget::StaticClass());
	TestNotNull(TEXT("Event feed widget should be created"), Widget);
	Widget->NativeConstruct();

	EventLog->AddEvent(FText::FromString(TEXT("Guard alerted")), EGameEventCategory::Awareness);

	TestTrue(TEXT("Event feed should update from the event-log delegate without widget tick"),
		Widget->GetEventFeedText().ToString().Contains(TEXT("Guard alerted")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPickupPromptWidgetShowsSwapMessageAndHoldProgressTest,
	"SneakGear.UI.PickupPromptWidget.ShowsSwapMessageAndHoldProgress",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPickupPromptWidgetShowsSwapMessageAndHoldProgressTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	UTestPickupPromptWidget* Widget = CreateWidget<UTestPickupPromptWidget>(World, UTestPickupPromptWidget::StaticClass());
	TestNotNull(TEXT("Pickup prompt widget should be created"), Widget);
	Widget->ConstructForTest();

	Widget->SetPickupInfo(FText::FromString(TEXT("Rifle")), FText::FromString(TEXT("Primary Weapon")));
	Widget->SetSwapPrompt(false);
	Widget->SetHoldProgress(0.6f);
	Widget->SetPromptVisible(true);

	TestTrue(TEXT("Default prompt should use the pickup text"), Widget->GetPromptDisplayText().ToString().Contains(TEXT("Pick up Rifle")));
	TestEqual(TEXT("Progress bar should stay hidden when swap is not required"),
		Widget->GetHoldProgressVisibility(),
		ESlateVisibility::Collapsed);

	Widget->SetSwapPrompt(true);
	Widget->SetHoldProgress(0.6f);

	TestTrue(TEXT("Swap prompt should use the hold-to-swap text"),
		Widget->GetPromptDisplayText().ToString().Contains(TEXT("Hold to swap Rifle")));
	TestEqual(TEXT("Progress bar should be visible when swap is required"),
		Widget->GetHoldProgressVisibility(),
		ESlateVisibility::Visible);
	TestTrue(TEXT("Progress bar should reflect the hold progress"),
		FMath::IsNearlyEqual(Widget->GetHoldProgressValue(), 0.6f, KINDA_SMALL_NUMBER));
	return true;
}

#endif
