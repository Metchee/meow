/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** command_handlers
*/

#ifndef HANDLERS_H
    #define HANDLERS_H

typedef int (*command_handler_t)(server_t *server,
    client_connection_t *client, const char *command);
typedef struct {
    const char *command;
    client_type_t client_type;
    command_handler_t handler;
} command_entry_t;

int handle_graphic_cmd(server_t *server,
    client_connection_t *client, const char *command);
int handle_team_cmd(server_t *server,
    client_connection_t *client, const char *command);
int handle_game_cmd(server_t *server,
    client_connection_t *client, const char *command);

static const command_entry_t command_table[] = {
    {"GRAPHIC", UNKNOWN_CLIENT, handle_graphic_cmd},
    {"*", UNKNOWN_CLIENT, handle_team_cmd},
    {"*", AI_CLIENT, handle_game_cmd},
    {"*", GUI_CLIENT, handle_game_cmd},
    {NULL, UNKNOWN_CLIENT, NULL}
};

#endif
