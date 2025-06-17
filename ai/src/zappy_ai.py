##
## EPITECH PROJECT, 2025
## header
## File description:
## zappy ai
##

import time
import random
from game_types import Direction, Resources
from network import NetworkClient
from protocol import ProtocolParser

class ZappyAI:
    def __init__(self, host: str, port: int, team_name: str):
        self.network = NetworkClient(host, port, team_name)
        self.parser = ProtocolParser()
        self.level = 1
        self.inventory = Resources()
        self.is_alive = True
        self.world_size = (0, 0)
        self.last_look_result: list[list[str]] = []

    def connect_to_server(self) -> bool:
        if not self.network.connect():
            print("AI: Failed to connect to network.")
            return False
        print("AI: Network connection established.")
        success, world_size = self.network.authenticate()
        if success:
            self.world_size = world_size
            print(f"AI: Authenticated. World size: {self.world_size}")
        else:
            print("AI: Authentication failed.")
        return success

    def update_inventory(self):
        print("AI: Requesting inventory...")
        if self.network.send_command("Inventory"):
            response = self.network.receive_message()
            if response == "dead":
                print("AI: Received 'dead' after Inventory request. AI is dead.")
                self.is_alive = False
            elif response:
                self.inventory = self.parser.parse_inventory(response)
                print(f"AI: Inventory updated: {self.inventory}")
            else:
                print("AI: No response or empty response for Inventory.")


        else:
            print("AI: Failed to send Inventory command.")
            self.is_alive = False

    def look_around(self):
        print("AI: Looking around...")
        if self.network.send_command("Look"):
            response = self.network.receive_message()
            if response == "dead":
                print("AI: Received 'dead' after Look request. AI is dead.")
                self.is_alive = False
            elif response:
                self.last_look_result = self.parser.parse_look_result(response)
                print(f"AI: Look result: {self.last_look_result}")
            else:
                print("AI: No response or empty response for Look.")
        else:
            print("AI: Failed to send Look command.")
            self.is_alive = False

    def move_forward(self):
        print("AI: Moving forward...")
        if self.network.send_command("Forward"):
            response = self.network.receive_message()
            if response == "ok":
                print("AI: Move forward successful.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Move forward failed or unexpected response: {response}")
            return False
        print("AI: Failed to send Forward command.")
        self.is_alive = False
        return False

    def turn_left(self):
        print("AI: Turning left...")
        if self.network.send_command("Left"):
            response = self.network.receive_message()
            if response == "ok":
                print("AI: Turn left successful.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Turn left failed or unexpected response: {response}")
            return False
        print("AI: Failed to send Left command.")
        self.is_alive = False
        return False

    def turn_right(self):
        print("AI: Turning right...")
        if self.network.send_command("Right"):
            response = self.network.receive_message()
            if response == "ok":
                print("AI: Turn right successful.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Turn right failed or unexpected response: {response}")
            return False
        print("AI: Failed to send Right command.")
        self.is_alive = False
        return False

    def take_object(self, obj: str):
        print(f"AI: Attempting to take {obj}...")
        if self.network.send_command(f"Take {obj}"):
            response = self.network.receive_message()
            if response == "ok":
                print(f"AI: Successfully took {obj}.")

                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Failed to take {obj} or unexpected response: {response}")
            return False
        print(f"AI: Failed to send Take {obj} command.")
        self.is_alive = False
        return False

    def find_food_in_vision(self) -> int:
        for i, tile_objects in enumerate(self.last_look_result):
            if 'food' in tile_objects:
                print(f"AI: Food found in vision at tile index {i}.")
                return i
        print("AI: No food found in current vision.")
        return -1

    def explore_randomly(self):
        print("AI: Exploring randomly...")

        action = random.choice(['forward', 'forward', 'forward', 'left', 'right'])
        if action == 'forward':
            self.move_forward()
        elif action == 'left':
            self.turn_left()
        else:
            self.turn_right()

    def survival_strategy(self):
        if not self.is_alive:
            return False

        self.update_inventory()
        if not self.is_alive:
            return False

        self.look_around()
        if not self.is_alive:
            return False

        print(f"AI Status - Food: {self.inventory.food}, Level: {self.level}, Alive: {self.is_alive}")
        if 'food' in self.last_look_result[0] if self.last_look_result else False:
            print("AI: Food on current tile. Attempting to take.")
            self.take_object("food")
            return True
        if self.inventory.food <= 5:
            print("AI: Food is low. Searching for food.")
            food_tile_idx = self.find_food_in_vision()

            if food_tile_idx == 0:
                self.take_object("food")
            elif food_tile_idx > 0:
                print(f"AI: Food detected at tile index {food_tile_idx}. Navigating...")
                if food_tile_idx == 2:
                    self.move_forward()
                elif food_tile_idx == 1:
                    self.turn_left()


                elif food_tile_idx == 3:
                    self.turn_right()
                else:
                    self.move_forward()
            else:
                print("AI: No food in vision, exploring randomly for food.")
                self.explore_randomly()
        else:
            print("AI: Food is sufficient. Exploring map.")
            self.explore_randomly()

        return True

    def set_object(self, obj: str):
        if self.network.send_command(f"Set {obj}"):
            response = self.network.receive_message()
            if response == "ok":
                print(f"AI: Successfully sent Set command for object '{obj}'.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Set command for object '{obj}' failed or unexpected response: {response}")
            return False


    def broadcast_text(self, text: str):
        if self.network.send_command(f"Broadcast {text}"):
            return self.network.receive_message() == "ok"
        print(f"AI: Failed to send Broadcast command with text '{text}'.")

    def fork(self):
        if self.network.send_command("Fork"):
            response = self.network.receive_message()
            if response == "ok":
                print("AI: Fork successful.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Fork failed or unexpected response: {response}")
            return False
        print("AI: Failed to send Fork command.")
        self.is_alive = False
        return False

    def eject(self):
        if self.network.send_command("Eject"):
            response = self.network.receive_message()
            if response == "ok":
                print("AI: Eject successful.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Eject failed or unexpected response: {response}")
            return False
        print("AI: Failed to send Eject command.")
        self.is_alive = False
        return False

    def incantation(self):
        if self.network.send_command("Incantation"):
            response = self.network.receive_message()
            if response == "ok":
                print("AI: Incantation successful.")
                return True
            elif response == "dead":
                self.is_alive = False
            print(f"AI: Incantation failed or unexpected response: {response}")
            return False
        print("AI: Failed to send Incantation command.")
        self.is_alive = False
        return False

    def run(self):
        if not self.connect_to_server():
            print("AI: Could not connect to server. Exiting.")
            return

        print(f"AI: Successfully started for team {self.network.team_name} on {self.network.host}:{self.network.port}")

        try:
            while self.is_alive:
                if not self.survival_strategy():
                    print("AI: Survival strategy indicated AI should stop or is dead.")
                    break
                time.sleep(0.1)
        except KeyboardInterrupt:
            print("\nAI: Keyboard interrupt detected. Stopping AI.")
        except Exception as e:
            print(f"AI: An error occurred during run: {e}")
        finally:
            print("AI: Disconnecting...")
            self.network.disconnect()
            print("AI: Disconnected.")
