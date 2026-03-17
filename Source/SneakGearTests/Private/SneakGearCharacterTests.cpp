#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Items/MedkitItemDefinition.h"
#include "Items/ScannerItemDefinition.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Game/GAS/HealthAttributeSet.h"

#include "SneakGearTestTypes.h"
#include "TestWorldHelpers.h"

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

#endif
