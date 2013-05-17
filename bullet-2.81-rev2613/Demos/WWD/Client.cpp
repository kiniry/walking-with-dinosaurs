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
		printf("to few arguments expected 2, recieved %d\n",argc);
		exit(-1);
	}
	ftype =static_cast<fitnessTest> (((int)*argv[2])-48);
	ID=  argv[1];
	printf("client %s started\n",ID.c_str());
	printf("ftype: %d\n",ftype);

	//Pipename
	std::stringstream result;

	result <<"\\\\" <<serverName <<"\\pipe\\" <<pipeName <<ID;
	fullPipeName= (result.str());

	printf("pipe name %s\n",fullPipeName.c_str());

	directory=getDirectory();
	printf("dir %s\n",	directory.c_str());
	creatureFilePath.append(directory);
	creatureFilePath.append("WWDCreatures");
	creatureFilePath.append(ID);
	creatureFilePath.append(".dat");

	printf("the creature path is %s\n",	creatureFilePath.c_str());
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
			printf("The named pipe %s is connected.\n", fullPipeName.c_str());
			break;
		}

		dwError = GetLastError();

		// Exit if an error other than ERROR_PIPE_BUSY occurs.
		if (ERROR_PIPE_BUSY != dwError)
		{
			wprintf(L"Unable to open named pipe w/err 0x%08lx\n", dwError);
			exit(-1);
		}

		// All pipe instances are busy, so wait for 5 seconds.
		if (!WaitNamedPipe(fullPipeName.c_str(), 5000))
		{
			dwError = GetLastError();
			wprintf(L"Could not open pipe: 5 second wait timed out.");
			exit(-1);
		}
	}

	// Set the read mode and the blocking mode of the named pipe. In this
	// sample, we set data to be read from the pipe as a stream of messages.
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL))
	{
		dwError = GetLastError();
		wprintf(L"SetNamedPipeHandleState failed w/err 0x%08lx\n", dwError);
		exit(-1);
	}
	printf("\n");
}

void pipeSim(std::vector<creature> *creatures){
	printf("Simulation starte\n");
	std::vector<Physics*> worlds;

	//run simulator
	for(int i=0;i< (int) creatures->size();i++){
		Physics* WWDPhysics = new Physics();
		WWDPhysics->setFitnesFunction(ftype);
		readDNA(&creatures->at(i).dna,WWDPhysics);
		WWDPhysics->runSimulation(); //runs a physics simulation and save the fitness values
		creatures->at(i).fitness = WWDPhysics->getFitness();
		printf("sim fit nr %d: %f\n",	i,creatures->at(i).fitness);
		delete WWDPhysics;
	}

	printf("\n");
}

void sendAcknowledge(){
	printf("sending ack\n");
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
		wprintf(L"WriteFile to pipe failed w/err 0x%08lx\n", dwError);
		exit(-1);
	}

	wprintf(L"Send %ld bytes to server: \"%s\"\n", cbWritten, chRequest);
	printf("\n");
}

void sendResult(std::vector<creature> *creatures){
	printf("Sending results\n");

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
		printf("fitness nr %d: %f\n",j, creatures->at(j).fitness);
		os.write((const char*)&creatures->at(j).fitness, sizeof(float));
	}
	if(!os.good()){
		printf("error\n");
		exit(-1);
	}
	os.flush();
	os.close();
	printf("sending done\n\n");
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
			wprintf(L"ReadFile from pipe failed w/err 0x%08lx\n", dwError);
			exit(-1);
		}

		printf("Receive %ld bytes from server: \"%s\"\n", cbRead, chResponse);
	} while (!fFinishRead); // Repeat loop if ERROR_MORE_DATA
	if (strcmp(chResponse, "go")==0){
		return true;
	}
	printf("\n");
	return false;
}