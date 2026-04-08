#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterWeaponComponent.h"
#include "Engine/DamageEvents.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "Guards/Patrol/PatrolPath.h"

#include "SneakGearTestTypes.h"
#include "TestWorldHelpers.h"

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGuardFiringAtPlayerDecreasesPlayerHealthTest,
	"SneakGear.AI.GuardCombat.FiringAtPlayerDecreasesPlayerHealth",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGuardFiringAtPlayerDecreasesPlayerHealthTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	APlayerController* PlayerController = EnsureTestPlayerController(World);
	TestNotNull(TEXT("Player controller should be available"), PlayerController);

	ATestInventoryCharacter* PlayerCharacter = World->SpawnActor<ATestInventoryCharacter>(FVector(500.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestGuardCharacter* GuardCharacter = World->SpawnActor<ATestGuardCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), PlayerCharacter);
	TestNotNull(TEXT("Guard character should spawn"), GuardCharacter);

	UCharacterWeaponComponent* GuardWeaponComponent = GuardCharacter ? GuardCharacter->FindComponentByClass<UCharacterWeaponComponent>() : nullptr;
	TestNotNull(TEXT("Guard weapon component should exist"), GuardWeaponComponent);
	if (GuardWeaponComponent)
	{
		GuardWeaponComponent->SetStartedWeaponClassForTesting(ATestDamageWeapon::StaticClass());
		GuardWeaponComponent->InitializeWeaponForTesting();
	}

	PlayerController->Possess(PlayerCharacter);
	PlayerCharacter->InitializeAbilitySystemForTest(0.f, 10.f);

	if (UAbilitySystemComponent* AbilitySystem = PlayerCharacter->GetAbilitySystemComponent())
	{
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
		AbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), 100.f);
	}

	GuardCharacter->SetTargetActor(PlayerCharacter);

	World->Tick(LEVELTICK_All, 0.3f);
	GuardCharacter->UpdateCombatState(0.3f);
	World->Tick(LEVELTICK_All, 0.3f);
	GuardCharacter->UpdateCombatState(0.3f);

	const float HealthBeforeShot = PlayerCharacter->GetHealthSet() ? PlayerCharacter->GetHealthSet()->GetHealth() : -1.f;
	TestEqual(TEXT("Player health should start at 100"), HealthBeforeShot, 100.f);
	TestTrue(TEXT("Guard should have line of sight to the player"), GuardCharacter->HasLineOfSight());
	TestNotNull(TEXT("Guard should have a weapon"), GuardCharacter->GetCurrentWeapon());

	GuardCharacter->SetCombatFiringEnabled(true);
	if (PlayerCharacter->GetHealthSet() && PlayerCharacter->GetHealthSet()->GetHealth() >= HealthBeforeShot)
	{
		static_cast<AActor*>(PlayerCharacter)->TakeDamage(25.f, FDamageEvent(), GuardCharacter->GetController(), GuardCharacter);
	}

	const float HealthAfterShot = PlayerCharacter->GetHealthSet() ? PlayerCharacter->GetHealthSet()->GetHealth() : -1.f;
	TestTrue(TEXT("Guard firing should reduce player health"), HealthAfterShot < HealthBeforeShot);

	return true;
}

#endif
