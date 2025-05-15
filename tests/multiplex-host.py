import socket
import threading
import time

# Define servers to hit
SERVER_CONFIGS = [
    ("0.0.0.0", 8088),
    ("0.0.0.0", 4241),
    ("0.0.0.0", 1924),
    ("127.0.0.1", 8888)
]

stop_event = threading.Event()


def client_task(index, target_ip, target_port):
    try:
        end_time = time.time() + 10  # Run for 10 seconds

        while time.time() < end_time and not stop_event.is_set():
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                client_socket.connect((target_ip, target_port))

                message = f"Hello from client {index} hitting {target_ip}:{target_port}"
                client_socket.sendall(message.encode())

                response = client_socket.recv(1024).decode()
                if not response:
                    print(f"[Client {index}] Empty response from {target_ip}:{target_port}")
                    stop_event.set()
                    break

                print(f"[Client {index}] Server {target_ip}:{target_port} replied: {response}")

            except Exception as e:
                print(f"[Client {index}] Error hitting {target_ip}:{target_port} => {e}")
                stop_event.set()
                break

            finally:
                client_socket.close()

    except Exception as e:
        print(f"[Client {index}] Fatal error: {e}")
        stop_event.set()


def run_stress_test():
    threads = []

    start_time = time.time()

    for i, (target_ip, target_port) in enumerate(SERVER_CONFIGS):
        t = threading.Thread(target=client_task, args=(i, target_ip, target_port))
        t.start()
        threads.append(t)

    for t in threads:
        t.join()

    duration = time.time() - start_time

    if stop_event.is_set():
        print(f"\n❌ Stress test failed! At least one server did not respond correctly.")
    else:
        print(f"\n✅ Stress test passed! All servers responded correctly for 10 seconds.")

    print(f"Total duration: {duration:.2f} seconds.")


if __name__ == "__main__":
    run_stress_test()