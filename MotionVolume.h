// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "MotionVolume.generated.h"


UCLASS()
class MOONDARK2_API AMotionVolume : public AActor
{
	GENERATED_BODY()
	
public:
    //Used for collisions with the player
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Physics)
    UBoxComponent* BoxCollider;
    
    //Used to indicate the direction of movement
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Physics)
    UArrowComponent* DirectionalArrow;
    
    //How much force will be added
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
    float speed = 1.0f;
    
	// Sets default values for this actor's properties
	AMotionVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
    //Pointer for array of current actors in the volume
    TArray<AActor*> currentActors;
	
	
};
