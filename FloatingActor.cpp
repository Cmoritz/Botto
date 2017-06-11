// Fill out your copyright notice in the Description page of Project Settings.

#include "Moondark2.h"
#include "FloatingActor.h"


// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    USceneComponent * SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = SceneComp;
    
    //Attach to Root
    BoxCollider->SetupAttachment(RootComponent);
    Mesh->SetupAttachment(RootComponent);
    
    //Make sure their relative locations are zeroed
    BoxCollider->SetRelativeLocation(FVector::ZeroVector);
    Mesh->SetRelativeLocation(FVector::ZeroVector);
    
    //Collision Event
    BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AFloatingActor::OnOverlap);
}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();
    
    maxLoc = GetActorLocation() + FVector(0,0,10);
    minLoc = GetActorLocation() + FVector(0,0,-10);
    currGoal = &maxLoc;
    isGoingUp = true;
    
    floatSpeed = 10.0f;
	
}

// Called every frame
void AFloatingActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    
    SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, FMath::FInterpConstantTo(GetActorLocation().Z,
                                                                                                  currGoal->Z, DeltaTime, floatSpeed)));
    
    if(maxLoc.Z - GetActorLocation().Z <= 1 && isGoingUp)
    {
        currGoal = &minLoc;
        isGoingUp = false;
    }
    else if(GetActorLocation().Z - minLoc.Z <= 1 && !isGoingUp)
    {
        currGoal = &maxLoc;
        isGoingUp = true;
    }
    
}

void AFloatingActor::OnOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor->IsA(AMoondark2Character::StaticClass()))
    {
        Player = Cast<AMoondark2Character>(OtherActor);
        touchedEvent(OtherActor);
        if(hasConsumeEvent)
        {
            consumeEvent(OtherActor);
        }
        Destroy(this);
    }
}

