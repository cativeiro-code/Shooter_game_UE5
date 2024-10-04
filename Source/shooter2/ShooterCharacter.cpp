// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "Sound/SoundWave.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include"Particles/ParticleSystemComponent.h"

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
	cameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);


	//create a follow camera 
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("followCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);//add camera to end of boom.
	followCamera->bUsePawnControlRotation = false; // 

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	
	GetCharacterMovement()->bOrientRotationToMovement =false;
	GetCharacterMovement()->RotationRate =FRotator(0.f,540.f,0.f) ;
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.5f;
	
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

void AShooterCharacter::shoot(const FInputActionValue& value)
{
	if (firesound)
	{
		UGameplayStatics::PlaySound2D(this, firesound);
	}
	const USkeletalMeshSocket* barrelSocket = GetMesh()->GetSocketByName("barrelsocket");
	if (barrelSocket)
	{
		const FTransform sockettransform = barrelSocket->GetSocketTransform(GetMesh());

		if (Muzzleflash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Muzzleflash,sockettransform);
		}
		//get current size of the viewport
		FVector2D ViewPortSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewPortSize);
		}
		//get screen space of the cross hairs 
		FVector2D CrossHairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);
		CrossHairLocation.Y -= 50.f;
		FVector CrossHairWorldPosition;
		FVector CrossHairWorldDirection;
		//get world position and direction of crosshairs 
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
			CrossHairLocation,
			CrossHairWorldPosition,
			CrossHairWorldDirection);

			if (bScreenToWorld)//verify if deprojection was successfull
			{
				FHitResult ScreenTraceHit;
				const FVector start{ CrossHairWorldPosition };
				const FVector End{ CrossHairWorldPosition + CrossHairWorldDirection * 50000.f };

				//set beam end point to the end of the line trace .
				FVector BeamEndPoint{ End };
				GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, start, End, ECollisionChannel::ECC_Visibility);
				if (ScreenTraceHit.bBlockingHit)//verify if was a hit 
				{
					//beam end point is now trace hit location 
					BeamEndPoint = ScreenTraceHit.Location;

				}
				//perform a second trace from the gun barrel 
				FHitResult weapontracehit;
				const FVector WeapontraceStart{ sockettransform.GetLocation() };
				const FVector weaponTraceEnd{ BeamEndPoint };
				GetWorld()->LineTraceSingleByChannel(weapontracehit,WeapontraceStart,weaponTraceEnd,ECollisionChannel::ECC_Visibility);

				if (weapontracehit.bBlockingHit)
				{
					BeamEndPoint = weapontracehit.Location;
				}
				//sapwn beam paricles after second trace from barrel 
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEndPoint);
				}

				if (BeamParticles)
				{
					UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, sockettransform);
					if (Beam)
					{
						Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
					}
				}
			}

		/*
		FHitResult FireHit;
		const FVector Start{ sockettransform.GetLocation() };
		const FQuat Rotation{ sockettransform.GetRotation() };
		const FVector RotationAxis{ Rotation.GetAxisX() };
		const FVector End{ (Start + RotationAxis) * 50'000 };

		FVector BeamEndPoint{ End };

		GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (FireHit.bBlockingHit)
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
			//DrawDebugPoint(GetWorld(), FireHit.Location, 5.f, FColor::Red, false, 2.f);

			BeamEndPoint = FireHit.Location;

		}
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.Location);
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, sockettransform );
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
			}
		}*/

	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("primeryfire"));
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

		EnhancedInputComponent->BindAction(shootAction, ETriggerEvent::Started, this, &AShooterCharacter::shoot);


	}

}

