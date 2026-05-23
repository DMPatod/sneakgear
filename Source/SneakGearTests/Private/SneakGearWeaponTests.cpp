#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

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
	Weapon->NotifyFireAnimation();

	TestEqual(TEXT("Weapon should broadcast a fire event when aim data comes from the owner"), BroadcastCount, 1);

	UTestWeaponFireModeComponent* FireMode = Weapon->GetTestFireMode();
	TestNotNull(TEXT("Test fire mode should be initialized"), FireMode);
	TestEqual(TEXT("Fire mode should execute once"), FireMode->FireCount, 1);
	TestEqual(TEXT("Aim origin should come from the aim provider"), FireMode->LastContext.AimOrigin, OwnerPawn->AimOrigin);
	TestEqual(TEXT("Aim direction should come from the aim provider"),
		FireMode->LastContext.AimDirection, OwnerPawn->AimDirection.GetSafeNormal());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseUsesAnimationDrivenFireNotifiesTest,
	"SneakGear.Weapon.WeaponBase.UsesAnimationDrivenFireNotifies",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseUsesAnimationDrivenFireNotifiesTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestAimPawn* OwnerPawn = World->SpawnActor<ATestAimPawn>();
	ATestWeapon* Weapon = World->SpawnActor<ATestWeapon>();
	TestNotNull(TEXT("Aim provider pawn should spawn"), OwnerPawn);
	TestNotNull(TEXT("Test weapon should spawn"), Weapon);

	Weapon->SetOwner(OwnerPawn);
	Weapon->DispatchBeginPlay();

	UTestWeaponFireModeComponent* FireMode = Weapon->GetTestFireMode();
	TestNotNull(TEXT("Test fire mode should be initialized"), FireMode);

	Weapon->StartFire();
	TestTrue(TEXT("Fire notify should execute the requested shot"), Weapon->NotifyFireAnimation());
	TestEqual(TEXT("Weapon should fire when the animation notify is received"), FireMode ? FireMode->FireCount : -1, 1);
	TestTrue(TEXT("Continuous fire should request the next animation notify immediately"), Weapon->IsFireNotifyPending());
	TestTrue(TEXT("Second fire notify should execute the next shot"), Weapon->NotifyFireAnimation());
	TestEqual(TEXT("Weapon should fire again when the next animation notify is received"), FireMode ? FireMode->FireCount : -1, 2);

	Weapon->StopFire();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHitscanWeaponBaseUsesPrimaryFireModeComponentTest,
	"SneakGear.Weapon.HitscanWeaponBase.UsesPrimaryFireModeComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHitscanWeaponBaseUsesPrimaryFireModeComponentTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestAimPawn* OwnerPawn = World->SpawnActor<ATestAimPawn>();
	ATestHitscanWeapon* Weapon = World->SpawnActor<ATestHitscanWeapon>();
	TestNotNull(TEXT("Aim pawn should spawn"), OwnerPawn);
	TestNotNull(TEXT("Test hitscan weapon should spawn"), Weapon);

	Weapon->SetOwner(OwnerPawn);
	Weapon->DispatchBeginPlay();

	int32 FiredEventCount = 0;
	Weapon->OnWeaponFiredEvent().AddLambda([&FiredEventCount]() { ++FiredEventCount; });

	Weapon->StartFire();
	Weapon->StopFire();
	Weapon->NotifyFireAnimation();

	TestEqual(TEXT("Hitscan weapon should broadcast the fired event"), FiredEventCount, 1);

	UTestWeaponFireModeComponent* FireMode = Weapon->GetTestFireMode();
	TestNotNull(TEXT("PrimaryFireMode component should be initialized after BeginPlay"), FireMode);
	TestEqual(TEXT("Hitscan fire should be delegated to the PrimaryFireMode component"),
		FireMode ? FireMode->FireCount : -1, 1);

	return true;
}

#endif
