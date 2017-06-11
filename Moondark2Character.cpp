// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Moondark2.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Moondark2Character.h"

//////////////////////////////////////////////////////////////////////////
// AMoondark2Character

AMoondark2Character::AMoondark2Character()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;
    
    //Set the initial gear score of the player to 0.
    gearScore = 0;
    
    //Find spell, load it
    static ConstructorHelpers::FObjectFinder<UBlueprint> projectileBP(TEXT("Blueprint'/Game/MyContent/Blueprints/ProjectileBPP.ProjectileBPP'"));
    if(projectileBP.Succeeded())
    {
        myProjectileClass = (UClass*)projectileBP.Object->GeneratedClass;
    }
    
    //Aim mode set off
    inDialog = false;
    actionClicked = false;
    
    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;
    
    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
    
    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
    
    // Create an aim camera boom (pulls in towards the player if there is a collision)
    AimBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("AimBoom"));
    AimBoom->SetupAttachment(RootComponent);
    AimBoom->TargetArmLength = 0.0f; // The camera follows at this distance behind the character
    AimBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
    
    // Create an aim camera
    AimCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimCamera"));
    AimCamera->SetupAttachment(AimBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    AimCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
    
    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
    // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMoondark2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMoondark2Character::jumpTriggered);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AMoondark2Character::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMoondark2Character::MoveRight);
    
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMoondark2Character::pressedAction);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMoondark2Character::releasedAction);
    
    //PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMoondark2Character::enterAimMode);
    //PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMoondark2Character::leaveAimMode);
    
    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMoondark2Character::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AMoondark2Character::LookUpAtRate);
    
    // handle touch devices
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AMoondark2Character::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AMoondark2Character::TouchStopped);
    
    // VR headset functionality
    PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMoondark2Character::OnResetVR);
}

void AMoondark2Character::jumpTriggered()
{
    if(!inDialog)
        Jump();
}

void AMoondark2Character::enterAimMode()
{
    //Allow controller rotation to be determined by the camera movement
    
    if(!inDialog)
    {
        //Dont orient rotation to movement since we now use controller rotation
        GetCharacterMovement()->bOrientRotationToMovement = false;
        UGameplayStatics::GetPlayerController(this, 0)->GetCharacter()->bUseControllerRotationYaw = true;
        inDialog = true;
    
        FVector sockLoc = GetMesh()->GetSocketLocation("RightHandSocket");
        FRotator myRot = GetController()->GetControlRotation();
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.Owner = this;
        
        currProjectile =  Cast<AProjectile>(GetWorld()->SpawnActor(myProjectileClass, &sockLoc, &myRot, SpawnInfo));
        currProjectile->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHandSocket"));
    }
    // FollowCamera->Deactivate();
    // AimCamera->Activate();
    /*else {
        UGameplayStatics::GetPlayerController(this, 0)->GetCharacter()->bUseControllerRotationYaw = false;
        inAimMode = false;
        
        FollowCamera->Activate();
        AimCamera->Deactivate();
    }*/
    
}

/** Called when e released down*/
void AMoondark2Character::leaveAimMode()
{
    UGameplayStatics::GetPlayerController(this, 0)->GetCharacter()->bUseControllerRotationYaw = false;
    
    FollowCamera->Activate();
    AimCamera->Deactivate();
}

void AMoondark2Character::pressedAction()
{
    actionClicked = true;
    /*
    //Orient back to movement
     GetCharacterMovement()->bOrientRotationToMovement = true;
    
    UGameplayStatics::GetPlayerController(this, 0)->GetCharacter()->bUseControllerRotationYaw = false;
    inDialog = false;
    //Disallow controller movement to be determined by aim camera.
    //if(currProjectile)
        //currProjectile->Destroy();
    
    SetActorRotation(FRotator(GetActorRotation().Pitch,AimBoom->GetComponentRotation().Yaw,AimBoom->GetComponentRotation().Roll));
    
    ACharacter::PlayAnimMontage(fireAnimMontage);
   
    
    FVector sockLoc = GetMesh()->GetSocketLocation("RightHandSocket");
    FRotator myRot = GetController()->GetControlRotation();
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Owner = this;
    
    currProjectile =  Cast<AProjectile>(GetWorld()->SpawnActor(myProjectileClass, &sockLoc, &myRot, SpawnInfo));
    currProjectile->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHandSocket"));*/
}

void AMoondark2Character::releasedAction()
{
    actionClicked = false;
}

// Called when the game starts or when spawned
void AMoondark2Character::BeginPlay()
{
    Super::BeginPlay();
    
    //Aiming feels authentic if camera is attached to a socket at the head.
    AimBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true), TEXT("HeadSocket"));
    
}

void AMoondark2Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
}

void AMoondark2Character::OnResetVR()
{
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMoondark2Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    Jump();
}

void AMoondark2Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    StopJumping();
}

void AMoondark2Character::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMoondark2Character::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMoondark2Character::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f) && !inDialog)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMoondark2Character::MoveRight(float Value)
{
    if ( (Controller != NULL) && (Value != 0.0f) && !inDialog)
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        // get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}



























