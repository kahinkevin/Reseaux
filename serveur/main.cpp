#undef UNICODE
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <iostream>
#include <algorithm>
#include <strstream>
#include <string>
#include <chrono>
#include <fstream>

using namespace std;

// link with Ws2_32.lib
#pragma comment( lib, "ws2_32.lib" )

// External functions
extern DWORD WINAPI EchoHandler(void* sd_);

/* Variables globales */
char question[200];
string informations[2];
//ofstream journal;

// List of Winsock error constants mapped to an interpretation string.
// Note that this list must remain sorted by the error constants'
// values, because we do a binary search on the list when looking up
// items.
static struct ErrorEntry {
	int nID;
	const char* pcMessage;

	ErrorEntry(int id, const char* pc = 0) :
		nID(id),
		pcMessage(pc)
	{
	}

	bool operator<(const ErrorEntry& rhs) const
	{
		return nID < rhs.nID;
	}
} gaErrorList[] = {
	ErrorEntry(0,                  "No error"),
	ErrorEntry(WSAEINTR,           "Interrupted system call"),
	ErrorEntry(WSAEBADF,           "Bad file number"),
	ErrorEntry(WSAEACCES,          "Permission denied"),
	ErrorEntry(WSAEFAULT,          "Bad address"),
	ErrorEntry(WSAEINVAL,          "Invalid argument"),
	ErrorEntry(WSAEMFILE,          "Too many open sockets"),
	ErrorEntry(WSAEWOULDBLOCK,     "Operation would block"),
	ErrorEntry(WSAEINPROGRESS,     "Operation now in progress"),
	ErrorEntry(WSAEALREADY,        "Operation already in progress"),
	ErrorEntry(WSAENOTSOCK,        "Socket operation on non-socket"),
	ErrorEntry(WSAEDESTADDRREQ,    "Destination address required"),
	ErrorEntry(WSAEMSGSIZE,        "Message too long"),
	ErrorEntry(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
	ErrorEntry(WSAENOPROTOOPT,     "Bad protocol option"),
	ErrorEntry(WSAEPROTONOSUPPORT, "Protocol not supported"),
	ErrorEntry(WSAESOCKTNOSUPPORT, "Socket type not supported"),
	ErrorEntry(WSAEOPNOTSUPP,      "Operation not supported on socket"),
	ErrorEntry(WSAEPFNOSUPPORT,    "Protocol family not supported"),
	ErrorEntry(WSAEAFNOSUPPORT,    "Address family not supported"),
	ErrorEntry(WSAEADDRINUSE,      "Address already in use"),
	ErrorEntry(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
	ErrorEntry(WSAENETDOWN,        "Network is down"),
	ErrorEntry(WSAENETUNREACH,     "Network is unreachable"),
	ErrorEntry(WSAENETRESET,       "Net connection reset"),
	ErrorEntry(WSAECONNABORTED,    "Software caused connection abort"),
	ErrorEntry(WSAECONNRESET,      "Connection reset by peer"),
	ErrorEntry(WSAENOBUFS,         "No buffer space available"),
	ErrorEntry(WSAEISCONN,         "Socket is already connected"),
	ErrorEntry(WSAENOTCONN,        "Socket is not connected"),
	ErrorEntry(WSAESHUTDOWN,       "Can't send after socket shutdown"),
	ErrorEntry(WSAETOOMANYREFS,    "Too many references, can't splice"),
	ErrorEntry(WSAETIMEDOUT,       "Connection timed out"),
	ErrorEntry(WSAECONNREFUSED,    "Connection refused"),
	ErrorEntry(WSAELOOP,           "Too many levels of symbolic links"),
	ErrorEntry(WSAENAMETOOLONG,    "File name too long"),
	ErrorEntry(WSAEHOSTDOWN,       "Host is down"),
	ErrorEntry(WSAEHOSTUNREACH,    "No route to host"),
	ErrorEntry(WSAENOTEMPTY,       "Directory not empty"),
	ErrorEntry(WSAEPROCLIM,        "Too many processes"),
	ErrorEntry(WSAEUSERS,          "Too many users"),
	ErrorEntry(WSAEDQUOT,          "Disc quota exceeded"),
	ErrorEntry(WSAESTALE,          "Stale NFS file handle"),
	ErrorEntry(WSAEREMOTE,         "Too many levels of remote in path"),
	ErrorEntry(WSASYSNOTREADY,     "Network system is unavailable"),
	ErrorEntry(WSAVERNOTSUPPORTED, "Winsock version out of range"),
	ErrorEntry(WSANOTINITIALISED,  "WSAStartup not yet called"),
	ErrorEntry(WSAEDISCON,         "Graceful shutdown in progress"),
	ErrorEntry(WSAHOST_NOT_FOUND,  "Host not found"),
	ErrorEntry(WSANO_DATA,         "No host data of that type was found")
};
const int kNumMessages = sizeof(gaErrorList) / sizeof(ErrorEntry);


//// WSAGetLastErrorMessage ////////////////////////////////////////////
// A function similar in spirit to Unix's perror() that tacks a canned 
// interpretation of the value of WSAGetLastError() onto the end of a
// passed string, separated by a ": ".  Generally, you should implement
// smarter error handling than this, but for default cases and simple
// programs, this function is sufficient.
//
// This function returns a pointer to an internal static buffer, so you
// must copy the data from this function before you call it again.  It
// follows that this function is also not thread-safe.
const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, int nErrorID = 0)
{
	// Build basic error string
	static char acErrorBuffer[256];
	ostrstream outs(acErrorBuffer, sizeof(acErrorBuffer));
	outs << pcMessagePrefix << ": ";

	// Tack appropriate canned message onto end of supplied message 
	// prefix. Note that we do a binary search here: gaErrorList must be
	// sorted by the error constant's value.
	ErrorEntry* pEnd = gaErrorList + kNumMessages;
	ErrorEntry Target(nErrorID ? nErrorID : WSAGetLastError());
	ErrorEntry* it = lower_bound(gaErrorList, pEnd, Target);
	if ((it != pEnd) && (it->nID == Target.nID)) {
		outs << it->pcMessage;
	}
	else {
		// Didn't find error in list, so make up a generic one
		outs << "unknown error";
	}
	outs << " (" << Target.nID << ")";

	// Finish error message off and return it.
	outs << ends;
	acErrorBuffer[sizeof(acErrorBuffer) - 1] = '\0';
	return acErrorBuffer;
}


/*******************************************************************
	Fonction: saisirParametres()

	Param�tres:
		adresseIP: Adresse IP du serveur
		port: Port d'�coute du serveur
		dureeSondage: Dur�e du sondage

	Retour: Aucun

	Description: S'occupe de saisir tous les param�tres du serveur
		en demandant � l'utilisateur l'adresse IP du serveur, son
		port d'�coute ainsi que la dur�e d�sir�e du sondage.

*******************************************************************/
void saisirParametres(char*& adresseIP, int& port, int& dureeSondage) {

	// TODO: Verifier que l'entree est bien une adresse IP
	char adresseIPTemp[16];
	cout << "Parametres du serveur" << endl << endl << "Entrer l'adresse IP du poste du serveur: ";
	gets_s(adresseIPTemp);
	for (size_t i = 0; i < sizeof(adresseIPTemp); i++) {
		adresseIP[i] = adresseIPTemp[i];
	}
	
	// Tant que la valeur de port n'est pas entre 6000 et 6050...
	while (port < 6000 || 6050 < port) {
		cout << endl << "Entrer le port d'ecoute (entre 6000 et 6050): ";
		cin >> port;
		// Verifie si la valeur entree est autre qu'un entier
		while (cin.fail()) {
			cout << endl << "Entrer le port d'ecoute (entre 6000 et 6050): ";
			cin.clear();
			cin.ignore(256, '\n');
			cin >> port;
		}
	}

	// Tant que la valeur de la duree du sondage est nulle ou negative...
	while (dureeSondage <= 0) {
		cout << endl << "Entrer la duree du sondage (en secondes): ";
		cin >> dureeSondage;
		// Verifie si la valeur entree est autre qu'un entier
		while (cin.fail()) {
			cout << endl << "Entrer la duree du sondage (en secondes): ";	
			cin.clear();
			cin.ignore(256, '\n');
			cin >> dureeSondage;
		}
	}
}

/*******************************************************************
	Fonction: saisirQuestion()

	Param�tres: Aucun

	Retour: Aucun

	Description: S'occupe de saisir la question du sondage que le
		serveur emettra a tous les clients.

*******************************************************************/
void saisirQuestion() {

	/* Initialisation des variables */
	char questionTemp[1001];
	bool toggle = false;

	/* Tant qu'une question valide n'est pas entree.. */
	while (!toggle) {
		cout << endl << "Entrer la question du sondage (maximum 200 caracteres): ";
		cin.ignore();
		gets_s(questionTemp);

		// Trouver la longueur de la question
		int index = 0;
		// On cherche ou la question se termine (\0)
		for (int i = 0; i < 1001; i++) {
			if (questionTemp[i] == '\0') { index = i; break; }
		}
		// La position de \0 indique la longueur de la question
		if (0 < index && index < 200) {
			for (int i = 0; i < index; i++) {
				question[i] = questionTemp[i];
			}
			toggle = true;
		}
		else { 
			// Si la longueur n'est pas respectee, on recommence..
			cout << endl << "Question trop longue ou trop courte!"; 
			toggle = false; 
		}
		cout << endl;
	}	
}

int ouvertureSondage(char* adresseIP, int port, int dureeSondage, SOCKET ServerSocket) {

	hostent *thisHost;
	thisHost = gethostbyname(adresseIP);
	char* ip;
	ip = inet_ntoa(*(struct in_addr*) *thisHost->h_addr_list);
	printf("Adresse locale trouvee: %s ", ip);

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(adresseIP);
	service.sin_port = htons(port);


	if (bind(ServerSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		cerr << WSAGetLastErrorMessage("bind() failed.") << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return 1;
	}

	if (listen(ServerSocket, 30) == SOCKET_ERROR) {
		cerr << WSAGetLastErrorMessage("Error listening on socket.") << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return 1;
	}

	printf("\nEn attente des connections des clients sur le port %d...\n\n", ntohs(service.sin_port));
	bool toggle = true;
	auto begin = chrono::high_resolution_clock::now();

	while (toggle) {

		sockaddr_in sinRemote;
		int nAddrSize = sizeof(sinRemote);
		// Create a SOCKET for accepting incoming requests.
		// Accept the connection.
		SOCKET sd = accept(ServerSocket, (sockaddr*)&sinRemote, &nAddrSize);
		
		auto end = chrono::high_resolution_clock::now();
		auto temps = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
		cout << temps << endl;
		if (temps >= dureeSondage) { 
			toggle = false; 
			strcpy(question, "Le sondage a expire!"); 
			cout << "Temps expire, sondage termine." << endl;
			sd = shutdown(ServerSocket, 2);
		}
		else {
			toggle = true;

			if (sd != INVALID_SOCKET) {
				cout << "Connection acceptee De : " <<
					inet_ntoa(sinRemote.sin_addr) << " : " <<
					ntohs(sinRemote.sin_port) << "." <<
					endl;
				informations[0] = inet_ntoa(sinRemote.sin_addr);
				informations[1] = ntohs(sinRemote.sin_port);
				DWORD nThreadID;
				CreateThread(0, 0, EchoHandler, (void*)sd, 0, &nThreadID);
			}
			else {
				cerr << WSAGetLastErrorMessage("Echec d'une connection.") <<
					endl;
			}

		}
	}


	return 0;
}

void sauvegarderReponse(string reponse) {
	ofstream journal;
	journal.open("journal.txt", ios::app);
	journal << informations[0] << " : "<< informations[1] << " - " << reponse << endl;
	journal.close();
}

int main(void) {

	//----------------------
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		cerr << "Error at WSAStartup()\n" << endl;
		return 1;
	}

	//----------------------
	// Create a SOCKET for listening for
	// incoming connection requests.
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		cerr << WSAGetLastErrorMessage("Error at socket()") << endl;
		WSACleanup();
		return 1;
	}
	char* option = "1";
	setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, option, sizeof(option));

	/* Initialisation des variables */
	char* adresseIP = new char[16];
	int port = 0;
	int dureeSondage = 0;
	int fonction3 = 0;

	/*Fonction 1*/
	//Saisie des param�tres du serveur(adresse IP, port d��coute entre 6000 et 6050, dur�e du sondage)
	saisirParametres(adresseIP, port, dureeSondage);

	/*Fonction 2*/
	//� Saisie de la question du sondage(200 caract�res maximum)
	saisirQuestion();


	/*Fonction 3*/
	//� Ouverture du sondage : � cette �tape, le serveur se met en mode �coute sur le port s�lectionn� et
	//lance le d�compte du sondage
	//� A chaque requ�te de connexion au port �cout�, v�rifier que le sondage est toujours actif.Si oui,
	//transmettre la question, sinon notifier le client que le sondage est termin�.
	//� Recevoir les r�ponses des clients

	/* La variable fonction3 retourne 1 si une erreur s'est produite dans la fonction*/
	fonction3 = ouvertureSondage(adresseIP, port, dureeSondage, ServerSocket);
	if (fonction3 == 1) { return 1; }

	// Fermer le socket lorsqu'il n'est plus utilis�
	closesocket(ServerSocket);

	WSACleanup();

	std::cout << "Appuyer sur ENTRER pour terminer...";
	std::cin.ignore();

	return 0;
}

//// EchoHandler ///////////////////////////////////////////////////////
// Handles the incoming data by reflecting it back to the sender.

DWORD WINAPI EchoHandler(void* sd_)
{
	SOCKET sd = (SOCKET)sd_;

	// Read Data from client
	char readBuffer[300], outBuffer[200];
	int readBytes;

	send(sd, question, 200, 0);

	readBytes = recv(sd, readBuffer, 300, 0);
	string reponse = readBuffer;
	if (readBytes > 0) {
		cout << informations[0] << " : " << informations[1] << " - " << reponse << endl;
		sauvegarderReponse(reponse);

		informations[0].clear();
		informations[1].clear();
	}
	else if (readBytes == SOCKET_ERROR) {
		cout << WSAGetLastErrorMessage("Echec de la reception !") << endl;
	}
	closesocket(sd);

	return 0;
}