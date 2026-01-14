import subprocess
import pathlib
import sys
import time
import platform
import matplotlib.pyplot as plt
import numpy as np
import colorama
from colorama import Fore, Style

colorama.init()

FAILED_COLOR = Fore.RED
PASSED_COLOR = Fore.GREEN
SYSTEM_COLOR = Style.RESET_ALL

def resolve_exe_path():
    exe_name = "rt"
    if platform.system().lower() == "windows":
        exe_name += ".exe"
    exe_path = pathlib.Path("./bin") / exe_name
    if not exe_path.exists():
        print(f"{FAILED_COLOR}Error: Executable {exe_path} not found.{SYSTEM_COLOR}")
        return None
    return str(exe_path.resolve())

def format_time(ms: int, width: int = 8):
    formatted = f"{ms:,}".replace(",", "'")
    return f"{formatted:>{width}}ms"

def plot_ray_and_box(ray_beg, ray_end, box_l, box_u, intersection=None, window_title="Ray and Box"):
    fig = plt.figure()
    fig.canvas.manager.set_window_title(window_title)

    ax = fig.add_subplot(111, projection='3d')

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
        ax.plot(edge[0], edge[1], edge[2], color='blue', linewidth=1)

    ray_beg = np.array(ray_beg)
    ray_end = np.array(ray_end)
    direction = ray_end - ray_beg
    if np.linalg.norm(direction) == 0:
        direction = np.array([1, 0, 0])

    extend = min(max(x_max - x_min, y_max - y_min, z_max - z_min) * 1.5, 20)
    ray_st = ray_beg - direction * extend
    ray_fn = ray_beg + direction * extend

    ax.plot(
        [ray_st[0], ray_fn[0]],
        [ray_st[1], ray_fn[1]],
        [ray_st[2], ray_fn[2]],
        color='gray', linewidth=1, label='Ray'
    )

    ax.scatter(*ray_beg, color='green' , s=50, label='Beg')
    ax.scatter(*ray_end, color='orange', s=50, label='End')

    if intersection is not None:
        ax.scatter(*intersection, color='red', s=25, label='AtP')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.legend()
    ax.set_title('Ray and Box in 3D')

    all_points = np.array([ray_st, ray_fn, box_l, box_u])
    if intersection is not None:
        all_points = np.vstack([all_points, intersection])
    ax.set_xlim(np.min(all_points[:,0])-1, np.max(all_points[:,0])+1)
    ax.set_ylim(np.min(all_points[:,1])-1, np.max(all_points[:,1])+1)
    ax.set_zlim(np.min(all_points[:,2])-1, np.max(all_points[:,2])+1)

    plt.show()

def run_test(executable, infile):
    try:
        with open(infile, "r") as fin:
            start = time.perf_counter()
            result = subprocess.run(
                [executable, "-t"],
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

    with open(infile, "r") as f:
        lines = f.read().splitlines()

        ray_beg = list(map(float, lines[0].split()))
        ray_end = list(map(float, lines[1].split()))

        box_l = list(map(float, lines[2].split()))
        box_u = list(map(float, lines[3].split()))

    intersection = None
    output_lines = actual.splitlines()
    if output_lines and output_lines[0].strip().lower() != "null":
        try:
            intersection = list(map(float, output_lines[0].split()))
        except Exception as e:
            print(f"{FAILED_COLOR}Intersection parse error: {e}{SYSTEM_COLOR}")

    plot_ray_and_box(ray_beg, ray_end, box_l, box_u, intersection, window_title=infile.name)

    verdict = "[PASS]"
    log_str = f"{verdict:<12}{infile.name:>8}{time_str:>12}"
    print(f"{PASSED_COLOR}{log_str}{SYSTEM_COLOR}")
    return True

def main():
    src_dir = pathlib.Path("./src")
    tst_dir = pathlib.Path("./tests")

    if not src_dir.exists():
        print(f"{FAILED_COLOR}Error: Src directory {src_dir} not found{SYSTEM_COLOR}")
        sys.exit(1)
    if not tst_dir.is_dir():
        print(f"{FAILED_COLOR}Error: Tst directory {tst_dir} not found{SYSTEM_COLOR}")
        sys.exit(1)

    executable = resolve_exe_path()
    if not executable:
        sys.exit(1)

    in_files = sorted(tst_dir.glob("*.in"))
    if not in_files:
        print(f"{FAILED_COLOR}No .in files found in tests/{SYSTEM_COLOR}")
        sys.exit(1)

    print(f"{'Verdict':<12}{'Test':>8}{'Time':>12}")
    for infile in in_files:
        run_test(executable, infile)

if __name__ == "__main__":
    main()
