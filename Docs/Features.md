# Current Features

## Player

- Third-person movement with stance handling
- Aim mode with first-person / third-person camera switching
- Cover and vault behavior
- Inventory-backed primary / secondary weapon slots
- Hold-to-open weapon selection menu
- Quick weapon indicator
- Shared aim-provider path for both player and AI weapons

## Combat

- `AWeaponBase` with generic aim-source support
- `AHitscanWeaponBase` with spread and recoil
- Hitscan fire mode
- Projectile fire mode
- Shared `UCharacterWeaponComponent` for non-inventory characters
- Player inventory weapon runtime through `UPlayerInventoryComponent`

## AI

- Guard awareness component with vision and awareness states
- Guard AI controller with blackboard synchronization
- Behavior Tree support required for guards
- Patrol paths driven by `APatrolPath` waypoint arrays
- Guard spawner (`AGuardSpawner`) and guard manager subsystem
- Guard archetype data assets for shared tuning
- Custom BT nodes:
  - `BTTask_SetNextPatrolPoint` — advance patrol index
  - `BTTask_StartGuardFire` / `BTTask_StopGuardFire` — firing control
  - `BTTask_SetGuardFiring` — direct fire-state assignment
  - `BTTask_ReloadGuardWeapon` — ammo management
  - `BTService_UpdateGuardFireState` — continuous fire state updates
  - `BTDecorator_IsGuardClipEmpty` — clip-empty check

## UI

- Main HUD widget
- Crosshair with spread + hit marker
- Radar widget with guard contacts
- Weapon status and quick indicator widgets
- Weapon selection menu widget
- Player vitals widget
- Stealth debug widget
- Event feed / event log subsystem

## GAS

- Health attribute set
- Stamina attribute set
- Ammo attribute set
