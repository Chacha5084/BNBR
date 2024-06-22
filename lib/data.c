#include "data.h"

/**
 *	\fn			void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...)
 *	\brief		Envoi d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		quoi : requête/réponse à serialiser avant l'envoi
 *	\param 		serial : pointeur sur la fonction de serialisation d'une requête/réponse
 *	\note		si le paramètre serial vaut NULL alors quoi est une chaîne de caractères
 *	\note		Si le mode est DGRAM, l'appel nécessite en plus l'adresse IP et le port.
 *	\result		paramètre sockEch modifié pour le mode DGRAM
 */
void envoyer(socket_t *sockEch, generic quoi,pFct serial, ...){
    buffer_t buff; //buffer d'envoie

    //serialiser les data dans buff
    if(serial !=NULL ) serial(quoi,buff);
    else strcpy(buff,(char*)quoi);

    if (sockEch->mode == SOCK_DGRAM){
        va_list pArg;
        va_start(pArg, serial);
            char *adrIp=va_arg(pArg, char *);
            int port = va_arg(pArg, int);
            envoyerMessDGRAM(sockEch, buff, adrIp, port);
        va_end(pArg);
    }
    else if(sockEch->mode == SOCK_STREAM){
        envoyerMessSTREAM(sockEch,buff);
    }
}

void envoyerMessDGRAM(socket_t * sockEch, buffer_t buff, char * adrIp, int port){
    adr2struct(&sockEch->addrDst, adrIp, port);
    CHECK(sendto(sockEch->fd, buff, strlen(buff)+1, 0,(struct sockaddr *)&sockEch->addrDst, sizeof sockEch->addrDst) , "Can't send");
}

void envoyerMessSTREAM(socket_t * sockEch,buffer_t buff){
    CHECK(write(sockEch->fd, buff, strlen(buff)+1), "Can't send");
}

/**
 *	\fn			void recevoir(socket_t *sockEch, generic quoi, pFct deSerial)
 *	\brief		Réception d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		quoi : requête/réponse reçue après dé-serialisation du buffer de réception
 *	\param 		deSerial : pointeur sur la fonction de dé-serialisation d'une requête/réponse
 *	\note		si le paramètre deSerial vaut NULL alors quoi est une chaîne de caractères
 *	\result		paramètre quoi modifié avec le requête/réponse reçue
 *				paramètre sockEch modifié pour le mode DGRAM
 *              Retourne 1 pour signifier que Quelque chose est arrivé
 */
int recevoir(socket_t *sockEch, generic quoi, pFct deSerial){
    buffer_t buff;
    socklen_t cltLen;
    struct sockaddr_in clt;

    cltLen = sizeof(clt);

    if (sockEch->mode == SOCK_DGRAM){
        CHECK(recvfrom(sockEch->fd, buff, sizeof(buff), 0,(struct sockaddr *)&clt, &cltLen) , "Can't receive");
        sockEch->addrDst=clt;
    }
    else if(sockEch->mode == SOCK_STREAM){
        read(sockEch->fd, buff, sizeof(buff));
    }

    if(deSerial != NULL )
        deSerial(buff,quoi);
    else 
        strcpy(quoi,buff);

    return 1;
}

/**
 * @brief Reçoit des données d'un socket non bloquant.
 *
 * Cette fonction essaie de recevoir des données à partir d'un socket, sans se bloquer.
 * Elle retourne NULL s'il n'y a pas de données disponibles à lire.
 *
 * @param sockEch Le socket à partir duquel recevoir les données.
 * @param quoi Un pointeur vers la structure où stocker les données reçues.
 * @param deSerial Un pointeur vers une fonction de désérialisation (peut être NULL).
 *
 * @return 1 en cas de succès, NULL si aucune donnée n'est disponible, -1 en cas d'erreur.
 *
 * @details
 * - Si le socket est de type SOCK_DGRAM (UDP), `recvfrom` est utilisé pour recevoir les données.
 * - Si le socket est de type SOCK_STREAM (TCP), `read` est utilisé pour recevoir les données.
 * - La fonction de désérialisation est appelée si elle est fournie, sinon les données sont copiées directement.
 * - La fonction utilise `select` pour vérifier si des données sont disponibles avant d'appeler `recvfrom` ou `read`.
 */
int recevoir_non_block(socket_t *sockEch, void *quoi, void (*deSerial)(const buffer_t, void *)) {
    buffer_t buff;
    socklen_t cltLen;
    struct sockaddr_in clt;
    cltLen = sizeof(clt);
    fd_set readfds;
    struct timeval timeout;

    // Clear the set
    FD_ZERO(&readfds);

    // Add sockEch->fd to the set
    FD_SET(sockEch->fd, &readfds);

    // Set timeout to 0 seconds and 0 microseconds
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    // Check if there is data to read
    int activity = select(sockEch->fd + 1, &readfds, NULL, NULL, &timeout);

    if (activity == -1) {
        perror("select error");
        return -1;
    }

    if (activity == 0) {
        // No data to read
        return NULL;
    }

    if (FD_ISSET(sockEch->fd, &readfds)) {
        if (sockEch->mode == SOCK_DGRAM) {
            ssize_t received = recvfrom(sockEch->fd, buff, sizeof(buff), 0, (struct sockaddr *)&clt, &cltLen);
            if (received < 0) {
                perror("recvfrom error");
                return -1;
            }
            sockEch->addrDst = clt;
        } else if (sockEch->mode == SOCK_STREAM) {
            ssize_t received = read(sockEch->fd, buff, sizeof(buff));
            if (received < 0) {
                perror("read error");
                return -1;
            }
        }

        if (deSerial != NULL) {
            deSerial(buff, quoi);
        } else {
            strcpy(quoi, buff);
        }

        return 1;
    }

    return NULL;
}