// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Moondark2Character.h"
#include "FloatingActor.generated.h"

UCLASS()
class MOONDARK2_API AFloatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float floatSpeed;
    
    //If true, this has a consume event that is triggered when collected
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool hasConsumeEvent;
    
    //Used for collisions with the player
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Physics)
    UBoxComponent* BoxCollider;
    
    //Used for the display of something
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
    UStaticMeshComponent* Mesh;
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Event")
    void touchedEvent(AActor* touchedActor);
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Event")
    void consumeEvent(AActor* touchedActor);
    
private:
    //Player who collects the floating objects
    AMoondark2Character* Player;
    
protected:
    
    UFUNCTION()
    void OnOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
private:
    FVector maxLoc;
    FVector minLoc;
    FVector* currGoal;
    bool isGoingUp;
	
};
