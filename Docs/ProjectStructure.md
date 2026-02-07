# Project Structure

This repo follows Unreal Engine's expected root layout. The folders below are the ones you should treat as source-of-truth vs generated output.

## Source of truth
- Config/         Engine and project configuration
- Content/        Game assets (uasset/umap)
- Plugins/        Local plugins
- Source/         C++ source code
- SneakGear.uproject  Project descriptor

## Generated (do not commit)
- Binaries/       Build outputs
- Build/          Staging outputs
- DerivedDataCache/  Editor cache
- Intermediate/  Build intermediates
- Saved/         Autosaves, logs, config overrides

## Conventions
- Keep new project docs in Docs/
- Put one-off scripts and automation in Scripts/
- Add third-party source drops under ThirdParty/ (create as needed)
