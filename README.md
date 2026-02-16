Sneak Gear
---

Unreal Engine 5.7 game project.

The main reference to the game is the Metal Gear Solid franchise.

## Current Features

### Core Player
- Third-person character with camera boom and aim FOV interpolation
- Aim toggle with first-person/third-person view switch
- Equip/holster weapon on sockets
- Toggle sprint input action on base third-person player
- Jump input action owned by base third-person player (inherited by stealth player)
- Aim + weapon logic extracted into components
- Aim tuning is configured directly via `PlayerAimComponent` Blueprint defaults
- Movement/camera values are configured via character/component Blueprint defaults (no `PlayerTuningData` asset)

### Combat
- Weapon base actor with fire rate and muzzle socket support
- Hitscan fire mode component
- Projectile fire mode component (camera-assisted aim + projectile spawn)
- Default pistol weapon

### Stealth + Cover
- Cover component with approach, lock, and exit flow
- Stealth signature component for detectable state

### AI
- Guard AI controller
- Patrol path actor
- Guard patrol data asset with per-waypoint action objects
- Interface-driven waypoint actions (`IWaypointAction`) with built-in StandBy/LookAround actions
- Guard awareness/vision/hearing data for radar
- Guard archetype data asset for shared tuning
- Guard manager world subsystem for guard registration and global coordination

### GAS (Attributes)
- Health and stamina attribute sets

### UI
- Radar widget with contacts and LOS state
- Player vitals widget (health + stamina bars/text)
- Weapon status widget (weapon name + fire rate)

## In Progress / Placeholder
- Inventory system (stub section)

## Goal Checklist (Stealth TPS like MGS5)

- [ ] Solid movement feel: walk/run/sprint, crouch, prone, vault, climb
- [ ] Contextual cover: snap, lean, blind-fire, corner transitions
- [ ] Advanced camera: shoulder swap, obstruction handling, cinematic takedowns
- [ ] AI perception: vision cones, hearing, peripheral vision, last known position
- [ ] AI behavior: patrol, investigate, search, alert, combat, call reinforcements
- [ ] Stealth feedback: awareness meter, audio/visual cues, body discovery
- [ ] Stealth tools: silencers, distractions, decoys, smoke, EMP
- [ ] Non-lethal options: chokeout, stun, tranquilizer, restraints
- [ ] Combat depth: recoil, spread, damage falloff, armor, cover penetration
- [ ] Weapons: swap/holster, upgrades, attachments, ammo types
- [ ] Inventory + loadout: slot system, weight/encumbrance, quick select
- [ ] Missions: objectives, stealth ratings, time/alert penalties
- [ ] NPC interaction: hostage rescue, escort, intel extraction
- [ ] World systems: day/night cycle, dynamic lighting, weather
- [ ] UI: minimap/radar, weapon HUD, vitals, interaction prompts
- [ ] Audio: footstep surface types, alert barks, suppressed shots
- [ ] Save/load and checkpointing
- [ ] Optimization pass for AI, lighting, and streaming
