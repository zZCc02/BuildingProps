// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SingletonControl.generated.h"

class APlayerController;
class AExplodeActor;

UCLASS()
class SMARTCITYTOOLSET_API ASingletonControl : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASingletonControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void CheckExplodeBuilding(FHitResult Result);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//射线检测
	UFUNCTION(BlueprintCallable, Category="Hit")
	void ChooseFloor();

protected:

	TArray<AExplodeActor*> Explodes;

	//这是命中的ExplodeActor
	AExplodeActor* ChooseExplodeActor;

	APlayerController* PC;

};
