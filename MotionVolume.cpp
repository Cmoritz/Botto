// Fill out your copyright notice in the Description page of Project Settings.

#include "Moondark2.h"
#include "MotionVolume.h"


// Sets default values
AMotionVolume::AMotionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    USceneComponent * SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    DirectionalArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    
    RootComponent = SceneComp;
    
    //Attach to Root
    BoxCollider->SetupAttachment(RootComponent);
    DirectionalArrow->SetupAttachment(RootComponent);
    
    //Zero relative locations
    DirectionalArrow->SetRelativeLocation(FVector::ZeroVector);

}

// Called when the game starts or when spawned
void AMotionVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMotionVolume::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    
    BoxCollider->GetOverlappingActors(currentActors);
    
    int numOver = currentActors.Num();
    UE_LOG(LogTemp, Warning, TEXT("%d"), numOver);
    
    //Loop to add the force in the positive Y direction
    for(int i = 0; i < currentActors.Num(); i++)
    {
        if(!currentActors[i]->IsA(AMotionVolume::StaticClass()))
           {
               FVector currLoc = currentActors[i]->GetActorLocation();
               FVector newVec = (DirectionalArrow->GetForwardVector() * speed);
               currentActors[i]->SetActorLocation(currLoc + newVec);
           }
    }

}

