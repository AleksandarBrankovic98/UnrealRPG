// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerComponent.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARWeaponAbilityBase.h"
#include "ARItemWeapon.h"
#include "ARCharacter.h"
#include "UI/Weapons/ARWeaponListWidget.h"
#include "UI/Weapons/ARWeaponUpgradeListWidget.h"

#include "DWBPFunctionLibrary.h"
#include "SDraggableWindowWidget.h"

#include "Engine/World.h"

// Sets default values for this component's properties
UARWeaponManagerComponent::UARWeaponManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UARWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	EquipedWeapons.SetNum(MAX_WEAPONS+1);
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;
	
	if (WeaponListClass)
	{
		WeaponListWidget = CreateWidget<UARWeaponListWidget>(MyPC, WeaponListClass);
		WeaponListWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WeaponListWidget->ARPC = Cast<AARPlayerController>(GetOwner());
		WeaponListWidget->WeaponManager = this;
		int32 Idx = 0;
		for (const TSubclassOf<class UARItemWeapon>& Weapon : WeaponClasses)
		{
			WeaponListWidget->AddItem(DragSlotClass, Idx);
			Idx++;
		}

		WeaponListWindowHandle = UDWBPFunctionLibrary::CreateWindowWithContent(WeaponListWidget, "Weapon List");
		WeaponListWindowHandle.Window.Pin()->bDestroyOnClose = false;
		WeaponListWindowHandle.Window.Pin()->SetVisibility(EVisibility::Collapsed);
	}

	if (MagazineUpgradeListClass)
	{
		MagazineUpgradeListWidget = CreateWidget<UARWeaponUpgradeListWidget>(MyPC, MagazineUpgradeListClass);
		MagazineUpgradeListWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		//MagazineUpgradeListWidget->ARPC = Cast<AARPlayerController>(GetOwner());
		MagazineUpgradeListWidget->WeaponManager = this;
		MagazineUpgradeListWidget->AddToViewport();
	}

	//POwner = MyPC->GetPawn();
}


// Called every frame
void UARWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UARWeaponManagerComponent::EquipWeapon(TSoftClassPtr<UGAAbilityBase> WeaponAbility)
{
	if (!WeaponAbility.IsValid())
	{
		return;
	}

	TArray<FGameplayTag> WeaponInput = GetInputTag(EAMGroup::Group001, EAMSlot::Slot001);
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();
	if (!AbilityComp)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByTag(WeaponAbility));
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::OnWeaponInputRead,
			WeaponAbility, WeaponInput);

		AbilityComp->SetAbilityToAction(WeaponAbility, WeaponInput, ReadyDelegate);
	}
	else
	{
		AbilityComp->SetAbilityToAction(WeaponAbility, WeaponInput, FAFOnAbilityReady());
		ExecuteAbilityReadyEvent(WeaponAbility);
	}
}
UGAAbilityBase* UARWeaponManagerComponent::GetCurrentWeapon()
{
	return GetAbility(ActiveGroup, EAMSlot::Slot001);
}

void UARWeaponManagerComponent::AddToWeaponInventory(TSubclassOf<class UARItemWeapon> InWeapon)
{
	WeaponClasses.Add(InWeapon);
}

void UARWeaponManagerComponent::BP_AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
void UARWeaponManagerComponent::AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	UARItemWeapon* Item = DuplicateObject<UARItemWeapon>(WeaponClasses[Idx].GetDefaultObject(), Character);
	if (Character)
	{
		Character->GetWeapons()->Holster(Group, Item);
		EquipedWeapons[AMEnumToInt<EAMGroup>(Group)] = Item;
		ActiveGroup = EAMGroup::Group005;
	}
	NativeEquipAbility(WeaponClasses[Idx].GetDefaultObject()->Ability,
		Group, EAMSlot::Slot001, POwner);

	if (IsClient())
	{
		ServerAddWeaponToManager(Group, Slot, Idx);
	}
	else
	{
		APlayerController* MyPC = Cast<APlayerController>(GetOwner());
		if (!MyPC)
			return;
		
		IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(POwner);
		if (!ABInt)
			return;

		Character->GetWeapons()->Holster(Group, Item);
		ActiveGroup = EAMGroup::Group005;
	}

}
void UARWeaponManagerComponent::ServerAddWeaponToManager_Implementation(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
bool UARWeaponManagerComponent::ServerAddWeaponToManager_Validate(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	return true;
}



void UARWeaponManagerComponent::OnWeaponInputRead(TSoftClassPtr<UGAAbilityBase> WeaponAbilityTag, TArray<FGameplayTag> InInputTags)
{
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();

	//AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, FAFOnAbilityReady());
}


void UARWeaponManagerComponent::OnAbilityReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag
	, const TArray<FGameplayTag>& InAbilityInput
	, EAMGroup InGroup, EAMSlot InSlot)
{
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	if (Character)
	{
		UGAAbilityBase* AbilityInstance = GetAbility(InGroup, InSlot);
		Character->Weapons2->SetAbilityToItem(static_cast<uint8>(InGroup), AbilityInstance);
	}
}


void UARWeaponManagerComponent::ShowHideAbilityManager()
{
	if (!WeaponListWidget)
		return;

	EVisibility Visibility = WeaponListWindowHandle.Window.Pin()->GetVisibility();

	if (Visibility == EVisibility::Collapsed)
	{
		WeaponListWindowHandle.Window.Pin()->SetVisibility(EVisibility::SelfHitTestInvisible);
	}
	else if (Visibility == EVisibility::SelfHitTestInvisible)
	{
		WeaponListWindowHandle.Window.Pin()->SetVisibility(EVisibility::Collapsed);
	}
}