#include "../include/serveur.h"

int main() {
    menu_accueil();
    endwin();
}

void menu_accueil(){

    init_ncurses();

    int choix = 0;

    while (1) {
        clear();

        attron(A_BOLD);
        attron(COLOR_PAIR(3));
        printw("Bienvenue dans le jeu de la bataille navale\n");
        printw("Que souhaitez vous faire ?\n\n");
        attroff(COLOR_PAIR(3));
        attroff(A_BOLD);

        // Affichage menu d'accueil
        switch (choix)
        {
            case 0:
                attron(COLOR_PAIR(2));
                printw("Parametrer une partie\n");
                attroff(COLOR_PAIR(2));

                printw("Quitter le programme\n");
            break;

            case 1:
                printw("Parametrer une partie\n");

                attron(COLOR_PAIR(1));
                printw("Quitter le programme\n");
                attroff(COLOR_PAIR(1));
            break;
            
            default:
                choix = 0;
                attron(COLOR_PAIR(2));
                printw("Parametrer une partie\n");
                attroff(COLOR_PAIR(2));

                printw("Quitter le programme\n");
            break;
        }

        refresh();

        // Gestion des touches
        int c = getch();
        switch (c) {

            // Fleche du haut 
            case KEY_UP:
                if (choix == 1)
                    choix --;
                break;

            // Fleche du bas
            case KEY_DOWN:

                if (choix == 0)
                    choix ++;
                break;

            // Touche entrée
            case 10:

                switch (choix) {
                    // Choix de création de partie
                    case 0:
                        menu_parametrage_game();
                        break;

                    // Choix de quitter    
                    case 1:
                        endwin();
                        exit(0);    
                        break;

                    // Choix inconnu
                    default:
                        // On remet le choix à 0
                        choix = 0;
                        break;
                    }

                break;
        }
    }

    endwin();
}

void menu_parametrage_game(){

    int parametre = 0;
    game_t game = init_game();

    while (1) {
        clear();

        // Paramétrage de la partie
        attron(COLOR_PAIR(3));
        attron(A_BOLD);
        printw("Parametrage de la partie                                           \n\n");
        attroff(COLOR_PAIR(3));
        attroff(A_BOLD);

        printw("Nombre de joueur :              <");
        if (parametre == 0)
            attron(A_REVERSE);
        printw("  %d  ", game.maxJoueur);
        attroff(A_REVERSE);
        printw(">\n");


        printw("Taille du plateau :             <");
        if (parametre == 1)
            attron(A_REVERSE);
        printw("  %d  ", game.taillePlateau);
        attroff(A_REVERSE);
        printw(">\n");

        printw("Taille des bateaux :            <");
        if (parametre == 2)
            attron(A_REVERSE);
        printw("  %d  ", game.tailleBateau);
        attroff(A_REVERSE);
        printw(">\n");

        printw("Temps pour jouer en seconde :   <");
        if (parametre == 3)
            attron(A_REVERSE);
        printw("  %d  ", game.timeout);
        attroff(A_REVERSE);
        printw(">\n");

        printw("Vue des bateaux sur le serveur  <");
        if (parametre == 4)
            attron(A_REVERSE);
        if(game.affchageBateau)
            printw("  Oui  ");
        else
            printw("  Non  ");
        attroff(A_REVERSE);
        printw(">\n");

        printw("\n---------------------------------------------------------------\n\n");

        attron(COLOR_PAIR(4));
        printw("Utilisez les touches bas et haut pour naviguer                    \n");
        printw("Utilisez les touches gauche et droite pour incrémenter la valeur  \n");
        printw("Appuyez sur la touche 'Entrée' pour valider vos parametres        \n");
        printw("Appuyez sur la touche 'q' pour revenir au menu principal          \n");
        attroff(COLOR_PAIR(4));

        printw("\n----------------------------------------------------------------\n\n");

        refresh();

        // Gestion des touches
        int c = getch();
        switch (c) {

            // Fleche du haut 
            case KEY_UP:

                if (parametre > 0)
                    parametre--;
                break;

            // Fleche du bas
            case KEY_DOWN:

                if (parametre < 4)
                    parametre ++;
                break;

            // Fleche du gauche 
            case KEY_LEFT:                
                switch (parametre)
                {
                case 0:
                    if (game.maxJoueur > 2)
                        game.maxJoueur--;
                    break;

                case 1:
                    if (game.taillePlateau > 4)
                        game.taillePlateau--;
                        if (game.taillePlateau < 2*game.tailleBateau){
                            game.tailleBateau = game.taillePlateau/2;
                        }
                    break;
                
                case 2:
                    if (game.tailleBateau > 2)
                        game.tailleBateau--;
                    break;

                case 3:
                    if (game.timeout > 5)
                        game.timeout--;
                    break;
                case 4:
                    if (game.affchageBateau)
                        game.affchageBateau = false;
                    else game.affchageBateau = true;
                    break;
                }

                break;
            // Fleche du droit
            case KEY_RIGHT:
                switch (parametre)
                {
                case 0:
                    if (game.maxJoueur < MAX_PLAYER)
                        game.maxJoueur ++;
                    break;

                case 1:
                    if (game.taillePlateau < MAX_BOARD_SIZE)
                        game.taillePlateau ++;
                        if (game.taillePlateau < 2 * game.tailleBateau){
                            game.tailleBateau = game.taillePlateau/2;
                        }
                    break;
                
                case 2:
                    if (game.tailleBateau < MAX_BOAT_SIZE)
                        game.tailleBateau ++;
                        if (game.taillePlateau < 2 * game.tailleBateau){
                            game.taillePlateau = game.tailleBateau*2;
                        }
                    break;

                case 3:
                        game.timeout ++;
                    break;

                case 4:
                    if (game.affchageBateau)
                        game.affchageBateau = false;
                    else game.affchageBateau = true;
                        break;
                }
                break;    

            // Touche entrée
            case 10:
                // Lancement de l'attente des personnes
                attente_joueur(game);
                break;
            case 'q':
                return;
        }
    }
}

void *attente_joueur_thread(void *arg){
    game_t *game = (game_t *)arg;

    int flag = 0;
    timeout(1000);

    while (flag == 0){
            affichage_attente_joueur(*game);
            refresh();
            int c = getch();
            switch (c)
            {
                // Touche q
                case 'q':
                    break;
                
                // Touche entrée
                case 10:
                    // Lancement de la partie
                    // Fermeture de la socket d'ecoute
                    if (game->nbJoueur < 2){
                        printw("Nombre de joueur insuffisant\n");
                        refresh();
                    } else {
                        game->etat = 1;
                        flag = 1;
                    }
                    break;
            }
    }

    timeout(-1);
    pthread_exit(NULL);
}

void affichage_attente_joueur(game_t game){
    clear();

    // Affichage des infos de la partie
    attron(COLOR_PAIR(3));
    attron(A_BOLD);
    printw("Attente des joueurs\n\n");
    attroff(COLOR_PAIR(3));
    attroff(A_BOLD);

    printw("Nombre de joueur : %d\n", game.maxJoueur);
    printw("Taille du plateau : %d\n", game.taillePlateau);
    printw("Taille des bateaux : %d\n", game.tailleBateau);
    printw("Temps pour jouer en seconde : %d\n", game.timeout);
    printw("Bateaux visible sur serveur : %d\n",game.affchageBateau);

    printw("\n\n");

    // Affichage des joueurs
    printw("Liste des joueurs connectés: \n");
    for (int i = 0; i <= game.nbJoueur; i++)
    {
        printw("%s\n", game.players[i].pseudo);
    }
    printw(" %d / %d \n\n", game.nbJoueur, game.maxJoueur);

    printw("Enter pour lancer la partie\n");
    printw("Appuyez sur la touche 'q' pour revenir au menu principal\n");

    refresh();
}

void attente_joueur(game_t game){
    // Création de la socket d'ecoute : 
    socket_t server_socket = creerSocketEcoute("127.0.0.1", 5000);

    // Mémoire partagée
    int shmid;
    key_t key = 1234;
    game_t *shared_game;

    if ((shmid = shmget(key, sizeof(game_t), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    if ((shared_game = shmat(shmid, NULL, 0)) == (game_t *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    *shared_game = game;
    shared_game->nbJoueur = 0;
    shared_game->etat = 0;

    // Threads D'affichage 
    pthread_t thread_affichage;
    pthread_create(&thread_affichage, NULL, attente_joueur_thread, shared_game);

    // Gestion des connexions
    while(1){
        // On vérifie si le nombre de joueur est atteint
        if (shared_game->nbJoueur == shared_game->maxJoueur){
        // On quitte la boucle
            pthread_join(thread_affichage, NULL);
            break;
        }

            // On attend une connexion
            socket_t client_socket = accepterClt(server_socket);            
            
                // La partie est déja lancée
            if (shared_game->etat == 1){
                /*
                req_t requete = format_req(-1, "nok", "La Partie est déja commencée\n");
                envoyer(&client_socket, &requete, serial_req);
                close(client_socket.fd);*/
                break;
            }

            // On connecte le client.
            else {

                // On envoie une req de pseudo  au client
                // TODO mettre en place la structure de message
                req_t requete = format_req(0, "ok", " ");
                envoyer(&client_socket, &requete, serial_req);

                // On récupère le pseudo du joueur
                req_t reponse;  
                recevoir(&client_socket, &reponse, unserial_req);

                if (reponse.id == 1){
                        printw("Pseudo du joueur : %s\n", reponse.infos);
                        // On récupère le pseudo du joueur
                        strcpy(shared_game->players[shared_game->nbJoueur].pseudo, reponse.infos);
                        shared_game->players[shared_game->nbJoueur].socket = client_socket;
                        shared_game->players[shared_game->nbJoueur].id = shared_game->nbJoueur;
                        shared_game->nbJoueur++;

                        envoyer(&client_socket, &requete, serial_req);
                } else {
                        // On envoie un message d'erreur
                        req_t requete = format_req(-1, "nok", "Erreur de connexion\n");
                        envoyer(&client_socket, &requete, serial_req);
                        close(client_socket.fd);
                }
                  
            }

        }

        close(server_socket.fd);

        lancement_partie(shared_game);
        refresh();
}

void lancement_partie(game_t *game){

    clear();
    
    attron(COLOR_PAIR(3));
    attron(A_BOLD);
    printw("Lancement de la partie\n\n");
    attroff(COLOR_PAIR(3));
    attroff(A_BOLD);

    // on envoie les infos de partie
    printw("Envoie des infos de partie en cours...\n");
    refresh();
    for (int i = 0; i <= game->nbJoueur-1; i++){
        envoyer_infos_partie(game, &game->players[i].socket);
    }

    // Reception des bateaux des joueur
    move(5,0);
    clrtoeol();
    printw("Réception des bateaux en cours ...\n");
    refresh();
    recevoir_bateaux(game, game->players);

    // On lance la partie
    move(7,0);
    printw("Lancement de la partie\n\n");
    printw("Appuyez sur une touche pour continuer\n");
    refresh();
    timeout(-1);
    getch();

    clear();
    
    attron(COLOR_PAIR(3));
    attron(A_BOLD);
    printw("Partie en cours\n\n");
    attroff(COLOR_PAIR(3));
    attroff(A_BOLD);
    afficher_plateau(*game);

    // boucle de jeu
    while(game->etat != 2){

        // Envoie du plateau
        envoyer_plateau(game);  

        // Envoie du plateau
        clear();
        for (int i = 0; i <= game->nbJoueur-1; i++){
            req_t req = format_req(5,"go","go");
            envoyer(&game->players[i].socket, &req ,serial_req);
            recevoir(&game->players[i].socket, &req, unserial_req);
        }

        // Reception des tirs
        for (int i = 0; i < game->nbJoueur; i++){
            recevoir_tir(game, &game->players[i]);
        }

        clear();
        attron(COLOR_PAIR(3));
        attron(A_BOLD);
        printw("Partie en cours\n\n");
        attroff(COLOR_PAIR(3));
        attroff(A_BOLD);
        afficher_plateau(*game);

        // On verifie si la partie est terminée
        verifier_fin_partie(game);
    }   
}

void afficher_plateau(game_t game) {
    //clear(); // Effacer l'écran

    for (int i = 0; i < game.taillePlateau; i++) {
        for (int j = 0; j < game.taillePlateau; j++) {
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
                    if (game.affchageBateau)
                        printw("[B]");
                    else
                        printw("[ ]");
                    break;
            }
            printw(" ");
        }
        printw("\n");
    }
    refresh();
}

void verifier_fin_partie(game_t *game){
    
    int nbBateau = game->plateau.nbBateau;
    int nbBateauCoule = 0;
    int flag = 0;

    // Pour chaque batteau
    for (int i = 0; i < nbBateau; i++){
        flag = 0;

        // Pour chaque cellule du bateau
        for (int j = 0; j < game->tailleBateau; j++){
            // On verifie si elles sont toutes hit
            if (game->plateau.bateaux[i].cell[j].status != HIT_SHOT){
                flag = 1;
            }
        }
        if (flag == 0){
            nbBateauCoule++;
        }
    }

    if (nbBateauCoule == nbBateau){
            game->etat = 2;

            for (int i = 0; i < game->nbJoueur; i++){

                req_t requete = format_req(9, "fin", " ");
                envoyer(&game->players[i].socket, &requete, serial_req);

                // TODO ecran de fin

                req_t reponse;
                recevoir(&game->players[i].socket, &reponse, unserial_req);
            }
    } else if (nbBateauCoule == nbBateau - 1) {

            int flag = -1;

            for (int i = 0; i < game->nbJoueur; i++){
                for (int j = 0; j < game->plateau.bateaux[i].taille; j++){
                    if (game->plateau.bateaux[i].cell[j].status != HIT_SHOT){
                        flag = i;
                    }
                }
            }

            for (int i = 0; i < game->nbJoueur; i++){
                if (flag == i){

                    req_t requete = format_req(10, "fin", " ");
                    envoyer(&game->players[i].socket, &requete, serial_req);

                    req_t reponse;
                    recevoir(&game->players[i].socket, &reponse, unserial_req);

                } else {

                    req_t requete = format_req(9, "fin", " ");
                    envoyer(&game->players[i].socket, &requete, serial_req);

                    req_t reponse;
                    recevoir(&game->players[i].socket, &reponse, unserial_req);

                }
            }

    } else {

        for (int i = 0; i < game->nbJoueur; i++){
            
            int flag = 0;

            // On vérifie tout les bateaux des joueurs
            for (int j = 0; j < game->plateau.bateaux[i].taille; j++){
                if (game->plateau.bateaux[i].cell[j].status != HIT_SHOT){
                    flag = 1;
                }
            }
            
            // Si encore en vie 
            if (flag == 1 ){
                req_t requete = format_req(7, "continue", " ");
                envoyer(&game->players[i].socket, &requete, serial_req);

                req_t reponse;
                recevoir(&game->players[i].socket, &reponse, unserial_req); 
            } else {
                    
                req_t requete = format_req(8, "stop", " ");
                envoyer(&game->players[i].socket, &requete, serial_req);

                req_t reponse;
                recevoir(&game->players[i].socket, &reponse, unserial_req);
            }
        }
    }
}

/** Reception */
void recevoir_tir(game_t *game, client_t *client){

    req_t reponse;
    recevoir(&client->socket, &reponse, unserial_req);

    if (reponse.id == 6){

        req_t requete = format_req(0, "ok", " ");
        envoyer(&client->socket, &requete, serial_req);

        // On récupère les coordonnées du tir
        int x, y;
        if (strcmp(reponse.nom,"coup") == 0){

            sscanf(reponse.infos, "%d;%d", &x, &y);

            // On vérifie si le tir est valide
            if (x >= 0 && x < game->taillePlateau && y >= 0 && y < game->taillePlateau){
                // On vérifie si le tir est un touché
                if (game->plateau.cell[y][x].status == BOAT){
                    printw("Tir touché\n");
                    game->plateau.cell[y][x].status = HIT_SHOT;
                    tirer_bateau(y,x,&game->plateau, game->plateau.nbBateau);

                } else if (game->plateau.cell[y][x].status == HIT_SHOT){
                    printw("Tir deja touché\n");
                } else {
                    printw("Tir pas touché\n");
                    game->plateau.cell[y][x].status = MISSED_SHOT;
                }
            }
        }
    } else {
        req_t requete = format_req(0, "ok", " ");
        envoyer(&client->socket, &requete, serial_req);
    }

    refresh();
}

void tirer_bateau(int x, int y, board_t *board, int nbBateau){
    for (int i = 0; i < nbBateau; i++){
        for (int j = 0; j < board->bateaux[i].taille; j++){
            if (board->bateaux[i].cell[j].x == y && board->bateaux[i].cell[j].y == x){
                board->bateaux[i].cell[j].status = HIT_SHOT;
            }
        }
    }
}

void recevoir_bateaux(game_t *game, client_t *client){

    for (int i = 0; i <= game->nbJoueur-1; i++){
        req_t req = format_req(3, "bateaux", " ");
        envoyer(&game->players[i].socket, &req ,serial_req);
    }

    int nbBateauxRecu = 0,j=0;
    bool flag=NULL;

    do
    {
        req_t reponse;

        flag = recevoir_non_block(&game->players[j].socket, &reponse, unserial_req);

        if(flag != NULL)
        {
            req_t requete = format_req(0, "ok", " ");
            envoyer(&game->players[j].socket, &requete, serial_req);

            if (reponse.id == 3){
                // On récupère les bateaux

                int x, y;
                char direction;

                // Utilisation de sscanf pour extraire les paramètres
                // x;y;direction
                sscanf(reponse.infos, "%d;%d;%c", &x, &y, &direction);

                // On place le bateau
                placer_bateau(&game->plateau,  game->players[j].id, game->tailleBateau, x, y, direction);
                flag = NULL;
                nbBateauxRecu++;
            }
            if(j == game->nbJoueur+1)
                j = 0;
            else
                j++;
        }
    }while(nbBateauxRecu != game->nbJoueur);
}

void placer_bateau(board_t *board, int id, int taille, int x, int y, char direction){
    if (direction == 'h'){

        for (int i = 0; i < taille; i++){
            board->bateaux[board->nbBateau].taille = taille;
            board->bateaux[board->nbBateau].cell[i].x = x;
            board->bateaux[board->nbBateau].cell[i].y = y - i;
            board->bateaux[board->nbBateau].cell[i].status = BOAT;
            board->cell[y - i][x].status = BOAT;
        }
            board->nbBateau++;

    } else if (direction == 'b'){

        for (int i = 0; i < taille; i++){
            board->bateaux[board->nbBateau].taille = taille;
            board->bateaux[board->nbBateau].cell[i].x = x;
            board->bateaux[board->nbBateau].cell[i].y = y + i;
            board->bateaux[board->nbBateau].cell[i].status = BOAT;
            board->cell[y + i][x].status = BOAT;
        }
            board->nbBateau++;

    } else if (direction == 'g'){

        for (int i = 0; i < taille; i++){
            board->bateaux[board->nbBateau].taille = taille;
            board->bateaux[board->nbBateau].cell[i].x = x - i;
            board->bateaux[board->nbBateau].cell[i].y = y;
            board->bateaux[board->nbBateau].cell[i].status = BOAT;
            board->cell[y][x - i].status = BOAT;
        }
            board->nbBateau++;

    } else if (direction == 'd'){

        for (int i = 0; i < taille; i++){
            board->bateaux[board->nbBateau].taille = taille;
            board->bateaux[board->nbBateau].cell[i].x = x + i;
            board->bateaux[board->nbBateau].cell[i].y = y;
            board->bateaux[board->nbBateau].cell[i].status = BOAT;
            board->cell[y][x + i].status = BOAT;
            
        }
            board->nbBateau++;
    }
}

/** Fonctionnalité d'envoi */
void envoyer_infos_partie(game_t * game, socket_t* socket_client){

    char infos[100];
    sprintf(infos, "%d;%d;%d;%d", game->nbJoueur, game->taillePlateau, game->tailleBateau, game->timeout);

    req_t req = format_req(4, "infos", infos);
    envoyer(socket_client, &req ,serial_req);

    recevoir(socket_client, &req, unserial_req);
}

void envoyer_plateau(game_t *game){

    int nbCases = game->taillePlateau * game->taillePlateau;
    char plateau[nbCases+1];
    strcpy(plateau, "");

    for(int i = 0; i < game->taillePlateau; i++){
        for(int j = 0; j< game->taillePlateau ; j++){

            switch (game->plateau.cell[j][i].status)
            {
                case MISSED_SHOT:
                    strcat(plateau,"1");
                    break;
                case HIT_SHOT:
                    strcat(plateau,"2");
                    break;
                case EMPTY:
                    strcat(plateau,"0");
                    break;
                case BOAT:
                    strcat(plateau,"0");
                    break;
            }
        }
    }

    for (int i = 0; i <= game->nbJoueur-1; i++){

        req_t req = format_req(2,"plateau",plateau);
        envoyer(&game->players[i].socket, &req ,serial_req);

        req_t reponse;
        recevoir(&game->players[i].socket, &req, unserial_req);
        
    }


}


/**  Fonctionnalités d'init */

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
    init_pair(3, COLOR_WHITE, COLOR_MAGENTA); // Texte rouge sur fond noir
    
    init_pair(4, COLOR_WHITE, COLOR_CYAN); // Texte rouge sur fond noir
}

game_t init_game(){
    game_t game;

    game.maxJoueur = 2;
    game.taillePlateau = 11;
    game.tailleBateau = 5;
    game.timeout = 15;
    
    game.nbJoueur = 0;
    game.etat = 0;
    game.affchageBateau = true;

    return game;
}


//** Fonctionnalités de requete **//

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
    sscanf(str_req, "%d;%99[^;];%999s", &req->id, req->nom, req->infos);
}
