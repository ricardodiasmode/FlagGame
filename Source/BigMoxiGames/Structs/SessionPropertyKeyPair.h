// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineKeyValuePair.h"
#include "SessionPropertyKeyPair.generated.h"

USTRUCT(BlueprintType)
struct FSessionPropertyKeyPair
{
	GENERATED_BODY()

	FName Key;
	
	FVariantData Data;
};