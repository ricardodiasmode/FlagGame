// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FirstPersonAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API UFirstPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	uint8 HasWeapon : 1 = false;
};
