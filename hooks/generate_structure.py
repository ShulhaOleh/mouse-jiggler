#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path

# TODO: add bash script for Linux

ROOT = Path.cwd()
SCRIPTS = ROOT / "scripts"
STRUCT = SCRIPTS / "structure.txt"
README = ROOT / "readme.md"

START = "<!-- STRUCTURE_START -->"
END = "<!-- STRUCTURE_END -->"


def run_generator():
    ps1 = SCRIPTS / "generate_structure.ps1"

    if not ps1.exists():
        raise FileNotFoundError(f"PowerShell script not found: {ps1}")

    try:
        subprocess.check_call(
            ["powershell", "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass", "-File", str(ps1)],
            cwd=str(SCRIPTS)
        )
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"PowerShell script execution failed: {e}")


def read_file(p: Path):
    return p.read_text(encoding="utf-8") if p.exists() else ""


def update_readme(tree_text: str) -> bool:
    content = read_file(README)
    if START not in content or END not in content:
        print(f"Markers {START} {END} not found in {README}", file=sys.stderr)
        return False

    before, rest = content.split(START, 1)
    _, after = rest.split(END, 1)
    new_block = f"{START}\n\n```\n{tree_text.rstrip()}\n```\n\n{END}"
    new_content = before + new_block + after

    if new_content != content:
        README.write_text(new_content, encoding="utf-8")
        return True
    return False


def git_add(paths):
    subprocess.check_call(["git", "add"] + [str(p) for p in paths])


def main():
    try:
        run_generator()
    except Exception as e:
        print("Failed to run structure generator:", e, file=sys.stderr)
        sys.exit(1)

    tree_text = read_file(STRUCT)
    if not tree_text:
        print(f"{STRUCT} is empty or missing", file=sys.stderr)
        sys.exit(1)

    changed = update_readme(tree_text)
    staged = False

    paths_to_add = [STRUCT]
    
    if changed:
        paths_to_add.append(README)
    
    try:
        git_add(paths_to_add)
        staged = True
    except subprocess.CalledProcessError as e:
        print("git add failed:", e, file=sys.stderr)

    if staged:
        print("Updated structure and staged changes:", ", ".join(str(p) for p in paths_to_add))
    else:
        print("No files staged.")
    
    return 0


if __name__ == "__main__":
    sys.exit(main())