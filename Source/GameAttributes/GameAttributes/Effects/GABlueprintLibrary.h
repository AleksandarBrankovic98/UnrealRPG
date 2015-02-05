#pragma once
#include "GAEffectTypes.h"
#include "GABlueprintLibrary.generated.h"
/*
	Base class for effect.

	Effect is object, which modify actor with EffectComponent (or something like that).

	Effects can be divided into three basic categories:

	1. Instant - effect is appiled to actor, modify actor properties, and is deactivated and then
	garbage collected.
	2. Duration - effect is appiled to actor for set duration, after which is removed.
	3. Infinite - effect will exist for entire duration of game. (or actor).


	The main idea behind effects is to extend them trough blueprints, the base class shouldn't assume
	any gameplay type behaviours, only allow to easily create them trough blueprint.

	How exactly will that look, I'm not 100% sure.

	Effects should also be able to modify other effects.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Causer - Delibetrly UObject, as causer doesn't always needs, to be actor.
		It can be ability which is UObject (;
		
		Which attributes we modify,
		how we modify.
		On whom we modify.

		Returns true if effect has been succesfully appiled to target.
		Otherwise false;
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static bool ApplyEffect(TSubclassOf<class UGAEffect> EffectClass, UObject* Causer, const FHitResult& Target, APawn* Instigator,
		float Duration, int32 PeriodCount, const FGameplayTag& EffectTag,const FGAEffectPolicy& EffectPolicy,
		TArray<FGAAttributeSpec> AttributesIn);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void RemoveEffects(AActor* CauserIn, AActor* TargetIn, const FGameplayTagContainer& EffectsTags, int32 EffectCount);

	/*
		Check if Target actor, has any effects active, matching provided tags.
	*/
	UFUNCTION(BlueprintPure, Category = "Game Effect System")
		static bool HasAnyEffect(AActor* Target, const FGameplayTagContainer& Tags);
};
