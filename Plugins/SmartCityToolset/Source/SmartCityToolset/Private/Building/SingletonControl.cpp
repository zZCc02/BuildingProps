// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/SingletonControl.h"

#include "DatasmithSceneActor.h"
#include "Building/ExplodeActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASingletonControl::ASingletonControl()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASingletonControl::BeginPlay()
{
	Super::BeginPlay();

	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PC->SetShowMouseCursor(true);
	

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExplodeActor::StaticClass(), FoundActor);

	for (AActor* Actor : FoundActor)
	{
		AExplodeActor* ExplodeActor = Cast<AExplodeActor>(Actor);
		if (ExplodeActor)
		{
			Explodes.Add(ExplodeActor);
		}
	}
	
}

void ASingletonControl::ChooseFloor()
{

	FVector WorldLocation;
	FVector WorldDirection;
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		FVector StartPos = WorldLocation;
		FVector EndPos = StartPos + (WorldDirection * 100000.f);

		FHitResult HitResult;
		FCollisionQueryParams CollisionQuery;
		CollisionQuery.AddIgnoredActor(this);

		TArray<AActor*> IgnoreActors;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartPos, EndPos, ETraceTypeQuery::TraceTypeQuery1, true, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);

		if (bHit)
		{
			CheckExplodeBuilding(HitResult);

			if (HitResult.GetActor()->GetAttachParentActor()->GetAttachParentActor() == ChooseExplodeActor->FloorArr.Last())
			{
				if (!ChooseExplodeActor->bExplode)
				{
					ChooseExplodeActor->BreakAnim(true);
				}
				else
				{
					ChooseExplodeActor->CurrentChooseFloor = HitResult.GetActor()->GetAttachParentActor()->GetAttachParentActor();
					//ChooseExplodeActor->BreakAnim(false);
					ChooseExplodeActor->ReverseAnim();
				}
			}
			else if(ChooseExplodeActor->bExplode && HitResult.GetActor()->GetAttachParentActor()->GetAttachParentActor() != ChooseExplodeActor->FloorArr.Last())
			{
				//UE_LOG(LogTemp, Warning, TEXT("PullAnim"));
				ChooseExplodeActor->CurrentChooseFloor = HitResult.GetActor()->GetAttachParentActor()->GetAttachParentActor();
				ChooseExplodeActor->PullAnim();
			}
		}
	}

}

void ASingletonControl::CheckExplodeBuilding(FHitResult Result)
{
	AActor* HitActorRoot = Result.GetActor()->GetAttachParentActor()->GetAttachParentActor()->GetAttachParentActor();

	if (ADatasmithSceneActor* HitDatasmithSceneActor = Cast<ADatasmithSceneActor>(HitActorRoot))
	{
		for (AExplodeActor* ExplodeActor : Explodes)
		{
			if (ExplodeActor->DatasmithSceneRoot == HitDatasmithSceneActor)
			{
				ChooseExplodeActor = ExplodeActor;
			}
		}
	}
}

// Called every frame
void ASingletonControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

