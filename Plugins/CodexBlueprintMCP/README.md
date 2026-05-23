# Codex Blueprint MCP

Editor-only Unreal plugin plus a stdio MCP server for reading and writing Blueprint assets from Codex.

## Unreal side

Enable the `CodexBlueprintMCP` plugin and launch the Unreal Editor. The plugin starts a localhost JSON bridge on:

```text
http://127.0.0.1:8765/codex-blueprint
```

Available bridge routes:

- `GET /health`
- `POST /list`
- `POST /read`
- `POST /create`
- `POST /add-variable`
- `POST /compile`

## MCP side

Point Codex at the stdio server:

```json
{
  "mcpServers": {
    "codex-blueprint": {
      "command": "python3",
      "args": [
        "/Users/dmpatod/UnrealProjects/SneakGear/Plugins/CodexBlueprintMCP/Scripts/codex_blueprint_mcp.py"
      ]
    }
  }
}
```

Set `CODEX_BLUEPRINT_MCP_URL` if the Unreal bridge is hosted on a different URL.

## Tools

- `blueprint_health`
- `list_blueprints`
- `read_blueprint`
- `create_blueprint`
- `add_blueprint_variable`
- `compile_blueprint`

The write surface is intentionally small for the first version: create assets, add member variables, compile, and save. Graph editing can be added behind explicit tools once the desired node and pin operations are defined.
