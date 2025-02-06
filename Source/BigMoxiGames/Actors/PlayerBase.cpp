// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"

#include "BigMoxiGames/GameplayFramework/Characters/GameplayCharacter.h"
#include "BigMoxiGames/GameplayFramework/GameStates/FlagGameState.h"
#include "BigMoxiGames/Pickables/Flag.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerBase::APlayerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	BaseCollisionArea = CreateDefaultSubobject<UBoxComponent>("BaseCollisionArea");
	RootComponent = BaseCollisionArea;
	BaseCollisionArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	BaseCollisionArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BaseCollisionArea->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnSomethingOverlap);

	bReplicates = true;
	bAlwaysRelevant = true;
}

void APlayerBase::OnSomethingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;
	
	if (OtherActor->IsA(AGameplayCharacter::StaticClass()))
	{
		TArray<AActor*> OutActors;
		OtherActor->GetAttachedActors(OutActors);

		for (AActor* CurrentActor : OutActors)
		{
			if (AFlag* Flag = Cast<AFlag>(CurrentActor))
			{
				if (AFlagGameState* GameState = Cast<AFlagGameState>(UGameplayStatics::GetGameState(GetWorld())))
					GameState->OnTeamWin(BaseTeam);

				Flag->ReturnToInitialPosition();

				break;
			}
		}
	}
}
