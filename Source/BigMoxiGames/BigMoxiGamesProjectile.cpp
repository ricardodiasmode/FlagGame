// Copyright Epic Games, Inc. All Rights Reserved.

#include "BigMoxiGamesProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/ProjectileTargetInterface.h"
#include "Misc/GeneralFunctionLibrary.h"

ABigMoxiGamesProjectile::ABigMoxiGamesProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABigMoxiGamesProjectile::OnOverlap);		// set up a notification for when this component hits something blocking
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ABigMoxiGamesProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;
	
	UAbilitySystemComponent* OwnerASC = nullptr;
	if (GetOwner())
	{
		OwnerASC = GetOwner()->GetComponentByClass<UAbilitySystemComponent>();

		if (OwnerASC == nullptr)
			GPrintError("Projectile OwnerASC is nullptr. It won't damage.");
	} else
	{
		GPrintError("Projectile has not owner. It won't damage.");
	}

	if (IProjectileTargetInterface* OtherAsTarget = Cast<IProjectileTargetInterface>(OtherActor))
	{
		if (OtherAsTarget->GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle SpecHandle;
			FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
			EffectContext.AddSourceObject(GetOwner());
			SpecHandle = OwnerASC->MakeOutgoingSpec(HitGameplayEffect, 1.f, EffectContext);

			if (!SpecHandle.IsValid())
			{
				GPrintError("Trying to apply effect by projectile but spec handle is not valid.");
				return;
			}
	
			OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),
				OtherAsTarget->GetAbilitySystemComponent());

			Destroy();
		} else
		{
			GPrintError("Target ASC is nullptr.");
		}
	}
}
