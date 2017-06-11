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
	hints.ai_protocol = IPPROTO_TCP;  // Protocole utilis� par le serveur

	// On indique le nom et le port du serveur auquel on veut se connecter
	char host[16];

	//char ipDebut[12] = { '1', '3', '2', '.', '2', '0', '7', '.', '2', '9', '.', '1' }; //132.207.29.1
	//char hostAvantFinValide[2] = { '0', '2' };

	//bool ipValide = true;

	// TODO: Verifier que l'entree est bien une adresse IP (VERIFIER LE FORMAT SEULEMENT)
	/* Saisie de l'adresse IP du serveur */
	printf("Entrer l'adresse IP du serveur: ");
	gets_s(host);
	

	/*do {
		printf("Entrer l'adresse IP complete du serveur (comprise entre 132.207.29.101 et 132.207.29.129): ");
		gets_s(host);

		size_t avantFinHost = 12, finHost = 13;

		//verifier que les 12 premiers caracteres sont valides : ils doivent etre 132.207.29.1
		for (size_t i = 0; i < sizeof(ipDebut) && ipValide; i++) {
			ipValide = host[i] == ipDebut[i];
		}

		//verifier que le quatrieme octet est entre 101 et 129
		if (host[avantFinHost] == hostAvantFinValide[0] && host[finHost] == '0') { //si c'est 132.207.29.100 
			ipValide = false;
		}
		else {//les autres cas (132.207.29.10x a 132.207.29.12x)
			ipValide = hostAvantFinValide[0] <= host[avantFinHost] && host[avantFinHost] <= hostAvantFinValide[1];
		}
	} while (!ipValide);*/

	char port[5];

	// TODO: Verifier que le port d'ecoute est entre 6000 et 6050
	/* Saisie du port d'ecoute */
	cout << "\nEntrer le port d'ecoute (entre 6000 et 6050): ";
	gets_s(port);
	

	// getaddrinfo obtient l'adresse IP du host donn�
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
		printf(motRecu);
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
	}

	//----------------------------
	// Demander � l'usager de repondre a la question
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
	else { printf("Message recu!"); }

	//printf("Nombre d'octets envoyes : %ld\n", iResult);

	// cleanup
	closesocket(leSocket);
	WSACleanup();

	printf("Appuyez une touche pour finir\n");
	getchar();
	return 0;
}