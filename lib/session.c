#include "session.h"


/**
 *	\fn			void adr2struct (struct sockaddr_in *addr, char *adrIP, short port)
 *	\brief		Transformer une adresse au format humain en structure SocketBSD
 *	\param		addr : structure d'adressage BSD d'une socket INET
 *	\param		adrIP : adresse IP de la socket créée
 *	\param		port : port de la socket créée
 *	\note		Le domaine dépend du mode choisi (TCP/UDP)
 *	\result		paramètre *adr modifié
 */
void adr2struct (struct sockaddr_in *addr, char *adrIP, short port){
    addr->sin_family = PF_INET;
    addr->sin_port = htons (port);
    addr->sin_addr.s_addr = inet_addr(adrIP);
    memset(&addr->sin_zero, 0, 8);
}


/**
 *	\fn			socket_t creerSocket (int mode)
 *	\brief		Création d'une socket de type DGRAM/STREAM
 *	\param		mode : mode connecté (STREAM) ou non (DGRAM)
 *	\result		socket créée selon le mode choisi
 */
socket_t creerSocket (int mode){
    socket_t temp;
    CHECK(temp.fd=socket(PF_INET, mode, 0), "Can't create socket");
    temp.mode=mode;
    return temp;
}

/**
 *	\fn			socket_t creerSocketAdr (int mode, char *adrIP, short port)
 *	\brief		Création d'une socket de type DGRAM/STREAM
 *	\param		mode : adresse IP de la socket créée
 *	\param		adrIP : adresse IP de la socket créée
 *	\param		port : port de la socket créée
 *	\result		socket créée dans le domaine choisi avec l'adressage fourni
 */
socket_t creerSocketAdr (int mode, char *adrIP, short port){
    socket_t temp = creerSocket(mode);
    struct sockaddr_in addr;
    adr2struct(&addr,adrIP,port);
    temp.addrLoc=addr;
    // temp.addrLoc.sin_addr.s_addr=inet_addr(adrIP);
    // temp.addrLoc.sin_port=htons(port);
    // temp.addrLoc.sin_family=PF_INET;
    // temp.addrLoc.sin_zero
    CHECK(bind(temp.fd, (struct sockaddr *) &temp.addrLoc, sizeof temp.addrLoc) , "Can't bind");

    return temp;
}


/**
 *	\fn			creerSocketEcoute (char *adrIP, short port)
 *	\brief		Création d'une socket d'écoute avec l'adressage fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à mettre en écoute
 *	\param		port : port TCP du serveur à mettre en écoute
 *	\result		socket créée avec l'adressage fourni en paramètre et dans un état d'écoute
 *	\note		Le domaine est nécessairement STREAM
 */
socket_t creerSocketEcoute (char *adrIP, short port){
    socket_t temp = creerSocketAdr(SOCK_STREAM, adrIP, port);
    CHECK(listen(temp.fd, 10) , "Can't calibrate");
    return temp;
}


/**
 *	\fn			socket_t accepterClt (const socket_t sockEcoute)
 *	\brief		Acceptation d'une demande de connexion d'un client
 *	\param		sockEcoute : socket d'écoute pour réception de la demande
 *	\result		socket (dialogue) connectée par le serveur avec un client
 */
socket_t accepterClt (const socket_t sockEcoute){
    socket_t socketDialogue;
    struct sockaddr_in clt;
    socklen_t cltLen=sizeof(clt);

    socketDialogue.fd = accept(sockEcoute.fd, (struct sockaddr *)&clt, &cltLen);
    if (socketDialogue.fd == -1){
        return (socket_t){-1};
    } else {
        socketDialogue.mode=SOCK_STREAM;
        socketDialogue.addrLoc=sockEcoute.addrLoc;
        socketDialogue.addrDst= clt;

        return socketDialogue;
    }
}


/**
 *	\fn			socket_t connecterClt2Srv (char *adrIP, short port)
 *	\brief		Crétaion d'une socket d'appel et connexion au seveur dont
 *				l'adressage est fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à connecter
 *	\param		port : port TCP du serveur à connecter
 *	\result		socket connectée au serveur fourni en paramètre
 */
socket_t connecterClt2Srv (char *adrIP, short port){
    socket_t socketDialogue=creerSocket(SOCK_STREAM);

    socketDialogue.mode=SOCK_STREAM;

    struct sockaddr_in serveur;
    adr2struct (&serveur, adrIP, port);
    socklen_t srvLen=sizeof(serveur);

    socketDialogue.addrDst=serveur;

    CHECK(connect(socketDialogue.fd, (struct sockaddr *)&serveur, srvLen) , "Can't connect to server");

    return socketDialogue;
}