// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{

	if (ShooterCharacter==nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		//get lateral speed
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		speed=Velocity.Size();

		//check if character is in air 
		bisInAir = ShooterCharacter->GetMovementComponent()->IsFalling();

		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bisAccelarating = true;
		}
		else
		{

			bisAccelarating = false;
		}

	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
