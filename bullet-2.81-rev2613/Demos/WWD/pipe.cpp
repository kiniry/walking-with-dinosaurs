#include "pipe.h"

void createPipeClient(){

	HANDLE clientPipe = INVALID_HANDLE_VALUE;

	DWORD dwError = ERROR_SUCCESS;

	//try to connect
	while(TRUE){

		clientPipe = CreateFile( 
			pipeName,		                // Pipe name 
			GENERIC_READ | GENERIC_WRITE,   // Read and write access
			0,                              // No sharing 
			NULL,                           // Default security attributes
			OPEN_EXISTING,                  // Opens existing pipe
			0,                              // Default attributes
			NULL                            // No template file
			);

		if (pipeName != INVALID_HANDLE_VALUE){
			printf("The named pipe (%s) is connected.\n", pipeName);
			break;
		}

		dwError = GetLastError();
		// Exit if an error other than ERROR_PIPE_BUSY occurs.
		if (ERROR_PIPE_BUSY != dwError)
		{
			printf("Unable to open named pipe w/err 0x%08lx\n", dwError);
			break;
		}

		// All pipe instances are busy, so wait for 5 seconds.
		if (!WaitNamedPipe(pipeName, 5000))
		{
			dwError = GetLastError();
			printf("Could not open pipe: 5 second wait timed out.");
			break;
		}
	}

	if(dwError==ERROR_SUCCESS){
		CloseHandle(clientPipe);
	}

}


void createPipeServer(){

	HANDLE serverPipe = INVALID_HANDLE_VALUE;

	serverPipe = CreateNamedPipe(
		pipeName,					// Pipe name.
		PIPE_ACCESS_DUPLEX,         // The pipe is duplex; both server and 
									// client processes can read from and 
									// write to the pipe
		PIPE_TYPE_MESSAGE |         // Message type pipe 
		PIPE_READMODE_MESSAGE |     // Message-read mode 
		PIPE_WAIT,                  // Blocking mode is enabled
		PIPE_UNLIMITED_INSTANCES,   // Max. instances
		BUFFER_SIZE,                // Output buffer size in bytes
		BUFFER_SIZE,                // Input buffer size in bytes
		NMPWAIT_USE_DEFAULT_WAIT,   // Time-out interval
		NULL                         // Security attributes
		);
	printf("The named pipe (%s) is created.\n",pipeName);
	printf("Waiting for the client's connection...\n");

	if (!ConnectNamedPipe( serverPipe, NULL)){
		printf("Client failled to connect connected.\n");

	}else{

		printf("Client is connected.\n");

	}
	CloseHandle(serverPipe);

}