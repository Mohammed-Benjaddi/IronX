import socket
import threading
import time

SERVER_HOST = "localhost"
SERVER_PORT = 8086
NUM_CLIENTS = 100

def client_task(index):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((SERVER_HOST, SERVER_PORT))
        message = f"Hello from client {index}"
        client_socket.sendall(message.encode())
        response = client_socket.recv(1024).decode()
        print(f"[Client {index}] Server replied: {response}")
    except Exception as e:
        print(f"[Client {index}] Error: {e}")
    finally:
        client_socket.close()

def run_stress_test():
    threads = []

    start_time = time.time()

    for i in range(NUM_CLIENTS):
        t = threading.Thread(target=client_task, args=(i,))
        t.start()
        threads.append(t)
        time.sleep(0.01)  # optional: stagger start slightly

    for t in threads:
        t.join()

    duration = time.time() - start_time
    print(f"\n✅ Stress test completed with {NUM_CLIENTS} clients in {duration:.2f} seconds.")

if __name__ == "__main__":
    run_stress_test()