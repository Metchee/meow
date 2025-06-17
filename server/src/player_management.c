/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** player management
*/

#include "../include/server.h"
#include "../include/map_parameters.h"
#include "../include/server_network.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

bool init_player(server_t *server, client_t *client, const char *command)
{
    client->type = AI_CLIENT;
    client->player = malloc(sizeof(player_t));
    if (!client->player)
        return false;
    client->player->id = server->next_player_id++;
    srand(time(NULL) + client->player->id);
    client->player->x = rand() % server->map_data->width;
    client->player->y = rand() % server->map_data->height;
    client->player->orientation = (rand() % 4) + 1;
    client->player->level = 1;
    client->player->inventory[FOOD] = 10;
    client->player->inventory[LINEMATE] = 0;
    client->player->inventory[DERAUMERE] = 0;
    client->player->inventory[SIBUR] = 0;
    client->player->inventory[MENDIANE] = 0;
    client->player->inventory[PHIRAS] = 0;
    client->player->inventory[THYSTAME] = 0;
    client->player->team_name = strdup(command);
    return true;
}

void free_player(client_t *client)
{
    if (!client || !client->player)
        return;
    free(client->player->team_name);
    free(client->player);
    client->player = NULL;
}

void notify_gui_new_player(server_t *server, player_t const *player)
{
    if (!server || !player)
        return;
    server_notify_clients_by_type(server, GUI_CLIENT, 
        "pnw %d %d %d %d %d %s\n",
        player->id,
        player->x,
        player->y,
        player->orientation,
        player->level,
        player->team_name);
}

void notify_gui_player_disconnect(server_t *server, player_t const *player)
{
    if (!server || !player)
        return;
    server_notify_clients_by_type(server, GUI_CLIENT, 
        "pdi %d\n",
        player->id);
}
