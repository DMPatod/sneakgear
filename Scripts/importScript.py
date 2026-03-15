
import unreal
import os

FBX_DIR = "/Users/dmpatod/BlenderProjects/Assets"
DEST_PATH = "/Game/Environment/VR"

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

def pick_first_existing(enum_cls, names):
    """Return the first enum value that exists in this UE build."""
    for n in names:
        if hasattr(enum_cls, n):
            return getattr(enum_cls, n)
    return None

for filename in os.listdir(FBX_DIR):
    if not filename.lower().endswith(".fbx"):
        continue

    task = unreal.AssetImportTask()
    task.filename = os.path.join(FBX_DIR, filename)
    task.destination_path = DEST_PATH
    task.automated = True
    task.replace_existing = True
    task.save = True

    ui = unreal.FbxImportUI()
    ui.import_mesh = True
    ui.import_as_skeletal = False
    ui.import_materials = False
    ui.import_textures = False
    ui.import_animations = False

    sm_data = ui.static_mesh_import_data

    # Vertex colors (✅ correct UE5 property)
    sm_data.vertex_color_import_option = unreal.VertexColorImportOption.REPLACE

    # Normals/tangents (✅ correct UE5 property: normal_import_method)
    # Enum names can vary slightly by UE version, so we pick the first that exists.
    normal_method = pick_first_existing(
        unreal.FBXNormalImportMethod,
        ["IMPORT_NORMALS_AND_TANGENTS", "IMPORT_NORMALS", "COMPUTE_NORMALS"]
    )
    if normal_method is not None:
        sm_data.normal_import_method = normal_method

    # Optional: how normals are generated if computing (MikkTSpace is typical)
    gen_method = pick_first_existing(
        unreal.FBXNormalGenerationMethod,
        ["MIKK_TSPACE", "MikkTSpace", "MikkTSpace_TangentSpace"]
    )
    if gen_method is not None and hasattr(sm_data, "normal_generation_method"):
        sm_data.normal_generation_method = gen_method

    # Lightmap UVs (property name is generate_lightmap_u_vs in UE docs)
    if hasattr(sm_data, "generate_lightmap_u_vs"):
        sm_data.generate_lightmap_u_vs = True

    task.options = ui
    asset_tools.import_asset_tasks([task])

print("FBX import complete.")
