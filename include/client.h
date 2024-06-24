#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../lib/data.h"
#include "../wiringPi/wiringPi/wiringPiI2C.h"


#define MAX_PLAYER 100
#define MAX_BOARD_SIZE 50
#define MAX_BOAT_SIZE 10

typedef struct socket socket_t; 

typedef struct{
    int id;
    char nom[100];
    char infos[1000];
} req_t;


// Structure du plateau de jeu
typedef enum {
    EMPTY,
    MISSED_SHOT,
    HIT_SHOT,
    BOAT,
} Status;

typedef enum {
    EN_JEU,
    DEAD,
    FIN_WIN,
    FIN_LOSE
} etat_Partie;

typedef struct {
    Status status;
} cell_t;

// Structure des bateaux
typedef struct {
    int x;
    int y;
    Status status;
} cell_bateau_t;

typedef struct {
    cell_bateau_t cell[MAX_BOAT_SIZE];
} bateau_t;

typedef struct {
    cell_t cell[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
} board_t;

// Structure du client
typedef struct{
    int id;
    char pseudo[100];
    socket_t socket;
} client_t;

// Structure du jeu
typedef struct 
{
    client_t players[100];
    int nbJoueur;
    int maxJoueur;

    board_t plateau;
    int taillePlateau;

    bateau_t bateau;

    int tailleBateau;
    int timeout;

    int etat;
} game_t;


int envoyer_bateau(socket_t *client_socket);
void attente_partie(socket_t *client_socket);
void jouer_partie(socket_t *client_socket);
void jouer_coup(socket_t *client_socket);
void recevoir_plateau(socket_t *client_socket);
void afficher_plateau(int cursor_x, int cursor_y);
void afficher_plateau_placer_bateau(int cursor_x, int cursor_y, char *direction);
void decode_plateau(char *infos);
void init_plateau();
void init_ncurses();
void choix_bateau(socket_t *client_socket);
int menu_placer_bateau(int cursor_x, int cursor_y, socket_t *client_socket);
req_t format_req(int id, char *nom, char *infos);
void serial_req(generic quoi, generic buff);
void unserial_req(generic buff, generic quoi);
void recevoir_etat(socket_t * client_socket);

void affichage7segments(int fd,int sec, int min);