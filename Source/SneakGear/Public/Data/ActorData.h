// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ActorData.generated.h"

UENUM()
enum class EActorType : uint8
{
	None UMETA(DisplayName = "None"),
	NPC UMETA(DisplayName = "NPC"),
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy")
};

USTRUCT(BlueprintType)
struct FActorText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;
};

USTRUCT(BlueprintType)
struct FActorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Actor Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Actor Data")
	EActorType ActorType = EActorType::None;

	UPROPERTY(EditAnywhere, Category = "Actor Data")
	FActorText Text;
};
