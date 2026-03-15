#include "Items/ScannerItemDefinition.h"

#include "Guards/GuardCharacter.h"
#include "Guards/GuardManagerSubsystem.h"
#include "Misc/DataValidation.h"
#include "Player/PlayerCharacterBase.h"
#include "UI/EventLogSubsystem.h"

UScannerItemDefinition::UScannerItemDefinition()
{
	SlotType = EPlayerItemSlot::Utility;
}

EDataValidationResult UScannerItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (ScanRadius <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("ScanRadius must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

bool UScannerItemDefinition::ExecuteUtility(APlayerCharacterBase* PlayerCharacter,
                                            UPlayerInventoryComponent* InventoryComponent) const
{
	(void)InventoryComponent;

	if (!PlayerCharacter)
	{
		return false;
	}

	UWorld* World = PlayerCharacter->GetWorld();
	if (!World)
	{
		return false;
	}

	UGuardManagerSubsystem* GuardManager = World->GetSubsystem<UGuardManagerSubsystem>();
	if (!GuardManager)
	{
		return false;
	}

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	int32 DetectedGuards = 0;
	float NearestDistanceMeters = TNumericLimits<float>::Max();

	for (AGuardCharacter* Guard : GuardManager->GetAliveGuards())
	{
		if (!Guard)
		{
			continue;
		}

		const float Distance = FVector::Dist(PlayerLocation, Guard->GetActorLocation());
		if (Distance > ScanRadius)
		{
			continue;
		}

		DetectedGuards++;
		NearestDistanceMeters = FMath::Min(NearestDistanceMeters, Distance / 100.f);
	}

	if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
	{
		const FText Message = DetectedGuards > 0
			? FText::Format(
				NSLOCTEXT("SneakGear", "ScannerDetectedGuards", "Scanner detected {0} guard(s). Nearest contact: {1}m"),
				FText::AsNumber(DetectedGuards),
				FText::AsNumber(FMath::RoundToInt(NearestDistanceMeters)))
			: NSLOCTEXT("SneakGear", "ScannerNoGuards", "Scanner found no guards nearby");

		EventLog->AddEvent(Message, EGameEventCategory::Info);
	}

	return true;
}
