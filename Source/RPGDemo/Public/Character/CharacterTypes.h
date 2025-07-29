#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
    Unequipped UMETA(DisplayName = "Unequipped"),
    EquippedOneHandWeapon UMETA(DisplayName = "EquippedOneHandWeapon")
};

UENUM(BlueprintType)
enum class ECharacterActionState : uint8 {
    Unoccupied UMETA(DisplayName = "Unoccupied"),
    HitReaction UMETA(DisplayName = "HitReaction"),
    Attacking UMETA(DisplayName = "Attacking"),
    EquippingWeapon UMETA(DisplayName = "EquippingWeapon")
};

UENUM(BlueprintType)
enum class EEnemyDeathPose : uint8 {
    EEDP_Alive UMETA(DisplayName = "Alive"),
    EEDP_Death UMETA(DisplayName = "Death")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
    EES_Patrolling UMETA(DisplayName = "Patrolling"),
    EES_Chasing UMETA(DisplayName = "Chasing"),
    EES_Attacking UMETA(DisplayName = "Attacking"),
    EES_Engaged UMETA(DisplayName = "Engaged"),
    EES_Dead UMETA(DisplayName = "Dead"),
    EES_NoState UMETA(DisplayName = "NoState")
};
