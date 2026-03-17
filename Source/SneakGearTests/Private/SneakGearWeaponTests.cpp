#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "SneakGearTestTypes.h"
#include "TestWorldHelpers.h"

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

#endif
