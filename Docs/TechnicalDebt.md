# Technical Debt

Current shortlist of project weaknesses worth addressing.

## Architecture

- Guard code still exposes several convenience query methods through `AGuardCharacter` that could move behind `UGuardAwarenessComponent` and `UGuardPatrolComponent`
- `Weapon/` is still organized as a broad technical bucket instead of a clearer combat feature slice
- Some UI policy still depends on controller-owned Blueprint events, even after moving most logic into player components

## AI

- Guard targeting currently assumes one primary target at a time; there is no last-known-position, investigation target, or search-state memory model yet
- Guard aiming still uses a simple target-location offset rather than sockets, eye points, or per-target aim points
- Guard BT setup is runtime-validated, but not yet editor-validated through proper data validation tooling

## Player / Inventory

- `UPlayerInventoryComponent` (1064 LOC) still owns runtime weapon storage, active-weapon slot, reserve ammo, and attachment presentation — pickup interaction lives in `UPlayerInventoryInteractionComponent`, combat-facing weapon behavior now goes through `UPlayerWeaponComponent`, and internal pickup/weapon-runtime logic is extracted into private helpers (`FPlayerInventoryPickupQuery`, `FPlayerInventoryWeaponRuntime`)
- Deprecated weapon/combat wrappers remain on `UPlayerInventoryComponent` for Blueprint and test compatibility; they should be removed after call sites and assets are migrated
- Inventory content rules are still lightweight; there is no stronger item definition/data model yet for loadout validation or progression

## Combat

- `AWeaponBase` and `AHitscanWeaponBase` still mix several responsibilities such as firing cadence, trace setup, aim consumption, recoil, and weapon state
- Reloading and ammo flow are functional, but there is no richer state model for reload timing, interruption, or replication-sensitive weapon state transitions

## Performance

- Radar updates are throttled, but still rebuild visible guard contact data on an interval rather than reacting to meaningful state changes
- Several UI widgets still poll player state every tick instead of updating from explicit gameplay events

## Tooling / Validation

- Automation coverage now spans inventory, AI, weapons, player character flow, animation, UI, and death/damage lifecycle, but many tests still emit warnings from lightweight test actors with missing meshes/sockets
- Guard BT setup is runtime-tested but still lacks editor-facing data validation for required blackboard keys and behavior tree assets
- There is no central project health checklist for verifying a new guard, weapon, or player setup in-editor
