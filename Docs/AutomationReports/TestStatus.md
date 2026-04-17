# Unreal Automation Test Status

## Summary

- Failed: 0
- Succeeded: 7
- Succeeded with warnings: 23
- Not run: 0

## Tests

| Test | Status | Duration (s) | Warnings | Errors |
| --- | --- | ---: | ---: | ---: |
| `SneakGear.AI.BehaviorTree.BTTaskSetNextPatrolPoint.AdvancesAndLoops` | Passed | 0.069 | 1 | 0 |
| `SneakGear.AI.GuardAIController.UpdatesBlackboardFromGuardTarget` | Passed | 0.043 | 0 | 0 |
| `SneakGear.AI.GuardCombat.FiringAtPlayerDecreasesPlayerHealth` | Passed | 0.043 | 7 | 0 |
| `SneakGear.Characters.BaseCharacter.DeathOnlyRunsOnce` | Passed | 0.041 | 0 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.CannotSelectEmptyWeaponSlot` | Passed | 0.008 | 0 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.ConsumableUseRestoresHealthConsumesAndLogs` | Passed | 0.042 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.EquippedItemSwitchLogsAndActivates` | Passed | 0.041 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NearbyPickupRespectsRange` | Passed | 0.041 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupHoldSwapsWithFloorItem` | Passed | 0.041 | 10 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupRequiresHoldToSwap` | Passed | 0.043 | 7 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.PickupReportsEventAndStoresUtilityItem` | Passed | 0.041 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.ReloadConsumesReserveAmmo` | Passed | 0.040 | 10 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.ReloadWaitsForWeaponDelay` | Passed | 0.042 | 12 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.UtilityScannerReportsNearbyGuards` | Passed | 0.042 | 6 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.WeaponPickupSpawnsAndSelectsRuntimeWeapon` | Passed | 0.042 | 8 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.CrouchCoverAllowsVault` | Passed | 0.042 | 7 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.FullInputFlow` | Passed | 0.042 | 10 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.PrimarySwapEquipsPickedUpWeapon` | Passed | 0.041 | 15 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.ProneCannotVault` | Passed | 0.041 | 6 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.SecondaryPickupKeepsPrimaryActive` | Passed | 0.041 | 11 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.StandingCoverAllowsVault` | Passed | 0.041 | 5 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.WeaponPickupEquipsWeapon` | Passed | 0.041 | 7 | 0 |
| `SneakGear.UI.EventFeedWidget.ShowsGuardStartedFiringNotification` | Passed | 0.042 | 3 | 0 |
| `SneakGear.UI.EventFeedWidget.ShowsPlayerDamageNotification` | Passed | 0.042 | 4 | 0 |
| `SneakGear.UI.EventFeedWidget.UpdatesFromEventDelegate` | Passed | 0.040 | 0 | 0 |
| `SneakGear.UI.PickupPromptWidget.ShowsSwapMessageAndHoldProgress` | Passed | 0.040 | 0 | 0 |
| `SneakGear.UI.PlayerVitalsWidget.UpdatesFromAttributeDelegates` | Passed | 0.041 | 4 | 0 |
| `SneakGear.UI.StanceWidget.UpdatesFromStanceDelegate` | Passed | 0.042 | 5 | 0 |
| `SneakGear.Weapon.WeaponBase.UsesAimProviderWithoutCamera` | Passed | 0.041 | 0 | 0 |
| `SneakGear.Weapon.WeaponBase.UsesAnimationDrivenFireInterval` | Passed | 0.041 | 0 | 0 |

## Details

### `SneakGear.AI.BehaviorTree.BTTaskSetNextPatrolPoint.AdvancesAndLoops`

Warnings:
- LogCrowdFollowing: Unable to find RecastNavMesh instance while trying to create UCrowdManager instance

### `SneakGear.AI.GuardCombat.FiringAtPlayerDecreasesPlayerHealth`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestGuardCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestGuardCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_3.Untitled:PersistentLevel.TestGuardCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.ConsumableUseRestoresHealthConsumesAndLogs`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_5.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_5.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_5.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_5.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.EquippedItemSwitchLogsAndActivates`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_6.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_6.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_6.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_6.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NearbyPickupRespectsRange`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupHoldSwapsWithFloorItem`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_8.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupRequiresHoldToSwap`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.PickupReportsEventAndStoresUtilityItem`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_10.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_10.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_10.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_10.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.ReloadConsumesReserveAmmo`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_11.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.ReloadWaitsForWeaponDelay`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_11.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_11.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.UtilityScannerReportsNearbyGuards`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_12.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_12.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.WeaponPickupSpawnsAndSelectsRuntimeWeapon`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.CrouchCoverAllowsVault`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_15.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.FullInputFlow`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(back_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(back_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(back_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_16.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.PrimarySwapEquipsPickedUpWeapon`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_17.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.ProneCannotVault`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_18.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_18.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_18.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_18.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_18.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_18.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.SecondaryPickupKeepsPrimaryActive`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.StandingCoverAllowsVault`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.WeaponPickupEquipsWeapon`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.UI.EventFeedWidget.ShowsGuardStartedFiringNotification`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_22.Untitled:PersistentLevel.TestGuardCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_22.Untitled:PersistentLevel.TestGuardCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_22.Untitled:PersistentLevel.TestGuardCharacter_0)

### `SneakGear.UI.EventFeedWidget.ShowsPlayerDamageNotification`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_23.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_23.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_23.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_23.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.UI.PlayerVitalsWidget.UpdatesFromAttributeDelegates`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_26.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_26.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_26.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_26.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.UI.StanceWidget.UpdatesFromStanceDelegate`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestInventoryCharacter_0)
