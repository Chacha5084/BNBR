/**
 *	@file		server.c
 *	@brief		Code source du serveur
 *	@author		Deroubaix & Houplain
 *	@date		08/04/2024
 *	@version	1.5
 */

#include "../include/client.h"


/**********************************************************/
// Variable globale 
/**********************************************************/
char pseudo[50];
game_t game;

req_t format_req(int id, char *nom, char *infos);
void serial_req(generic quoi, generic buff);
void unserial_req(generic buff, generic quoi);

#define     LED    29
#define     BUZZER 1
#define     I2C_ADDRESS 0x70

int hexValues[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
/**********************************************************/
// MAIN du programme
/**********************************************************/

int main(int argc, char *argv[]){
    buffer_t buffer;
    char adresse[16];
    int port;

    if (argc == 3) {
        strncpy(adresse, argv[1], 16 - 1);
        adresse[16 - 1] = '\0';
        port = atoi(argv[2]);
    } else {
        strncpy(adresse, "127.0.0.1", 16 - 1);
        adresse[16 - 1] = '\0';
        port = 8080;
    }


    printf("Bonjour, en attente de connexion au serveur\n\n");

    socket_t client_socket = connecterClt2Srv(adresse, port );

    /* Identification */
        req_t reponse;
        recevoir(&client_socket, &reponse, unserial_req);

        if (reponse.id == 0){

            /* Récupération du pseudo */
            strcpy(pseudo,"");

            while (strlen(pseudo) == 0){
                printf("Saisisez votre pseudo : ");
                scanf("%s", pseudo);

                if (strlen(pseudo) == 0){
                    printf("Votre pseudo est vide");
                }
            }

            // Envoie de la requete d'identification     
            req_t requete = format_req(1, "identification", pseudo);
            envoyer(&client_socket, &requete, serial_req);
            
                            
            reponse.id = -2;
            strcpy(reponse.nom, "");
            strcpy(reponse.infos, "");

            recevoir(&client_socket, &reponse, unserial_req);

            if (reponse.id == 0){

                printf("Connexion établie \n\n");
                // Attente d'une partie 
                attente_partie(&client_socket);

            } else {

                printf("Connexion imposible \n\n");
                close(client_socket.fd);
                return 0;
            }
        }

        close(client_socket.fd);
        endwin();
        return 0;
}

int envoyer_bateau(socket_t *client_socket){

    init_plateau();

    req_t reponse;
    // Envoie des bateaux   
    recevoir(client_socket, &reponse, unserial_req);

    if (reponse.id == 3){
        choix_bateau(client_socket);
        return 0;
    }
    else {
        printf("Erreur de réception des informations de parties...\n");
        return -1;
    }
}

void attente_partie(socket_t *client_socket){

        req_t reception;

        recevoir(client_socket, &reception, unserial_req);

        if(reception.id == 4){
            
            //acqittement
            req_t requete = format_req(0, "ok", "recep OK\n");
            envoyer(client_socket, &requete, serial_req);

            sscanf(reception.infos,"%d;%d;%d;%d",&game.nbJoueur,&game.taillePlateau,&game.tailleBateau,&game.timeout);

            printf("Informations de partie : \n");
            printf("Nombre de Joueurs : %d\n", game.nbJoueur);
            printf("Timeout : %d\n", game.timeout);
            printf("Taille du plateau : %d\n", game.taillePlateau);
            printf("Taille des bateaux : %d\n", game.tailleBateau);

            // Envoie de son bateau au serveur
            int retour = envoyer_bateau(client_socket);
            if (retour == 0){
                // Lancement de la partie.
                jouer_partie(client_socket);
            } else {
                printf("Erreur de réception des informations de partie...\n");
            }
        } else {
            printf("Erreur de réception des informations de partie...\n");
        }
}

void jouer_partie(socket_t *client_socket){
    game.etat = EN_JEU;

    while (1){
        printw("Reception plateau \n");
        recevoir_plateau(client_socket);

        if (game.etat == EN_JEU){
            jouer_coup(client_socket);
        } else {
            clear();
            printw("Partie en cours\n");
            
            afficher_plateau(MAX_BOARD_SIZE+1,MAX_BOARD_SIZE+1);

            printw("\n\nNavire coulé... Vous ferez mieux la prochaine fois !\n");
            refresh();

            /********************** */

            req_t reception;
            recevoir(client_socket,&reception,serial_req);

            req_t requete = format_req(0, "recept", " "); 
            envoyer(client_socket,&requete,serial_req);

            requete = format_req(6, "mort", " ");
            envoyer(client_socket, &requete, serial_req);

            recevoir(client_socket, &reception, unserial_req);
        }

        recevoir_etat(client_socket);

        if (game.etat == FIN_WIN || game.etat == FIN_LOSE){
            break;
        }
    }

    clear();

    if (game.etat == FIN_WIN) {
        printw("Félicitation Moussaillon, vous serez nommé capitaine !");
    } else {
        printw("Nous ne sommes pas sur le titanic, faite mieux la prochaine fois !");
    }
    timeout(-1);
    getch();
}

void recevoir_etat(socket_t * client_socket){
    
    req_t reception;
    recevoir(client_socket, &reception, unserial_req);
    if (reception.id == 10){
        //acqittement
        req_t requete = format_req(0, "ok", "recep OK\n");
        envoyer(client_socket, &requete, serial_req);

        printw("Bataille terminé, rentrons au bercail !\n");
        game.etat = 2;

    } else if (reception.id == 9){
        //acqittement
        req_t requete = format_req(0, "ok", "recep OK\n");
        envoyer(client_socket, &requete, serial_req);

        printw("Bataille terminé, rentrons au bercail !\n");
        game.etat = 3;

    } else if (reception.id == 7){

        req_t requete = format_req(0, "ok", "recep OK\n");
        envoyer(client_socket, &requete, serial_req);

        printw("Vous etes toujours vivant, continuer la bataille ! \n");

    } else if (reception.id == 8){
        
        req_t requete = format_req(0, "ok", "recep OK\n");
        envoyer(client_socket, &requete, serial_req);

        game.etat = 1;
    } else {
        printf("Erreur de réception des informations de partie...\n");
    }
    refresh();
}

void jouer_coup(socket_t *client_socket){

    req_t reception, requete;
    recevoir(client_socket, &reception, unserial_req);

    if (reception.id == 5){

        //acqittement
        requete = format_req(0, "ok", "recep OK\n");
        envoyer(client_socket, &requete, serial_req);
#ifdef RPI
        // TODO affichage 7 segment pour le temps restant
        int fd = wiringPiI2CSetup(I2C_ADDRESS); // Initialisation de l'afficheur 7 segments
        // Configuration de l'afficheur 7 segments
        wiringPiI2CWriteReg8(fd, 0x21, 0x01);
        wiringPiI2CWriteReg8(fd, 0x81, 0x00);
        wiringPiI2CWriteReg8(fd, 0xef, 0x00);
#endif

        int cursor_x = 0;
        int cursor_y = 0;

        // CHOIX DU CLIENT **************************************************************************************
        int ch;
        time_t start_time = time(NULL);
        time_t current_time = 0;

        timeout(1000);
        do {

            ch = getch(); // Lire une touche du clavier

            clear(); // Effacer l'écran

            // Si le temps est écoulé, sortir de la boucle
            current_time = time(NULL);          
            int time_elapsed = (int)difftime(current_time, start_time);
            int time_remaining = game.timeout - time_elapsed;

            printw("Jeu en cours\n");
            printw("Temps restant : %d\n", time_remaining);

            #ifdef RPI
                affichage7segments(fd,time_remaining,0);
            #endif

            if (time_remaining <= 0) {
                // Temps écoulé, sortir de la boucle
                break;
            }

            // Déplacer le curseur en fonction de la touche pressée, mais uniquement si la case est disponible
            switch (ch) {
                // Case fleche 
                    case KEY_UP:
                            if (cursor_y > 0){
                                cursor_y--;
                            }
                        break;
                    case KEY_DOWN:
                            if (cursor_y < game.taillePlateau - 1){
                                cursor_y++;  
                            }
                        break;
                    case KEY_LEFT:
                            if (cursor_x > 0) {
                                cursor_x--;
                            }
                        break;
                    case KEY_RIGHT:
                            if (cursor_x < game.taillePlateau - 1) {
                                cursor_x++;
                            }
                        break;
            }

                afficher_plateau(cursor_x, cursor_y);

                printw("\n\nDeplacer le curseur avec les flèches directionnelles\n");
                printw("Attendez la fin du timer\n");

                refresh();

        } while (1);

        // Envoie du coup
        char infos[10];
        sprintf(infos, "%d;%d", cursor_x, cursor_y);
        req_t requete = format_req(6, "coup", infos);
        envoyer(client_socket, &requete, serial_req);

        req_t reponse;
        recevoir(client_socket, &reponse, unserial_req);
        
    } else {
        printf("Erreur de réception des informations de parties...\n");
    }
}

void afficher_plateau(int cursor_x, int cursor_y) {
    //clear(); // Effacer l'écran

    for (int i = 0; i < game.taillePlateau; i++) {
        for (int j = 0; j < game.taillePlateau; j++) {
            if (cursor_x == j && cursor_y == i) {
                attron(A_REVERSE); // Inverser les couleurs pour le curseur
            }
            switch (game.plateau.cell[i][j].status) {
                case MISSED_SHOT:
                    printw("[o]");
                    break;
                case HIT_SHOT:
                    printw("[x]");
                    break;
                case EMPTY:
                    printw("[ ]");
                    break;
                case BOAT:
                    printw("[B]");
                    break;
            }
            if (cursor_x == j && cursor_y == i) {
                attroff(A_REVERSE);
            }
            printw(" ");
        }
        printw("\n");
    }
    refresh();
}

int recevoir_plateau(socket_t *client_socket){
        // Reception du plateau
        req_t reception;
        recevoir(client_socket, &reception, unserial_req);
        refresh();

        if(reception.id == 2){  
            //acqittement
            req_t requete = format_req(0, "ok", "recep OK\n");
            envoyer(client_socket, &requete, serial_req);    
            
            // Affichage du plateau
            decode_plateau(reception.infos);

        } else {
            printf("Erreur de réception des informations de parties...\n");
        }
}

void afficher_plateau_placer_bateau(int cursor_x, int cursor_y, char *direction){

    board_t plateau;
        // Initialisation du plateau
    for (int i = 0; i < game.taillePlateau; i++){
        for (int j = 0; j < game.taillePlateau; j++){
           plateau.cell[i][j].status = EMPTY;
        }
    }

    switch (*direction)
    {
        case 'h':

            if (cursor_y >= game.tailleBateau-1){
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y - i][cursor_x].status = BOAT;
                }
            } else {
                *direction = 'b';
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y + i][cursor_x].status = BOAT;
                }
            }
            break;

        case 'b':

            if (cursor_y <= game.taillePlateau - game.tailleBateau){
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y + i][cursor_x].status = BOAT;
                }
            } else {
                *direction = 'h';
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y - i][cursor_x].status = BOAT;
                }
            }
            break;

        case 'g':

            if (cursor_x >= game.tailleBateau-1){
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y][cursor_x - i].status = BOAT;
                }
            } else {
                *direction = 'd';
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y][cursor_x + i].status = BOAT;
                }
            }
            break;

        case 'd':

            if (cursor_x <= game.taillePlateau - game.tailleBateau){
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y][cursor_x + i].status = BOAT;
                }
            } else {
                *direction = 'g';
                for (int i = 0; i < game.tailleBateau; i++){
                    plateau.cell[cursor_y][cursor_x - i].status = BOAT;
                }
            }
            break;
    }

    clear(); // Effacer l'écran

    printw("Placer votre bateau\n\n");

    for (int i = 0; i < game.taillePlateau; i++){
        for (int j = 0; j < game.taillePlateau; j++){
            switch (plateau.cell[i][j].status)
            {
                case EMPTY:
                        printw("[ ] ");
                    break;
                case BOAT:
                        attron(A_REVERSE);
                        printw("[B] ");
                        attroff(A_REVERSE);
                    break;
            }
        }
        printw("\n");
    }

    printw("\n\nChanger la direction avec les flèches directionnelles\n");
    printw("Valider avec la touche entrée\n");
    printw("Annuler avec la touche q\n");
    refresh();
}

void decode_plateau(char *infos){
    
    board_t plateau;
    // Décodage du plateau
    for (int i = 0; i < game.taillePlateau; i++){
        for (int j = 0; j < game.taillePlateau; j++){
            int index = i * game.taillePlateau + j;

            if (infos[index] == 48 ){
                plateau.cell[j][i].status = EMPTY;
            } else if (infos[index] == 49){
                plateau.cell[j][i].status = MISSED_SHOT;
            } else if (infos[index] == 50){
                plateau.cell[j][i].status = HIT_SHOT;
            } else if (infos[index] == 51){
                plateau.cell[j][i].status = BOAT;
            } else {
                plateau.cell[j][i].status = EMPTY;
            }

        }
    }

    game.plateau = plateau;
    for (int i = 0; i < game.tailleBateau; i++){
        if (game.plateau.cell[game.bateau.cell[i].y][game.bateau.cell[i].x].status == EMPTY){
            game.plateau.cell[game.bateau.cell[i].y][game.bateau.cell[i].x].status = BOAT;
        }
    }
}

void init_plateau(){
    // Initialisation du plateau
    for (int i = 0; i < game.taillePlateau; i++){
        for (int j = 0; j < game.taillePlateau; j++){
            game.plateau.cell[i][j].status = EMPTY;
        }
    }
}

void init_ncurses(){
    initscr();
    keypad(stdscr, TRUE); // Activer la prise en charge des touches spéciales
    start_color(); // Activer les couleurs
    curs_set(0); // Masquer le curseur
    noecho(); // Désactiver l'affichage des caractères saisis
    cbreak(); // Désactiver la mise en mémoire tampon de ligne


    // Définition des couleurs
    init_pair(1, COLOR_BLACK, COLOR_RED); // Texte rouge sur fond noir
    init_pair(2, COLOR_WHITE, COLOR_GREEN); // Texte rouge sur fond noir
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Texte rouge sur fond noir
    init_pair(4, COLOR_CYAN, COLOR_BLACK); // Texte rouge sur fond noir
}

void choix_bateau(socket_t *client_socket){

    int cursor_x = 0;
    int cursor_y = 0;

    // DISPLAY NCURSES  **************************************************************************************
    init_ncurses(); // Initialiser ncurses
    clear(); 
    printw("Placer votre bateau\n\n");

    afficher_plateau(cursor_x, cursor_y);

    printw("\n\nDeplacer le curseur avec les flèches directionnelles\n");
    printw("Placer le bateau avec la touche entrée\n");

    refresh();

    // CHOIX DU CLIENT **************************************************************************************
    int ch;
    do {
        ch = getch(); // Lire une touche du clavier
        // Déplacer le curseur en fonction de la touche pressée, mais uniquement si la case est disponible
        switch (ch) {
            // Case fleche 
                case KEY_UP:
                        if (cursor_y > 0){
                            cursor_y--;
                            //display_matrix(cursor_x, cursor_y, matrix);
                        }
                    break;
                case KEY_DOWN:
                        if (cursor_y < game.taillePlateau - 1){
                            cursor_y++;
                            //display_matrix(cursor_x, cursor_y, matrix);   
                        }
                    break;
                case KEY_LEFT:
                        if (cursor_x > 0) {
                            cursor_x--;
                            //display_matrix(cursor_x, cursor_y, matrix);
                        }
                    break;
                case KEY_RIGHT:
                        if (cursor_x < game.taillePlateau - 1) {
                            cursor_x++;
                            //display_matrix(cursor_x, cursor_y, matrix);
                        }
                    break;

            // Case touche entrée
            case 10:
                // On passe en mode placement de bateau
                if (menu_placer_bateau(cursor_x, cursor_y, client_socket) == 0){
                    clear();    // Effacer l'écran
                    return;
                } else {
                    clear();
                }
                break;
        }
        clear(); 
        printw("Placer votre bateau\n\n");

        afficher_plateau(cursor_x, cursor_y);

        printw("\n\nDeplacer le curseur avec les flèches directionnelles\n");
        printw("Placer le bateau avec la touche entrée\n");

        refresh();

    } while (1); // Sortir du programme si la touche 'q' est pressée
    clear();    // Effacer l'écran
}

int menu_placer_bateau(int cursor_x, int cursor_y, socket_t *client_socket){

    char direction = 'd';
    // Todo afficage du menu de placement de bateau
    afficher_plateau_placer_bateau(cursor_x, cursor_y, &direction);
    req_t requete, reponse;
    char infos[10];
    
    int ch;
    do {
        ch = getch(); // Lire une touche du clavier
        // Déplacer le curseur en fonction de la touche pressée, mais uniquement si la case est disponible
        switch (ch) {
            // Case fleche 
                case KEY_UP:
                    direction = 'h';
                    break;
                case KEY_DOWN:
                    direction = 'b';
                    break;
                case KEY_LEFT:
                    direction = 'g';
                    break;
                case KEY_RIGHT:
                    direction = 'd';
                    break;

            // Case touche entrée
            case 10:
                switch (direction)
                {

                case 'h':
                        for (int i = 0; i < game.tailleBateau; i++){
                            game.bateau.cell[i].x = cursor_x;
                            game.bateau.cell[i].y = cursor_y - i;
                        }
                    break;    

                case 'b':
                        for (int i = 0; i < game.tailleBateau; i++){
                            game.bateau.cell[i].x = cursor_x;
                            game.bateau.cell[i].y = cursor_y + i;
                        }
                    break;

                case 'g':
                        for (int i = 0; i < game.tailleBateau; i++){
                            game.bateau.cell[i].x = cursor_x -i;
                            game.bateau.cell[i].y = cursor_y;
                        }
                    break;

                case 'd':
                        for (int i = 0; i < game.tailleBateau; i++){
                            game.bateau.cell[i].x = cursor_x + i;
                            game.bateau.cell[i].y = cursor_y;
                        }
                    break;
                
                default:
                    break;
                }

                sprintf(infos, "%d;%d;%c", cursor_x, cursor_y, direction);
                requete = format_req(3, "boat", infos);
                envoyer(client_socket, &requete, serial_req);

                recevoir(client_socket, &reponse, unserial_req);
                
                if (reponse.id == 0){
                    printf("Bateau placé avec succès\n");
                    return 0;
                } else {
                    printf("Erreur de réception des informations de parties...\n");
                }
                break;

            // Case touche q    
            case 'q':                    
                break;
        }

        afficher_plateau_placer_bateau(cursor_x, cursor_y, &direction);

    } while (ch != 'q'); // Sortir du programme si la touche 'q' est pressée
    return -1;
}

/**********************************************************/
// Fonction de requete
/**********************************************************/

req_t format_req(int id, char *nom, char *infos){
    req_t req;
    req.id = id;
    strncpy(req.nom, nom, sizeof(req.nom) - 1);
    strncpy(req.infos, infos, sizeof(req.infos) - 1);
    return req;
}

void serial_req(generic quoi, generic buff){
    req_t *req = (req_t *)quoi;
    sprintf(buff, "%d;%s;%s", req->id, req->nom, req->infos);
}

void unserial_req(generic buff, generic quoi){
    // Cast du paramètre quoi en pointeur vers une structure req_t
    req_t *req = (req_t *)quoi;

    // String de la requête
    char *str_req = (char *)buff;

    // Désérialisation de la requête
    sscanf(str_req, "%d;%99[^;];%99s", &req->id, req->nom, req->infos);
}

#ifdef RPI
    void affichage7segments(int fd,int sec, int min){

        int unite_min= min % 10 ;
        int unite_sec= sec % 10;
        int dizaine_min= min / 10;
        int dizaine_sec=  sec / 10;

        wiringPiI2CWriteReg8(fd, 0x00, hexValues[dizaine_min]); // On affiche le chiffre 1 (tout à gauche)
        wiringPiI2CWriteReg8(fd, 0x02, hexValues[unite_min]); // On affiche le chiffre 2
        wiringPiI2CWriteReg8(fd, 0x04, 0x02);//On affiche ":"
        wiringPiI2CWriteReg8(fd, 0x06, hexValues[dizaine_sec]); // On affiche le chiffre 3
        wiringPiI2CWriteReg8(fd, 0x08, hexValues[unite_sec]); // On affiche le chiffre 4 (tout à droite)
    }
#endif