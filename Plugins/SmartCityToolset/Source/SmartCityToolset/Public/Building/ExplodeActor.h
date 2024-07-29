// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplodeActor.generated.h"

class UTimelineComponent;
class AStaticMeshActor;
class ADatasmithSceneActor;

UCLASS()
class SMARTCITYTOOLSET_API AExplodeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplodeActor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetMobileMode(EComponentMobility::Type);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//楼层分层动画
	UFUNCTION(BlueprintCallable)
	void BreakAnim(bool bForward);
	UFUNCTION()
	void BreakAnimFinish();
	UFUNCTION()
	void GetBreakAnimFloat(float Value);

	//楼层抽拉动画
	UFUNCTION(BlueprintCallable)
	void PullAnim();
	UFUNCTION()
	void PullAnimFinish();
	UFUNCTION()
	void GetPullAnimFloat(float Value);

	//复原函数
	void ReverseAnim();

protected:

	//绑定函数
	void BindBreakAnim();
	void BindPullAnim();

	//抽拉函数
	void PullForwardAnim(float Value);
	void PullReverseAnim(float Value);

	//楼层数组排序
	void SortFloorArray();


protected:

	//初始位置数组和分层目标位置数组
	TMap<AActor*, FVector> InitialPos;
	TMap<AActor*, FVector> TargetPos;

	//偏移变量
	UPROPERTY(EditAnywhere, Category= "Offset")
	FVector ExplodeOffset;
	UPROPERTY(EditAnywhere, Category= "Offset")
	FVector PullOffset;
	
	FVector CurrentPullOffset;
	FVector LastPullOffset;


public:

	UPROPERTY(EditAnywhere)
	ADatasmithSceneActor* DatasmithSceneRoot;

	//时间轴组件
	UPROPERTY()
	UTimelineComponent* TL_Break;
	UPROPERTY()
	UTimelineComponent* TL_Pull;

	//楼层虚拟体
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> FloorArr;
	//楼层索引左侧的名称，用来分割名称对数组排序
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FloorLName;
	

	AActor* CurrentChooseFloor;
	AActor* LastChooseFloor;

	//曲线资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* Curve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* PullCurve;

	//楼层是否已经分层
	bool bExplode;

};
