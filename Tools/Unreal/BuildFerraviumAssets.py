"""Import Ferravium item meshes, smelter mesh and generated item icons in the SML Starter Project."""
import os
import struct
import zlib
import unreal

PROJECT = unreal.Paths.project_dir()
PLUGIN_ROOT = os.path.join(PROJECT, "Mods", "GameFeatures", "FICSITContracts")
SOURCE_ROOT = os.path.join(PROJECT, "Mods", "FICSITContracts", "ContentSource")
RESOURCE_ROOT = os.path.join(SOURCE_ROOT, "Resources", "Ferravium")
MESH_ROOT = os.path.join(RESOURCE_ROOT, "Mesh")
ICON_ROOT = os.path.join(RESOURCE_ROOT, "UI")
SMELTER_ROOT = os.path.join(SOURCE_ROOT, "Buildables", "FerraviumSmelter")
MOUNT = "/FICSITContracts"


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def make_icon(kind, path):
    width = height = 256
    pixels = bytearray(width * height * 3)

    def put(x, y, colour):
        if 0 <= x < width and 0 <= y < height:
            offset = (y * width + x) * 3
            pixels[offset:offset + 3] = bytes(colour)

    def rect(x0, y0, x1, y1, colour):
        for y in range(max(0, y0), min(height, y1)):
            for x in range(max(0, x0), min(width, x1)):
                put(x, y, colour)

    def line(x0, y0, x1, y1, colour, thickness=3):
        dx = abs(x1 - x0)
        sx = 1 if x0 < x1 else -1
        dy = -abs(y1 - y0)
        sy = 1 if y0 < y1 else -1
        error = dx + dy
        while True:
            radius = max(0, thickness // 2)
            rect(x0 - radius, y0 - radius, x0 + radius + 1, y0 + radius + 1, colour)
            if x0 == x1 and y0 == y1:
                break
            twice = 2 * error
            if twice >= dy:
                error += dy
                x0 += sx
            if twice <= dx:
                error += dx
                y0 += sy

    def polygon(points, colour):
        minimum_y = max(0, min(point[1] for point in points))
        maximum_y = min(height - 1, max(point[1] for point in points))
        for y in range(minimum_y, maximum_y + 1):
            intersections = []
            for index, first in enumerate(points):
                second = points[(index + 1) % len(points)]
                if first[1] == second[1]:
                    continue
                if min(first[1], second[1]) <= y < max(first[1], second[1]):
                    ratio = (y - first[1]) / float(second[1] - first[1])
                    intersections.append(first[0] + ratio * (second[0] - first[0]))
            intersections.sort()
            for index in range(0, len(intersections) - 1, 2):
                for x in range(int(intersections[index]), int(intersections[index + 1]) + 1):
                    put(x, y, colour)

    def circle(cx, cy, radius, colour):
        for y in range(cy - radius, cy + radius + 1):
            for x in range(cx - radius, cx + radius + 1):
                if (x - cx) * (x - cx) + (y - cy) * (y - cy) <= radius * radius:
                    put(x, y, colour)

    for y in range(height):
        blend = y / float(height)
        for x in range(width):
            put(x, y, (int(6 + 5 * blend), int(15 + 8 * blend), int(26 + 12 * blend)))

    rect(12, 12, 244, 244, (22, 39, 53))
    rect(17, 17, 239, 239, (8, 17, 27))
    orange = (255, 103, 27)
    glow = (255, 157, 51)
    steel = (91, 119, 143)
    pale = (188, 211, 226)
    dark = (35, 52, 67)
    black = (4, 9, 15)
    cyan = (44, 186, 226)

    if kind == "ore":
        polygon([(45, 163), (60, 91), (113, 42), (177, 53), (215, 112),
                 (207, 180), (151, 218), (77, 207)], dark)
        polygon([(60, 91), (113, 42), (133, 111), (92, 149)], (61, 81, 101))
        polygon([(113, 42), (177, 53), (181, 128), (133, 111)], (29, 46, 62))
        polygon([(133, 111), (181, 128), (207, 180), (151, 218), (131, 167)], (46, 65, 82))
        polygon([(45, 163), (92, 149), (131, 167), (151, 218), (77, 207)], (20, 34, 47))
        line(76, 171, 113, 129, orange, 7)
        line(113, 129, 144, 118, glow, 6)
        line(144, 118, 174, 77, orange, 7)
        line(120, 181, 157, 154, orange, 5)
        line(157, 154, 189, 147, glow, 4)
        circle(113, 129, 5, glow)
    elif kind == "concentrate":
        polygon([(40, 177), (72, 94), (118, 121), (148, 71), (217, 166), (183, 204), (75, 207)], (43, 62, 80))
        polygon([(72, 94), (118, 121), (99, 178), (40, 177)], steel)
        polygon([(118, 121), (148, 71), (171, 159), (99, 178)], (80, 103, 123))
        polygon([(148, 71), (217, 166), (171, 159)], (31, 48, 65))
        polygon([(99, 178), (171, 159), (183, 204), (75, 207)], (22, 36, 49))
        for px, py in ((84, 116), (146, 126), (178, 170), (113, 190), (64, 180)):
            line(px - 7, py + 4, px, py - 7, orange, 3)
        circle(145, 126, 4, glow)
    elif kind == "ingot":
        polygon([(37, 104), (190, 81), (220, 153), (72, 190)], steel)
        polygon([(37, 104), (190, 81), (169, 121), (58, 137)], pale)
        polygon([(58, 137), (169, 121), (220, 153), (72, 190)], (46, 66, 84))
        line(51, 143, 202, 124, orange, 7)
        line(72, 161, 207, 143, glow, 3)
        line(80, 100, 185, 86, (227, 239, 246), 3)
    else:
        # Furnace silhouette, heat window, twin exhausts and FICSIT-style hazard trim.
        rect(44, 105, 212, 211, black)
        rect(34, 187, 222, 221, orange)
        rect(53, 95, 203, 188, steel)
        rect(66, 108, 190, 177, dark)
        rect(78, 121, 178, 164, orange)
        rect(89, 130, 167, 155, black)
        rect(97, 136, 159, 149, glow)
        rect(72, 63, 112, 111, dark)
        rect(145, 63, 185, 111, dark)
        rect(80, 40, 104, 66, steel)
        rect(153, 40, 177, 66, steel)
        rect(76, 35, 108, 42, orange)
        rect(149, 35, 181, 42, orange)
        rect(191, 125, 216, 168, black)
        rect(195, 131, 212, 148, cyan)
        for x in range(55, 204, 24):
            line(x, 198, x + 12, 198, black, 4)

    # Four corner fasteners and a subtle status marker.
    for x in (27, 229):
        for y in (27, 229):
            circle(x, y, 3, orange)
    circle(218, 38, 5, cyan)

    raw = b"".join(b"\x00" + bytes(pixels[y * width * 3:(y + 1) * width * 3]) for y in range(height))

    def chunk(kind_bytes, data):
        body = kind_bytes + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xffffffff)

    png = (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", struct.pack(">2I5B", width, height, 8, 2, 0, 0, 0))
        + chunk(b"IDAT", zlib.compress(raw, 9))
        + chunk(b"IEND", b"")
    )
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "wb") as output:
        output.write(png)


def import_asset(asset_tools, filename, destination, name):
    require(os.path.isfile(filename), "Source asset missing: " + filename)
    task = unreal.AssetImportTask()
    task.filename = filename
    task.destination_path = destination
    task.destination_name = name
    task.replace_existing = True
    task.automated = True
    task.save = True
    asset_tools.import_asset_tasks([task])
    asset_path = destination + "/" + name + "." + name
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    require(asset is not None, "Import failed: " + asset_path)
    return asset


require(os.path.isfile(os.path.join(PLUGIN_ROOT, "FICSITContracts.uplugin")),
        "Game Feature plugin not found: " + PLUGIN_ROOT)
require(os.path.isfile(os.path.join(MESH_ROOT, "Ferravium.mtl")),
        "Ferravium.mtl is missing from the resource mesh folder.")
require(os.path.isfile(os.path.join(SMELTER_ROOT, "Ferravium.mtl")),
        "Ferravium.mtl sidecar is missing from the smelter folder.")

os.makedirs(ICON_ROOT, exist_ok=True)
icon_specs = [
    ("ore", "TX_FerraviumOre_256"),
    ("concentrate", "TX_FerraviumConcentrate_256"),
    ("ingot", "TX_FerraviumIngot_256"),
    ("smelter", "TX_FerraviumSmelter_256"),
]
for kind, name in icon_specs:
    make_icon(kind, os.path.join(ICON_ROOT, name + ".png"))

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
resource_mesh_path = MOUNT + "/Resources/Ferravium/Mesh"
resource_ui_path = MOUNT + "/Resources/Ferravium/UI"
smelter_mesh_path = MOUNT + "/Buildables/FerraviumSmelter"

for filename, destination, name in [
    (os.path.join(MESH_ROOT, "FerraviumOre.obj"), resource_mesh_path, "SM_FerraviumOre"),
    (os.path.join(MESH_ROOT, "FerraviumConcentrate.obj"), resource_mesh_path, "SM_FerraviumConcentrate"),
    (os.path.join(MESH_ROOT, "FerraviumIngot.obj"), resource_mesh_path, "SM_FerraviumIngot"),
    (os.path.join(SMELTER_ROOT, "FerraviumSmelter.obj"), smelter_mesh_path, "SM_FerraviumSmelter"),
]:
    import_asset(asset_tools, filename, destination, name)

for kind, name in icon_specs:
    import_asset(asset_tools, os.path.join(ICON_ROOT, name + ".png"), resource_ui_path, name)

unreal.log("Ferravium source meshes and UI icons imported. Native item descriptors, recipes and the smelter resolve these asset paths.")
unreal.log_warning("This script does not place Ferravium resource nodes in the vanilla World Partition map and does not compile the C++ plugin.")
