// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Structs/SessionPropertyKeyPair.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlagGameSessionsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API UFlagGameSessionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static FSessionPropertyKeyPair MakeLiteralSessionPropertyString(FName Key, FString Value)
	{
		FSessionPropertyKeyPair Prop;
		Prop.Key = Key;
		Prop.Data.SetValue(Value);
		return Prop;
	}
};
