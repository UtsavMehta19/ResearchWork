#include <stdio.h>
#include <iostream>
#include <winsock.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define PORT1 9090

using namespace std;

int main()
{
	int clientSocket;
	struct sockaddr_in structure;
	int output = 0;
	char storage[50] = { 0 };

	//creating WSA environment
	WSADATA wsaVar;

	output = WSAStartup(MAKEWORD(2, 2), &wsaVar);
	if (output < 0)
	{
		cout << "Failed in WSA environment section" << endl;
		exit(EXIT_FAILURE);
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0)
	{
		cout << "Error in socket descriptor" << endl;
		exit(EXIT_FAILURE);
	}

	structure.sin_addr.s_addr = inet_addr("192.168.1.2"); //my ip address for home wifi
	structure.sin_port = htons(PORT1);
	structure.sin_family = AF_INET;
	memset(&(structure.sin_zero), 0, 8);

	//note a different call
	output = connect(clientSocket, (struct sockaddr*)&structure, sizeof(structure));
	if (output < 0)
	{
		cout << "Error in connect call" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "THE CLIENT HAS CONNECTED TO THE SERVER SUCCESSFULLY" << endl << endl;

	int timesteps;

	/***STARTING THE CHAT***/

	//this chunk is to exchange time-steps

	recv(clientSocket, storage, 50, 0);
	cout << storage << endl;
	fgets(storage, 50, stdin);
	send(clientSocket, storage, strlen(storage), 0);
	timesteps = storage[0] - '0';

	//A few instructions before starting
	cout << "FOLLOW THESE INSTRUCTIONS STRICTLY:-" << endl;
	cout << "1. Enter all values without spaces." << endl;
	cout << "2. Enter the first input in 8 digits, as done normally." << endl;
	cout << "3. Enter all following inputs as 9 digit numbers, with the first digit being 0 always." << endl;

	//this chunk is to exchange starting instruction

	printf("\n");
	recv(clientSocket, storage, 50, 0);
	cout << storage << endl;

	//this chunk is to create a space to store input values (no. of cars entering)
	
	char correction;

	//this chunk is to take input from client 
	for (int i = 0; i < timesteps; ++i)
	{
		char* newTraffic = (char*)malloc(9 * sizeof(char));
		fgets(newTraffic, 9, stdin);
		send(clientSocket, newTraffic, strlen(newTraffic), 0);
		cout << "Sent time-step " << i + 1 << " to the server" << endl;
		printf("\n");
		if (i == (timesteps - 1))
		{
			break;
		}
		cin >> correction;
		free(newTraffic);
		newTraffic = NULL;
		
	}

	return 0;
}
