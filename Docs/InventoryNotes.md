# Player UI / Inventory Notes

## UI Components

Player UI controller logic has been split into components:

| Component | Responsibility |
|-----------|---------------|
| `UPlayerHUDComponent` | Radar, crosshair, HUD widget ownership |
| `UPlayerWeaponMenuComponent` | Quick select and weapon selection menu flow |

## Inventory Components

| Component | Responsibility |
|-----------|---------------|
| `UPlayerInventoryComponent` | Slot items, active weapon slot, reserve ammo, clip queries, recent-fire tracking |
| `UPlayerInventoryInteractionComponent` | Nearby pickup search and item pickup flow |
| `UPlayerWeaponComponent` | Player-side weapon firing and reload delegation |
