/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** message handling and processing
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/server.h"
#include "../include/message_protocol.h"
#include "../include/server_network.h"

int handle_client_disconnect(client_connection_t *client,
    ssize_t bytes_read)
{
    if (bytes_read == 0) {
        printf("Client closed connection (fd: %d)\n", client->client.fd);
    } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("recv");
    }
    return ERROR;
}

static void send_tile_info(client_connection_t *client,
    map_parameters_t const *map_data, world_t const *world, int width)
{
    if (!client || !map_data || !world || !world->tiles)
        return;
    if (width < 0 || width >= map_data->width)
        return;
    for (int height = 0; height < map_data->height; height++) {
        server_send_to_client(client, "bct %d %d %d %d %d %d %d %d %d\n",
            width, height,
            world->tiles[height][width].resources[FOOD],
            world->tiles[height][width].resources[LINEMATE],
            world->tiles[height][width].resources[DERAUMERE],
            world->tiles[height][width].resources[SIBUR],
            world->tiles[height][width].resources[MENDIANE],
            world->tiles[height][width].resources[PHIRAS],
            world->tiles[height][width].resources[THYSTAME]);
    }
}

static void send_existing_players_info(client_connection_t *gui_client,
    server_t const *server)
{
    client_connection_t *current = NULL;

    if (!gui_client || !server)
        return;
    current = server->clients;
    while (current) {
        if (current->client.type == AI_CLIENT && current->client.player) {
            server_send_to_client(gui_client, "pnw %d %d %d %d %d %s\n",
                current->client.player->id,
                current->client.player->x,
                current->client.player->y,
                current->client.player->orientation,
                current->client.player->level,
                current->client.player->team_name);
        }
        current = current->next;
    }
}

int handle_graphic_command(client_connection_t *client,
    map_parameters_t const *map_data, world_t const *world, server_t *server)
{
    if (!client || !map_data || !world || !server)
        return ERROR;
    printf("GUI client identified and connected (fd: %d)\n", client->client.fd);
    client->client.type = GUI_CLIENT;
    server_send_to_client(client, "msz %d %d\n", map_data->width,
        map_data->height);
    for (int i = 0; i < map_data->width; i++) {
        send_tile_info(client, map_data, world, i);
    }
    send_existing_players_info(client, server);
    server_notify_clients_by_type(server, GUI_CLIENT, 
        "# GUI client connected (fd: %d)\n", client->client.fd);
    return SUCCESS;
}

static int process_complete_command(client_connection_t *client)
{
    if (client->buffer_pos > 0) {
        printf("Received command from fd %d: '%s'\n",
            client->client.fd, client->read_buffer);
        if (client->pending_commands < MAX_PENDING_COMMANDS) {
            client_add_command(client, client->read_buffer);
        } else {
            printf("Command queue full for client fd: %d\n",
                client->client.fd);
            server_send_to_client(client, "ko\n");
        }
    }
    client->buffer_pos = 0;
    return SUCCESS;
}

int process_received_data(client_connection_t *client,
    char *temp_buffer, ssize_t bytes_read)
{
    for (int i = 0; i < bytes_read; i++) {
        if (client->buffer_pos >= BUFFER_SIZE - 1) {
            printf("Buffer overflow for client fd: %d\n", client->client.fd);
            return ERROR;
        }
        client->read_buffer[client->buffer_pos] = temp_buffer[i];
        if (temp_buffer[i] == '\n') {
            client->read_buffer[client->buffer_pos] = '\0';
            process_complete_command(client);
        } else {
            client->buffer_pos++;
        }
    }
    return SUCCESS;
}

int handle_team_command(client_connection_t *client,
    const char *command, map_parameters_t const *map_data, server_t *server)
{
    printf("AI client with team '%s' (fd: %d)\n", command, client->client.fd);
    if (!init_player(server, &client->client, command))
        return ERROR;
    notify_gui_new_player(server, client->client.player);
    server_send_to_client(client, "%d\n", map_data->team_size);
    server_send_to_client(client, "%d %d\n", map_data->width,
        map_data->height);
    return SUCCESS;
}

int handle_game_command(server_t *server, client_connection_t *client,
    const char *command)
{
    char *cmd = NULL;
    char *newline = NULL;
    player_t *player = NULL;

    if (!server || !client || !command)
        return ERROR;
    
    player = client->client.player;
    if (!player) {
        server_send_to_client(client, "ko\n");
        return ERROR;
    }
    
    cmd = strdup(command);
    if (!cmd)
        return ERROR;
    newline = strchr(cmd, '\n');
    if (newline)
        *newline = '\0';

    if (strcmp(cmd, "Forward") == 0 || strcmp(cmd, "forward") == 0) {
        int new_x = player->x;
        int new_y = player->y;
        
        switch (player->orientation) {
            case 1: new_y = (new_y - 1 + server->map_data->height) % server->map_data->height; break;
            case 2: new_x = (new_x + 1) % server->map_data->width; break;  
            case 3: new_y = (new_y + 1) % server->map_data->height; break;
            case 0: new_x = (new_x - 1 + server->map_data->width) % server->map_data->width; break;
        }
        
        player->x = new_x;
        player->y = new_y;
        
        server_send_to_client(client, "ok\n");
        
        server_notify_clients_by_type(server, GUI_CLIENT, 
            "ppo %d %d %d %d\n", player->id, player->x, player->y, player->orientation);
            
    } else if (strcmp(cmd, "Right") == 0 || strcmp(cmd, "right") == 0) {
        player->orientation = (player->orientation + 1) % 4;
        
        server_send_to_client(client, "ok\n");
        
        server_notify_clients_by_type(server, GUI_CLIENT, 
            "ppo %d %d %d %d\n", player->id, player->x, player->y, player->orientation);
            
    } else if (strcmp(cmd, "Left") == 0 || strcmp(cmd, "left") == 0) {
        player->orientation = (player->orientation + 3) % 4;
        
        server_send_to_client(client, "ok\n");
        
        server_notify_clients_by_type(server, GUI_CLIENT, 
            "ppo %d %d %d %d\n", player->id, player->x, player->y, player->orientation);
            
    } else if (strcmp(cmd, "Look") == 0 || strcmp(cmd, "look") == 0) {
        server_send_to_client(client, "[ ]\n");
        
    } else if (strcmp(cmd, "Inventory") == 0 || strcmp(cmd, "inventory") == 0) {
        server_send_to_client(client, "[ food %d, linemate %d, deraumere %d, sibur %d, mendiane %d, phiras %d, thystame %d ]\n",
            player->inventory[0],
            player->inventory[1],
            player->inventory[2],  
            player->inventory[3],
            player->inventory[4],
            player->inventory[5],
            player->inventory[6]);
            
    } else {
        server_send_to_client(client, "ko\n");
    }
    
    free(cmd);
    return SUCCESS;
}