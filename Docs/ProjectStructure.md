# Project Structure

This repo follows Unreal Engine's expected root layout. The folders below are the ones you should treat as source-of-truth vs generated output.

## Source of truth
- `Config/`              Engine and project configuration
- `Content/`             Game assets (uasset/umap)
- `Docs/`                Project documentation and automation reports
- `Plugins/`             Local plugins
- `Scripts/`             Build and automation shell scripts
- `Source/`              C++ source code
- `SneakGear.uproject`   Project descriptor

## Generated (do not commit)
- `Binaries/`            Build outputs
- `Build/`               Staging outputs
- `DerivedDataCache/`    Editor cache
- `Intermediate/`        Build intermediates
- `Saved/`               Autosaves, logs, config overrides

---

## C++ Modules

The project is split into three source modules and one local plugin.

| Module | Type | Purpose |
|--------|------|---------|
| `SneakGear` | Runtime | Main gameplay — player, guards, weapons, AI, items, radar, game state |
| `SneakGearCore` | Runtime | Shared low-dependency types, enums, structs, and UI interfaces |
| `SneakGearTests` | UncookedOnly | Automated test suite (Unreal automation framework) |
| `SneakGearUI` | Plugin (Runtime) | HUD widgets — radar, crosshair, vitals, weapon HUD, event feed |

Dependency direction: `SneakGearTests` → `SneakGear` → `SneakGearCore` ← `SneakGearUI`

---

## Source Layout — SneakGear

Feature ownership architecture. Each folder owns a vertical slice of gameplay, not a technical layer.

### `Characters/`
Base character shared by player and guards.

| Class | Purpose |
|-------|---------|
| `ABaseCharacter` | GAS setup, damage handling, death hook |

### `Player/`
Everything owned by the player character.

| Class | Purpose |
|-------|---------|
| `APlayerCharacterBase` | Third-person movement, camera, aim, weapon host |
| `ASneakGearPlayerCharacter` | Stealth-specific behavior (cover, vault) |
| `ASneakGearPlayerController` | Player controller |
| `UPlayerInventoryComponent` | Inventory item slots, item definitions, reserve ammo, weapon-slot storage |
| `UPlayerInventoryInteractionComponent` | Nearby pickup search and item pickup flow |
| `UPlayerWeaponComponent` | Combat-facing weapon equip, fire, reload, state, and animation notify API |
| `UPlayerHUDComponent` | Radar, crosshair, HUD widget ownership |
| `UPlayerAimComponent` | Aiming mechanics |
| `UPlayerLocomotionComponent` | Movement handling |
| `UPlayerWeaponSelectionComponent` | Quick select and hold-to-open weapon selection menu flow |

### `Guards/`
Guard AI character, controller, behavior tree nodes, and patrol.

| Class | Purpose |
|-------|---------|
| `AGuardCharacter` | Guard character with awareness/patrol/combat |
| `AGuardAIController` | Behavior Tree controller with blackboard synchronization |
| `UGuardAwarenessComponent` | Vision-based perception, awareness state machine |
| `UGuardPatrolComponent` | Patrol path navigation |
| `APatrolPath` | Waypoint actor (vector array) |
| `AGuardSpawner` | Spawns guards at runtime |
| `UGuardManagerSubsystem` | Global guard registry |
| `UGuardArchetypeData` | Shared tuning data asset |
| `BTTask_SetNextPatrolPoint` | Advance patrol index |
| `BTTask_StartGuardFire` | Begin firing |
| `BTTask_StopGuardFire` | End firing |
| `BTTask_SetGuardFiring` | Direct fire-state assignment |
| `BTTask_ReloadGuardWeapon` | Trigger reload |
| `BTService_UpdateGuardFireState` | Continuous fire state updates |
| `BTDecorator_IsGuardClipEmpty` | Clip-empty condition check |

### `Weapon/`
Weapon actors and fire modes. Shared by player and guards via `IWeaponAimProvider`.

| Class | Purpose |
|-------|---------|
| `AWeaponBase` | Abstract base weapon; firing, reloading, delegates |
| `AHitscanWeaponBase` | Raycast-based firing with spread and recoil |
| `AUnarmedWeapon` | Melee / unarmed weapon |
| `UHitscanFireMode` | Raycast fire mode implementation |
| `UProjectileFireMode` | Projectile spawn fire mode |
| `UWeaponFireModeComponent` | Manages fire mode state |
| `IWeaponAimProvider` | Aim data interface (implemented by player and guards) |

### `Items/`
Item data definitions and world pickups.

| Class | Purpose |
|-------|---------|
| `UPlayerItemDefinition` | Base item data asset |
| `UEquipmentItemDefinition` | Equipment item subtype |
| `UConsumableItemDefinition` | Consumable item subtype |
| `UUtilityItemDefinition` | Utility item subtype |
| `UNightVisionGogglesItemDefinition` | Night vision item |
| `UMedkitItemDefinition` | Medkit item |
| `UScannerItemDefinition` | Scanner item |
| `AWorldItemPickup` | Floor pickup actor with mesh |
| `UPlayerItemPickupComponent` | Pickup detection component |
| `IPickupItemInterface` | Pickup interaction interface |

### `Components/`
Shared gameplay components used across player and guards.

| Class | Purpose |
|-------|---------|
| `UCoverComponent` | Detects cover geometry |
| `UCoverStateComponent` | Manages cover entry/exit state |
| `UStealthSignatureComponent` | Character stealth signature |
| `UCharacterWeaponComponent` | Weapon system for non-inventory characters (guards) |

### `Radar/`
Guard contact tracking and HUD visualization feed.

| Class | Purpose |
|-------|---------|
| `URadarRegistrySubsystem` | Global guard contact registration |
| `URadarEmitterComponent` | Per-guard radar emission |

### `Game/`
Mission state and GAS attribute sets.

| Class | Purpose |
|-------|---------|
| `ASandboxGameState` | Mission lifecycle (start, complete, reset) |
| `AMissionStartTarget` | Mission start trigger |
| `AMissionReachTarget` | Mission reach-objective trigger |
| `UHealthAttributeSet` | Health and MaxHealth GAS attributes |
| `UStaminaAttributeSet` | Stamina GAS attributes |
| `UAmmoAttributeSet` | Ammo reserve GAS attributes |

---

## Source Layout — SneakGearCore

Shared types with no dependency on SneakGear gameplay classes.

| Path | Purpose |
|------|---------|
| `Types/PlayerInventoryTypes.h` | Inventory slot enums and weapon slot structs |
| `Types/GuardAwarenessTypes.h` | Awareness state enums |
| `Types/PlayerMovementTypes.h` | Movement stance enums |
| `Types/PlayerUITypes.h` | UI data structs |
| `Types/RadarTypes.h` | Radar contact structs |
| `UI/PlayerUIDataSource.h` | `IPlayerUIDataSource` — interface for HUD data queries |
| `UI/WeaponMenuActions.h` | Weapon menu action enums |

---

## Source Layout — SneakGearTests

Automated test suites using the Unreal Automation Framework.

| File | Tests |
|------|-------|
| `TestInventory` | Inventory slot management, ammo bookkeeping, pickup/swap rules, weapon slot state |
| `TestAI` | Guard awareness, perception, state transitions |
| `TestWeapons` | Weapon firing, reload, primary fire mode delegation |
| `TestCharacters` | Character lifecycle, death, damage handling |
| `TestUIWidgets` | UI data source responses and widget state |

Run tests:
```bash
./Scripts/run-tests.sh
./Scripts/run-tests.sh SneakGear.Inventory. --no-build
```

Reports are written to `Docs/AutomationReports/`.

---

## Plugin — SneakGearUI

HUD and widget layer. Depends on `SneakGearCore` for `IPlayerUIDataSource`.

| Widget | Purpose |
|--------|---------|
| `UPlayerHUDWidget` | Root HUD container |
| `UCrosshairWidget` / `UCrosshairNativeWidget` | Crosshair with spread and hit marker |
| `URadarWidget` | Guard contacts on minimap |
| `UWeaponStatusWidget` | Clip and ammo status |
| `UWeaponQuickIndicatorWidget` | Active weapon quick indicator |
| `UWeaponSelectionMenuWidget` | Hold-to-open weapon selection menu |
| `UPlayerVitalsWidget` / `UPlayerVitalsNativeWidget` | Health and stamina display |
| `UStanceWidget` | Movement stance indicator |
| `UPickupPromptWidget` / `UPickupPromptNativeWidget` | Nearby pickup interaction prompt |
| `UInfoPromptWidget` / `UInfoPromptNativeWidget` | General info/prompt overlay |
| `UEventFeedWidget` | On-screen event log feed |
| `UEventLogSubsystem` | Game subsystem feeding events to the HUD |
| `UPlayerDebugWidget` | Stealth/AI debug overlay |

---

## Content Layout

| Folder | Purpose |
|--------|---------|
| `Content/Blueprints/` | Blueprint actors (characters, game state, spawner, pickups) |
| `Content/Characters/` | Character mesh and animation assets |
| `Content/Data/` | Data assets (item definitions, guard archetypes) |
| `Content/Levels/` | Playable maps |
| `Content/UI/` | Widget blueprints |
| `Content/Environment/` | Level geometry and props |
| `Content/Input/` | Enhanced Input action definitions |
| `Content/GameplayCues/` | Particle effects and audio cues |

---

## Conventions

- New project docs go in `Docs/`
- One-off scripts and automation go in `Scripts/`
- Third-party source drops go under `ThirdParty/` (create as needed)
- Feature slices own their full vertical — actor, component, BT node, data asset — rather than grouping by technical type
- Classes that belong to two or more features go in `Components/` (shared components) or `Characters/` (shared base)
