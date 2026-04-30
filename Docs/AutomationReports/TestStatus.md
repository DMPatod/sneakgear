# Unreal Automation Test Status

## Summary

- Failed: 0
- Succeeded: 10
- Succeeded with warnings: 28
- Not run: 0

## Tests

| Test | Status | Duration (s) | Warnings | Errors |
| --- | --- | ---: | ---: | ---: |
| `SneakGear.AI.BehaviorTree.BTTaskSetNextPatrolPoint.AdvancesAndLoops` | Passed | 0.077 | 1 | 0 |
| `SneakGear.AI.GuardAIController.UpdatesBlackboardFromGuardTarget` | Passed | 0.042 | 0 | 0 |
| `SneakGear.AI.GuardCombat.FiringAtPlayerDecreasesPlayerHealth` | Passed | 0.042 | 8 | 0 |
| `SneakGear.Characters.BaseCharacter.DeathOnlyRunsOnce` | Passed | 0.042 | 0 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.CannotSelectEmptyWeaponSlot` | Passed | 0.008 | 0 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.ConsumableUseRestoresHealthConsumesAndLogs` | Passed | 0.042 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.EffectHandleBookkeeping` | Passed | 0.008 | 0 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.EquippedItemSwitchLogsAndActivates` | Passed | 0.041 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.FailedUtilityUseDoesNotConsumeItem` | Passed | 0.042 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NearbyPickupRespectsRange` | Passed | 0.042 | 4 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupHoldSwapsWithFloorItem` | Passed | 0.042 | 11 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupRequiresHoldToSwap` | Passed | 0.043 | 8 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.NonWeaponPickupPreservesActiveWeapon` | Passed | 0.042 | 10 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.PickupReportsEventAndStoresUtilityItem` | Passed | 0.043 | 6 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.ReloadConsumesReserveAmmo` | Passed | 0.041 | 10 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.ReloadWaitsForWeaponDelay` | Passed | 0.042 | 12 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.SupportCollectionMaintainsActiveIndex` | Passed | 0.008 | 0 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.UtilityScannerReportsNearbyGuards` | Passed | 0.043 | 6 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.WeaponDoesNotAutoResumeFireAfterReload` | Passed | 0.043 | 10 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.WeaponPickupSpawnsAndSelectsRuntimeWeapon` | Passed | 0.042 | 10 | 0 |
| `SneakGear.Inventory.PlayerInventoryComponent.WeaponStateMachineTracksFireAndReload` | Passed | 0.044 | 10 | 0 |
| `SneakGear.Player.SneakGearPlayerAnimInstance.MirrorsWeaponState` | Passed | 0.043 | 10 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.CrouchCoverAllowsVault` | Passed | 0.043 | 8 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.FullInputFlow` | Passed | 0.044 | 11 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.PrimarySwapEquipsPickedUpWeapon` | Passed | 0.043 | 16 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.ProneCannotVault` | Passed | 0.041 | 7 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.SecondaryPickupKeepsPrimaryActive` | Passed | 0.042 | 12 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.StandingCoverAllowsVault` | Passed | 0.043 | 5 | 0 |
| `SneakGear.Player.SneakGearPlayerCharacter.WeaponPickupEquipsWeapon` | Passed | 0.042 | 8 | 0 |
| `SneakGear.UI.EventFeedWidget.ShowsGuardStartedFiringNotification` | Passed | 0.041 | 3 | 0 |
| `SneakGear.UI.EventFeedWidget.ShowsPlayerDamageNotification` | Passed | 0.042 | 6 | 0 |
| `SneakGear.UI.EventFeedWidget.UpdatesFromEventDelegate` | Passed | 0.041 | 0 | 0 |
| `SneakGear.UI.PickupPromptWidget.ShowsSwapMessageAndHoldProgress` | Passed | 0.041 | 0 | 0 |
| `SneakGear.UI.PlayerVitalsWidget.UpdatesFromAttributeDelegates` | Passed | 0.042 | 4 | 0 |
| `SneakGear.UI.StanceWidget.UpdatesFromStanceDelegate` | Passed | 0.042 | 7 | 0 |
| `SneakGear.Weapon.HitscanWeaponBase.FireModeComponentIsBypassed` | Passed | 0.042 | 0 | 0 |
| `SneakGear.Weapon.WeaponBase.UsesAimProviderWithoutCamera` | Passed | 0.042 | 0 | 0 |
| `SneakGear.Weapon.WeaponBase.UsesAnimationDrivenFireNotifies` | Passed | 0.042 | 0 | 0 |

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
- LogTemp: TestInventoryCharacter_0 took 25 damage from TestGuardCharacter_0. Current health: 75

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

### `SneakGear.Inventory.PlayerInventoryComponent.FailedUtilityUseDoesNotConsumeItem`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_7.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NearbyPickupRespectsRange`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_8.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_8.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_8.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_8.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupHoldSwapsWithFloorItem`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_9.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NearbyWeaponPickupRequiresHoldToSwap`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_10.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.NonWeaponPickupPreservesActiveWeapon`

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

### `SneakGear.Inventory.PlayerInventoryComponent.PickupReportsEventAndStoresUtilityItem`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_11.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_11.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_12.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.ReloadConsumesReserveAmmo`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_13.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.ReloadWaitsForWeaponDelay`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_13.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_13.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_14.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.UtilityScannerReportsNearbyGuards`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_14.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_14.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_15.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_15.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_15.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_15.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.WeaponDoesNotAutoResumeFireAfterReload`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_16.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.WeaponPickupSpawnsAndSelectsRuntimeWeapon`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_16.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_16.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_17.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Inventory.PlayerInventoryComponent.WeaponStateMachineTracksFireAndReload`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_18.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.Player.SneakGearPlayerAnimInstance.MirrorsWeaponState`

Warnings:
- LogSpawn: Destroying /Temp/Untitled_18.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSpawn: Destroying /Temp/Untitled_18.Untitled:PersistentLevel.None, which doesn't have a valid world pointer
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_19.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.CrouchCoverAllowsVault`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_20.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.FullInputFlow`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(back_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(back_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(back_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_21.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.PrimarySwapEquipsPickedUpWeapon`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_22.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.ProneCannotVault`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_23.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.SecondaryPickupKeepsPrimaryActive`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_w_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_24.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.StandingCoverAllowsVault`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_25.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_25.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_25.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_25.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_25.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.Player.SneakGearPlayerCharacter.WeaponPickupEquipsWeapon`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(hand_r_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(SneakGearPlayerCharacter /Temp/Untitled_26.Untitled:PersistentLevel.SneakGearPlayerCharacter_0)

### `SneakGear.UI.EventFeedWidget.ShowsGuardStartedFiringNotification`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestGuardCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestGuardCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(spine_socket): No SkeletalMesh for Component(CharacterMesh0) Actor(TestGuardCharacter /Temp/Untitled_27.Untitled:PersistentLevel.TestGuardCharacter_0)

### `SneakGear.UI.EventFeedWidget.ShowsPlayerDamageNotification`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_28.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_28.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_28.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_28.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogTemp: TestInventoryCharacter_0 took 25 damage from Actor_0. Current health: 75
- LogTemp: EventFeedWidget received damage event: Player took 25 damage from Actor_0 (Health: 75)

### `SneakGear.UI.PlayerVitalsWidget.UpdatesFromAttributeDelegates`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_31.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_31.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_31.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_31.Untitled:PersistentLevel.TestInventoryCharacter_0)

### `SneakGear.UI.StanceWidget.UpdatesFromStanceDelegate`

Warnings:
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
- LogSkinnedMeshComp: GetSocketInfoByName(first_person_camera_attachment): No SkeletalMesh for Component(CharacterMesh0) Actor(TestInventoryCharacter /Temp/Untitled_32.Untitled:PersistentLevel.TestInventoryCharacter_0)
