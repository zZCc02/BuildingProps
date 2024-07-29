// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/ExplodeActor.h"
#include "DatasmithSceneActor.h"
#include "Components/TimelineComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AExplodeActor::AExplodeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TL_Break = CreateDefaultSubobject<UTimelineComponent>(TEXT("TL_Break"));
	TL_Pull = CreateDefaultSubobject<UTimelineComponent>(TEXT("TL_Pull"));

	//SetMobileMode(EComponentMobility::Movable
}

// Called when the game starts or when spawned
void AExplodeActor::BeginPlay()
{
	Super::BeginPlay();

	if (DatasmithSceneRoot)
	{
		//获取绑定在DatasmithSceneActor下一级actor数组
		DatasmithSceneRoot->GetAttachedActors(FloorArr, true,false);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("DatasmithSceneActor is nullptr"));
		return;
	}

	//根据名称进行排序
	SortFloorArray();
	
	
	for (AActor* Act : FloorArr)
	{
		if (Act)
		{
			FString ActorDisplayName = Act->GetActorLabel();
			//UE_LOG(LogTemp, Warning, TEXT("Floor Name : %s"), *ActorDisplayName);
			UKismetSystemLibrary::PrintString(GetWorld(), ActorDisplayName, true, true);
		}
	}
	
	for(int32 i = 0; i < FloorArr.Num(); i++)
	{
		//这是TMap<AActor* , FVector>添加
		InitialPos.Add(FloorArr[i], FloorArr[i]->GetActorLocation());
		//这里根据偏移值计算楼层拆分后的目标Z值
		TargetPos.Add(FloorArr[i], FVector(FloorArr[i]->GetActorLocation().X, FloorArr[i]->GetActorLocation().Y, FloorArr[i]->GetActorLocation().Z + (ExplodeOffset.Z * i)));
		//UE_LOG(LogTemp, Log, TEXT("TargetPos[%i]: %s"), i, *TargetPos[i].ToString());
	}

	//绑定Timeline事件
	BindBreakAnim();
	BindPullAnim();
	
	CurrentChooseFloor = LastChooseFloor = nullptr;
	
}

void AExplodeActor::SortFloorArray()
{
	FloorArr.Sort([this](const AActor& A, const AActor& B)
	{
		FString StringA, StringB;
		FString IndexA, IndexB;
		bool bSuccessA = UKismetStringLibrary::Split(A.GetActorLabel(), FloorLName, StringA, IndexA);
		bool bSuccessB = UKismetStringLibrary::Split(B.GetActorLabel(), FloorLName, StringB, IndexB);

		//分割成功，将IndexA和IndexB转换为Int比较
		if (bSuccessA && bSuccessB)
		{
			int32 IntIndexA = FCString::Atoi(*IndexA);
			int32 IntIndexB = FCString::Atoi(*IndexB);
			return IntIndexA < IntIndexB;
		}
		
		//分割失败，按照字符串比较
		return A.GetActorLabel() < B.GetActorLabel();
	});
}

void AExplodeActor::BindBreakAnim()
{
	if (Curve)
	{
		FOnTimelineFloat OnTimelineFloat;
		FOnTimelineEvent OnTimelineEvent;

		OnTimelineFloat.BindUFunction(this, FName("GetBreakAnimFloat"));
		OnTimelineEvent.BindUFunction(this, FName("BreakAnimFinish"));

		TL_Break->AddInterpFloat(Curve, OnTimelineFloat);
		//TL_Break->SetTimelineLength(ETimelineLengthMode::TL_LastKeyFrame);
		TL_Break->SetTimelineFinishedFunc(OnTimelineEvent);

		TL_Break->SetLooping(false);
	}
}

void AExplodeActor::BindPullAnim()
{
	if (PullCurve)
	{
		FOnTimelineFloat OnPullTLFloat;
		FOnTimelineEvent OnPullTLEvent;

		OnPullTLFloat.BindUFunction(this, FName("GetPullAnimFloat"));
		OnPullTLEvent.BindUFunction(this, FName("PullAnimFinish"));

		TL_Pull->AddInterpFloat(PullCurve, OnPullTLFloat);
		TL_Pull->SetTimelineFinishedFunc(OnPullTLEvent);

		TL_Pull->SetLooping(false);
	}
}


void AExplodeActor::SetMobileMode(EComponentMobility::Type)
{
	//设置移动性
	TArray<AActor*> AttachActors;
	DatasmithSceneRoot->GetAttachedActors(AttachActors,true, true);
	for (AActor* AttachAct : AttachActors)
	{
		if (AStaticMeshActor* SMActor = Cast<AStaticMeshActor>(AttachAct))
		{
			SMActor->SetMobility(EComponentMobility::Movable);
		}
	}
}

void AExplodeActor::BreakAnim(bool bForward)
{
	if (bForward)
	{
		TL_Break->PlayFromStart();
	}
	else
	{
		TL_Break->ReverseFromEnd();
	}
}

void AExplodeActor::BreakAnimFinish()
{
	FVector* LastFloorPosPtr = InitialPos.Find(FloorArr.Last());
	bExplode = LastFloorPosPtr && (FloorArr.Last()->GetActorLocation() != *LastFloorPosPtr);
}

void AExplodeActor::GetBreakAnimFloat(float Value)
{
	ensure(FloorArr.Last());
	for (AActor* FloorActor : FloorArr)
	{
		FVector TempPos = FMath::Lerp(*InitialPos.Find(FloorActor), *TargetPos.Find(FloorActor), Value);
		FloorActor->SetActorLocation(TempPos);
	}
	//UE_LOG(LogTemp, Log, TEXT("Timeline Value: %f"), Value);
}

void AExplodeActor::PullAnim()
{
	if (!bExplode)
	{
		return;
	}
	CurrentPullOffset = ((CurrentChooseFloor->GetActorForwardVector()*PullOffset.X)
						   + (CurrentChooseFloor->GetActorRightVector()*PullOffset.Y)
						   + (CurrentChooseFloor->GetActorUpVector()*PullOffset.Z)
						   + CurrentChooseFloor->GetActorLocation());
	//UE_LOG(LogTemp, Warning, TEXT("CurrentPullOffset: %s"), *CurrentPullOffset.ToString());
		
	TL_Pull->PlayFromStart();
}

void AExplodeActor::PullAnimFinish()
{
	if (CurrentChooseFloor == FloorArr.Last())
	{
		LastChooseFloor = nullptr;
		return;
	}
	//如果LastChooseFloor为nullptr有两种情况，第一种是我第一次点击，第二次是我将抽出的楼层推回去了
	if (!LastChooseFloor)
	{
		LastChooseFloor = CurrentChooseFloor;
		LastPullOffset = CurrentPullOffset;
		return;
	}
	if (LastChooseFloor)
	{
		if (LastChooseFloor != CurrentChooseFloor)
		{
			//上一次点击的楼层和现在点击的楼层不相等说明楼层被拉出
			LastChooseFloor = CurrentChooseFloor;
			LastPullOffset = CurrentPullOffset;
		}
		else if (LastChooseFloor == CurrentChooseFloor)
		{
			//上一次点击的楼层和现在点击的楼层相等说明楼层被推回，如果LastChooseFloor为nullptr就只会调用抽出的函数
			LastChooseFloor = nullptr;
		}
	}
}

void AExplodeActor::GetPullAnimFloat(float Value)
{
	if (CurrentChooseFloor)
	{
		if (CurrentChooseFloor == FloorArr.Last())
		{
			PullReverseAnim(Value);
			return;
		}
		if (!LastChooseFloor)
		{
			PullForwardAnim(Value);
		}
		if (LastChooseFloor && LastChooseFloor != CurrentChooseFloor && CurrentPullOffset != LastPullOffset)
		{
			PullForwardAnim(Value);
			PullReverseAnim(Value);
		}
		if (LastChooseFloor && LastChooseFloor == CurrentChooseFloor)
		{
			PullReverseAnim(Value);
		}
		
	}
}

void AExplodeActor::PullForwardAnim(float Value)
{
	if (CurrentChooseFloor)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Lerp from %s to %s with alpha %f"), 
			   *TargetPos[ChooseFloorIndex].ToString(), 
			   *CurrentPullOffset.ToString(), 
			   Value);*/
		FVector LerpLocation = FMath::Lerp(*TargetPos.Find(CurrentChooseFloor), CurrentPullOffset, Value);
		CurrentChooseFloor->SetActorLocation(LerpLocation);
	}
}

void AExplodeActor::PullReverseAnim(float Value)
{
	FVector LerpLocation = FMath::Lerp(LastPullOffset, *TargetPos.Find(LastChooseFloor), Value);
	LastChooseFloor->SetActorLocation(LerpLocation);
}



// Called every frame
void AExplodeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplodeActor::ReverseAnim()
{
	if (CurrentChooseFloor == FloorArr.Last() && !LastChooseFloor)
	{
		BreakAnim(false);
		return;
	}
	PullAnim();
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]()
	{
		BreakAnim(false);
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.f, false);
	//BreakAnim(false);
}


