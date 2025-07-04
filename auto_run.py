import subprocess
import signal
import time
import os

BINARY = "./webserv config/templat.toml"  # Change this to your executable if needed

def build():
    print("\033[94m[BUILDING...]\033[0m")
    result = subprocess.run(["make"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    print(result.stdout)
    if result.returncode != 0:
        print("\033[91m[MAKE FAILED]\033[0m")
        print(result.stderr)
        return False
    return True

def run():
    if not os.path.exists(BINARY):
        print(f"\033[91m[BINARY NOT FOUND: {BINARY}]\033[0m")
        return None
    print("\033[92m[RUNNING... Ctrl+C to restart]\033[0m")
    return subprocess.Popen(BINARY)

def main():
    while True:
        if not build():
            time.sleep(1)
            continue

        proc = run()
        if proc is None:
            time.sleep(1)
            continue

        try:
            proc.wait()
        except KeyboardInterrupt:
            print("\033[93m[RESTARTING...]\033[0m")
            try:
                proc.terminate()
                proc.wait(timeout=2)
            except Exception:
                proc.kill()

        time.sleep(1)  # avoid too rapid loops

if __name__ == "__main__":
    main()
