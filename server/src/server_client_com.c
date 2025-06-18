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
#include "command_handlers.h"
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
    const command_entry_t *entry = NULL;

    if (!server || !client || !command)
        return ERROR;
    for (int i = 0; command_table[i].handler != NULL; i++) {
        entry = &command_table[i];
        if (entry->client_type != client->client.type)
            continue;
        if (strcmp(entry->command, "*") == 0 ||
            strcmp(entry->command, command) == 0) {
            return entry->handler(server, client, command);
        }
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
            usleep((7 * 1000000) / server->map_data->frec);
            server_process_command(server, client, command);
            free(command);
        }
    }
    return SUCCESS;
}
