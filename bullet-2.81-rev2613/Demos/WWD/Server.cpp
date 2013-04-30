#include "Server.h"

int pipeServerMain(int cores, int populationSize, int nrOfGenerations, std::vector<int> ancestor){
	std::vector<creature> creatures;

	//create creatures
	creatures.push_back(creature());
	creatures.at(0).dna=ancestor;
	creatures.at(0).fitness=0;
	for(int i=1; i<populationSize;i++){
		creatures.push_back(creature());
		creatures.at(i).dna=mutate(ancestor,2);
		creatures.at(i).fitness=101;
	}
	printf("creatures %d\n", creatures.size());
	printf("DNA ");
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

		//get creatures
		creatures=getResults(cores);

		creatures=evolve(creatures);

		for(int j=0;j< (int) (creatures.size());j++){
			printf("nr %d %f\n",j,creatures.at(j).fitness);
		}

		printf("round %d \n",i);
	}
	sendOrders(stop);

	//close pipes
	getchar();
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
		ShellExecute( NULL, "open", filePathAbs, (commandArgs.str()).c_str(), NULL, SW_SHOWMINIMIZED );
	}
}

int setupServer(int cores){
	directory=  getDirectory();

	for(int i =0;i<cores;i++){
		std::stringstream	 creatureFilePath;

		creatureFilePath<<directory.c_str()<<"WWDCreatures" <<i<<".dat";
		creatureFilePaths.push_back(creatureFilePath.str());

		//Pipename
		std::stringstream result;
		result <<"\\\\" <<serverName <<"\\pipe\\" <<pipeName <<i;
		fullPipeNames.push_back(result.str());
		pipes.push_back(INVALID_HANDLE_VALUE);

		// Create the named pipe.
		pipes.at(i) = CreateNamedPipe(
			fullPipeNames.at(i).c_str(),             // Pipe name.
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

		printf("The named pipe %s is created.\n", fullPipeNames.at(i).c_str());
	}
	return 0;
}

int waitForClients(int cores){

	// Wait for the client to connect.
	wprintf(L"Waiting for the client's connection...\n");
	for(int i =0; i<cores;i++){
	if (!ConnectNamedPipe(pipes.at(i), NULL))
	{
		if (ERROR_PIPE_CONNECTED != GetLastError())
		{
			dwError = GetLastError();
			wprintf(L"ConnectNamedPipe failed w/err 0x%08lx\n", dwError);
			return -1;
		}
	}
	wprintf(L"Client is connected.\n");
	}
	return 0;
}

int sendCreatures(std::vector<creature> Creatures, int cores){
	for(int id=0;id<cores;id++){
		printf("writing creatures from %s\n", creatureFilePaths.at(id).c_str());
		std::ofstream os (creatureFilePaths.at(id));
		int noCreatures;

		int min = Creatures.size()/cores;
		int rest =  Creatures.size()%cores;
		if(id<rest){
			noCreatures=min+1;
		}else{
			noCreatures=min;
		}
		printf("writing %d/%d creatures to client %d\n", noCreatures, Creatures.size(), id);
		os.write((const char*)&noCreatures, sizeof(int));

		for(int j =id; j<Creatures.size();j+=cores){
			int size = Creatures.at(j).dna.size();
			os.write((const char*)&size, sizeof(int));
			os.write((const char*)&Creatures.at(j).dna[0], sizeof(int)*size);
			os.write((const char*)&Creatures.at(j).fitness, sizeof(float));
		}
		os.close();
	}

	printf("writing done\n");

	return 0;
}

int sendOrders(int j){
	std::string chResponse;
	//wchar_t chResponse[] =L"go";						  
	 
	if(j==go){
	chResponse = "go";
	}else{
	chResponse = "stop";
	} 	

	for(int i=0; i<pipes.size();i++){
		DWORD cbResponse, cbWritten;
		cbResponse = sizeof(chResponse);

		if (!WriteFile(
			pipes.at(i),     // Handle of the pipe
			chResponse.c_str(),     // Buffer to write
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
		printf("ack recieved\n\n");
	}
}

std::vector<creature> getResults(int cores){

	printf("getting results\n");
	std::vector<creature> results;

	//TODO:
	for(int	i =0;i<cores;i++){
		std::vector<creature> tmp =	 getCreatures(creatureFilePaths.at(i));
		results.insert(results.end(), tmp.begin(), tmp.end());
		printf("fitness nr %d: %f\n",i, tmp.at(i).fitness);
	}
	
	printf("\n");
	return results;
}