# Sneak Gear

Unreal Engine 5.7 stealth-action project inspired by the Metal Gear Solid series.

## Current State

The project currently has:
- A third-person player with aim, cover, inventory-backed weapon slots, and weapon selection UI
- Guard AI with awareness, patrol paths, Behavior Tree-driven movement, and combat response
- Shared GAS-based health, stamina, and ammo attributes
- Radar, crosshair, weapon HUD, vitals, and debug UI

## Source Layout

The runtime code is moving toward feature ownership instead of type-based folders.

Main feature slices:
- `Source/SneakGear/Public/Player`
- `Source/SneakGear/Public/Guards`
- `Source/SneakGear/Public/Weapon`
- `Source/SneakGear/Public/UI`
- `Source/SneakGear/Public/Radar`
- `Source/SneakGear/Public/Characters`

Shared low-dependency types live in:
- `Source/SneakGearCore/Public/Types`

Ownership rules:
- `ABaseCharacter`: generic character lifecycle, GAS setup, damage handling, death hook
- `AThirdPersonPlayerCharacter`: generic player movement, camera, aim, and weapon host
- `AStealthPlayerCharacter`: stealth-player-specific behavior only
- `AGuardCharacter`: guard-specific behavior only
- `UPlayerItemComponent`: player inventory and active-weapon state facade
- `UGuardAwarenessComponent` / `UGuardPatrolComponent`: guard state and patrol composition

## Current Features

### Player
- Third-person movement with stance handling
- Aim mode with first-person / third-person camera switching
- Cover and vault behavior
- Inventory-backed primary / secondary weapon slots
- Hold-to-open weapon selection menu
- Quick weapon indicator
- Shared aim-provider path for both player and AI weapons

### Combat
- `AWeaponBase` with generic aim-source support
- Hitscan fire mode
- Projectile fire mode
- `ARealWeaponBase` with spread, recoil, and penetration logic
- Shared `UCharacterWeaponComponent` for non-inventory characters
- Player inventory weapon runtime through `UPlayerItemComponent`

### AI
- Guard awareness component with vision and awareness states
- Guard AI controller with blackboard synchronization
- Behavior Tree support required for guards
- Patrol paths driven by `APatrolPath` waypoint arrays
- Custom BT task: `BTTask_SetNextPatrolPoint`
- Guard spawner and guard manager subsystem
- Guard archetype data for shared tuning

### UI
- Main HUD widget
- Crosshair with spread + hit marker
- Radar widget with guard contacts
- Weapon status and quick indicator widgets
- Weapon selection menu widget
- Player vitals widget
- Stealth debug widget
- Event feed / event log subsystem

### GAS
- Health attribute set
- Stamina attribute set
- Ammo attribute set

## Guard Setup

Guards are no longer supported through legacy patrol fallback logic. A valid Behavior Tree is required.

Required guard setup:
1. Place a `GuardCharacter` or spawn one through `AGuardSpawner`
2. Assign a `BehaviorTreeAsset`
3. Ensure the BT has a blackboard
4. Assign a `PatrolPath` if the guard should patrol

Recommended blackboard keys:
- `TargetActor` as `Object`
- `Awareness` as `Float`
- `AwarenessState` as `Int`
- `HasLineOfSight` as `Bool`
- `PatrolLocation` as `Vector`
- `PatrolIndex` as `Int`

Recommended Behavior Tree shape:

```text
Root
  Selector

    Sequence
      Decorator: AwarenessState == Alerted
      Move To(TargetActor)

    Sequence
      Decorator: Awareness > 0.0
      Wait(0.2)

    Sequence
      Decorator: Awareness <= 0.0
      Loop
        Set Next Patrol Point
        Move To(PatrolLocation)
        Wait(0.2)
```

Notes:
- `AGuardAIController` updates guard blackboard values continuously
- Patrol path data now comes only from `APatrolPath.Waypoints`
- Legacy `GuardPatrolData` and `WaypointAction` systems were removed

## Player UI / Inventory Notes

Player UI controller logic has been split into components:
- `UPlayerHUDComponent`: radar, crosshair, HUD widget ownership
- `UPlayerWeaponMenuComponent`: quick select and selection menu flow

Inventory / weapon-slot state now lives behind:
- `UPlayerItemComponent`

That component owns:
- slot items
- active weapon slot
- reserve ammo queries
- clip queries
- nearby pickup search
- recent-fire tracking

## Known Constraints

- Guard Behavior Trees assume the blackboard keys listed above
- Guard aiming and BT chasing now use the same target source
- Player weapon slots now require both a valid slot item and a runtime weapon actor
- Character death callbacks are latched and should fire once
- Radar updates are throttled instead of rebuilt every frame

## Suggested Next Work

- Add editor validation for invalid guard BT / blackboard setups
- Move more guard query APIs behind guard components
- Reorganize `Weapon/` into a clearer `Combat/` slice
- Improve AI aiming points to use sockets / eyes instead of actor center offsets
- Add automated tests or validation checks for inventory, guard setup, and BT assumptions

## Technical Debt

This is the current shortlist of project weaknesses worth addressing.

### Architecture

- Guard code still exposes several convenience query methods through `AGuardCharacter` that could move behind `UGuardAwarenessComponent` and `UGuardPatrolComponent`
- `Weapon/` is still organized as a broad technical bucket instead of a clearer combat feature slice
- Some UI policy still depends on controller-owned Blueprint events, even after moving most logic into player components

### AI

- Guard targeting currently assumes one primary target at a time; there is no last-known-position, investigation target, or search-state memory model yet
- Guard aiming still uses a simple target-location offset rather than sockets, eye points, or per-target aim points
- Guard BT setup is runtime-validated, but not yet editor-validated through proper data validation tooling

### Player / Inventory

- `UPlayerItemComponent` now owns most inventory and weapon-slot state, but it is still doing inventory, pickup queries, runtime weapon spawning, ammo bookkeeping, and attachment presentation in one class
- Inventory content rules are still lightweight; there is no stronger item definition/data model yet for loadout validation or progression

### Combat

- `AWeaponBase` and `ARealWeaponBase` still mix several responsibilities such as firing cadence, trace setup, aim consumption, recoil, and weapon state
- Reloading and ammo flow are functional, but there is no richer state model for reload timing, interruption, or replication-sensitive weapon state transitions

### Performance

- Radar updates are throttled, but still rebuild visible guard contact data on an interval rather than reacting to meaningful state changes
- Several UI widgets still poll player state every tick instead of updating from explicit gameplay events

### Tooling / Validation

- The project still lacks automated tests for:
  - guard BT assumptions
  - inventory slot validity
  - weapon firing behavior across player and AI owners
  - death / damage lifecycle expectations
- There is no central project health checklist for verifying a new guard, weapon, or player setup in-editor

## Goal Checklist

- [ ] Solid movement feel: walk/run/sprint, crouch, prone, vault, climb
- [ ] Contextual cover: snap, lean, blind-fire, corner transitions
- [ ] Advanced camera: shoulder swap, obstruction handling, cinematic takedowns
- [ ] AI perception: vision cones, hearing, peripheral vision, last known position
- [ ] AI behavior: patrol, investigate, search, alert, combat, reinforcements
- [ ] Stealth feedback: awareness meter, audio/visual cues, body discovery
- [ ] Stealth tools: silencers, distractions, decoys, smoke, EMP
- [ ] Non-lethal options: chokeout, stun, tranquilizer, restraints
- [ ] Combat depth: recoil, spread, damage falloff, armor, cover penetration
- [ ] Weapons: swap/holster, upgrades, attachments, ammo types
- [ ] Inventory + loadout: slot system, weight/encumbrance, quick select
- [ ] Missions: objectives, stealth ratings, time/alert penalties
- [ ] NPC interaction: hostage rescue, escort, intel extraction
- [ ] World systems: day/night cycle, dynamic lighting, weather
- [ ] UI: radar, weapon HUD, vitals, interaction prompts
- [ ] Audio: footsteps, alert barks, suppressed shots
- [ ] Save/load and checkpointing
- [ ] Optimization pass for AI, lighting, and streaming
