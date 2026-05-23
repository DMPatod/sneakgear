#!/usr/bin/env python3
"""MCP stdio server for the Codex Blueprint MCP Unreal Editor plugin."""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request
from typing import Any


UNREAL_URL = os.environ.get("CODEX_BLUEPRINT_MCP_URL", "http://127.0.0.1:8765")
BASE_PATH = "/codex-blueprint"


TOOLS = [
    {
        "name": "blueprint_health",
        "description": "Check whether the Unreal Editor Codex Blueprint MCP plugin is reachable.",
        "inputSchema": {"type": "object", "properties": {}, "additionalProperties": False},
    },
    {
        "name": "list_blueprints",
        "description": "List Blueprint assets under /Game.",
        "inputSchema": {"type": "object", "properties": {}, "additionalProperties": False},
    },
    {
        "name": "read_blueprint",
        "description": "Read Blueprint metadata, variables, graphs, nodes, and pins.",
        "inputSchema": {
            "type": "object",
            "required": ["asset_path"],
            "properties": {
                "asset_path": {
                    "type": "string",
                    "description": "Blueprint object path, for example /Game/Blueprints/BP_Player.BP_Player.",
                }
            },
            "additionalProperties": False,
        },
    },
    {
        "name": "create_blueprint",
        "description": "Create a Blueprint asset in the Unreal project.",
        "inputSchema": {
            "type": "object",
            "required": ["package_path", "name"],
            "properties": {
                "package_path": {"type": "string", "description": "Package folder, for example /Game/Blueprints."},
                "name": {"type": "string", "description": "New Blueprint asset name."},
                "parent_class": {
                    "type": "string",
                    "description": "Parent class path. Defaults to Actor. Example: /Script/Engine.Actor.",
                },
            },
            "additionalProperties": False,
        },
    },
    {
        "name": "add_blueprint_variable",
        "description": "Add a member variable to a Blueprint, then compile and save it.",
        "inputSchema": {
            "type": "object",
            "required": ["asset_path", "name"],
            "properties": {
                "asset_path": {"type": "string"},
                "name": {"type": "string"},
                "type": {
                    "type": "string",
                    "description": "bool, int, float, string, name, text, or vector. Defaults to string.",
                },
            },
            "additionalProperties": False,
        },
    },
    {
        "name": "compile_blueprint",
        "description": "Compile and save a Blueprint.",
        "inputSchema": {
            "type": "object",
            "required": ["asset_path"],
            "properties": {"asset_path": {"type": "string"}},
            "additionalProperties": False,
        },
    },
]


def read_message() -> dict[str, Any] | None:
    headers: dict[str, str] = {}
    while True:
        line = sys.stdin.buffer.readline()
        if line == b"":
            return None
        line = line.decode("ascii").strip()
        if line == "":
            break
        key, _, value = line.partition(":")
        headers[key.lower()] = value.strip()

    length = int(headers.get("content-length", "0"))
    if length <= 0:
        return None
    return json.loads(sys.stdin.buffer.read(length).decode("utf-8"))


def write_message(message: dict[str, Any]) -> None:
    payload = json.dumps(message, separators=(",", ":")).encode("utf-8")
    sys.stdout.buffer.write(f"Content-Length: {len(payload)}\r\n\r\n".encode("ascii"))
    sys.stdout.buffer.write(payload)
    sys.stdout.buffer.flush()


def call_unreal(path: str, payload: dict[str, Any] | None = None, method: str = "POST") -> dict[str, Any]:
    data = None if payload is None else json.dumps(payload).encode("utf-8")
    request = urllib.request.Request(
        f"{UNREAL_URL}{BASE_PATH}{path}",
        data=data,
        method=method,
        headers={"Content-Type": "application/json"},
    )
    try:
        with urllib.request.urlopen(request, timeout=15) as response:
            return json.loads(response.read().decode("utf-8"))
    except urllib.error.URLError as exc:
        return {
            "ok": False,
            "error": f"Unreal Editor bridge is not reachable at {UNREAL_URL}{BASE_PATH}: {exc}",
        }


def tool_result(data: dict[str, Any]) -> dict[str, Any]:
    return {
        "content": [
            {
                "type": "text",
                "text": json.dumps(data, indent=2, sort_keys=True),
            }
        ],
        "isError": not bool(data.get("ok", False)),
    }


def call_tool(name: str, arguments: dict[str, Any]) -> dict[str, Any]:
    if name == "blueprint_health":
        return tool_result(call_unreal("/health", None, method="GET"))
    if name == "list_blueprints":
        return tool_result(call_unreal("/list", {}))
    if name == "read_blueprint":
        return tool_result(call_unreal("/read", arguments))
    if name == "create_blueprint":
        return tool_result(call_unreal("/create", arguments))
    if name == "add_blueprint_variable":
        return tool_result(call_unreal("/add-variable", arguments))
    if name == "compile_blueprint":
        return tool_result(call_unreal("/compile", arguments))
    return tool_result({"ok": False, "error": f"Unknown tool: {name}"})


def handle_request(message: dict[str, Any]) -> dict[str, Any] | None:
    method = message.get("method")
    request_id = message.get("id")
    if request_id is None:
        return None

    if method == "initialize":
        return {
            "jsonrpc": "2.0",
            "id": request_id,
            "result": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "serverInfo": {"name": "codex-blueprint-mcp", "version": "0.1.0"},
            },
        }
    if method == "tools/list":
        return {"jsonrpc": "2.0", "id": request_id, "result": {"tools": TOOLS}}
    if method == "tools/call":
        params = message.get("params", {})
        result = call_tool(params.get("name", ""), params.get("arguments", {}) or {})
        return {"jsonrpc": "2.0", "id": request_id, "result": result}

    return {
        "jsonrpc": "2.0",
        "id": request_id,
        "error": {"code": -32601, "message": f"Method not found: {method}"},
    }


def main() -> int:
    while True:
        message = read_message()
        if message is None:
            return 0
        response = handle_request(message)
        if response is not None:
            write_message(response)


if __name__ == "__main__":
    raise SystemExit(main())
