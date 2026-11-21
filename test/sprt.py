import os
import subprocess
import shutil
import tempfile
from pathlib import Path

# --- Configuration ---
OLD_REF = "HEAD~1"
FASTCHESS = "fastchess"    # Path to fastchess executable if not in PATH
# ---------------------


def run_cmd(cmd, cwd=None):
    print(f"-> Running: {' '.join(cmd)}")
    subprocess.run(cmd, cwd=cwd, check=True)


def build_engine(source_dir, output_binary_path):
    print(f"Building engine in {source_dir}...")

    run_cmd(["make", "clean"], cwd=source_dir)
    run_cmd(["make"], cwd=source_dir)
    
    # Locate and copy binary
    built_binary = Path(source_dir) / "build" / "cubeeb"
    if not built_binary.exists():
        raise FileNotFoundError(f"Expected binary not found at {built_binary}")
    
    shutil.copy(built_binary, output_binary_path)
    print(f"Successfully copied binary to {output_binary_path}")

def main():
    repo_root = Path.cwd().resolve()
    sprt_dir = repo_root / "sprt"
    sprt_dir.mkdir(exist_ok=True)
    
    engine_old = sprt_dir / "engine-old"
    engine_new = sprt_dir / "engine-new"
    
    print(f"\n============== Step 1: Building old ({OLD_REF}) ==============")

    with tempfile.TemporaryDirectory() as temp_dir:

        temp_repo = Path(temp_dir) / "old_chess"

        run_cmd(["git", "clone", str(repo_root), str(temp_repo)])
        run_cmd(["git", "checkout", OLD_REF], cwd=temp_repo)
        build_engine(temp_repo, engine_old)
        
    print(f"\n============== Step 2: Building Current Patch (New) ==============")

    build_engine(repo_root, engine_new)
    
    print(f"\n============== Step 3: Starting SPRT Test ==============")

    fastchess_cmd = [
        FASTCHESS,
        "-engine", f"cmd={engine_new}", "name=Cubeeb New",
        "-engine", f"cmd={engine_old}", "name=Cubeeb Old",
        "-each", "tc=10+0.1",
        "-repeat", "-games", "10000",
        "-sprt", "elo0=0", "elo1=10", "alpha=0.05", "beta=0.05", "model=normalized",
        "-concurrency", "10"
    ]
    
    run_cmd(fastchess_cmd, cwd=repo_root)

if __name__ == "__main__":
    main()