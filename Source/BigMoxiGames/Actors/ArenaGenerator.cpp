// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGenerator.h"

#include "PlayerBase.h"
#include "BigMoxiGames/Enums/PlayerTeam.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/GeneralFunctionLibrary.h"


// Sets default values
AArenaGenerator::AArenaGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	ArenaRoot = CreateDefaultSubobject<USceneComponent>("ArenaRoot");
	RootComponent = ArenaRoot;

	ArenaMesh = CreateDefaultSubobject<UStaticMeshComponent>("ArenaMesh");
	ArenaMesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AArenaGenerator::DestroyAllCreatedObjects()
{
	while (!SpawnedComponents.IsEmpty())
	{
		if (IsValid(SpawnedComponents[0]))
			SpawnedComponents[0]->DestroyComponent();
		SpawnedComponents.RemoveAt(0);
	}

	TArray<AActor*> OutActors;
	// This call is not heavy since it uses hash buckets
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);
	while (!OutActors.IsEmpty())
	{
		if (IsValid(OutActors[0]))
			OutActors[0]->Destroy();
		OutActors.RemoveAt(0);
	}

	// This call is not heavy since it uses hash buckets
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), OutActors);
	while (!OutActors.IsEmpty())
	{
		if (IsValid(OutActors[0]))
			OutActors[0]->Destroy();
		OutActors.RemoveAt(0);
	}
}

void AArenaGenerator::SetupSpawnedProp(UStaticMeshComponent* Prop,
	const float PropsSpawnBoundsLimiter,
	const FVector& Scale)
{
	FVector UnusedOrigin;
	FVector ArenaBounds;
	float UnusedRadius;
	UKismetSystemLibrary::GetComponentBounds(ArenaMesh, UnusedOrigin, ArenaBounds, UnusedRadius);

	const float RandomX = FMath::RandRange(-ArenaBounds.X, ArenaBounds.X);
	const float RandomY = FMath::RandRange(-ArenaBounds.Y, ArenaBounds.Y);
	const FVector RandomRelativeLocationInBounds = FVector(RandomX*PropsSpawnBoundsLimiter, RandomY*PropsSpawnBoundsLimiter, 0.f);
	Prop->SetWorldLocation(ArenaMesh->GetComponentLocation() + RandomRelativeLocationInBounds);
	Prop->SetWorldScale3D(Scale);
}

void AArenaGenerator::CreateNewComponents()
{
	if (ArenaProps.Num() == 0)
		return;

	for (FArenaProp CurrentProp : ArenaProps)
	{
		for (int i = 0; i < CurrentProp.NumberOfProps; i++)
		{
			UStaticMeshComponent* SpawnedComponent = Cast<UStaticMeshComponent>(UGeneralFunctionLibrary::CreateComponentOnInstance(this, UStaticMeshComponent::StaticClass(), RootComponent));
			SpawnedComponent->SetStaticMesh(CurrentProp.PropMesh);
			SpawnedComponents.Add(SpawnedComponent);
			SpawnedComponent->SetIsReplicated(true);

			SetupSpawnedProp(SpawnedComponent,
				CurrentProp.PropsSpawnBoundsLimiter,
				CurrentProp.PropScale);
		}
	}
}

void AArenaGenerator::GetPlayerStartLocation(const int Index, FVector& DistanceOffset, FVector& RightOffsetDesiredLocation)
{
	static constexpr float PlayerUpOffset = 90.f;
	
	const bool IsXAxis = PlayerStartDistanceAxis == EArenaAxis::X;
	DistanceOffset = IsXAxis ? FVector(PlayerStartDistanceFromCenter, 0.f, 0.f) : FVector(0.f, PlayerStartDistanceFromCenter, 0.f);

	const FVector RightOffsetInitialLocation = IsXAxis ? FVector(0.f, DistanceBetweenEachPlayerStart * -NumberOfPlayerStarts/2, 0.f) : FVector(DistanceBetweenEachPlayerStart * -NumberOfPlayerStarts/2, 0.f, 0.f);
	const FVector RightOffset =  IsXAxis ? FVector(0.f, DistanceBetweenEachPlayerStart * (Index + 0.5), 0.f) : FVector(DistanceBetweenEachPlayerStart * (Index + 0.5), 0.f, 0.f);

	RightOffsetDesiredLocation = RightOffsetInitialLocation + RightOffset + FVector(0.f, 0.f, PlayerUpOffset);
}

void AArenaGenerator::SpawnPlayerStartAtLocation(const FVector& DistanceOffset,
	const FVector& RightOffsetDesiredLocation,
	const bool IsRedTeam)
{
	APlayerStart* SpawnedPlayerStart = GetWorld()->SpawnActor<APlayerStart>();
	SpawnedPlayerStart->SetActorLocation(DistanceOffset + RightOffsetDesiredLocation);

	FVector StartLocation = SpawnedPlayerStart->GetActorLocation();
	FVector EndLocation = ArenaMesh->GetComponentLocation();
	EndLocation.Z = StartLocation.Z;
	SpawnedPlayerStart->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation));
	SpawnedPlayerStart->Tags.Add(UEnum::GetValueAsName(IsRedTeam ? EPlayerTeam::Red : EPlayerTeam::Blue));
}

void AArenaGenerator::CreatePlayerStarts()
{	
	FVector DistanceOffset;
	FVector RightOffsetDesiredLocation;
	
	// Team1
	for(int i = 0; i < NumberOfPlayerStarts; i++)
	{
		GetPlayerStartLocation(i, DistanceOffset, RightOffsetDesiredLocation);

		SpawnPlayerStartAtLocation(DistanceOffset, RightOffsetDesiredLocation, true);
	}

	// Team2
	for(int i = 0; i < NumberOfPlayerStarts; i++)
	{
		GetPlayerStartLocation(i, DistanceOffset, RightOffsetDesiredLocation);
		
		SpawnPlayerStartAtLocation(-DistanceOffset, RightOffsetDesiredLocation, false);
	}
}

void AArenaGenerator::CreatePlayerBase()
{
	FVector DistanceOffset;
	FVector UnusedRightOffset;
	GetPlayerStartLocation(0, DistanceOffset, UnusedRightOffset);

	APlayerBase* RedPlayerBase = GetWorld()->SpawnActor<APlayerBase>(PlayerBaseClass);
	if (!RedPlayerBase)
	{
		GPrintError("Could not spawn player base.");
		return;
	}
	
	RedPlayerBase->SetActorLocation(DistanceOffset);
	RedPlayerBase->BaseTeam = EPlayerTeam::Red;

	APlayerBase* BluePlayerBase = GetWorld()->SpawnActor<APlayerBase>(PlayerBaseClass);
	if (!BluePlayerBase)
	{
		GPrintError("Could not spawn player base.");
		return;
	}
	BluePlayerBase->SetActorLocation(-DistanceOffset);
	BluePlayerBase->BaseTeam = EPlayerTeam::Blue;
}

void AArenaGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		DestroyAllCreatedObjects(); // destroy client-created objects, if any
		return;
	}
	
	DestroyAllCreatedObjects();

	ArenaMesh->SetWorldScale3D(ArenaScale);
	
	CreateNewComponents();

	CreatePlayerStarts();

	CreatePlayerBase();
}


