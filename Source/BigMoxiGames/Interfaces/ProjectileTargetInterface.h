// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileTargetInterface.generated.h"

class UAbilitySystemComponent;

UINTERFACE(Blueprintable)
class BIGMOXIGAMES_API UProjectileTargetInterface : public UInterface
{
	GENERATED_BODY()
};

class IProjectileTargetInterface
{    
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() = 0;	
};
