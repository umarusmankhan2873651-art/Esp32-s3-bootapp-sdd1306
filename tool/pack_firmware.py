#!/usr/bin/env python3
"""Pack a PlatformIO/Arduino build output into a SlayerX-loadable folder."""
import argparse, hashlib, json, os, shutil, sys

def find_app(src):
    for root, _, files in os.walk(src):
        for f in files:
            if f == "firmware.bin":
                return os.path.join(root, f)
    for root, _, files in os.walk(src):
        for f in files:
            if f.endswith(".bin") and "bootloader" not in f and "partitions" not in f:
                return os.path.join(root, f)
    return None

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("build_dir")
    ap.add_argument("--name", required=True)
    ap.add_argument("--version", default="1.0.0")
    ap.add_argument("--out", default="dist")
    ap.add_argument("--assets", default=None)
    args = ap.parse_args()

    app = find_app(args.build_dir)
    if not app:
        sys.exit("no app .bin found in " + args.build_dir)

    dest = os.path.join(args.out, args.name.lower().replace(" ", "_"))
    os.makedirs(dest, exist_ok=True)

    target = os.path.join(dest, "app.bin")
    shutil.copy2(app, target)
    size = os.path.getsize(target)
    md5 = hashlib.md5(open(target, "rb").read()).hexdigest()

    manifest = {
        "name": args.name, "version": args.version, "app": "app.bin",
        "size": size, "md5": md5, "built_for": "slayerx",
        "psram": "octal", "flash_mb": 16,
        "assets": "assets" if args.assets else None,
        "notes": ""
    }
    with open(os.path.join(dest, "manifest.json"), "w") as f:
        json.dump(manifest, f, indent=2)

    if args.assets and os.path.isdir(args.assets):
        shutil.copytree(args.assets, os.path.join(dest, "assets"), dirs_exist_ok=True)

    print(f"packed -> {dest}\n  size: {size} bytes\n  md5 : {md5}")
    if size > 5 * 1024 * 1024:
        print("  WARNING: exceeds the 5 MB ota_0 slot")

if __name__ == "__main__":
    main()