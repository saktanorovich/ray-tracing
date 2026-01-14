
import pathlib

tests_dir = pathlib.Path("tests")
tests_dir.mkdir(exist_ok=True)

def write_test(idx, ray_beg, ray_end, box_l, box_u):
    infile = tests_dir / f"{idx:02d}.in"
    with open(infile, "w") as f:
        f.write(f"{ray_beg[0]} {ray_beg[1]} {ray_beg[2]}\n")
        f.write(f"{ray_end[0]} {ray_end[1]} {ray_end[2]}\n")
        f.write(f"{box_l[0]} {box_l[1]} {box_l[2]}\n")
        f.write(f"{box_u[0]} {box_u[1]} {box_u[2]}\n")

manual_tests = [
    ((0,0,0),(1,1,1),(0.5,0.5,0.5),(2,2,2)),  #  1. Ray hits center of the box
    ((1,1,1),(2,2,2),(0,0,0),(3,3,3)),        #  2. Ray starts inside the box and exits at upper corner
    ((0,0,0),(1,0,0),(2,2,2),(3,3,3)),        #  3. Ray misses the box completely
    ((0,1,1),(2,1,1),(1,0,0),(3,2,2)),        #  4. Ray passes through a face of the box
    ((0,0,0),(3,3,3),(1,1,1),(2,2,2)),        #  5. Ray passes through a vertex of the box
    ((0,1,1),(5,1,1),(1,0,0),(3,2,2)),        #  6. Ray parallel to X axis, hits the box
    ((0,3,3),(5,3,3),(1,0,0),(3,2,2)),        #  7. Ray parallel to X axis, misses the box
    ((1,1,0),(1,1,2),(0,0,0),(2,2,2)),        #  8. Ray starts on a face and goes inside
    ((0,0,0),(2,2,0),(0,0,0),(2,2,2)),        #  9. Ray starts on an edge and goes inside
    ((0,0,0),(1,1,1),(0,0,0),(2,2,2)),        # 10. Ray starts on a vertex and goes inside
]

for idx, t in enumerate(manual_tests, start=1):
    write_test(idx, *t)

print("10 tests successfully created in 'tests/' directory")