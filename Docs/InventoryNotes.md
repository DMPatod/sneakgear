# Player UI / Inventory Notes

## UI Components

Player UI controller logic has been split into components:

| Component | Responsibility |
|-----------|---------------|
| `UPlayerHUDComponent` | Radar, crosshair, HUD widget ownership |
| `UPlayerWeaponMenuComponent` | Quick select and weapon selection menu flow |
| `UPlayerWeaponSelectionComponent` | Weapon menu state logic |

## Inventory Components

| Component | Responsibility |
|-----------|---------------|
| `UPlayerInventoryComponent` | Slot items, active weapon slot, reserve ammo, clip queries, recent-fire tracking |
| `UPlayerInventoryInteractionComponent` | Nearby pickup search and item pickup flow |
| `UPlayerWeaponComponent` | Player-side weapon firing and reload delegation |

### Internal Helpers (private to `UPlayerInventoryComponent`)

| Class | Responsibility |
|-------|---------------|
| `FPlayerInventoryPickupQuery` | Pickup search and floor-item scan logic |
| `FPlayerInventoryWeaponRuntime` | Runtime weapon spawning, ammo bookkeeping, attachment lifecycle |
