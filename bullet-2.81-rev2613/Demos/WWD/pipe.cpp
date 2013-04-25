#include "pipe.h"

enum {stop, go}	;

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
	
	setupServer(cores);

	//star programs
	startPrograms(cores);

	waitForClients(cores);



	for(int i=0;i<nrOfGenerations;i++){
		sendCreatures(creatures, cores);
		sendOrders(go);

		receiveAcknowledges();


		creatures=evolve(creatures);



		for(int j=0;j< (int) (creatures.size()/5.f);j++){
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
PathAppend(filePathAbs,"Bullet_User_Manual.pdf");

for(int i=0;i<cores;i++){	  
	
	LPCSTR commandArgs = "";
	ShellExecute( NULL, "open", filePathAbs, commandArgs, NULL, SW_SHOW );
	
}
}

int setupServer(int cores){

	// Create the named pipe.
	pipe = CreateNamedPipe(
		FULL_PIPE_NAME,             // Pipe name.
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

	if (pipe == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		wprintf(L"Unable to create named pipe w/err 0x%08lx\n", dwError);
		return -1;
	}

}

int waitForClients(int cores){
	wprintf(L"The named pipe (%s) is created.\n", FULL_PIPE_NAME);

	// Wait for the client to connect.
	wprintf(L"Waiting for the client's connection...\n");
	if (!ConnectNamedPipe(pipe, NULL))
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


int sendCreatures(std::vector<creature> creatures, int cores){


	return 0;
}

int sendOrders(int i){
	wchar_t chResponse[] = L"go";
	/*if(i==go){
	chResponse = L"go";
	}else{
	chResponse = L"st";
	}  */
	DWORD cbResponse, cbWritten;
	cbResponse = sizeof(chResponse);


	if (!WriteFile(
		pipe,     // Handle of the pipe
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

	return 0;
}

void receiveAcknowledges(){
   // 
    // Receive a request from client.
    // 
	    wchar_t chRequest[BUFFER_SIZE];
        DWORD cbRequest, cbRead;
        cbRequest = sizeof(chRequest);
    BOOL fFinishRead = FALSE;
    do
    {


        fFinishRead = ReadFile(
            pipe,     // Handle of the pipe
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
	}
}

int pipeClientMain(int argc,char** argv){


	//set/get parameters
	// thread id, 



	setupClient();




	while(receiveOrders()){

		//get creatures
		std::vector<creature> creatures = getCreatures();

		pipeSim(creatures);

		//send results back

		sendResult(creatures);


		//act on ordes
		//set new par or kill it
	}

	CloseHandle(pipe);
	pipe = INVALID_HANDLE_VALUE;

	return 0;
}

void setupClient(){

	// Try to open the named pipe identified by the pipe name.
	while (TRUE) 
	{
		pipe = CreateFile( 
			(LPCSTR)FULL_PIPE_NAME,                 // Pipe name 
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
			wprintf(L"The named pipe (%s) is connected.\n", FULL_PIPE_NAME);
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
		if (!WaitNamedPipe((LPCSTR)FULL_PIPE_NAME, 5000))
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
	std::vector<creature> creatures;
	return creatures;
}

void pipeSim(std::vector<creature> creatures){
	std::vector<Physics*> worlds;
	for(int j =0; j<creatures.size(); j++){
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