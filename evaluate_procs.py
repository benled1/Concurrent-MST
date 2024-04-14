import subprocess
import time
import os

# List of Bash scripts
bash_scripts = {"Serial":"sbatch serial.sh", 
                "Thread":"sbatch thread.sh", 
                "Mpi":"sbatch mpi.sh"}

def delete_file_if_exists(filename):
    try:
        os.remove(filename)
    except FileNotFoundError:
        pass
    except Exception as e:
        print(f"Error deleting file {filename}: {str(e)}")

def run_slurm(script):
    subprocess.run(script, shell=True)

def wait_for_file(filename):
    while True:
        try:
            with open(filename, 'r') as file:
                line = file.readline().strip()
                if line.lower().startswith("elapsed time:"):
                    time_str = line.lower().replace('elapsed time:', '').replace('ms', '').strip()
                    milliseconds = float(time_str)
                    return milliseconds / 1000.0
        except FileNotFoundError:
            # Reading too  early
            pass
        except Exception as e:
            # idk weird stuff was happening
            print(f"Error reading from {filename}: {str(e)}")
        time.sleep(1)

filenames = ['test_serial.txt', 'test_thread.txt', 'test_mpi.txt']

for filename in filenames:
    # Get rid of the old files
    delete_file_if_exists(filename)

for script_name, script_path in bash_scripts.items():
    # queue up jobs
    run_slurm(script_path)
    print(f"{script_name} is queued")

for filename in filenames:
    time_value = wait_for_file(filename)
    # Print the elapsed time from files
    print(f"Elapsed time in {filename}: {time_value:.3f} s") 