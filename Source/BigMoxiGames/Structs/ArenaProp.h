// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaProp.generated.h"


USTRUCT()
struct FArenaProp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* PropMesh = nullptr;

	UPROPERTY(EditAnywhere)
	FVector PropScale = FVector(1.f);

	UPROPERTY(EditAnywhere)
	int NumberOfProps = 1;

	UPROPERTY(EditAnywhere)
	float PropsSpawnBoundsLimiter = 0.75f;
};
