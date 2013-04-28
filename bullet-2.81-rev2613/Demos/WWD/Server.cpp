#include "Server.h"

int pipeServerMain(int cores, int populationSize, int nrOfGenerations, std::vector<int> ancestor){


	std::vector<creature> creatures;

	//create creatures
	creatures.push_back(creature());
	creatures.at(0).dna=ancestor;
	creatures.at(0).fitness=0;
	for(int i=1; i<populationSize;i++){
		creatures.push_back(creature());
		creatures.at(i).dna=mutate(ancestor);
		creatures.at(i).fitness=0;
	}
	printf("creatures %d\n", creatures.size());
	for(int i=0;i<creatures.at(0).dna.size(); i++){

	 printf("%d ",creatures.at(0).dna.at(i));
	}
	printf("\n");

	setupServer(cores);

	//star programs
	startPrograms(cores);

	waitForClients(cores);



	for(int i=0;i<nrOfGenerations;i++){
		sendCreatures(creatures, cores);
		sendOrders(go);

		receiveAcknowledges();


		creatures=evolve(creatures);



		for(int j=0;j< (int) (creatures.size());j++){
			printf("nr %d %f\n",j,creatures.at(j).fitness);
		}

		printf("round %d \n",i);

	}
	sendOrders(stop);
	
	return 0;
}

void startPrograms(int cores){
char fileName[MAX_PATH];
HINSTANCE hInstance = GetModuleHandle(NULL);
GetModuleFileName(hInstance, fileName, MAX_PATH);
 
	
//make absolute
std::vector<int> slashes;
char filePathAbs[MAX_PATH];
int j =0;
for(int i =0;i<MAX_PATH; i++){
	if(fileName[i]=='\\'){
		slashes.push_back(i);
	}
	if(fileName[i]=='.' && fileName[i+1]=='.' && fileName[i+2]=='\\'){
		slashes.pop_back();
		i++;
		j=slashes.back()+1;
		slashes.pop_back();
	}else{
		filePathAbs[j]=fileName[i];
		j++;
	}


}

PathRemoveFileSpec(filePathAbs);
PathAddBackslash(filePathAbs);
PathAppend(filePathAbs,"Client.exe");
for(int i=0;i<cores;i++){	 
	std::stringstream commandArgs;
	commandArgs<<i;
	ShellExecute( NULL, "open", filePathAbs, (commandArgs.str()).c_str(), NULL, SW_SHOW );
	
}

}

int setupServer(int cores){


	for(int i =0;i<cores;i++){
	//Pipename
	std::stringstream result;
	result <<"\\\\" <<serverName <<"\\pipe\\" <<pipeName <<i;
	fullPipeNames.push_back( (result.str()).c_str());
	pipes.push_back(INVALID_HANDLE_VALUE);



	// Create the named pipe.
	pipes.at(i) = CreateNamedPipe(
		fullPipeNames.at(i),             // Pipe name.
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

	if (pipes.at(i) == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		wprintf(L"Unable to create named pipe w/err 0x%08lx\n", dwError);
		return -1;
	}

	}
	return 0;
}

int waitForClients(int cores){

	
	printf("The named pipe (%s) is created.\n", fullPipeNames.at(0));

	// Wait for the client to connect.
	wprintf(L"Waiting for the client's connection...\n");
	if (!ConnectNamedPipe(pipes.at(0), NULL))
	{
		if (ERROR_PIPE_CONNECTED != GetLastError())
		{
			dwError = GetLastError();
			wprintf(L"ConnectNamedPipe failed w/err 0x%08lx\n", dwError);
			return -1;
		}
	}
	wprintf(L"Client is connected.\n");
	return 0;
}
	



int sendCreatures(std::vector<creature> Creatures, int cores){

	for(int i=0;i<cores;i++){
		std::stringstream filename;
		filename << "WWDCreatures"<<i<<".txt";
		std::ofstream creatures;
		creatures.open(filename.str());
		for(int j =i; j<Creatures.size();j+=cores){
			for(int v=0;v<	 Creatures.at(j).dna.size();v++){
			creatures<<Creatures.at(j).dna.at(v)<<" ";
			}

			creatures<<"\n";
		}
		creatures.close();
	}
	

	return 0;
}

int sendOrders(int i){
	wchar_t chResponse[] = L"go";
	/*
	if(i==go){
	chResponse = L"go";
	}else{
	chResponse = L"stop";
	} 	*/

	for(int i=0; i<pipes.size();i++){
	DWORD cbResponse, cbWritten;
	cbResponse = sizeof(chResponse);


	if (!WriteFile(
		pipes.at(i),     // Handle of the pipe
		chResponse,     // Buffer to write
		cbResponse,     // Number of bytes to write 
		&cbWritten,     // Number of bytes written 
		NULL            // Not overlapped I/O
		))
	{
		dwError = GetLastError();

		if (ERROR_BROKEN_PIPE == GetLastError()){
			wprintf(L"broken Pipe w/err 0x%08lx\n", dwError);
			return -1;
		}
		wprintf(L"WriteFile to pipe failed w/err 0x%08lx\n", dwError);
		return -1;
	}
	}
	return 0;
}

void receiveAcknowledges(){
   // 
    // Receive a request from client.
    // 
	printf("waiting for Ack\n");
	    wchar_t chRequest[BUFFER_SIZE];
        DWORD cbRequest, cbRead;
        cbRequest = sizeof(chRequest);
    BOOL fFinishRead = FALSE;
    do{


        fFinishRead = ReadFile(
			pipes.at(0),     // Handle of the pipe
            chRequest,      // Buffer to receive data
            cbRequest,      // Size of buffer in bytes
            &cbRead,        // Number of bytes read
            NULL            // Not overlapped I/O
            );

		if (ERROR_BROKEN_PIPE == GetLastError()){
		    wprintf(L"broken Pipe w/err 0x%08lx\n", dwError);
			exit(-1);
		}

        if (!fFinishRead && ERROR_MORE_DATA != GetLastError())
        {
            dwError = GetLastError();
            wprintf(L"ReadFile from pipe failed w/err 0x%08lx\n", dwError);
            exit(-1);
        }

        wprintf(L"Receive %ld bytes from client: \"%s\"\n", cbRead, chRequest);

    } while (!fFinishRead); // Repeat loop if ERROR_MORE_DATA

	if (wcscmp(chRequest, L"DONE")!=0){
		wprintf(L"Handshake failed:\n");
		exit(-1);	
	}else{
	   printf("ack recieved\n");
	}
}