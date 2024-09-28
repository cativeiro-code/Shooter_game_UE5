// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//create camera boom! pulls in to the character if there is a collision.
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraBoom"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 300.f;//distance of the character 
	cameraBoom->bUsePawnControlRotation = true;//rotate the arm based on the controller !


	//create a follow camera 
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("followCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);//add camera to end of boom.
	followCamera->bUsePawnControlRotation = false; // 
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(defaultMappingContext, 0);
		}
	}



		

}

void AShooterCharacter::move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller!= nullptr)
	{
		//check and find the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//check the forward vector
		const FVector forwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//check the right vecotr
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
		//add movement
		AddMovementInput(forwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	

}

void AShooterCharacter::look(const FInputActionValue& value)
{
	FVector2D lookvector = value.Get<FVector2D>();

	if (Controller!= nullptr)
	{
		AddControllerYawInput(lookvector.X);
		AddControllerPitchInput(lookvector.Y);
	}

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(jumpAction,ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::move);

		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::look);


	}

}

