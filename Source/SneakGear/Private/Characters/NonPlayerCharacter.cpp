#include "Characters/NonPlayerCharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Misc/DataValidation.h"

ANonPlayerCharacter::ANonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!bSpawnAtLevelStart)
	{
		if (HasAuthority())
		{
			Destroy();
			return;
		}

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
		return;
	}

	if (HasAuthority() && Controller == nullptr)
	{
		SpawnDefaultController();
	}
}

EDataValidationResult ANonPlayerCharacter::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!BehaviorTreeAsset)
	{
		Context.AddError(FText::FromString(TEXT("Non-player character must have a BehaviorTreeAsset assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!BlackboardAsset && (!BehaviorTreeAsset || !BehaviorTreeAsset->BlackboardAsset))
	{
		Context.AddError(FText::FromString(TEXT("Non-player character must define a blackboard either explicitly or through its BehaviorTreeAsset.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
