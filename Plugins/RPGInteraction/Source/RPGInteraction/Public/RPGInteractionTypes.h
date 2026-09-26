// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGInteractionTypes.generated.h"


UENUM(BlueprintType)
enum class ERPGHighlightStyle : uint8
{
	Unhighlight = 0,
	Default = 1,
	Quest   = 2,
	Danger  = 3
};
