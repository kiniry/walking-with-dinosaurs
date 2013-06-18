#include "Client.h"

int pipeClientMain(int argc,char* argv[]){
	//set/get parameters
	setParameters(argc,argv);

	setupClient();

	while(receiveOrders()){
		//get creatures
		std::vector<creature> creatures = getCreatures(creatureFilePath);

		pipeSim(&creatures);

		//send results back
		sendResult(&creatures);

		//send Done message
		sendAcknowledge();
	}

	CloseHandle(pipe);
	pipe = INVALID_HANDLE_VALUE;

	return 0;
}

void setParameters(int argc,char* argv[]){
	//ID
	if(argc!=3){
#ifdef _DEBUG
		printf("to few arguments expected 2, recieved %d\n",argc);
#endif
		exit(-1);
	}
	ftype =static_cast<fitnessTest> (((int)*argv[2])-48);
	ID=  argv[1];
#ifdef _DEBUG
	printf("client %s started\n",ID.c_str());
	printf("ftype: %d\n",ftype);
#endif
	//Pipename
	std::stringstream result;

	result <<"\\\\" <<serverName <<"\\pipe\\" <<pipeName <<ID;
	fullPipeName= (result.str());
#ifdef _DEBUG
	printf("pipe name %s\n",fullPipeName.c_str());
#endif
	directory=getDirectory();
#ifdef _DEBUG
	printf("dir %s\n",	directory.c_str());
#endif
	creatureFilePath.append(directory);
	creatureFilePath.append("WWDCreatures");
	creatureFilePath.append(ID);
	creatureFilePath.append(".dat");
#ifdef _DEBUG
	printf("the creature path is %s\n",	creatureFilePath.c_str());
#endif
}

void setupClient(){
	// Try to open the named pipe identified by the pipe name.
	while (TRUE)
	{
		pipe = CreateFile(
			fullPipeName.c_str(),                 // Pipe name
			GENERIC_READ | GENERIC_WRITE,   // Read and write access
			0,                              // No sharing
			NULL,                           // Default security attributes
			OPEN_EXISTING,                  // Opens existing pipe
			0,                              // Default attributes
			NULL                            // No template file
			);

		// If the pipe handle is opened successfully ...
		if (pipe != INVALID_HANDLE_VALUE)
		{
#ifdef _DEBUG
			printf("The named pipe %s is connected.\n", fullPipeName.c_str());
#endif
			break;
		}

		dwError = GetLastError();

		// Exit if an error other than ERROR_PIPE_BUSY occurs.
		if (ERROR_PIPE_BUSY != dwError)
		{
#ifdef _DEBUG
			wprintf(L"Unable to open named pipe w/err 0x%08lx\n", dwError);
#endif
			exit(-1);
		}

		// All pipe instances are busy, so wait for 5 seconds.
		if (!WaitNamedPipe(fullPipeName.c_str(), 5000))
		{
			dwError = GetLastError();
#ifdef _DEBUG
			wprintf(L"Could not open pipe: 5 second wait timed out.");
#endif
			exit(-1);
		}
	}

	// Set the read mode and the blocking mode of the named pipe. In this
	// sample, we set data to be read from the pipe as a stream of messages.
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL))
	{
		dwError = GetLastError();
#ifdef _DEBUG
		wprintf(L"SetNamedPipeHandleState failed w/err 0x%08lx\n", dwError);
#endif
		exit(-1);
	}
#ifdef _DEBUG
	printf("\n");
#endif
}

void pipeSim(std::vector<creature> *creatures){
#ifdef _DEBUG
	printf("Simulation starte\n");
#endif
	std::vector<Physics*> worlds;

	//run simulator
	for(int i=0;i< (int) creatures->size();i++){
		Physics* WWDPhysics = new Physics();
		WWDPhysics->addFitnessFunction(ftype,1);
		readDNA(&creatures->at(i).dna,WWDPhysics);
		WWDPhysics->runSimulation(); //runs a physics simulation and save the fitness values
		creatures->at(i).fitness = WWDPhysics->getFitness();
#ifdef _DEBUG
		printf("sim fit nr %d: %f\n",	i,creatures->at(i).fitness);
#endif
		delete WWDPhysics;
	}
#ifdef _DEBUG
	printf("\n");
#endif
}

void sendAcknowledge(){
#ifdef _DEBUG
	printf("sending ack\n");
#endif
	//send results back
	wchar_t chRequest[] = L"DONE";
	DWORD cbRequest, cbWritten;
	cbRequest = sizeof(chRequest);

	if (!WriteFile(
		pipe,                      // Handle of the pipe
		chRequest,                  // Message to be written
		cbRequest,                  // Number of bytes to write
		&cbWritten,                 // Number of bytes written
		NULL                        // Not overlapped
		))
	{
		dwError = GetLastError();
#ifdef _DEBUG
		wprintf(L"WriteFile to pipe failed w/err 0x%08lx\n", dwError);
#endif
		exit(-1);
	}
#ifdef _DEBUG
	wprintf(L"Send %ld bytes to server: \"%s\"\n", cbWritten, chRequest);
	printf("\n");
#endif
}

void sendResult(std::vector<creature> *creatures){
#ifdef _DEBUG
	printf("Sending results\n");
#endif

	std::stringstream filename;
	filename << directory<< "WWDCreatures"<<ID<<".dat";
	std::ofstream os;
	os.open(filename.str(),std::ios::out | std::ios::binary);

	int noCreatures =creatures->size();

	os.write((const char*)&noCreatures, sizeof(int));

	for(int j =0; j< (int)creatures->size();j++){
		int size = creatures->at(j).dna.size();
		os.write((const char*)&size, sizeof(int));

		os.write((const char*)&creatures->at(j).dna[0], sizeof(int)*size);
#ifdef _DEBUG
		printf("fitness nr %d: %f\n",j, creatures->at(j).fitness);
#endif
		os.write((const char*)&creatures->at(j).fitness, sizeof(float));
	}
	if(!os.good()){
#ifdef _DEBUG
		printf("error\n");
#endif
		exit(-1);
	}
	os.flush();
	os.close();
#ifdef _DEBUG
	printf("sending done\n\n");
#endif
}

bool receiveOrders(){
	char chResponse[BUFFER_SIZE];
	DWORD cbResponse, cbRead;
	cbResponse = sizeof(chResponse);

	BOOL fFinishRead = FALSE;
	do{
		fFinishRead = ReadFile(
			pipe,                  // Handle of the pipe
			chResponse,             // Buffer to receive the reply
			cbResponse,             // Size of buffer in bytes
			&cbRead,                // Number of bytes read
			NULL                    // Not overlapped
			);

		if (!fFinishRead && ERROR_MORE_DATA != GetLastError())
		{
			dwError = GetLastError();
#ifdef _DEBUG
			wprintf(L"ReadFile from pipe failed w/err 0x%08lx\n", dwError);
#endif
			exit(-1);
		}
#ifdef _DEBUG
		printf("Receive %ld bytes from server: \"%s\"\n", cbRead, chResponse);
#endif
	} while (!fFinishRead); // Repeat loop if ERROR_MORE_DATA
	if (strcmp(chResponse, "go")==0){
		return true;
	}
#ifdef _DEBUG
	printf("\n");
#endif
	return false;
}