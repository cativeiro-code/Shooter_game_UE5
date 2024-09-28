// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ShooterCharacter.generated.h"



class UInputMappingContext;
class UInputAction;

UCLASS()
class SHOOTER2_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* defaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input", meta = (AllowPrivateAccess = "true"))
	UInputAction* jumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input", meta = (AllowPrivateAccess = "true"))
	UInputAction* moveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input", meta = (AllowPrivateAccess = "true"))
	UInputAction* lookAction;


	void move(const FInputActionValue& Value);

	void look(const FInputActionValue& value);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/*camera boom positioning camera behind the character!*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* cameraBoom;
	
	//Camera that follow the character !
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* followCamera;


public:
	/*returns camera boom suboject!*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const
	{
		return cameraBoom;
	}
	
	//return follow camera subobject
	FORCEINLINE UCameraComponent* GetfollowCamera() const
	{
		return followCamera;
	}
};
