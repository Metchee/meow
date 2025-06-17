/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** map_parameters
*/

#ifndef MAP_PARAMETERS_H
    #define MAP_PARAMETERS_H
    #include <stdbool.h>
    #include "server.h"
    #include "player.h"
    #include "ressources.h"
    #define TIME_UNIT_REP 20

typedef struct map_parameters_s {
    int height;
    int width;
    int port;
    char **team_names;
    int team_size;
    int frec;
    int team_count;
} map_parameters_t;

typedef struct egg_s {
    int x, y;
    int team_id;
} egg_t;

typedef struct tile_s {
    ressources_t resources[7];
    player_t **players;
    egg_t **eggs;
} tile_t;

typedef struct world_s {
    int width, height;
    tile_t **tiles;
} world_t;

world_t *create_map(map_parameters_t *map_data);
map_parameters_t *parse_arguments(int argc, char **argv);
bool args_error_handling(map_parameters_t *map_data);
void free_map_parameters(map_parameters_t *map_data);

#endif /* !MAP_PARAMETERS_H_ */
