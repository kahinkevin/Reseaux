#undef UNICODE
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>

// Link avec ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

/* Constantes globales */
//IP :
#define DELIMITEUR_IP '.'
#define N_DELIMITEUR_IP 3
#define LONGUEUR_ADRESSE_IP 16
#define	MINIMUM_OCTET_IP 0
#define MAXIMUM_OCTET_IP 255
#define BASE_DIX 10

//PORT : (Utilisation de caractères dans le client, car getaddrinfo)
#define INDICE_AVANT_FIN_PORT 2
#define INDICE_FIN_PORT 3
#define LONGUEUR_PORT 5

//SONDAGE :
#define TAILLE_QUESTION 200
#define TAILLE_MOT_RECU 199
#define TAILLE_REPONSE 300
#define TAILLE_MOT_ENVOYE 299

using namespace std;

/*******************************************************************
	Fonction: estCaractereException()
	Paramètres:
		caractereIP : caractere actuel lu pour verifier le format
		IP
	Retour: vrai si le caractere est en dehors de la plage de
		caractères 0 et 9, sinon faux
	Description: Vérifie que l'octet contient des caractères
		d'exception
*******************************************************************/
bool estCaractereException(char caractereIP) {
	return (caractereIP < '0' || '9' < caractereIP);
}

/*******************************************************************
	Fonction: estPoint()

	Paramètres:
		caractereIP : caractere actuel lu pour verifier le format IP

	Retour: vrai si le caractere est un point, sinon faux

	Description: Vérifie que l'octet est un point
*******************************************************************/
bool estPoint(char caractereIP) {
	return (caractereIP == DELIMITEUR_IP);
}

/*******************************************************************
	Fonction: estLongueurValide()

	Paramètres:
		indexAdresseIP : l'index du caractère lu actuellement
		longueurAdresseIP : la longueur de l'adresse IP

	Retour: vrai si l'adresse IP a une longueur valide, sinon faux

	Description: Vérifie que l'adresse IP passée en paramètre a
		une longueur valide, c'est-à-dire plus petite que sa longueur
		totale
*******************************************************************/
bool estLongueurValide(size_t indexAdresseIP, size_t longueurAdresseIP) {
	return (indexAdresseIP < longueurAdresseIP);
}

/*******************************************************************
	Fonction: extraireOctetIP()

	Paramètres:
		octetIP (par référence) : l'octet IP en type size_t
		indexAdresseIP (par référence) : l'index du caractère
		lu actuellement
		longueurAdresseIP : la longueur de l'adresse IP
		adresseIPTemp : adresse IP à vérifier, entrée par
		l'utilisateur

	Retour: vrai si l'octet IP est valide, sinon faux

	Description: Convertit un octet IP de type char en size_t
*******************************************************************/
bool extraireOctetIP(size_t& octetIP, size_t& indexAdresseIP, size_t longueurAdresseIP,
	const char* adresseIPTemp) {
	//parcourir l'octetIP jusqu'au point
	while (estLongueurValide(indexAdresseIP, longueurAdresseIP) && !estPoint(adresseIPTemp[indexAdresseIP])) {
		char caractereActuel = adresseIPTemp[indexAdresseIP++];

		//permet de rejeter les lettres ou d'autres caracteres qui ne sont pas des chiffres
		if (estCaractereException(caractereActuel)) {
			return false;
		}

		//permet de décaler les chiffres lus vers la gauche, tout en convertissant le char en size_t
		octetIP = (octetIP * BASE_DIX) + (caractereActuel - '0');
	}
	return true;
}

/*******************************************************************
	Fonction: estFormatIP()

	Paramètres:
	adresseIPTemp : l'adresse IP du serveur entrée par
		l'utilisateur

	Retour: vrai si l'adresse IP respecte le format IPv4, sinon
		faux

	Description: Vérifie que l'adresse IP passée en paramètre est
		valide (format uniquement).
		rejete les adresses dépassant le nombre de points, les
		caractères qui ne sont pas des chiffres, les longueurs
		invalides, etc.
	Source : http://ideone.com/ZmUjeM
*******************************************************************/
bool estFormatIP(char* adresseIPTemp) {
	//IP invalide si l'adresse est nulle
	if (adresseIPTemp == NULL) {
		return false;
	}

	size_t nPoints = 0, indexAdresseIP = 0;
	const size_t longueurAdresseIP = strlen(adresseIPTemp);

	while (estLongueurValide(indexAdresseIP, longueurAdresseIP)) {
		//le premier caractère n'est pas un point
		if (estPoint(adresseIPTemp[indexAdresseIP])) {
			return false;
		}

		//extraire un octet IP
		size_t octetIP = 0;
		extraireOctetIP(octetIP, indexAdresseIP, longueurAdresseIP, adresseIPTemp);

		//si la longueur est toujours valide, continuer de vérifier l'adresse et augmenter le nombre de '.'
		//rencontrés
		if (estLongueurValide(indexAdresseIP, longueurAdresseIP)) {
			++nPoints;
		}
		//permet notamment d'éliminer les cas tels que 132.207.x.y. et 132.256.x.y
		if (N_DELIMITEUR_IP < nPoints || octetIP < MINIMUM_OCTET_IP || MAXIMUM_OCTET_IP < octetIP) {
			return false;
		}

		//pour sauter le point à la fin de l'octet
		++indexAdresseIP;
	}

	//permet de rejeter les adresses n'ayant pas assez de points
	if (nPoints < N_DELIMITEUR_IP) {
		return false;
	}
	else {
		return true;
	}
}

bool estPortValide(char* portTemp) {

	const size_t tailleDebutPort = 2;
	char debutPort[tailleDebutPort] = { '6', '0' }; //mock pour port 60xy
	bool debutPortValide = true;


	//verifier que les 2 premiers caracteres sont valides : ils doivent etre 60
	for (size_t i = 0; i < tailleDebutPort && debutPortValide; i++) {
		debutPortValide = portTemp[i] == debutPort[i];
	}
	
	//Si le début du port est invalide, rejeter le port
	if (!debutPortValide) {
		return false;
	}
	//verifier le cas 605x, où x ne doit pas etre 0
	if (debutPortValide && portTemp[INDICE_AVANT_FIN_PORT] == '5' && portTemp[INDICE_FIN_PORT] != '0') {
		return false;
	}
	//les autres cas : 600x a 6050 
	if (debutPortValide && '0' <= portTemp[INDICE_AVANT_FIN_PORT] && portTemp[INDICE_AVANT_FIN_PORT] <= '5') {
		return true;
	}
}

/*******************************************************************
	Fonction: saisirParametres()

	Paramètres:
		adresseIP: Adresse IP du serveur
		port: Port d'écoute du serveur

	Retour: Aucun

	Description: S'occupe de saisir tous les paramètres du serveur
		en demandant à l'utilisateur l'adresse IP du serveur, son
		port d'écoute.

*******************************************************************/
void saisirParametres(char*& adresseIP, char*& port) {

	cout << "Parametres du serveur a joindre" << endl;

	char adresseIPTemp[LONGUEUR_ADRESSE_IP];
	//demander l'adresse IP tant qu'elle n'est pas valide
	do {
		cout << endl << "Entrer l'adresse IP du poste du serveur a joindre: ";
		gets_s(adresseIPTemp);
	} while (!estFormatIP(adresseIPTemp));

	//enregistrer l'adresse IP validée
	for (size_t i = 0; i < sizeof(adresseIPTemp); i++) {
		adresseIP[i] = adresseIPTemp[i];
	}

	char portTemp[LONGUEUR_PORT];
	//demander le port tant qu'il n'est pas valide
	do {
		cout << endl << "Entrer le port d'ecoute (entre 6000 et 6050): ";
		gets_s(portTemp);
	} while (!estPortValide(portTemp));

	//enregistrer le port validé
	for (size_t i = 0; i < sizeof(portTemp); i++) {
		port[i] = portTemp[i];
	}

}

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET leSocket;// = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char motEnvoye[TAILLE_REPONSE];
	char motRecu[TAILLE_QUESTION];
	int iResult;

	//--------------------------------------------
	// InitialisATION de Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("Erreur de WSAStartup: %d\n", iResult);
		return 1;
	}
	// On va creer le socket pour communiquer avec le serveur
	leSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (leSocket == INVALID_SOCKET) {
		printf("Erreur de socket(): %ld\n\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		return 1;
	}
	//--------------------------------------------
	// On va chercher l'adresse du serveur en utilisant la fonction getaddrinfo.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;        // Famille d'adresses
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  // Protocole utilisé par le serveur

									  /* Initialisation des variables */
	char* adresseIP = new char[LONGUEUR_ADRESSE_IP];
	char* port = new char[LONGUEUR_PORT];

	/*Fonction 1*/
	//Saisie des paramètres du serveur(adresse IP, port d’écoute entre 6000 et 6050)
	saisirParametres(adresseIP, port);

	// getaddrinfo obtient l'adresse IP du host donné
	iResult = getaddrinfo(adresseIP, port, &hints, &result);
	if (iResult != 0) {
		printf("Erreur de getaddrinfo: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	//---------------------------------------------------------------------		
	//On parcours les adresses retournees jusqu'a trouver la premiere adresse IPV4
	while ((result != NULL) && (result->ai_family != AF_INET))
		result = result->ai_next;


	//-----------------------------------------
	if (((result == NULL) || (result->ai_family != AF_INET))) {
		freeaddrinfo(result);
		printf("Impossible de recuperer la bonne adresse\n\n");
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		return 1;
	}

	sockaddr_in *adresse;
	adresse = (struct sockaddr_in *) result->ai_addr;
	//----------------------------------------------------
	printf("\nAdresse trouvee pour le serveur %s : %s\n\n", adresseIP, inet_ntoa(adresse->sin_addr));
	printf("Tentative de connexion au serveur %s avec le port %s\n\n", inet_ntoa(adresse->sin_addr), port);

	// On va se connecter au serveur en utilisant l'adresse qui se trouve dans
	// la variable result.
	iResult = connect(leSocket, result->ai_addr, (int)(result->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
		printf("Impossible de se connecter au serveur %s sur le port %s\n\n", inet_ntoa(adresse->sin_addr), port);
		freeaddrinfo(result);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		return 1;
	}

	printf("Connecte au serveur %s:%s\n\n", adresseIP, port);
	freeaddrinfo(result);


	//------------------------------
	// On va recevoir la question du serveur
	iResult = recv(leSocket, motRecu, TAILLE_MOT_RECU, 0);
	if (iResult > 0) {
		printf("Question: ");
		printf(motRecu);
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
	}

	//----------------------------
	// Demander à l'usager de repondre a la question
	printf("\nReponse a la question (maximum de 300 caracteres): ");
	gets_s(motEnvoye);

	//-----------------------------
	// Envoyer la reponse au serveur
	iResult = send(leSocket, motEnvoye, TAILLE_MOT_ENVOYE, 0);
	if (iResult == SOCKET_ERROR) {
		printf("Erreur du send: %d\n", WSAGetLastError());
		closesocket(leSocket);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();

		return 1;
	}
	else { printf("Message recu! "); }

	// libération des variables
	closesocket(leSocket);
	WSACleanup();
	delete[] adresseIP;
	delete[] port;

	printf("Appuyez une touche pour finir\n");
	getchar();

	return 0;
}