#include "Client.h"


int pipeClientMain(int argc,char* argv[]){


	//set/get parameters
	// thread id,

	
	//setParameters(argc,argv);

	
	
	setupClient();


	while(receiveOrders()){
		//get creatures
		std::vector<creature> creatures = getCreatures();

		pipeSim(creatures);

		//send results back

		sendResult(creatures);

		sendAcknowledge();
		//act on ordes
		//set new par or kill it
	}

	CloseHandle(pipe);
	pipe = INVALID_HANDLE_VALUE;
	return 0;
}

void setParameters(int argc,char* argv[]){
   
	//ID
	if(argc!=2){
	printf("to few arguments expected 2, recieved %d\n",argc);
	  exit(-1);
	}  
	std::stringstream id;
	id << argv[1];
	ID =id.str(); 
	 

	printf("client  %s started\n",ID);


	//Pipename
	std::stringstream result;

	//result <<"\\\\" <<serverName <<"\\pipe\\" <<pipeName <<ID;
	result <<"\\\\.\\pipe\\WWD" <<ID;
	//fullPipeName= (result.str()).c_str();

	printf("pipe name %s\n",fullPipeName);
	//printf("pipe name %s\n",result.str());





//filepath creatures
char fileName[MAX_PATH];
HINSTANCE hInstance = GetModuleHandle(NULL);
GetModuleFileName(hInstance, fileName, MAX_PATH);
 
	
//make absolute
std::vector<int> slashes;
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
PathAppend(filePathAbs,"WWDCreatures");
PathAppend(filePathAbs,ID.c_str());
PathAppend(filePathAbs,".txt");
}

void setupClient(){

	// Try to open the named pipe identified by the pipe name.
	while (TRUE) 
	{
		pipe = CreateFile( 
			fullPipeName,                 // Pipe name 
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
			wprintf(L"The named pipe (%s) is connected.\n", fullPipeName);
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
		if (!WaitNamedPipe(fullPipeName, 5000))
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

}

std::vector<creature> getCreatures(){

    std::string STRING;
	std::ifstream infile;
	infile.open ("names.txt");
        while(!infile.eof()) // To get you all the lines.
        {
	        getline(infile,STRING); // Saves the line in STRING.
        }
	infile.close();

	std::vector<creature> creatures;
	return creatures;
}

void pipeSim(std::vector<creature> creatures){
	printf("Simulation starte\n");
	std::vector<Physics*> worlds;
	for(int j =0; j<(int)creatures.size(); j++){
		//init world
		Physics* WWDPhysics = new Physics(creatures.at(j).dna);

		//init creature
		readDNA(&creatures.at(j).dna,WWDPhysics);

		worlds.push_back(WWDPhysics);
	}

	//run simulator
	for(int i=0;i< (int) worlds.size();i++){
		worlds.at(i)->runSimulation(); //runs a physics simulation and save the fitness values
	}


	for(int j= worlds.size()-1; j>=0; j--){
		creatures.at(j).fitness = worlds.at(j)->getFitness();

	}

	for(int j= worlds.size()-1; j>=0; j--){
		delete worlds.at(j);
		worlds.pop_back();
	}

}


void sendAcknowledge(){

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

}

void sendResult(std::vector<creature> creatures){
	   printf("Sending results");


}

bool receiveOrders(){


	wchar_t chResponse[BUFFER_SIZE];
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

		wprintf(L"Receive %ld bytes from server: \"%s\"\n", cbRead, chResponse);

	} while (!fFinishRead); // Repeat loop if ERROR_MORE_DATA
	if (wcscmp(chResponse, L"go")==0){
		return true;	

	}
	return false;

}