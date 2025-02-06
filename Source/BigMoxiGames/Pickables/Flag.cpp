// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"

#include "TP_PickUpComponent.h"
#include "Misc/GeneralFunctionLibrary.h"

// Sets default values
AFlag::AFlag()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>("FlagMesh");
	FlagMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootComponent = FlagMesh;
	
	PickupComponent = CreateDefaultSubobject<UTP_PickUpComponent>("PickupComponent");
	PickupComponent->SetupAttachment(RootComponent);
	PickupComponent->OnPickUp.AddUniqueDynamic(this, &ThisClass::AttachFlag);

	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);
}

void AFlag::BeginPlay()
{
	Super::BeginPlay();

	InitialPosition = GetActorLocation();
}

void AFlag::OnParentDestroyed(AActor* DestroyedActor)
{
	DestroyedActor->OnDestroyed.RemoveAll(this);
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Waiting till next tick to register so the flag can back to initial position
	GetWorldTimerManager().SetTimerForNextTick(PickupComponent, &UTP_PickUpComponent::RegisterOverlap);
}

void AFlag::AttachFlag(AGameplayCharacter* PickUpCharacter)
{
	if (GetAttachParentActor()) // If already attached
		return;
	
	if (AttachToComponent(PickUpCharacter->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale))
	{
		PickUpCharacter->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnParentDestroyed);
	} else
	{
		GPrintError("Flag attachment fail");
	}
}

void AFlag::ReturnToInitialPosition()
{
	GetAttachParentActor()->OnDestroyed.RemoveAll(this);
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	SetActorLocation(InitialPosition);

	// Waiting till next tick to register so the flag can back to initial position
	GetWorldTimerManager().SetTimerForNextTick(PickupComponent, &UTP_PickUpComponent::RegisterOverlap);
}
