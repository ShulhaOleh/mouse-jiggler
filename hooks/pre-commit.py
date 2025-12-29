#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent
SCRIPTS = ROOT / "scripts"


def run_script(script_path: Path, description: str) -> bool:
    print(f"Running: {description}...")
    try:
        result = subprocess.run(
            [sys.executable, str(script_path)],
            cwd=str(ROOT),
            capture_output=False,
            check=True
        )
        print(f"{description} completed")
        return True
    except subprocess.CalledProcessError as e:
        print(f"{description} failed with exit code {e.returncode}", file=sys.stderr)
        return False
    except Exception as e:
        print(f"{description} error: {e}", file=sys.stderr)
        return False


def main():
    hooks = [
        (SCRIPTS / "generate_structure.py", "Generate project structure")
    ]
    
    print("=" * 60)
    print("Running pre-commit hooks...")
    print("=" * 60)
    
    failed = []
    for script_path, description in hooks:
        if not script_path.exists():
            print(f"Skipping {description}: {script_path} not found")
            continue
        
        if not run_script(script_path, description):
            failed.append(description)
        print()
    
    print("=" * 60)
    if failed:
        print(f"Pre-commit failed. Failed hooks: {', '.join(failed)}")
        return 1
    else:
        print("All pre-commit hooks passed")
        return 0


if __name__ == "__main__":
    sys.exit(main())
