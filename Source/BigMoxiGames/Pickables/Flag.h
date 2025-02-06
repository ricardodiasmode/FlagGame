// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/GameplayFramework/Characters/GameplayCharacter.h"
#include "GameFramework/Actor.h"
#include "Flag.generated.h"

class UTP_PickUpComponent;

UCLASS()
class BIGMOXIGAMES_API AFlag : public AActor
{
	GENERATED_BODY()

private:
	FVector InitialPosition;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Gameplay, meta=(AllowPrivateAccess = "true"))
	UTP_PickUpComponent* PickupComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Gameplay, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* FlagMesh = nullptr;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void AttachFlag(AGameplayCharacter* PickUpCharacter);
	
	void ReturnToInitialPosition();

	// Sets default values for this actor's properties
	AFlag();

};
