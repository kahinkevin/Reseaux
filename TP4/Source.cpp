#undef UNICODE

#include <winsock2.h>
#include <iostream>
#include <algorithm>
#include <strstream>

using namespace std;

// link with Ws2_32.lib
#pragma comment( lib, "ws2_32.lib" )

// External functions
//extern DWORD WINAPI EchoHandler(void* sd_);
//extern void DoSomething(char *src, char *dest);

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

void fonction1() {}

void fonction2() {}

void fonction3() {}

void fonction4() {}

void fonction5() {}

void fonction6() {}

void fonction7() {}


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

	/*Fonction 1*/
	//� Saisie de la question du sondage(200 caract�res maximum)
	
	/*Fonction 2*/
	//� Ouverture du sondage : � cette �tape, le serveur se met en mode �coute sur le port s�lectionn� et
	//lance le d�compte du sondage
	
	/*Fonction 3*/
	//� A chaque requ�te de connexion au port �cout�, v�rifier que le sondage est toujours actif.Si oui,
	//transmettre la question, sinon notifier le client que le sondage est termin�.
		
	/*Fonction 4*/
	//� Recevoir les r�ponses des clients
	
	/*Fonction 5*/
	//� Tenir un journal des r�ponses.Il s'agit d'un fichier nomm� journal au format.txt �dit� par le
	//serveur, qui comprend sur chaque ligne l�adresse IP, le port source et la r�ponse associ�e.
		
	/*Fonction 6*/
	//� Fermeture du sondage : � l�expiration du temps allou� au sondage, les nouvelles connexions se
	//voient notifier que le sondage est termin�.Les connexions encore en cours peuvent �tre soit
	//poursuivie, soit coup�es, selon votre pr�f�rence.)
		/*Fonction 7*/	//� Afficher au cours de l�ex�cution les r�ponses re�ues(adresse IP, port source, r�ponse)

}