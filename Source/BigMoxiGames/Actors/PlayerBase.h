// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerBase.generated.h"

enum class EPlayerTeam : uint8;
class UBoxComponent;

UCLASS()
class BIGMOXIGAMES_API APlayerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* BaseCollisionArea = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EPlayerTeam BaseTeam;

public:
	UFUNCTION()
	void OnSomethingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Sets default values for this actor's properties
	APlayerBase();
};
