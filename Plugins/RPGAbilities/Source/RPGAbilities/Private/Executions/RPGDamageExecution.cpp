// Fill out your copyright notice in the Description page of Project Settings.


#include "Executions/RPGDamageExecution.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RPGAbilityTags.h"
#include "Attributes/RPGHealthSet.h"
#include "Attributes/RPGCombatSet.h"


URPGDamageExecution::URPGDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void URPGDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float SourceDamage = 0.f;	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvalParams, SourceDamage);
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, TargetArmor);
	
	const float BaseDamage = Spec.GetSetByCallerMagnitude(RPGTags::SetByCaller_Damage, false, 0.f);
	
	float Final = FMath::Max(SourceDamage + BaseDamage - TargetArmor, MinimumDamage);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URPGHealthSet::GetIncomingDamageAttribute(), EGameplayModOp::AddBase, Final));
}
