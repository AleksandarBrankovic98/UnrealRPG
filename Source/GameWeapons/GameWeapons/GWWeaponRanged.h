#pragma once
#include "GWWeapon.h"
#include "GWWeaponRanged.generated.h"

/*
	Base class for all weapons.
	Mele and ranged weapons need separate classes (at least).
*/
UCLASS(BlueprintType, Blueprintable, hideCategories=(Animation, Transform, Mesh, Physics, Collision, Clothing, Lighting, "Physics Volume", "Skeletal Mesh"))
class GAMEWEAPONS_API AGWWeaponRanged : public AGWWeapon
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Indicates that HitInfo trace should be done every frame.
		Might be useful for displaying beam effects
		or targeting helpers, which need to be updated every frame.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bTraceEveryTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float Range;
	/*
		How often weapon can refire. Either automatic, semi-automatic or single shot.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float FireRate;

	/*
		Reaload time. Either for single bullet or for entire magazine.
		Depends on reload State type.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float ReloadTime;

	//possibly move it to separate object ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 MagazineSize;

	/*
		Maximum ammo this weapon can have.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 MaximumAmmo;
	/*
		Base spread for this weapon.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float BaseSpread;
	/*
		If weapon fires continously, spread will increase by this multiplier, every time weapon shoot.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float SpreadMultiplier;
	/*
		Amount of bullets shoot in single shoot. Default 1.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 BurstSize;
	/*
		Separate from BurstSize. Shotgun can have high burst size, but still only one ammo cost.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 AmmoCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float MaximumChargeTime;
	/*
		Type of ammo, this weapon currently have.
		Ammo type indicates type of damage this weapon will deal.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Modifications")
		TSubclassOf<class UGWAmmo> TypeOfAmmo;
protected:
	UPROPERTY(BlueprintReadOnly, Category ="Ammo")
	class UGWAmmo* CurrentAmmo;
public:
	/*
		Type of reaload used by this weapon.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ReloadState;

	UPROPERTY(EditAnywhere, Category = "Cosmetics")
		UAnimMontage* FiringAnimation;

	UPROPERTY(EditAnywhere, Category = "Cosmetics")
		UAnimMontage* ReloadAnimation;

	virtual void Tick(float DeltaSeconds) override;

	/*
		Function used to execute fireing weapon.
		It will use CurrentAmmo, to apply damage to Target.

		@param TargetIn - Target to which ammo will hit.zx
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Weapons")
		void FireWeapon(const FHitResult& TargetIn, float DamageIn, APawn* InstigatorIn);

protected:
	UPROPERTY(Replicated)
	float RemainingMagazineAmmo;
	UPROPERTY(Replicated)
	float RemaningAmmo;
	bool CheckIfHaveAmmo();
	void SubtractAmmo();
	void CalculateReloadAmmo();
	bool CheckIfCanReload();
public:
	/*
		Set remaning ammo in magazine from external source. Ie. saved data.
	*/
	inline void SetRemainingMagazineAmmo(int32 RemainingAmmoIn){ RemainingMagazineAmmo = RemainingAmmoIn; };
	/*
		Get remaning ammo  in magazine . For example for saving ;).
	*/
	inline int32 GetRemainingMagazineAmmo() { return RemainingMagazineAmmo; };

	/*
		Set remaning total ammo from weapon in external source. Ie. saved data.
	*/
	inline void SetRemaningAmmofloat(int32 RemainingAmmoIn){ RemaningAmmo = RemainingAmmoIn; };
	/*
		Get remaning total ammo for this weapon from external source. For example for saving ;).
	*/
	inline int32 GetRemaningAmmo() { return RemaningAmmo; };

public:
	/*
		Ranged weapons need special trace methods, which will account for spread.
		also spread should be calculcated based on how long weapon fire.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Targeting")
	class UGWTraceRangedWeapon* TargetingMethod;

	virtual void BeginPlay() override;

	virtual void InitializeWeapon() override;

	virtual void InputPressed() override;
	virtual void InputReleased() override;

	virtual void InputReload();

	virtual void ActionBegin() override;
	virtual void ActionEnd() override;

	virtual void BeginFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginFire();

	virtual void EndFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndFire();

	/*
		Begin reloading weapon. for example start playing animation.
	*/
	virtual void BeginReload();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginReload();
	/*
		Finish reloading. At this point it should be safe to add  ammo to magazine.
	*/
	virtual void EndReload(); //that's probabaly not needed.
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndReload();


	UFUNCTION(BlueprintImplementableEvent)
		void OnFireBegin();
	UFUNCTION(BlueprintImplementableEvent)
		void OnFireEnd();
protected:

	UFUNCTION()
		virtual void OnRep_Shooting();
	UPROPERTY(ReplicatedUsing = OnRep_ReloadBeign)
		int8 ReloadBeginCount;
	UFUNCTION()
		virtual void OnRep_ReloadBeign();
	UPROPERTY(ReplicatedUsing = OnRep_ReloadEnd)
		int8 ReloadEndCount;
	UFUNCTION()
		virtual void OnRep_ReloadEnd();

	virtual void OnRep_HitInfo() override;
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Weapon Firing"))
		void OnWeaponFiring(const FVector& Origin, const FVector& HitLocation);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Reload Begin"))
		void OnReloadBegin();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Reload End"))
		void OnReloadEnd();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Weapon Charging"))
		void OnWeaponCharging();
private:
	class IIGISkeletalMesh* SkeletalMeshInt;

	bool bIsWeaponFiring;
	/*
		There will be subclass of weapon parts
		there are going to be different for each weapon type (bow doesn't have barrel, I think).
		In editor, those can be setup by using Instanced properties
		at runetime they are going to be set, trough class pointer, and then constructed
		using FinishCustomization()
	 */
	//UPROPERTY()
	//	TSubclassOf<WeaponPart> Part;
	/*
		Ammo type is type of damage which instant hit weapon will deal.
		Also it can cause special effects like freezing, immolation etc.
		Class pointer on purpose so it can be changed on runtime.
	*/

};
