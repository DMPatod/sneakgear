#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Game/GAS/HealthAttributeSet.h"
#include "Game/GAS/StaminaAttributeSet.h"
#include "UI/EventLogSubsystem.h"

#include "SneakGearTestTypes.h"
#include "TestWorldHelpers.h"

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
