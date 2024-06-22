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

    if(deSerial !=NULL ) 
        deSerial(buff,quoi);
    else 
        strcpy(quoi,buff);

    return 1;
}