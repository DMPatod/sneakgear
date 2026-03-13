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
#include "Guards/Patrol/PatrolPath.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "Game/GAS/StaminaAttributeSet.h"
#include "Tests/AutomationEditorCommon.h"

#include "SneakGearTestTypes.h"

namespace
{
UWorld* CreateTestWorld()
{
	return FAutomationEditorCommonUtils::CreateNewMap();
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
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBaseCharacterDeathOnlyRunsOnceTest,
	"SneakGear.Characters.BaseCharacter.DeathOnlyRunsOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBaseCharacterDeathOnlyRunsOnceTest::RunTest(const FString& Parameters)
{
	ATestDeathCharacter* Character = NewObject<ATestDeathCharacter>();
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
	PlayerController->Possess(Character);

	Character->InitializeAbilitySystemForTest(0.f, 10.f);
	if (UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent())
	{
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UStaminaAttributeSet::GetMaxStaminaAttribute(), 50.f);
		AbilitySystem->SetNumericAttributeBase(UStaminaAttributeSet::GetStaminaAttribute(), 50.f);
	}

	UTestPlayerVitalsWidget* Widget = CreateWidget<UTestPlayerVitalsWidget>(PlayerController, UTestPlayerVitalsWidget::StaticClass());
	TestNotNull(TEXT("Vitals widget should be created"), Widget);
	Widget->NativeConstruct();

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
	PlayerController->Possess(Character);

	UTestStanceWidget* Widget = CreateWidget<UTestStanceWidget>(PlayerController, UTestStanceWidget::StaticClass());
	TestNotNull(TEXT("Stance widget should be created"), Widget);
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

#endif
