#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "NonPlayerCharacter.generated.h"

class UBehaviorTree;
class UBlackboardData;

UCLASS(Abstract)
class SNEAKGEAR_API ANonPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ANonPlayerCharacter();

	UFUNCTION(BlueprintPure, Category="AI|BehaviorTree")
	UBehaviorTree* GetBehaviorTreeAsset() const
	{
		return BehaviorTreeAsset;
	}

	UFUNCTION(BlueprintPure, Category="AI|BehaviorTree")
	UBlackboardData* GetBlackboardAsset() const
	{
		return BlackboardAsset;
	}

protected:
	UPROPERTY(EditInstanceOnly, Category="Spawn")
	bool bSpawnAtLevelStart = true;

	UPROPERTY(EditDefaultsOnly, Category="AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category="AI|BehaviorTree")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	virtual void BeginPlay() override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
};
