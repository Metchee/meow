##
## EPITECH PROJECT, 2025
## header
## File description:
## ai network zappy
##

import socket
import sys
from typing import Optional, Tuple

class NetworkClient:
    def __init__(self, host: str, port: int, team_name: str):
        self.host = host
        self.port = port
        self.team_name = team_name
        self.socket: Optional[socket.socket] = None
        self.is_connected = False
        self.pending_commands = 0
        self.max_pending = 10
        self._read_buffer = b""

    def connect(self) -> bool:
        if self.is_connected:
            print(f"Network: Already connected to {self.host}:{self.port}")
            return True
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(10)
            print(f"Network: Attempting to connect to {self.host}:{self.port}...")
            self.socket.connect((self.host, self.port))
            self.is_connected = True
            self.pending_commands = 0
            self._read_buffer = b""
            print(f"Network: Successfully connected to {self.host}:{self.port}")
            return True
        except socket.timeout:
            print(f"Network: Connection to {self.host}:{self.port} timed out.")
            self.socket = None
            return False
        except Exception as e:
            print(f"Network: Connection failed to {self.host}:{self.port} - {e}")
            self.socket = None
            return False

    def disconnect(self):
        if self.socket:
            try:
                self.socket.shutdown(socket.SHUT_RDWR)
            except OSError:
                pass
            self.socket.close()
            self.socket = None
        self.is_connected = False
        print("Network: Disconnected.")

    def send_command(self, command: str) -> bool:
        if not self.is_connected or not self.socket:
            print(f"Network: Cannot send command '{command}'. Not connected.")
            return False
        if self.pending_commands >= self.max_pending:
            print(f"Network: Cannot send command '{command}'. Max pending commands ({self.max_pending}) reached.")
            return False

        try:
            message = f"{command}\n"
            self.socket.sendall(message.encode('utf-8'))
            self.pending_commands += 1
            print(f"Network: Sent -> {command.strip()}")
            return True
        except Exception as e:
            print(f"Network: Failed to send command '{command}': {e}")
            self.is_connected = False
            self.disconnect()
            return False

    def receive_message(self) -> str:
        try:
            data = self.socket.recv(1024).decode().strip()
            if data:
                self.pending_commands = max(0, self.pending_commands - 1)
                print(f"Received: {data}")
            return data
        except Exception as e:
            print(f"Failed to receive message: {e}")
            self.is_connected = False
            return ""

    def receive_line(self) -> str:
        buffer = b""
        while True:
            chunk = self.socket.recv(1)
            if not chunk:
                break
            buffer += chunk
            if chunk == b"\n":
                break
        return buffer.decode('utf-8', errors='ignore').strip()

        newline_idx = self._read_buffer.find(b"\n")

        while newline_idx == -1:
            try:
                data = self.socket.recv(2048)
                if not data:
                    print("Network: Connection closed by server while receiving.")
                    self.is_connected = False
                    self.disconnect()
                    if self._read_buffer:
                        remaining_message = self._read_buffer.decode('utf-8', errors='ignore').strip()
                        self._read_buffer = b""
                        print(f"Network: Received (partial from buffer before dead) <- {remaining_message}")
                        return remaining_message
                    return "dead"
                self._read_buffer += data
                newline_idx = self._read_buffer.find(b"\n")
            except socket.timeout:
                print("Network: Timeout while receiving message.")
                if self._read_buffer:
                    pass
                return ""
            except Exception as e:
                print(f"Network: Failed to receive message (in loop): {e}")
                self.is_connected = False
                self.disconnect()
                if self._read_buffer:
                    remaining_message = self._read_buffer.decode('utf-8', errors='ignore').strip()
                    self._read_buffer = b""
                    print(f"Network: Received (partial from buffer before exception) <- {remaining_message}")
                    return remaining_message
                return ""


        message_line_bytes = self._read_buffer[:newline_idx]
        self._read_buffer = self._read_buffer[newline_idx+1:]

        message_str = message_line_bytes.decode('utf-8', errors='ignore').strip()

        if message_str and message_str not in ["dead"] and \
           not message_str.startswith("message") and \
           not message_str.startswith("eject:"):
            self.pending_commands = max(0, self.pending_commands - 1)

        print(f"Network: Received <- {message_str}")
        return message_str

    def authenticate(self) -> Tuple[bool, Tuple[int, int]]:
        if not self.is_connected:
            print("Network: Cannot authenticate. Not connected.")
            return False, (0, 0)
        try:
            print("Network: Starting authentication...")
            welcome = self.receive_line()
            if welcome != "WELCOME":
                print(f"Network: Authentication failed. Expected 'WELCOME', got '{welcome}'")
                return False, (0, 0)
            if not self.send_command(self.team_name):
                print(f"Network: Authentication failed. Could not send team name '{self.team_name}'.")
                return False, (0, 0)
            client_num = self.receive_line()
            if client_num == "ko":
                print("Network: Authentication failed. Server returned 'ko'.")
                return False, (0, 0)
            dimensions = self.receive_line()
            if dimensions:
                dim_parts = dimensions.strip().split()
                if len(dim_parts) >= 2:
                    world_size = (int(dim_parts[0]), int(dim_parts[1]))
                    print(f"World size: {world_size}")
                    return True, world_size
            return False, (0, 0)
        except Exception as e:
            print(f"Network: Authentication process failed with exception: {e}")
            return False, (0, 0)
