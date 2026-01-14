
import argparse
import subprocess
import shutil
import pathlib
import sys
import os
import time
import platform
from pathlib import Path
import matplotlib.pyplot as plt

FAILED_COLOR = "\033[91m"
PASSED_COLOR = "\033[92m"
SYSTEM_COLOR = "\033[0m"

def build_with_cmake(build_path: str):
    build_dir = Path(build_path)

    # ✅ Remove existing build directory
    if build_dir.exists():
        shutil.rmtree(build_dir)

    # ✅ Create new build directory
    build_dir.mkdir(parents=True, exist_ok=True)

    # ✅ Run CMake configuration
    try:
        subprocess.run(["cmake", ".."], cwd=build_dir, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error during CMake configuration: {e}")
        raise

    # ✅ Build the project
    try:
        subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error during build: {e}")
        raise

    # ✅ Return path to executable
    exe_name = "rt"
    if platform.system().lower() == "windows":
        exe_name += ".exe"
    exe_path = pathlib.Path("./bin") / exe_name
    if not exe_path.exists():
        print(f"Error: Executable {exe_path} not found after build.")
        return None
    return str(exe_path.resolve())

def format_time(ms: int, width: int = 8) -> str:
    formatted = f"{ms:,}".replace(",", "'")
    return f"{formatted:>{width}}ms"

def plot_ray_and_box(ray_beg, ray_end, box_l, box_u, window_title="Ray and Box"):
    """Render the ray and box in 3D using Matplotlib."""
    fig = plt.figure()
    fig.canvas.manager.set_window_title(window_title)  # Set window title to test name
    ax = fig.add_subplot(111, projection='3d')

    # Box edges
    x_min, y_min, z_min = box_l
    x_max, y_max, z_max = box_u
    edges = [
        ([x_min, x_max], [y_min, y_min], [z_min, z_min]),
        ([x_min, x_max], [y_max, y_max], [z_min, z_min]),
        ([x_min, x_min], [y_min, y_max], [z_min, z_min]),
        ([x_max, x_max], [y_min, y_max], [z_min, z_min]),
        ([x_min, x_max], [y_min, y_min], [z_max, z_max]),
        ([x_min, x_max], [y_max, y_max], [z_max, z_max]),
        ([x_min, x_min], [y_min, y_max], [z_max, z_max]),
        ([x_max, x_max], [y_min, y_max], [z_max, z_max]),
        ([x_min, x_min], [y_min, y_min], [z_min, z_max]),
        ([x_max, x_max], [y_min, y_min], [z_min, z_max]),
        ([x_min, x_min], [y_max, y_max], [z_min, z_max]),
        ([x_max, x_max], [y_max, y_max], [z_min, z_max]),
    ]
    for edge in edges:
        ax.plot(edge[0], edge[1], edge[2], color='blue')

    # Ray
    ax.plot([ray_beg[0], ray_end[0]],
            [ray_beg[1], ray_end[1]],
            [ray_beg[2], ray_end[2]], color='red', linewidth=2)

    # Points (Beg and End without coordinates)
    ax.scatter(*ray_beg, color='green', s=50, label='Beg')
    ax.scatter(*ray_end, color='orange', s=50, label='End')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.legend()
    ax.set_title('Ray and Box in 3D')

    plt.show()

def run_test(executable, infile, outfile, tol):
    """Run a single test and visualize in 3D."""
    try:
        with open(infile, "r") as fin:
            start = time.perf_counter()
            result = subprocess.run(
                [executable],
                stdin=fin,
                capture_output=True,
                text=True,
                check=False
            )
            end = time.perf_counter()
            elapsed_ms = int((end - start) * 1000)
            time_str = format_time(elapsed_ms, width=6)
    except Exception as e:
        print(f"Error running {executable} with {infile}: {e}")
        return False

    actual = result.stdout.strip()

    # Read input values for visualization
    with open(infile, "r") as f:
        lines = f.read().splitlines()
        ray_beg = list(map(float, lines[0].split()))
        ray_end = list(map(float, lines[1].split()))
        box_l = list(map(float, lines[2].split()))
        box_u = list(map(float, lines[3].split()))

    # Write .ans file
    ans_file = infile.with_suffix(".ans")
    ans_file.write_text(actual)

    # Show 3D visualization
    plot_ray_and_box(ray_beg, ray_end, box_l, box_u, window_title=infile.name)

    verdict = "[PASS]"
    log_str = f"{verdict:<12}{infile.name:>8}{time_str:>12}"
    print(f"{PASSED_COLOR}{log_str}{SYSTEM_COLOR}")
    return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--tol", type=float, default=1e-4, help="Tolerance for float comparison")
    args = parser.parse_args()

    src_dir = pathlib.Path("./src")
    tests_dir = pathlib.Path("./tests")

    if not src_dir.exists():
        print(f"Error: Source directory {src_dir} not found")
        sys.exit(1)
    if not tests_dir.is_dir():
        print(f"Error: Tests directory {tests_dir} not found")
        sys.exit(1)

    executable = build_with_cmake("build")
    if not executable:
        sys.exit(1)

    in_files = sorted(tests_dir.glob("*.in"))
    if not in_files:
        print("No .in files found in tests/")
        sys.exit(1)

    test_count = len(in_files)
    pass_count = 0

    print(f"{'Verdict':<12}{'Test':>8}{'Time':>12}")
    for infile in in_files:
        outfile = infile.with_suffix(".out")
        if run_test(executable, infile, outfile, args.tol):
            pass_count += 1

    status = "✅" if pass_count == test_count else "❌"
    summary_line = f"\nSummary {status}: {pass_count}/{test_count} tests processed"
    print(f"{PASSED_COLOR if pass_count == test_count else FAILED_COLOR}{summary_line}{SYSTEM_COLOR}")

if __name__ == "__main__":
    main()
