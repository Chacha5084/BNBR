#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../lib/data.h"

#define MAX_PLAYER 100
#define MAX_BOARD_SIZE 50
#define MAX_BOAT_SIZE 10

typedef struct socket socket_t; 

typedef struct{
    int id; // type d'info envoyée
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
    int taille;
    int id_client;
} bateau_t;

typedef struct {
    cell_t cell[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    bateau_t bateaux[100];
    int nbBateau;
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

    int tailleBateau;
    int timeout;

    bool affchageBateau;
    int etat;
} game_t;


// Fonctions

void menu_accueil();
void menu_parametrage_game();
void attente_joueur(game_t game);
void lancement_partie(game_t *game);

void init_ncurses();
game_t init_game();


void affichage_attente_joueur(game_t game);

void *attente_joueur_thread(void *arg);

void verifier_fin_partie(game_t *game);
void recevoir_tir(game_t *game, client_t *client);
void envoyer_infos_partie(game_t * game, socket_t* socket_client);
void envoyer_plateau(game_t *game);


void tirer_bateau(int x, int y, board_t *board, int nbBateau);
void placer_bateau(board_t *board, int id, int taille, int x, int y, char direction);

void recevoir_bateaux(game_t *game, client_t client[]);


req_t format_req(int id, char *nom, char *infos);
void serial_req(generic quoi, generic buff);
void unserial_req(generic buff, generic quoi);


void afficher_plateau(game_t game);
