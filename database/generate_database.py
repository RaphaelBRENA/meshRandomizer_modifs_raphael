import os
import shutil
import subprocess
from concurrent.futures import ThreadPoolExecutor

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BASE_DIR)
EXE_PATH = os.path.join(PROJECT_ROOT, "meshRandomizer.exe")

INPUTS_DIR = os.path.join(BASE_DIR, "inputs")
OUTPUTS_DIR = os.path.join(BASE_DIR, "outputs")

MESH_CONFIGS = [
    ("femur_s4", 1.0),
    ("cyl50_s4", 0.1),
    ("hex_sphe_s3", 2.0),
    ("cortex_s3", 2.0),
]

NUM_PERTURBATIONS = 250

def setup_directories():
    print("Setting up directory structure...")
    os.makedirs(INPUTS_DIR, exist_ok=True)
    os.makedirs(OUTPUTS_DIR, exist_ok=True)
    for mesh_name, _ in MESH_CONFIGS:
        os.makedirs(os.path.join(OUTPUTS_DIR, mesh_name), exist_ok=True)

def move_inputs():
    print("Checking and moving input meshes...")
    for mesh_name, _ in MESH_CONFIGS:
        filename = f"{mesh_name}.m3d"
        src_path = os.path.join(PROJECT_ROOT, filename)
        dest_path = os.path.join(INPUTS_DIR, filename)
        
        if os.path.exists(dest_path):
            continue
            
        if os.path.exists(src_path):
            print(f"Moving {filename} to database/inputs/")
            shutil.move(src_path, dest_path)
            
            oct_filename = f"{mesh_name}.oct"
            oct_src = os.path.join(PROJECT_ROOT, oct_filename)
            oct_dest = os.path.join(INPUTS_DIR, oct_filename)
            if os.path.exists(oct_src):
                shutil.move(oct_src, oct_dest)
        else:
            print(f"Warning: Base mesh {filename} not found in root or inputs directory!")

def run_perturbation(args):
    mesh_name, idx = args
    d = 0.05 + 0.40 * (idx / (NUM_PERTURBATIONS - 1.0) if NUM_PERTURBATIONS > 1 else 0.0)
    input_file = os.path.join(INPUTS_DIR, f"{mesh_name}.m3d")
    output_base = os.path.join(OUTPUTS_DIR, mesh_name, f"{mesh_name}_p{idx}")
    seed = 1000 + idx
    keep_surface = "1"
    
    cmd = [
        EXE_PATH,
        input_file,
        output_base,
        str(d),
        str(seed),
        keep_surface
    ]
    
    try:
        subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
        return True, mesh_name, idx, None
    except subprocess.CalledProcessError as e:
        error_msg = e.stderr.decode('utf-8', errors='ignore') or e.stdout.decode('utf-8', errors='ignore')
        return False, mesh_name, idx, f"Exit code {e.returncode}: {error_msg}"
    except Exception as e:
        return False, mesh_name, idx, str(e)

def generate_database():
    setup_directories()
    move_inputs()
    
    print("\nPreparing tasks...")
    tasks = []
    for mesh_name, _ in MESH_CONFIGS:
        for idx in range(NUM_PERTURBATIONS):
            tasks.append((mesh_name, idx))
            
    total_tasks = len(tasks)
    print(f"Starting parallel generation of {total_tasks} perturbed meshes using ThreadPoolExecutor...")
    
    success_count = 0
    failure_count = 0
    failures = []
    
    max_workers = os.cpu_count() or 4
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        results = executor.map(run_perturbation, tasks)
        
        for success, mesh_name, idx, err in results:
            if success:
                success_count += 1
            else:
                failure_count += 1
                failures.append((mesh_name, idx, err))
            
            if (success_count + failure_count) % 100 == 0:
                print(f"Progress: {success_count + failure_count}/{total_tasks} completed...")

    print("\n" + "="*50)
    print("GENERATION SUMMARY")
    print("="*50)
    print(f"Total Tasks: {total_tasks}")
    print(f"Successful:  {success_count}")
    print(f"Failed:      {failure_count}")
    
    if failure_count > 0:
        print("\nFailures:")
        for mesh_name, idx, err in failures[:10]:
            print(f"  - {mesh_name} (index {idx}): {err}")
        if len(failures) > 10:
            print(f"  ... and {len(failures) - 10} more failures.")
        return False
    else:
        print("\nAll tasks completed successfully!")
        return True

def verify_files():
    print("\nRunning file verification check...")
    expected_files_per_mesh = NUM_PERTURBATIONS * 2
    total_expected = len(MESH_CONFIGS) * expected_files_per_mesh
    actual_count = 0
    missing_files = []
    
    for mesh_name, _ in MESH_CONFIGS:
        mesh_dir = os.path.join(OUTPUTS_DIR, mesh_name)
        file_list = os.listdir(mesh_dir)
        
        for idx in range(NUM_PERTURBATIONS):
            m3d_file = f"{mesh_name}_p{idx}.m3d"
            vtk_file = f"{mesh_name}_p{idx}.vtk"
            
            if m3d_file not in file_list:
                missing_files.append(os.path.join(mesh_name, m3d_file))
            else:
                if os.path.getsize(os.path.join(mesh_dir, m3d_file)) == 0:
                    missing_files.append(os.path.join(mesh_name, f"{m3d_file} (empty)"))
                else:
                    actual_count += 1
                    
            if vtk_file not in file_list:
                missing_files.append(os.path.join(mesh_name, vtk_file))
            else:
                if os.path.getsize(os.path.join(mesh_dir, vtk_file)) == 0:
                    missing_files.append(os.path.join(mesh_name, f"{vtk_file} (empty)"))
                else:
                    actual_count += 1

    print(f"Expected files: {total_expected}")
    print(f"Found non-empty files: {actual_count}")
    
    if missing_files:
        print(f"Verification FAILED. {len(missing_files)} missing or empty files:")
        for f in missing_files[:10]:
            print(f"  - {f}")
        return False
    else:
        print("Verification PASSED.")
        return True

if __name__ == "__main__":
    success = generate_database()
    if success:
        verify_files()
