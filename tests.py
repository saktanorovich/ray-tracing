
import pathlib
import random

# Create the tests directory if it does not exist
tests_dir = pathlib.Path("tests")
tests_dir.mkdir(exist_ok=True)

def write_test(idx, ray_beg, ray_end, box_l, box_u):
    """Write .in file for a single test case."""
    infile = tests_dir / f"{idx:02d}.in"
    with open(infile, "w") as f:
        f.write(f"{ray_beg[0]} {ray_beg[1]} {ray_beg[2]}\n")
        f.write(f"{ray_end[0]} {ray_end[1]} {ray_end[2]}\n")
        f.write(f"{box_l[0]} {box_l[1]} {box_l[2]}\n")
        f.write(f"{box_u[0]} {box_u[1]} {box_u[2]}\n")

# Manual corner cases (first 10 tests)
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

# Write manual tests
for idx, t in enumerate(manual_tests, start=1):
    write_test(idx, *t)

# Generate random tests with guaranteed intersections
for idx in range(11, 51):
    # Random box
    box_l = [round(random.uniform(-5, 0), 3) for _ in range(3)]
    box_u = [round(random.uniform(1, 5), 3) for _ in range(3)]

    # Box center
    center = [(box_l[i] + box_u[i]) / 2 for i in range(3)]

    # Decide ray type: outside -> inside OR inside -> outside OR on face -> inside
    ray_type = random.choice(["outside", "inside", "face"])

    if ray_type == "outside":
        # Pick a random face and entry point on that face
        face = random.randint(0, 5)  # 6 faces
        entry = center[:]
        axis = face // 2
        entry[axis] = box_l[axis] if face % 2 == 0 else box_u[axis]
        other_axes = [i for i in range(3) if i != axis]
        for i in other_axes:
            entry[i] = round(random.uniform(box_l[i], box_u[i]), 3)
        # Ray starts far outside aiming at entry point
        ray_beg = [entry[i] + (random.uniform(-10, -6) if face % 2 == 0 else random.uniform(6, 10)) for i in range(3)]
        ray_end = entry

    elif ray_type == "inside":
        # Ray starts inside the box and exits outward
        ray_beg = [round(random.uniform(box_l[i], box_u[i]), 3) for i in range(3)]
        # Aim outside: pick a random direction away from center
        ray_end = [ray_beg[i] + random.uniform(5, 10) for i in range(3)]

    else:  # ray_type == "face"
        # Ray starts on a random face and goes inside
        face = random.randint(0, 5)
        ray_beg = center[:]
        axis = face // 2
        ray_beg[axis] = box_l[axis] if face % 2 == 0 else box_u[axis]
        other_axes = [i for i in range(3) if i != axis]
        for i in other_axes:
            ray_beg[i] = round(random.uniform(box_l[i], box_u[i]), 3)
        # Ray end inside the box
        ray_end = center

    write_test(idx, ray_beg, ray_end, box_l, box_u)

print("✅ 50 tests successfully created in 'tests/' directory")
