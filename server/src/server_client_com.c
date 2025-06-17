/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** server/client communication
*/

#include <string.h>
#include <stdlib.h>
#include "server.h"
#include "message_protocol.h"
#include "server_network.h"
#include <stdio.h>

int server_read_client_data(server_t *server, client_connection_t *client)
{
    char temp_buffer[BUFFER_SIZE];
    ssize_t bytes_read = 0;

    if (!server || !client)
        return ERROR;
    bytes_read = recv(client->client.fd,
        temp_buffer, sizeof(temp_buffer) - 1, 0);
    if (bytes_read <= 0)
        return handle_client_disconnect(client, bytes_read);
    temp_buffer[bytes_read] = '\0';
    return process_received_data(client, temp_buffer, bytes_read);
}

int server_process_command(server_t *server, client_connection_t *client,
    const char *command)
{
    if (!server || !client || !command)
        return ERROR;
    if (client->client.type == UNKNOWN_CLIENT && strcmp(command, "GUI") == 0) {
        return handle_graphic_command(client, server->map_data,
            server->world, server);
    }
    if (client->client.type == UNKNOWN_CLIENT) {
        return handle_team_command(client, command, server->map_data, server);
    }
    if (client->client.type == AI_CLIENT ||
        client->client.type == GUI_CLIENT) {
        return handle_game_command(server, client, command);
    }
    server_send_to_client(client, "ko\n");
    return SUCCESS;
}

int server_process_client_commands(server_t *server,
    client_connection_t *client)
{
    char *command = NULL;

    if (!server || !client)
        return ERROR;
    while (client->command_queue) {
        command = client_get_next_command(client);
        if (command) {
            server_process_command(server, client, command);
            free(command);
        }
    }
    return SUCCESS;
}
