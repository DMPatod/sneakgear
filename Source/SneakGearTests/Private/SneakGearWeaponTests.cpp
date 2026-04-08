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

	TestEqual(TEXT("Weapon should broadcast a fire event when aim data comes from the owner"), BroadcastCount, 1);

	UTestWeaponFireModeComponent* FireMode = Weapon->GetTestFireMode();
	TestNotNull(TEXT("Test fire mode should be initialized"), FireMode);
	TestEqual(TEXT("Fire mode should execute once"), FireMode->FireCount, 1);
	TestEqual(TEXT("Aim origin should come from the aim provider"), FireMode->LastContext.AimOrigin, OwnerPawn->AimOrigin);
	TestEqual(TEXT("Aim direction should come from the aim provider"),
		FireMode->LastContext.AimDirection, OwnerPawn->AimDirection.GetSafeNormal());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseUsesAnimationDrivenFireIntervalTest,
	"SneakGear.Weapon.WeaponBase.UsesAnimationDrivenFireInterval",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseUsesAnimationDrivenFireIntervalTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ATestAimPawn* OwnerPawn = World->SpawnActor<ATestAimPawn>();
	ATestDelayedFireWeapon* Weapon = World->SpawnActor<ATestDelayedFireWeapon>();
	TestNotNull(TEXT("Aim provider pawn should spawn"), OwnerPawn);
	TestNotNull(TEXT("Delayed fire weapon should spawn"), Weapon);

	Weapon->SetOwner(OwnerPawn);
	Weapon->DispatchBeginPlay();

	UTestWeaponFireModeComponent* FireMode = Weapon->GetTestFireMode();
	TestNotNull(TEXT("Test fire mode should be initialized"), FireMode);

	Weapon->StartFire();
	TestEqual(TEXT("Weapon should fire immediately on start"), FireMode ? FireMode->FireCount : -1, 1);

	for (int32 Index = 0; Index < 99; ++Index)
	{
		World->Tick(LEVELTICK_All, 0.1f);
		Weapon->Tick(0.1f);
	}
	TestEqual(TEXT("Weapon should not fire again before the 10 second interval completes"), FireMode ? FireMode->FireCount : -1, 1);

	for (int32 Index = 0; Index < 2; ++Index)
	{
		World->Tick(LEVELTICK_All, 0.1f);
		Weapon->Tick(0.1f);
	}
	TestEqual(TEXT("Weapon should fire again after the 10 second interval completes"), FireMode ? FireMode->FireCount : -1, 2);

	Weapon->StopFire();
	return true;
}

#endif
