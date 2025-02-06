// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Enums/ArenaAxis.h"
#include "BigMoxiGames/Structs/ArenaProp.h"
#include "GameFramework/Actor.h"
#include "Math/Axis.h"
#include "ArenaGenerator.generated.h"

class APlayerBase;

UCLASS()
class BIGMOXIGAMES_API AArenaGenerator : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UStaticMeshComponent*> SpawnedComponents;

protected:
	UPROPERTY()
	USceneComponent* ArenaRoot = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArenaMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
	FVector ArenaScale;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<APlayerBase> PlayerBaseClass;

	UPROPERTY(EditAnywhere, Category = "Config|Props")
	TArray<FArenaProp> ArenaProps;

	UPROPERTY(EditAnywhere, Category = "Config|PlayerStart")
	int NumberOfPlayerStarts = 3;

	UPROPERTY(EditAnywhere, Category = "Config|PlayerStart")
	float PlayerStartDistanceFromCenter = 1700.f;

	UPROPERTY(EditAnywhere, Category = "Config|PlayerStart")
	float DistanceBetweenEachPlayerStart = 80.f;

	UPROPERTY(EditAnywhere, Category = "Config|PlayerStart")
	EArenaAxis PlayerStartDistanceAxis = EArenaAxis::X;

private:
	void CreatePlayerBase();
	
	void DestroyAllCreatedObjects();
	
	void SetupSpawnedProp(UStaticMeshComponent* Prop,
		const float PropsSpawnBoundsLimiter,
		const FVector& Scale);
	
	void CreateNewComponents();
	
	void GetPlayerStartLocation(const int Index, FVector& DistanceOffset, FVector& RightOffsetDesiredLocation);
	
	void SpawnPlayerStartAtLocation(const FVector& DistanceOffset,
		const FVector& RightOffsetDesiredLocation,
		const bool IsRedTeam);

	void CreatePlayerStarts();


protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Sets default values for this actor's properties
	AArenaGenerator();
};
