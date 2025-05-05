import socket

def test_server_response():
    server_host = "localhost"
    server_port = 8088
    
    try:
        print("Connecting to server...")
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_host, server_port))
        print("Connected! Sending message...")
        
        test_message = "Hello server"
        client_socket.send(test_message.encode())
        print(f"Sent: '{test_message}'. Waiting for response...")
        
        response = client_socket.recv(1024).decode()
        print(f"Server replied: {response}")
        
    except Exception as e:
        print(f"Error: {e}")
    finally:
        client_socket.close()

if __name__ == "__main__":
    test_server_response()