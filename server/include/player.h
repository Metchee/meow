/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** player header
*/

#ifndef PLAYER_H
    #define PLAYER_H

    #include <stdbool.h>
    #include "ressources.h"
    #include "server.h"

typedef struct player_s {
    int id;
    int x, y;
    int orientation;
    int level;
    ressources_t inventory[7];
    char *team_name;
} player_t;

#endif /* !PLAYER_H */
