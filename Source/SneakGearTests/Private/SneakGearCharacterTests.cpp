#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Items/MedkitItemDefinition.h"
#include "Items/ScannerItemDefinition.h"
#include "Components/Cover/CoverStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/SneakGearPlayerAnimInstance.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Game/GAS/HealthAttributeSet.h"

#include "SneakGearTestTypes.h"
#include "TestCharacters.h"
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

bool ConfigureCharacterForVaultTest(UWorld* World, ASneakGearPlayerCharacter* Character, FAutomationTestBase& Test,
	float ObstacleHeight = 70.f, float StandingMaxHeight = 60.f, float CrouchingMaxHeight = 80.f)
{
	Test.TestNotNull(TEXT("Test world should exist for cover setup"), World);
	Test.TestNotNull(TEXT("Player character should exist for cover setup"), Character);
	if (!World || !Character)
	{
		return false;
	}

	ATestCoverObstacle* Floor = World->SpawnActor<ATestCoverObstacle>(FVector(230.f, 0.f, -25.f), FRotator::ZeroRotator);
	Test.TestNotNull(TEXT("Test floor should spawn"), Floor);
	if (!Floor)
	{
		return false;
	}

	Floor->SetBoxExtent(FVector(160.f, 160.f, 25.f));

	Character->SetActorLocation(FVector(0.f, 0.f, 96.f));
	Character->SetActorRotation(FRotator::ZeroRotator);

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		Move->SetMovementMode(MOVE_Walking);
	}

	UCoverStateComponent* CoverState = Character->FindComponentByClass<UCoverStateComponent>();
	Test.TestNotNull(TEXT("Cover state component should exist"), CoverState);
	if (!CoverState)
	{
		return false;
	}

	FCoverHit CoverHit;
	CoverHit.bValid = true;
	CoverHit.ImpactPoint = FVector(100.f, 0.f, 0.f);
	CoverHit.Normal = FVector(-1.f, 0.f, 0.f);
	CoverHit.Tangent = FVector(0.f, 1.f, 0.f);
	CoverHit.SnapLocation = FVector(58.f, 0.f, 96.f);
	CoverHit.SnapRotation = FRotator::ZeroRotator;
	CoverHit.ObstacleHeight = ObstacleHeight;
	CoverState->TestSetLockedCover(CoverHit);
	CoverState->TestSetVaultMaxObstacleHeights(StandingMaxHeight, CrouchingMaxHeight);

	TickWorld(World, 2);
	Test.TestTrue(TEXT("Character should report locked cover after test setup"), Character->IsInCover());
	return Character->IsInCover();
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
	TestTrue(TEXT("Character should be destroyed when health reaches zero"), Character->IsActorBeingDestroyed());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerAnimInstanceMirrorsWeaponStateTest,
	"SneakGear.Player.SneakGearPlayerAnimInstance.MirrorsWeaponState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerAnimInstanceMirrorsWeaponStateTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	ATestPickupActor* PickupActor = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UPlayerItemDefinition* WeaponDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());

	TestNotNull(TEXT("Player character should spawn"), Character);
	TestNotNull(TEXT("Pickup actor should spawn"), PickupActor);
	TestNotNull(TEXT("Weapon definition should be created"), WeaponDefinition);
	TestNotNull(TEXT("Player character mesh should exist"), Character ? Character->GetMesh() : nullptr);
	USneakGearPlayerAnimInstance* AnimInstance = Character && Character->GetMesh()
		? NewObject<USneakGearPlayerAnimInstance>(Character->GetMesh())
		: nullptr;
	TestNotNull(TEXT("Anim instance should be created"), AnimInstance);

	WeaponDefinition->ItemId = TEXT("AnimStatePrimaryRifle");
	WeaponDefinition->DisplayName = FText::FromString(TEXT("Anim State Primary Rifle"));
	WeaponDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	WeaponDefinition->WeaponClass = ATestWeapon::StaticClass();
	PickupActor->GetPickupComponent()->SetItemDefinitionForTest(WeaponDefinition);

	TestTrue(TEXT("Weapon pickup should succeed"), Character->GetItemComponent()->PickUpFromFloor(PickupActor));
	Character->GetItemComponent()->SetAmmoReserve(EAmmoType::Light, 3, 10);

	AWeaponBase* Weapon = Character->GetCurrentWeapon();
	TestNotNull(TEXT("Runtime weapon should exist"), Weapon);
	if (Weapon)
	{
		Weapon->DispatchBeginPlay();
	}

	AnimInstance->RefreshFromCharacterForTest(Character);
	TestEqual(TEXT("Anim instance should expose idle weapon state"),
		AnimInstance->WeaponState, EPlayerInventoryWeaponState::Idle);
	TestFalse(TEXT("Idle weapon should not expose fire pending"), AnimInstance->bWeaponFirePending);
	TestFalse(TEXT("Idle weapon should not expose reload"), AnimInstance->bIsReloading);

	Character->GetItemComponent()->StartActiveWeaponFire();
	AnimInstance->RefreshFromCharacterForTest(Character);
	TestEqual(TEXT("Anim instance should expose fire-requested weapon state"),
		AnimInstance->WeaponState, EPlayerInventoryWeaponState::FireRequested);
	TestTrue(TEXT("Fire-requested weapon state should drive fire pending"), AnimInstance->bWeaponFirePending);
	TestFalse(TEXT("Fire-requested weapon state should not drive reload"), AnimInstance->bIsReloading);

	Character->GetItemComponent()->StopActiveWeaponFire();
	TestTrue(TEXT("Fire notify should consume the queued shot"),
		Character->GetItemComponent()->NotifyActiveWeaponFireAnimation());
	AnimInstance->RefreshFromCharacterForTest(Character);
	TestEqual(TEXT("Anim instance should return to idle after released shot"),
		AnimInstance->WeaponState, EPlayerInventoryWeaponState::Idle);

	TestTrue(TEXT("Reload should start after one shot"),
		Character->GetItemComponent()->ReloadActiveWeapon());
	AnimInstance->RefreshFromCharacterForTest(Character);
	TestEqual(TEXT("Anim instance should expose reloading weapon state"),
		AnimInstance->WeaponState, EPlayerInventoryWeaponState::Reloading);
	TestFalse(TEXT("Reloading weapon state should not drive fire pending"), AnimInstance->bWeaponFirePending);
	TestTrue(TEXT("Reloading weapon state should drive reload"), AnimInstance->bIsReloading);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterWeaponPickupEquipsWeaponTest,
	"SneakGear.Player.SneakGearPlayerCharacter.WeaponPickupEquipsWeapon",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterWeaponPickupEquipsWeaponTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	ATestPickupActor* PickupActor = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UPlayerItemDefinition* WeaponDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());

	TestNotNull(TEXT("Player character should spawn"), Character);
	TestNotNull(TEXT("Pickup actor should spawn"), PickupActor);
	TestNotNull(TEXT("Weapon definition should be created"), WeaponDefinition);

	WeaponDefinition->ItemId = TEXT("PickedUpPrimaryRifle");
	WeaponDefinition->DisplayName = FText::FromString(TEXT("Picked Up Primary Rifle"));
	WeaponDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	WeaponDefinition->WeaponClass = ATestWeapon::StaticClass();
	PickupActor->GetPickupComponent()->SetItemDefinitionForTest(WeaponDefinition);

	TestTrue(TEXT("Nearby pickup input should collect the weapon"), Character->GetItemComponent()->TryPickUpNearbyFloorItem(150.f));
	TestTrue(TEXT("Primary weapon slot should contain the picked-up item"),
		Character->GetItemComponent()->HasItem(EPlayerItemSlot::PrimaryWeapon));
	TestTrue(TEXT("Weapon should remain equipped after pickup"),
		Character->GetItemComponent()->IsWeaponEquipped());
	TestEqual(TEXT("Picked-up primary weapon should become the active weapon slot"),
		Character->GetItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);
	TestNotNull(TEXT("Picking up a weapon should leave the character with an active runtime weapon"),
		Character->GetCurrentWeapon());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterSecondaryPickupKeepsPrimaryActiveTest,
	"SneakGear.Player.SneakGearPlayerCharacter.SecondaryPickupKeepsPrimaryActive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterSecondaryPickupKeepsPrimaryActiveTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	ATestPickupActor* PrimaryPickup = World->SpawnActor<ATestPickupActor>(FVector(50.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestPickupActor* SecondaryPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UPlayerItemDefinition* PrimaryWeaponDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	UPlayerItemDefinition* SecondaryWeaponDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());

	TestNotNull(TEXT("Player character should spawn"), Character);
	TestNotNull(TEXT("Primary weapon pickup should spawn"), PrimaryPickup);
	TestNotNull(TEXT("Secondary weapon pickup should spawn"), SecondaryPickup);
	TestNotNull(TEXT("Primary weapon definition should be created"), PrimaryWeaponDefinition);
	TestNotNull(TEXT("Secondary weapon definition should be created"), SecondaryWeaponDefinition);

	PrimaryWeaponDefinition->ItemId = TEXT("StartingPrimaryRifle");
	PrimaryWeaponDefinition->DisplayName = FText::FromString(TEXT("Starting Primary Rifle"));
	PrimaryWeaponDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	PrimaryWeaponDefinition->WeaponClass = ATestWeapon::StaticClass();
	PrimaryPickup->GetPickupComponent()->SetItemDefinitionForTest(PrimaryWeaponDefinition);

	SecondaryWeaponDefinition->ItemId = TEXT("PickedUpSecondarySMG");
	SecondaryWeaponDefinition->DisplayName = FText::FromString(TEXT("Picked Up Secondary SMG"));
	SecondaryWeaponDefinition->SlotType = EPlayerItemSlot::SecondaryWeapon;
	SecondaryWeaponDefinition->WeaponClass = ATestWeapon::StaticClass();
	SecondaryPickup->GetPickupComponent()->SetItemDefinitionForTest(SecondaryWeaponDefinition);

	TestTrue(TEXT("Initial primary pickup should succeed"), Character->GetItemComponent()->PickUpFromFloor(PrimaryPickup));
	TestEqual(TEXT("Primary weapon should be the active slot after initial pickup"),
		Character->GetItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);

	TestTrue(TEXT("Secondary pickup should succeed"), Character->GetItemComponent()->TryPickUpNearbyFloorItem(150.f));
	TestTrue(TEXT("Secondary slot should contain the picked-up weapon"),
		Character->GetItemComponent()->HasItem(EPlayerItemSlot::SecondaryWeapon));
	TestEqual(TEXT("Picking up a secondary weapon should keep the active slot on the primary weapon"),
		Character->GetItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);
	TestTrue(TEXT("A weapon should still be equipped after the secondary pickup"),
		Character->GetItemComponent()->IsWeaponEquipped());
	TestNotNull(TEXT("Current weapon should remain valid after the secondary pickup"),
		Character->GetCurrentWeapon());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterPrimarySwapEquipsPickedUpWeaponTest,
	"SneakGear.Player.SneakGearPlayerCharacter.PrimarySwapEquipsPickedUpWeapon",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterPrimarySwapEquipsPickedUpWeaponTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	ATestPickupActor* InitialPrimaryPickup = World->SpawnActor<ATestPickupActor>(FVector(50.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestPickupActor* InitialSecondaryPickup = World->SpawnActor<ATestPickupActor>(FVector(60.f, 0.f, 0.f), FRotator::ZeroRotator);
	ATestPickupActor* SwapPrimaryPickup = World->SpawnActor<ATestPickupActor>(FVector(75.f, 0.f, 0.f), FRotator::ZeroRotator);
	UPlayerItemDefinition* InitialPrimaryDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	UPlayerItemDefinition* InitialSecondaryDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());
	UPlayerItemDefinition* SwapPrimaryDefinition = NewObject<UPlayerItemDefinition>(GetTransientPackage());

	TestNotNull(TEXT("Player character should spawn"), Character);
	TestNotNull(TEXT("Initial primary pickup should spawn"), InitialPrimaryPickup);
	TestNotNull(TEXT("Initial secondary pickup should spawn"), InitialSecondaryPickup);
	TestNotNull(TEXT("Swap primary pickup should spawn"), SwapPrimaryPickup);
	TestNotNull(TEXT("Initial primary definition should be created"), InitialPrimaryDefinition);
	TestNotNull(TEXT("Initial secondary definition should be created"), InitialSecondaryDefinition);
	TestNotNull(TEXT("Swap primary definition should be created"), SwapPrimaryDefinition);

	InitialPrimaryDefinition->ItemId = TEXT("InitialPrimaryRifle");
	InitialPrimaryDefinition->DisplayName = FText::FromString(TEXT("Initial Primary Rifle"));
	InitialPrimaryDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	InitialPrimaryDefinition->WeaponClass = ATestWeapon::StaticClass();
	InitialPrimaryPickup->GetPickupComponent()->SetItemDefinitionForTest(InitialPrimaryDefinition);

	InitialSecondaryDefinition->ItemId = TEXT("InitialSecondarySMG");
	InitialSecondaryDefinition->DisplayName = FText::FromString(TEXT("Initial Secondary SMG"));
	InitialSecondaryDefinition->SlotType = EPlayerItemSlot::SecondaryWeapon;
	InitialSecondaryDefinition->WeaponClass = ATestWeapon::StaticClass();
	InitialSecondaryPickup->GetPickupComponent()->SetItemDefinitionForTest(InitialSecondaryDefinition);

	SwapPrimaryDefinition->ItemId = TEXT("SwapPrimaryRifle");
	SwapPrimaryDefinition->DisplayName = FText::FromString(TEXT("Swap Primary Rifle"));
	SwapPrimaryDefinition->SlotType = EPlayerItemSlot::PrimaryWeapon;
	SwapPrimaryDefinition->WeaponClass = ATestWeapon::StaticClass();
	SwapPrimaryPickup->GetPickupComponent()->SetItemDefinitionForTest(SwapPrimaryDefinition);

	TestTrue(TEXT("Initial primary pickup should succeed"), Character->GetItemComponent()->PickUpFromFloor(InitialPrimaryPickup));
	TestTrue(TEXT("Initial secondary pickup should succeed"), Character->GetItemComponent()->PickUpFromFloor(InitialSecondaryPickup));
	TestTrue(TEXT("Both weapon slots should be populated before the swap"),
		Character->GetItemComponent()->HasItem(EPlayerItemSlot::PrimaryWeapon) &&
		Character->GetItemComponent()->HasItem(EPlayerItemSlot::SecondaryWeapon));
	TestEqual(TEXT("Primary weapon should still be active before the swap"),
		Character->GetItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);

	Character->TestTriggerNearbyPickupHoldInput();

	const FPlayerInventoryItem EquippedPrimary = Character->GetItemComponent()->GetItem(EPlayerItemSlot::PrimaryWeapon);
	TestEqual(TEXT("Primary slot should now contain the swapped-in floor weapon"),
		EquippedPrimary.ItemId,
		SwapPrimaryDefinition->ItemId);
	TestEqual(TEXT("Swapping in a new primary weapon should make the primary slot active"),
		Character->GetItemComponent()->GetActiveWeaponSlot(),
		EPlayerItemSlot::PrimaryWeapon);
	TestNotNull(TEXT("Current weapon should remain valid after swapping the primary weapon"),
		Character->GetCurrentWeapon());
	TestEqual(TEXT("The swapped-out primary weapon should be left on the floor"),
		SwapPrimaryPickup->GetPickupComponent()->GetItemDefinition(),
		InitialPrimaryDefinition);

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterStandingCoverAllowsVaultTest,
	"SneakGear.Player.SneakGearPlayerCharacter.StandingCoverAllowsVault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterStandingCoverAllowsVaultTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), Character);
	if (!World || !Character)
	{
		return false;
	}

	if (!ConfigureCharacterForVaultTest(World, Character, *this, 50.f, 60.f, 80.f))
	{
		return false;
	}

	TestEqual(TEXT("Character should still be standing before the vault check"), Character->Stance, EStance::Standing);
	TestTrue(TEXT("Standing cover should expose vault availability when the obstacle is under the standing max height"),
		Character->IsVaultAvailable());

	Character->TestTriggerJumpInput();

	TestTrue(TEXT("Jump input should start vaulting from standing when the obstacle fits the standing max"),
		Character->IsVaulting());
	TestEqual(TEXT("Standing vault should keep the stance on standing"), Character->Stance, EStance::Standing);
	TestFalse(TEXT("Vaulting from standing should exit locked cover"), Character->IsInCover());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterCrouchCoverAllowsVaultTest,
	"SneakGear.Player.SneakGearPlayerCharacter.CrouchCoverAllowsVault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterCrouchCoverAllowsVaultTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), Character);
	if (!World || !Character)
	{
		return false;
	}

	if (!ConfigureCharacterForVaultTest(World, Character, *this, 70.f, 60.f, 80.f))
	{
		return false;
	}

	TestFalse(TEXT("Standing should not be able to vault an obstacle above the standing max height"),
		Character->IsVaultAvailable());

	Character->SetStance(EStance::Crouching);
	TickWorld(World, 2);

	TestEqual(TEXT("Character should be crouching before the vault attempt"), Character->Stance, EStance::Crouching);
	TestTrue(TEXT("Crouching should expose vault availability when the obstacle fits the crouching max height"),
		Character->IsVaultAvailable());

	Character->TestTriggerJumpInput();

	TestTrue(TEXT("Jump input should start vaulting when crouched in valid cover"), Character->IsVaulting());
	TestEqual(TEXT("Vault takeoff should force the player back to standing"), Character->Stance, EStance::Standing);
	TestFalse(TEXT("Vaulting should exit the locked cover state"), Character->IsInCover());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSneakGearPlayerCharacterProneCannotVaultTest,
	"SneakGear.Player.SneakGearPlayerCharacter.ProneCannotVault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSneakGearPlayerCharacterProneCannotVaultTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateTestWorld();
	TestNotNull(TEXT("Test world should be created"), World);

	ASneakGearPlayerCharacter* Character = World->SpawnActor<ASneakGearPlayerCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("Player character should spawn"), Character);
	if (!World || !Character)
	{
		return false;
	}

	if (!ConfigureCharacterForVaultTest(World, Character, *this, 50.f, 60.f, 80.f))
	{
		return false;
	}

	Character->SetStance(EStance::Prone);

	TestEqual(TEXT("Character should be prone before the vault check"), Character->Stance, EStance::Prone);
	TestFalse(TEXT("Prone should never expose vault availability"), Character->IsVaultAvailable());

	Character->TestTriggerJumpInput();

	TestFalse(TEXT("Jump input while prone should not start vaulting"), Character->IsVaulting());

	return true;
}

#endif
