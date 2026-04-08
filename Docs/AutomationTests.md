# Automated Tests

## Running Tests

Run the full automation suite:

```bash
./Scripts/run-tests.sh
```

Run a narrower subset:

```bash
./Scripts/run-tests.sh SneakGear.Inventory.
./Scripts/run-tests.sh SneakGear.UI. --no-build
```

## Notes

- The script builds `SneakGearEditor` first unless `--no-build` is passed
- Reports are exported to `Docs/AutomationReports/`
- The generated Markdown summary is at [`Docs/AutomationReports/TestStatus.md`](AutomationReports/TestStatus.md)
- Override `UE_EDITOR_BIN` or `UE_BUILD_SH` if Unreal is installed in a different location

## Test Suites

| Suite | File | Coverage |
|-------|------|----------|
| `SneakGear.Inventory` | `TestInventory` | Slot management, ammo bookkeeping, weapon slot state |
| `SneakGear.AI` | `TestAI` | Guard awareness, perception, state transitions |
| `SneakGear.Weapons` | `TestWeapons` | Firing, reload, fire mode switching |
| `SneakGear.Characters` | `TestCharacters` | Character lifecycle, death, damage handling |
| `SneakGear.UI` | `TestUIWidgets` | UI data source responses and widget state |
