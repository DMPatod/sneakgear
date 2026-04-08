# C4 Architecture — SneakGear

C4 diagrams at the **Container** (module) and **Component** (feature slice) levels.

---

## Level 1 — System Context

```mermaid
C4Context
  title System Context — SneakGear

  Person(player, "Player", "Controls the player character via keyboard/gamepad")

  System_Boundary(sg, "SneakGear (Unreal Engine 5.7)") {
    System(game, "SneakGear Game", "Stealth-action sandbox with guard AI, inventory, weapons, and mission state")
  }

  Rel(player, game, "Plays")
```

---

## Level 2 — Containers (Modules)

```mermaid
C4Container
  title Container Diagram — C++ Modules and Plugin

  Person(player, "Player")

  System_Boundary(sg, "SneakGear") {
    Container(sneakgear, "SneakGear", "UE C++ Runtime Module", "Gameplay: player, guards, weapons, items, radar, mission state")
    Container(sneakgearcore, "SneakGearCore", "UE C++ Runtime Module", "Shared types, enums, structs, and UI data-source interfaces")
    Container(sneakgearui, "SneakGearUI", "UE C++ Plugin", "HUD widgets: radar, crosshair, vitals, weapon status, event feed")
    Container(sneakgeartests, "SneakGearTests", "UE C++ UncookedOnly Module", "Automated test suite — inventory, AI, weapons, characters, UI")
  }

  System_Ext(uegas, "GameplayAbilitySystem", "UE built-in plugin — attribute sets and gameplay effects")
  System_Ext(uebt, "BehaviorTree / AIModule", "UE built-in plugin — guard AI execution")
  System_Ext(ueinput, "EnhancedInput", "UE built-in plugin — input action mapping")

  Rel(player, sneakgear, "Plays via")
  Rel(sneakgear, sneakgearcore, "Imports shared types from")
  Rel(sneakgear, sneakgearui, "Pushes HUD state to")
  Rel(sneakgearui, sneakgearcore, "Uses IPlayerUIDataSource from")
  Rel(sneakgear, uegas, "Uses for health / stamina / ammo attributes")
  Rel(sneakgear, uebt, "Drives guard AI via")
  Rel(sneakgear, ueinput, "Reads player input from")
  Rel(sneakgeartests, sneakgear, "Tests")
  Rel(sneakgeartests, sneakgearcore, "Tests")
```

---

## Level 3 — Components (Feature Slices within SneakGear)

```mermaid
C4Component
  title Component Diagram — SneakGear Feature Slices

  Container_Boundary(sg, "SneakGear Runtime Module") {

    Component(characters, "Characters", "ABaseCharacter", "Base GAS character: lifecycle, damage, death hook")

    Component(player, "Player", "APlayerCharacterBase\nASneakGearPlayerCharacter", "Third-person movement, aim, cover, vault, camera")
    Component(playercomps, "Player Components", "UPlayerInventoryComponent\nUPlayerWeaponComponent\nUPlayerHUDComponent\nUPlayerInventoryInteractionComponent\nUPlayerWeaponMenuComponent", "Inventory, weapon firing, HUD ownership, pickup, weapon menu")

    Component(guards, "Guards", "AGuardCharacter\nAGuardAIController\nUGuardAwarenessComponent\nUGuardPatrolComponent", "Guard AI: awareness, patrol, combat, BT synchronization")
    Component(bttasks, "Guard BT Nodes", "BTTask_SetNextPatrolPoint\nBTTask_StartGuardFire / StopGuardFire\nBTTask_ReloadGuardWeapon\nBTService_UpdateGuardFireState\nBTDecorator_IsGuardClipEmpty", "Behavior tree tasks, services, and decorators")

    Component(weapon, "Weapon", "AWeaponBase\nAHitscanWeaponBase\nUHitscanFireMode\nUProjectileFireMode\nIWeaponAimProvider", "Firing, reload, spread/recoil, aim-source interface")

    Component(items, "Items", "UPlayerItemDefinition\nAWorldItemPickup\nUPlayerItemPickupComponent", "Item data assets and world pickups")

    Component(sharedcomps, "Shared Components", "UCoverComponent\nUCoverStateComponent\nUStealthSignatureComponent\nUCharacterWeaponComponent", "Cover geometry, stealth signature, guard weapon host")

    Component(radar, "Radar", "URadarRegistrySubsystem\nURadarEmitterComponent", "Guard contact registration and HUD feed")

    Component(game, "Game", "ASandboxGameState\nUHealthAttributeSet\nUStaminaAttributeSet\nUAmmoAttributeSet", "Mission lifecycle and GAS attribute sets")
  }

  Rel(player, characters, "Extends")
  Rel(guards, characters, "Extends")

  Rel(player, playercomps, "Owns")
  Rel(playercomps, weapon, "Fires and reloads via")
  Rel(playercomps, items, "Stores slot items from")
  Rel(playercomps, sharedcomps, "Uses cover and stealth")
  Rel(player, radar, "Contacts displayed on HUD via")

  Rel(guards, bttasks, "Executes")
  Rel(guards, weapon, "Fires via UCharacterWeaponComponent")
  Rel(guards, sharedcomps, "Uses UCharacterWeaponComponent")
  Rel(guards, radar, "Emits contacts to")

  Rel(player, game, "Attributes tracked in")
  Rel(guards, game, "Attributes tracked in")
```

---

## Level 3 — Components (SneakGearUI Plugin)

```mermaid
C4Component
  title Component Diagram — SneakGearUI Plugin

  Container_Boundary(ui, "SneakGearUI Plugin") {
    Component(hud, "PlayerHUDWidget", "Root HUD container", "Owns and composes all HUD sub-widgets")
    Component(crosshair, "CrosshairWidget\nCrosshairNativeWidget", "Spread visualization\nHit marker feedback", "Renders crosshair driven by weapon state")
    Component(radar, "RadarWidget", "Minimap radar", "Renders guard contacts from RadarRegistrySubsystem")
    Component(vitals, "PlayerVitalsWidget\nPlayerVitalsNativeWidget", "Health / stamina bars", "Driven by IPlayerUIDataSource")
    Component(weapon, "WeaponStatusWidget\nWeaponQuickIndicatorWidget\nWeaponSelectionMenuWidget", "Clip / ammo / active weapon\nHold-to-open menu", "Weapon HUD layer")
    Component(prompt, "PickupPromptWidget\nInfoPromptWidget", "Pickup and info overlays", "Contextual interaction prompts")
    Component(stance, "StanceWidget", "Movement stance indicator", "Shows current player stance")
    Component(eventfeed, "EventFeedWidget\nEventLogSubsystem", "On-screen event log", "Subsystem feeds events; widget renders them")
    Component(debug, "PlayerDebugWidget", "Stealth/AI debug overlay", "Dev-only awareness and stealth data")
  }

  Container_Ext(core, "SneakGearCore", "IPlayerUIDataSource, widget data types")
  Container_Ext(sneakgear, "SneakGear", "RadarRegistrySubsystem, gameplay state")

  Rel(hud, crosshair, "Contains")
  Rel(hud, radar, "Contains")
  Rel(hud, vitals, "Contains")
  Rel(hud, weapon, "Contains")
  Rel(hud, stance, "Contains")
  Rel(hud, prompt, "Shows contextually")
  Rel(hud, eventfeed, "Contains")
  Rel(hud, debug, "Contains (dev builds)")

  Rel(vitals, core, "Queries via IPlayerUIDataSource")
  Rel(weapon, core, "Queries via IPlayerUIDataSource")
  Rel(radar, sneakgear, "Reads RadarRegistrySubsystem")
  Rel(eventfeed, sneakgear, "EventLogSubsystem receives gameplay events")
```
