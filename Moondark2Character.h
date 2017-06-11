// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/Character.h"
#include "Projectile.h"
#include "Moondark2Character.generated.h"

UCLASS(config=Game)
class AMoondark2Character : public ACharacter
{
	GENERATED_BODY()
    
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;
    
    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
    
    /** Camera boom positioning the camera at the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* AimBoom;
    
    /** Aim camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* AimCamera;
    
public:
    AMoondark2Character();
    
    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    float BaseTurnRate;
    
    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    float BaseLookUpRate;
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick(float DeltaSeconds) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Score)
    int gearScore;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    TSubclassOf<class AActor> myProjectileClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
    UAnimMontage* fireAnimMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera)
    AProjectile* currProjectile;
    
    //Boolean used to track when player is in a dialog option. Most input functions will not run when in dialog
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Camera)
    bool inDialog;
    
    //Boolean used to track when the player is holding down the left mouse action or not
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Camera)
    bool actionClicked;
    
protected:
    
    /** Resets HMD orientation in VR. */
    void OnResetVR();
    
    /** Called for forwards/backward input */
    void MoveForward(float Value);
    
    /** Called for side to side input */
    void MoveRight(float Value);
    
    /** Called when mouse clicked*/
    void pressedAction();
    
    /** Called when mouse released*/
    void releasedAction();
    
    /** Player Action on clicked*/
    void playerClickedAction();
    
    /** Called when e pressed down*/
    void enterAimMode();
    
    /** Called when e released down*/
    void leaveAimMode();
    
    /** Called when space is pressed down*/
    void jumpTriggered();
    
    /**
     * Called via input to turn at a given rate.
     * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate);
    
    /**
     * Called via input to turn look up/down at a given rate.
     * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void LookUpAtRate(float Rate);
    
    /** Handler for when a touch input begins. */
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    
    /** Handler for when a touch input stops. */
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
    
protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // End of APawn interface
    
public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
