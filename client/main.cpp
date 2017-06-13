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
#define DELIMITEUR_IP '.'
#define N_DELIMITEUR_IP 3
#define BASE_DIX 10
#define	MINIMUM_OCTET_IP 0
#define MAXIMUM_OCTET_IP 255

//PORT : (Utilisation de caractères dans le client, car getaddrinfo)
#define INDICE_AVANT_FIN_PORT 2
#define INDICE_FIN_PORT 3
#define	MINIMUM_PORT 6000
#define MAXIMUM_OCTET_IP 6050

using namespace std;

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET leSocket;// = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char motEnvoye[300];
	char motRecu[200];
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

	// On indique le nom et le port du serveur auquel on veut se connecter
	char host[16];

	/* Saisie de l'adresse IP du serveur */
	printf("Entrer l'adresse IP du serveur: ");
	gets_s(host);


	char port[5];

	// TODO: Verifier que le port d'ecoute est entre 6000 et 6050
	/* Saisie du port d'ecoute */
	cout << "\nEntrer le port d'ecoute (entre 6000 et 6050): ";
	gets_s(port);
	

	// getaddrinfo obtient l'adresse IP du host donné
	iResult = getaddrinfo(host, port, &hints, &result);
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
	printf("\nAdresse trouvee pour le serveur %s : %s\n\n", host, inet_ntoa(adresse->sin_addr));
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

	printf("Connecte au serveur %s:%s\n\n", host, port);
	freeaddrinfo(result);


	//------------------------------
	// On va recevoir la question du serveur
	iResult = recv(leSocket, motRecu, 199, 0);
	if (iResult > 0) {
		//printf("Nombre d'octets recus: %d\n", iResult);
		//motRecu[iResult] = '\0';
		printf("Question: ");
		printf(motRecu);
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
	}

	//----------------------------
	// Demander à l'usager de repondre a la question
	printf("\nReponse a la question: ");
	gets_s(motEnvoye);

	//-----------------------------
	// Envoyer la reponse au serveur
	iResult = send(leSocket, motEnvoye, 299, 0);
	if (iResult == SOCKET_ERROR) {
		printf("Erreur du send: %d\n", WSAGetLastError());
		closesocket(leSocket);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();

		return 1;
	}
	else { printf("Message recu! "); }

	//printf("Nombre d'octets envoyes : %ld\n", iResult);

	// cleanup
	closesocket(leSocket);
	WSACleanup();

	printf("Appuyez une touche pour finir\n");
	getchar();
	return 0;
}