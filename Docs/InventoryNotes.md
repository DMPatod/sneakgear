# Player UI / Inventory Notes

## UI Components

Player UI controller logic has been split into components:

| Component | Responsibility |
|-----------|---------------|
| `UPlayerHUDComponent` | Radar, crosshair, HUD widget ownership |
| `UPlayerWeaponSelectionComponent` | Quick select and hold-to-open weapon selection menu flow |

## Inventory Components

| Component | Responsibility |
|-----------|---------------|
| `UPlayerInventoryComponent` | Slot items, item definitions, reserve ammo, weapon-slot storage, item use/effect bookkeeping |
| `UPlayerInventoryInteractionComponent` | Nearby pickup search and item pickup flow |
| `UPlayerWeaponComponent` | Combat-facing weapon equip, fire, reload, state, clip queries, recent-fire tracking, animation notify delegation |

`UPlayerInventoryComponent` still exposes deprecated weapon/combat wrappers for existing Blueprint and test compatibility. New gameplay code should use `UPlayerWeaponComponent` for active-weapon behavior.

### Internal Helpers (private to `UPlayerInventoryComponent`)

| Class | Responsibility |
|-------|---------------|
| `FPlayerInventoryPickupQuery` | Pickup search and floor-item scan logic |
| `FPlayerInventoryWeaponRuntime` | Runtime weapon spawning, ammo bookkeeping, attachment lifecycle |
