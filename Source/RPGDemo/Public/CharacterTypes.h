#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
    Unequipped UMETA(DisplayName = "Unequipped"),
    EquippedOneHandWeapon UMETA(DisplayName = "EquippedOneHandWeapon")
};

UENUM(BlueprintType)
enum class ECharacterActionState : uint8 {
    Unoccupied UMETA(DisplayName = "Unoccupied"),
    Attacking UMETA(DisplayName = "Attacking"),
    EquippingWeapon UMETA(DisplayName = "EquippingWeapon")
};

UENUM(BlueprintType)
enum class EEnemyDeathPose : uint8 { EEDP_Alive UMETA(DisplayName = "Alive"), EEDP_Death UMETA(DisplayName = "Death") };