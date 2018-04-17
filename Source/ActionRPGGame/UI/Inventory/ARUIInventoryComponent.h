// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARUIInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARInventoryScreenWidget> InventoryViewClass;
	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARListItemWeaponWidget> ListItemWeaponClass;

	UPROPERTY(BlueprintReadOnly, Category = "ActionRPGGame|UI|Inventory")
		class UARInventoryScreenWidget* InventoryView;
public:	
	// Sets default values for this component's properties
	UARUIInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreateInventoryView(AARPlayerController* PC);
	void ShowHideInventory();

protected:
	UFUNCTION()
		void OnWeaponAdded(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item);
	UFUNCTION()
		void OnWeaponUpdated(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item);
	UFUNCTION()
		void OnWeaponRemoved(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item);

public:
	void ShowWeaponsForSlot(class UARItemView* ForSlot);

	void AddWeaponToSlot(uint8 TargetNetIndex
		, uint8 TargetLocalIndex
		, uint8 SourceNetIndex
		, uint8 SourceLocalIndex);

	void UnequipWeaponFromSlot(uint8 SourceNetIndex, uint8 SourceLocalIndex);
};
