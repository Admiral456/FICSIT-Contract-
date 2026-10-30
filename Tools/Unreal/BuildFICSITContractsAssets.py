"""Run inside the Satisfactory 1.2 / SML 3.12 Unreal Editor Python console."""
import os
import struct
import zlib
import unreal

PROJECT = unreal.Paths.project_dir()
PLUGIN_ROOT = os.path.join(PROJECT, "Mods", "GameFeatures", "FICSITContracts")
SOURCE_ROOT = os.path.join(PROJECT, "Mods", "FICSITContracts", "ContentSource")
MESH_SOURCE = os.path.join(SOURCE_ROOT, "Buildables", "DispatchTerminal", "DispatchTerminal.obj")
ICON_SOURCE = os.path.join(SOURCE_ROOT, "UI", "TX_DispatchTerminal_256.png")
PLUGIN_MOUNT = "/FICSITContracts"

def require(ok, message):
    if not ok:
        raise RuntimeError(message)

def make_icon(path):
    w = h = 256
    pixels = bytearray(w * h * 3)
    def put(x, y, c):
        if 0 <= x < w and 0 <= y < h:
            i = (y * w + x) * 3
            pixels[i:i+3] = bytes(c)
    def rect(x0, y0, x1, y1, c):
        for y in range(y0, y1):
            for x in range(x0, x1):
                put(x, y, c)
    def line(x0, y0, x1, y1, c, thick=1):
        dx, dy = abs(x1-x0), -abs(y1-y0)
        sx, sy = (1 if x0<x1 else -1), (1 if y0<y1 else -1)
        err = dx + dy
        while True:
            rect(x0-thick//2, y0-thick//2, x0+thick//2+1, y0+thick//2+1, c)
            if x0 == x1 and y0 == y1: break
            e2 = 2 * err
            if e2 >= dy: err += dy; x0 += sx
            if e2 <= dx: err += dx; y0 += sy
    for y in range(h):
        for x in range(w):
            t = y / float(h)
            put(x, y, (int(7+5*t), int(17+7*t), int(28+12*t)))
    orange=(255,105,28); steel=(92,111,128); pale=(210,229,240); cyan=(48,205,244); black=(3,10,17)
    rect(38,34,218,218,orange)
    rect(44,40,212,212,(19,31,43))
    rect(61,55,195,147,steel)
    rect(67,61,189,141,black)
    rect(78,73,178,77,cyan)
    rect(78,86,153,90,pale)
    rect(78,99,168,103,pale)
    rect(78,112,132,116,orange)
    rect(79,126,96,132,cyan); rect(103,119,120,132,cyan)
    rect(127,108,144,132,cyan); rect(151,96,168,132,cyan)
    rect(101,151,155,178,steel)
    rect(112,151,144,188,steel)
    rect(68,183,188,198,orange)
    rect(78,188,178,193,black)
    for x in (82,98,114,130,146,162,178):
        rect(x,204,x+5,209,cyan if x in (82,130,178) else pale)
    # Corner fasteners and status lamps.
    for x in (51,201):
        for y in (47,205):
            rect(x-3,y-3,x+4,y+4,orange)
    rect(119,22,137,32,cyan)
    # PNG encoder (RGB, no external imaging dependency).
    raw=b"".join(b"\x00"+bytes(pixels[y*w*3:(y+1)*w*3]) for y in range(h))
    def chunk(kind, data):
        body=kind+data
        return struct.pack(">I",len(data))+body+struct.pack(">I",zlib.crc32(body)&0xffffffff)
    png=b"\x89PNG\r\n\x1a\n"+chunk(b"IHDR",struct.pack(">2I5B",w,h,8,2,0,0,0))+chunk(b"IDAT",zlib.compress(raw,9))+chunk(b"IEND",b"")
    os.makedirs(os.path.dirname(path),exist_ok=True)
    with open(path,"wb") as f: f.write(png)

require(os.path.isfile(MESH_SOURCE), "DispatchTerminal.obj not found: "+MESH_SOURCE)
make_icon(ICON_SOURCE)
asset_tools=unreal.AssetToolsHelpers.get_asset_tools()
for filename, destination, name in [
    (MESH_SOURCE, PLUGIN_MOUNT+"/Buildables/DispatchTerminal", "SM_DispatchTerminal"),
    (ICON_SOURCE, PLUGIN_MOUNT+"/UI", "TX_DispatchTerminal_256")
]:
    task=unreal.AssetImportTask()
    task.filename=filename
    task.destination_path=destination
    task.destination_name=name
    task.replace_existing=True
    task.automated=True
    task.save=True
    asset_tools.import_asset_tasks([task])

mesh_path=PLUGIN_MOUNT+"/Buildables/DispatchTerminal/SM_DispatchTerminal.SM_DispatchTerminal"
icon_path=PLUGIN_MOUNT+"/UI/TX_DispatchTerminal_256.TX_DispatchTerminal_256"
mesh=unreal.EditorAssetLibrary.load_asset(mesh_path)
require(mesh is not None, "Terminal mesh import failed.")
# The gunmetal/steel surfaces use the game's customization-aware material.
# Orange, black and screen slots retain their dedicated source materials.
factory_material=unreal.EditorAssetLibrary.load_asset("/Game/FactoryGame/-Shared/Material/MI_Factory_Base_01")
if factory_material:
    static_materials=mesh.get_editor_property("static_materials")
    for static_material in static_materials:
        slot_name=str(static_material.get_editor_property("material_slot_name"))
        if slot_name in ("Gunmetal","Steel"):
            static_material.set_editor_property("material_interface",factory_material)
    mesh.set_editor_property("static_materials",static_materials)
else:
    unreal.log_warning("MI_Factory_Base_01 was not found; verify the game material path before final packaging.")
unreal.EditorAssetLibrary.save_loaded_asset(mesh)
require(unreal.EditorAssetLibrary.load_asset(icon_path) is not None, "Terminal icon import failed.")

game_feature_class=unreal.load_class(None,"/Script/FactoryGame.FGGameFeatureData")
require(game_feature_class is not None,"FGGameFeatureData class unavailable; check SML 3.12 Starter Project.")
feature_path=PLUGIN_MOUNT+"/FICSITContracts"
if not unreal.EditorAssetLibrary.does_asset_exist(feature_path):
    asset_tools.create_asset("FICSITContracts",PLUGIN_MOUNT,game_feature_class,unreal.DataAssetFactory())
feature=unreal.EditorAssetLibrary.load_asset(feature_path+".FICSITContracts")
require(feature is not None,"Failed to create FICSITContracts Game Feature Data asset.")
unreal.EditorAssetLibrary.save_loaded_asset(feature)
unreal.log("Dispatch Terminal mesh, icon and Game Feature Data asset created. Set Initial State to Active before packaging.")
