// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER2_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

private:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "movement", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	//the speed of the character 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "movement",meta=(AllowPrivateAccess = "true"))
	float speed;
	//if the character is in air 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "movement", meta = (AllowPrivateAccess = "true"))
	bool bisInAir;
	//if the character is accelarating 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "movement", meta = (AllowPrivateAccess = "true"))
	bool bisAccelarating;
	
};
