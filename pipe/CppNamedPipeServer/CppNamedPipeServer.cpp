#include <stdio.h>
#include <windows.h>



// The full name of the pipe in the format of \\servername\pipe\pipename.
#define SERVER_NAME         L"."
#define PIPE_NAME           L"WWD"
#define FULL_PIPE_NAME      L"\\\\" SERVER_NAME L"\\pipe\\" PIPE_NAME

#define BUFFER_SIZE     1024

// Response message from client to server. '\0' is appended in the end 
// because the client may be a native C++ application that expects NULL 
// termiated string.
#define RESPONSE_MESSAGE    L"Default response from server huehuehue"


int wmain(int argc, wchar_t* argv[])
{
    DWORD dwError = ERROR_SUCCESS;
    PSECURITY_ATTRIBUTES pSa = NULL;
    HANDLE hNamedPipe = INVALID_HANDLE_VALUE;


    // Create the named pipe.
    hNamedPipe = CreateNamedPipe(
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

    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        dwError = GetLastError();
        wprintf(L"Unable to create named pipe w/err 0x%08lx\n", dwError);
        goto Cleanup;
    }

    wprintf(L"The named pipe (%s) is created.\n", FULL_PIPE_NAME);

    // Wait for the client to connect.
    wprintf(L"Waiting for the client's connection...\n");
    if (!ConnectNamedPipe(hNamedPipe, NULL))
    {
        if (ERROR_PIPE_CONNECTED != GetLastError())
        {
            dwError = GetLastError();
            wprintf(L"ConnectNamedPipe failed w/err 0x%08lx\n", dwError);
            goto Cleanup;
        }
    }
    wprintf(L"Client is connected.\n");

    // 
    // Receive a request from client.
    // 

    BOOL fFinishRead = FALSE;
    do
    {
        wchar_t chRequest[BUFFER_SIZE];
        DWORD cbRequest, cbRead;
        cbRequest = sizeof(chRequest);

        fFinishRead = ReadFile(
            hNamedPipe,     // Handle of the pipe
            chRequest,      // Buffer to receive data
            cbRequest,      // Size of buffer in bytes
            &cbRead,        // Number of bytes read
            NULL            // Not overlapped I/O
            );

		if (ERROR_BROKEN_PIPE == GetLastError()){
		    wprintf(L"broken Pipe w/err 0x%08lx\n", dwError);
			goto Cleanup;
		}

        if (!fFinishRead && ERROR_MORE_DATA != GetLastError())
        {
            dwError = GetLastError();
            wprintf(L"ReadFile from pipe failed w/err 0x%08lx\n", dwError);
            goto Cleanup;
        }

        wprintf(L"Receive %ld bytes from client: \"%s\"\n", cbRead, chRequest);

    } while (!fFinishRead); // Repeat loop if ERROR_MORE_DATA

    // 
    // Send a response from server to client.
    // 

    wchar_t chResponse[] = RESPONSE_MESSAGE;
    DWORD cbResponse, cbWritten;
    cbResponse = sizeof(chResponse);

    if (!WriteFile(
        hNamedPipe,     // Handle of the pipe
        chResponse,     // Buffer to write
        cbResponse,     // Number of bytes to write 
        &cbWritten,     // Number of bytes written 
        NULL            // Not overlapped I/O
        ))
    {
        dwError = GetLastError();
        wprintf(L"WriteFile to pipe failed w/err 0x%08lx\n", dwError);
        goto Cleanup;
    }

    wprintf(L"Send %ld bytes to client: \"%s\"\n", cbWritten, chResponse);

    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting. Then disconnect the client's connection. 
    FlushFileBuffers(hNamedPipe);
    DisconnectNamedPipe(hNamedPipe);

Cleanup:

    // Centralized cleanup for all allocated resources.
    if (hNamedPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
    }

    return dwError;
}

