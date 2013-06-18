#include "Server.h"

creature pipeServerMain(int cores, int populationSize, int nrOfGenerations, std::vector<int> ancestor,fitnessTest type, progressInfo* proInfo, BOOLEAN* stopSim){
	std::srand(std::time(0));
	initEvolution(populationSize);
	std::vector<creature>* creatures = new std::vector<creature>();
	//create creatures
	creatures->push_back(creature());
	creatures->at(0).dna=ancestor;
	creatures->at(0).fitness=0;
	for(int i=1; i<populationSize;i++){
		creatures->push_back(creature());
		creatures->at(i).dna=mutate(ancestor,expectedDiviation);
		creatures->at(i).fitness=0;
	}

	setupServer(populationSize, cores);

	//star programs
	startPrograms(type);

	waitForClients();
	for(int i=1; i<populationSize;i++){
		assertFloat(0,creatures->at(i).fitness,0.0005);
	}
	int start =0;
	for(int i=0;i<nrOfGenerations;i++){
		sendCreatures(creatures, start);
		sendOrders(go);

		receiveAcknowledges();

		//get creatures
		getResults(creatures, start);
		assertInt(creatures->size(), populationSize);

		//Create MTree's
		for(int j=0;j<(int)creatures->size();j++){
			creatures->at(j).treePointer=getMTree(&creatures->at(j).dna);
		}

		proInfo->stats=evolve(creatures);//evolve clean up the MTree's so no need for that

//		for(int j=0;j< (int) (creatures->size()/5.+0.5);j++){
///			printf("nr %d %f\n",j,creatures->at(j).fitness);
//		}

		start=survivors;
		proInfo->rounds = i+1;
		if(*stopSim){
			*stopSim=false;
			proInfo->rounds=nrOfGenerations;
			break;
		}
//		printf("end of round %d \n",i);
	}
	sendOrders(stop);

	//close pipes
	cleanUp();
	creature result = creatures->at(0);
	delete creatures;
	return result;
}

void startPrograms(fitnessTest type){
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

	int show=SW_HIDE;
#ifdef _DEBUG
	show=SW_MINIMIZE;
#endif
	for(int i=0;i<pipes.size();i++){
		std::stringstream commandArgs;
		commandArgs<<i<<" "<<type;
		int error=(int) ShellExecute( NULL, "open", filePathAbs, (commandArgs.str()).c_str(), NULL, show);
		if(error < 32){
			printf("Error: faild to start Client\n");
			exit(-1);
		}
	}
}

int setupServer(int pop, int cores){
	directory=  getDirectory();

	int programsNeeded;

	if(pop<cores){
		programsNeeded=pop;
	}else{
		programsNeeded=cores;
	}

	for(int i =0;i<programsNeeded;i++){
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
	}
	return 0;
}

// Wait for the client to connect.
int waitForClients(){
	for(int i =0; i<pipes.size();i++){
		if (!ConnectNamedPipe(pipes.at(i), NULL))
		{
			if (ERROR_PIPE_CONNECTED != GetLastError())
			{
				dwError = GetLastError();
				wprintf(L"ConnectNamedPipe failed w/err 0x%08lx\n", dwError);
				return -1;
			}
		}
	}
	return 0;
}

void sendCreatures(std::vector<creature>* Creatures, int start){
	int min = (Creatures->size()-start)/pipes.size();
	int rest = (Creatures->size()-start)%pipes.size();

	for(int id=0;id<pipes.size();id++){
		std::ofstream os;
		os.open(creatureFilePaths.at(id),std::ios::out | std::ios::binary);
		if(!os.good()){
			printf("good()=%d" , os.good());
			printf(" eof()=%d" , os.eof());
			printf(" fail()=%d", os.fail());
			printf(" bad()=%d\n", os.bad());
			printf("error\n");
			exit(-1);
		}

		int noCreatures;

		if(id<rest){
			noCreatures=min+1;
		}else{
			noCreatures=min;
		}

		os.write((const char*)&noCreatures, sizeof(int));

		for(int j =id+start; j<Creatures->size();j+=pipes.size()){
			int size = Creatures->at(j).dna.size();
			os.write((const char*)&size, sizeof(int));
			os.write((const char*)&Creatures->at(j).dna[0], sizeof(int)*size);
			os.write((const char*)&Creatures->at(j).fitness, sizeof(float));
		}
		if(!os.good()){
			printf("good()=%d" , os.good());
			printf(" eof()=%d" , os.eof());
			printf(" fail()=%d", os.fail());
			printf(" badd()=%d\n", os.bad());
			printf("error\n");
			exit(-1);
		}
		os.flush();
		os.close();
	}
}

int sendOrders(int j){
	std::string chResponse;

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
/**
*	waits for acknowledges from the clients
*/
void receiveAcknowledges(){
	for(int i =0;i<pipes.size();i++){
		wchar_t chRequest[BUFFER_SIZE];
		DWORD cbRequest, cbRead;
		cbRequest = sizeof(chRequest);
		BOOL fFinishRead = FALSE;
		do{
			fFinishRead = ReadFile(
				pipes.at(i),     // Handle of the pipe
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
		} while (!fFinishRead); // Repeat loop if ERROR_MORE_DATA

		if (wcscmp(chRequest, L"DONE")!=0){
			wprintf(L"Handshake failed:\n");
			exit(-1);
		}
	}
}

void getResults(std::vector<creature>* creatures, int start){
	creatures->erase(creatures->begin()+start, creatures->end());
	for(int	i =0;i<pipes.size();i++){
		std::vector<creature> tmp =	 getCreatures(creatureFilePaths.at(i));
		creatures->insert(creatures->end(), tmp.begin(), tmp.end());
	}
}

void cleanUp(){
	for(int i=0;i<creatureFilePaths.size();i++){
		remove(creatureFilePaths.at(i).c_str());
	}
	creatureFilePaths.clear();

	pipes.clear();

	fullPipeNames.clear();
}