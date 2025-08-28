// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorData.h"
#include "Engine/DataAsset.h"
#include "ActorDefinition.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SNEAKGEAR_API UActorDefinition : public UDataAsset
{
public:
	virtual UActorDefinition* CreateActorCopy() const;

protected:
	UPROPERTY(EditAnywhere)
	FName ID;

	UPROPERTY(EditAnywhere)
	EActorType ActorType;

	UPROPERTY(EditAnywhere)
	FActorText ActorText;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> WorldMesh;
	
private:
	GENERATED_BODY()
};
