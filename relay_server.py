#!/usr/bin/env python3
"""
Qt Chess Relay Server
A simple relay/signaling server for cross-network chess game connections.

This server allows players to connect using only a room code, eliminating
the need for IP addresses and port forwarding.
"""

import socket
import json
import threading
import random
import time
import sys
from datetime import datetime

class RelayServer:
    def __init__(self, host='0.0.0.0', port=8080):
        self.host = host
        self.port = port
        self.rooms = {}  # room_code -> {host: client_socket, guest: client_socket}
        self.clients = {}  # client_socket -> {client_id, room_code, role}
        self.server_socket = None
        self.running = False
        
    def log(self, message):
        """Log message with timestamp"""
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        print(f"[{timestamp}] {message}")
        
    def generate_room_code(self):
        """Generate a unique 4-digit room code"""
        while True:
            code = str(random.randint(1000, 9999))
            if code not in self.rooms:
                return code
                
    def send_message(self, client_socket, message):
        """Send JSON message to client"""
        try:
            data = json.dumps(message) + '\n'
            client_socket.send(data.encode('utf-8'))
            return True
        except Exception as e:
            self.log(f"Error sending message: {e}")
            return False
            
    def handle_client(self, client_socket, address):
        """Handle individual client connection"""
        client_id = f"client_{address[0]}_{address[1]}"
        self.clients[client_socket] = {
            'client_id': client_id,
            'room_code': None,
            'role': None,
            'address': address
        }
        
        self.log(f"New client connected: {client_id} from {address}")
        
        # Send welcome message
        self.send_message(client_socket, {
            'type': 'welcome',
            'client_id': client_id,
            'server_version': '1.0'
        })
        
        buffer = ""
        try:
            while self.running:
                data = client_socket.recv(4096).decode('utf-8')
                if not data:
                    break
                    
                buffer += data
                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    if line.strip():
                        try:
                            message = json.loads(line)
                            self.process_message(client_socket, message)
                        except json.JSONDecodeError as e:
                            self.log(f"JSON decode error: {e}")
                            
        except Exception as e:
            self.log(f"Client handler error: {e}")
        finally:
            self.cleanup_client(client_socket)
            
    def process_message(self, client_socket, message):
        """Process received message from client"""
        msg_type = message.get('type')
        client_info = self.clients.get(client_socket, {})
        client_id = client_info.get('client_id', 'unknown')
        
        self.log(f"Received {msg_type} from {client_id}")
        
        if msg_type == 'hello':
            # Already sent welcome, just acknowledge
            pass
            
        elif msg_type == 'create_room':
            self.handle_create_room(client_socket)
            
        elif msg_type == 'join_room':
            room_code = message.get('room_code')
            self.handle_join_room(client_socket, room_code)
            
        elif msg_type == 'leave_room':
            self.handle_leave_room(client_socket)
            
        elif msg_type == 'game_message':
            room_code = message.get('room_code')
            game_data = message.get('data', {})
            self.relay_game_message(client_socket, room_code, game_data)
            
        elif msg_type == 'ping':
            self.send_message(client_socket, {'type': 'pong'})
            
    def handle_create_room(self, client_socket):
        """Handle room creation request"""
        room_code = self.generate_room_code()
        client_info = self.clients[client_socket]
        
        self.rooms[room_code] = {
            'host': client_socket,
            'guest': None,
            'created_at': time.time()
        }
        
        client_info['room_code'] = room_code
        client_info['role'] = 'host'
        
        self.log(f"Room {room_code} created by {client_info['client_id']}")
        
        self.send_message(client_socket, {
            'type': 'room_created',
            'room_code': room_code
        })
        
    def handle_join_room(self, client_socket, room_code):
        """Handle join room request"""
        client_info = self.clients[client_socket]
        
        if room_code not in self.rooms:
            self.send_message(client_socket, {
                'type': 'error',
                'message': f'Room {room_code} does not exist'
            })
            return
            
        room = self.rooms[room_code]
        if room['guest'] is not None:
            self.send_message(client_socket, {
                'type': 'error',
                'message': f'Room {room_code} is full'
            })
            return
            
        # Join the room
        room['guest'] = client_socket
        client_info['room_code'] = room_code
        client_info['role'] = 'guest'
        
        self.log(f"Client {client_info['client_id']} joined room {room_code}")
        
        # Notify guest
        self.send_message(client_socket, {
            'type': 'room_joined',
            'room_code': room_code
        })
        
        # Notify host
        host_socket = room['host']
        host_info = self.clients.get(host_socket, {})
        self.send_message(host_socket, {
            'type': 'opponent_joined',
            'opponent_id': client_info['client_id']
        })
        
    def handle_leave_room(self, client_socket):
        """Handle leave room request"""
        client_info = self.clients.get(client_socket, {})
        room_code = client_info.get('room_code')
        
        if not room_code or room_code not in self.rooms:
            return
            
        room = self.rooms[room_code]
        role = client_info['role']
        
        # Notify opponent
        if role == 'host' and room['guest']:
            self.send_message(room['guest'], {'type': 'opponent_left'})
        elif role == 'guest' and room['host']:
            self.send_message(room['host'], {'type': 'opponent_left'})
            
        # Clean up room
        if role == 'host':
            del self.rooms[room_code]
            self.log(f"Room {room_code} closed")
        else:
            room['guest'] = None
            
        client_info['room_code'] = None
        client_info['role'] = None
        
    def relay_game_message(self, sender_socket, room_code, game_data):
        """Relay game message to opponent"""
        if room_code not in self.rooms:
            return
            
        room = self.rooms[room_code]
        sender_info = self.clients.get(sender_socket, {})
        
        # Determine opponent socket
        if sender_info['role'] == 'host':
            opponent_socket = room['guest']
        else:
            opponent_socket = room['host']
            
        if opponent_socket:
            self.send_message(opponent_socket, {
                'type': 'game_message',
                'data': game_data
            })
            
    def cleanup_client(self, client_socket):
        """Clean up after client disconnects"""
        if client_socket in self.clients:
            client_info = self.clients[client_socket]
            self.log(f"Client disconnected: {client_info['client_id']}")
            
            # Leave room if in one
            self.handle_leave_room(client_socket)
            
            # Remove from clients
            del self.clients[client_socket]
            
        try:
            client_socket.close()
        except:
            pass
            
    def start(self):
        """Start the relay server"""
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(5)
        
        self.running = True
        self.log(f"Relay server started on {self.host}:{self.port}")
        
        try:
            while self.running:
                try:
                    client_socket, address = self.server_socket.accept()
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, address),
                        daemon=True
                    )
                    client_thread.start()
                except socket.timeout:
                    continue
                except Exception as e:
                    if self.running:
                        self.log(f"Accept error: {e}")
                        
        except KeyboardInterrupt:
            self.log("Server interrupted by user")
        finally:
            self.stop()
            
    def stop(self):
        """Stop the relay server"""
        self.running = False
        if self.server_socket:
            self.server_socket.close()
        self.log("Server stopped")

def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Qt Chess Relay Server')
    parser.add_argument('--host', default='0.0.0.0', help='Host address (default: 0.0.0.0)')
    parser.add_argument('--port', type=int, default=8080, help='Port number (default: 8080)')
    
    args = parser.parse_args()
    
    server = RelayServer(host=args.host, port=args.port)
    
    try:
        server.start()
    except Exception as e:
        print(f"Error starting server: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
