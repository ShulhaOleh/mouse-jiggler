#!/usr/bin/env python3
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent
README = ROOT / "readme.md"
COMMENTS_FILE = ROOT / "scripts" / "project_comments.json"

START = "<!-- STRUCTURE_START -->"
END = "<!-- STRUCTURE_END -->"

EXCLUDED_DIRS = {".git", ".github", "release"}
EXCLUDED_FILES = {".gitignore"}


def load_comments() -> dict[str, str]:
    if not COMMENTS_FILE.exists():
        return {}
    
    try:
        with open(COMMENTS_FILE, encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        return {}


def generate_tree(path: Path, prefix: str = "", rel_path: str = "") -> list[tuple[str, str]]:
    lines = []
    try:
        items = list(path.iterdir())
    except PermissionError:
        return lines

    items = [
        item for item in items
        if item.name not in EXCLUDED_DIRS and item.name not in EXCLUDED_FILES
    ]

    dirs = sorted(
        (item for item in items if item.is_dir()),
        key=lambda p: p.name.lower()
    )
    files = sorted(
        (item for item in items if not item.is_dir()),
        key=lambda p: p.name.lower()
    )

    ordered_items = dirs + files

    count = len(ordered_items)
    for i, item in enumerate(ordered_items):
        is_last = (i == count - 1)
        connector = "└── " if is_last else "├── "

        name = item.name + ("/" if item.is_dir() else "")
        item_rel_path = f"{rel_path}{name}".lstrip("/")
        
        line = f"{prefix}{connector}{name}"
        lines.append((line, item_rel_path))

        if item.is_dir():
            new_prefix = f"{prefix}    " if is_last else f"{prefix}│   "
            lines.extend(generate_tree(item, new_prefix, item_rel_path))

    return lines


def format_tree_with_comments(lines: list[tuple[str, str]], comments: dict[str, str]) -> list[str]:
    if not lines:
        return []
    
    max_length = max(len(line) for line, _ in lines)
    padding = 4
    
    formatted = []
    for line, path in lines:
        if path in comments:
            spaces = max_length - len(line) + padding
            formatted.append(f"{line}{' ' * spaces}# {comments[path]}")
        else:
            formatted.append(line)
    
    return formatted


def generate_structure() -> str:
    comments = load_comments()
    root_name = ROOT.name
    
    lines = [(f"{root_name}/", f"{root_name}/")]
    lines.extend(generate_tree(ROOT, rel_path=""))
    
    formatted_lines = format_tree_with_comments(lines, comments)
    return "\n".join(formatted_lines)


def update_readme(tree_text: str) -> bool:
    if not README.exists():
        print(f"readme not found: {README}", file=sys.stderr)
        return False
    
    content = README.read_text(encoding="utf-8")
    
    if START not in content or END not in content:
        print(f"Markers {START} and {END} not found in {README}", file=sys.stderr)
        return False
    
    before, rest = content.split(START, 1)
    _, after = rest.split(END, 1)
    new_block = f"{START}\n\n```\n{tree_text.rstrip()}\n```\n\n{END}"
    new_content = before + new_block + after
    
    if new_content != content:
        README.write_text(new_content, encoding="utf-8")
        return True
    return False


tree_text = generate_structure()

if not tree_text:
    print("Failed to generate structure", file=sys.stderr)
    sys.exit(1)

print(f"Checking {README.name}...")
readme_changed = update_readme(tree_text)

if readme_changed:
    print(f"{README.name} structure updated")
    try:
        subprocess.check_call(["git", "add", str(README)])
        print(f"{README.name} staged for commit")
    except subprocess.CalledProcessError as e:
        print(f"Failed to stage {README.name}: {e}", file=sys.stderr)
        sys.exit(1)
else:
    print(f"{README.name} structure is already up to date")