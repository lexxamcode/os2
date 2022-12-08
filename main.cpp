#include <iostream>
#include <windows.h>
#include <math.h>
#include <conio.h>

using namespace std;

DWORD WINAPI square_thread(void* arg)
{
    cout << endl;
    HANDLE semaphore_square = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, true, "square_sem");
    cout << "Square thread[" << (*(int*)arg) << "]: opening semaphore - " << GetLastError() << endl;
    while(true)
    {
        WaitForSingleObject(semaphore_square, INFINITE);
        cout << "Square thread[" << (*(int*)arg) << "]: waiting for releasing semaphore - " << GetLastError() << endl;
        HANDLE channel = CreateFile(".\\channel.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        cout << "Square thread[" << (*(int*)arg) << "]: creating file - " << GetLastError() << endl;

        HANDLE mapped_channel = CreateFileMappingA(channel, 0, PAGE_READWRITE, 0, 0, NULL);
        cout << "Square thread[" << (*(int*)arg) << "]: mapping file - " << GetLastError() << endl;

        float *got = (float*) MapViewOfFile(mapped_channel, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        cout << "Square thread[" << (*(int*)arg) << "]: got values - " << GetLastError() << endl;
        float result = got[*(int*)arg]*got[*(int*)arg];
        cout << "Square thread[" << (*(int*)arg) << "]: " << got[*(int*)arg] << "^ = " << result << endl;
        got[*(int*)arg] = result;

        CloseHandle(mapped_channel);
        cout << "Square thread[" << (*(int*)arg) << "]: unmapping file - " << GetLastError() << endl;
        CloseHandle(channel);
        cout << "Square thread[" << (*(int*)arg) << "]: closing file - " << GetLastError() << endl;
        ReleaseSemaphore(semaphore_square, 1, NULL);
        cout << "Square thread[" << (*(int*)arg) << "]: releasing semaphore - " << GetLastError() << endl;

        cout << endl;
    }
}

DWORD WINAPI sum_thread(void* arg)
{
    cout << endl;
    HANDLE semaphore_sum = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, true, "sum_sem");
    cout << "Sum thread: waiting for releasing semaphore - " << GetLastError() << endl;
    while(true)
    {
        WaitForSingleObject(semaphore_sum, INFINITE);
        cout << "Sum thread: waiting for releasing semaphore - " << GetLastError() << endl;
        HANDLE channel = CreateFile(".\\channel.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        cout << "Sum thread: creating file - " << GetLastError() << endl;

        HANDLE mapped_channel = CreateFileMappingA(channel, 0, PAGE_READWRITE, 0, 0, NULL);
        cout << "Sum thread: mapping file - " << GetLastError() << endl;

        float *got = (float*) MapViewOfFile(mapped_channel, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        cout << "Sum thread: got values - " << GetLastError() << endl;
        float result = got[0] + got[1];
        cout << "Sum thread: " << got[0] << " + " << got[1] << " = " << result << endl;

        got[0] = result;

        CloseHandle(mapped_channel);
        cout << "Sum thread: unmapping file - " << GetLastError() << endl;
        CloseHandle(channel);
        cout << "Sum thread: closing file - " << GetLastError() << endl;
        ReleaseSemaphore(semaphore_sum, 1, NULL);
        cout << "Sum thread: releasing semaphore - " << GetLastError() << endl;

        cout << endl;
    }
}

DWORD WINAPI root_thread(void* arg)
{
    cout << endl;
    HANDLE semaphore_root = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, true, "root_sem");
    cout << "Root thread: waiting for releasing semaphore - " << GetLastError() << endl;
    while(true)
    {
        WaitForSingleObject(semaphore_root, INFINITE);
        cout << "Root thread: waiting for releasing semaphore - " << GetLastError() << endl;
        HANDLE channel = CreateFile(".\\channel.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        cout << "Root thread: creating file - " << GetLastError() << endl;

        HANDLE mapped_channel = CreateFileMappingA(channel, 0, PAGE_READWRITE, 0, 0, NULL);
        cout << "Root thread: mapping file - " << GetLastError() << endl;

        float *got = (float*) MapViewOfFile(mapped_channel, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        cout << "Root thread: got values - " << GetLastError() << endl;
        float result = sqrt(got[0]);
        cout << "Root thread: " << got[0] << "^1/2 = " << result << endl;

        got[0] = result;

        CloseHandle(mapped_channel);
        cout << "Root thread: unmapping file - " << GetLastError() << endl;
        CloseHandle(channel);
        cout << "Root thread: closing file - " << GetLastError() << endl;
        ReleaseSemaphore(semaphore_root, 1, NULL);
        cout << "Root thread: releasing semaphore - " << GetLastError() << endl;

        cout << endl;
    }
}



int main(int argc, char* argv[])
{
    float a, b, result;
    cin >> a >> b;
    int first = 0, second = 1;

    //Create semaphores
    HANDLE semaphore_square = CreateSemaphoreA(NULL, 0, 1, "square_sem");
    HANDLE semaphore_sum = CreateSemaphoreA(NULL, 0, 1, "sum_sem");
    HANDLE semaphore_root = CreateSemaphoreA(NULL, 0, 1, "root_sem");
    cout << "Creating semaphores: " << GetLastError() << endl;

    //Starting threads
    HANDLE a_square_thread_handle = CreateThread(NULL, 0, &square_thread, &first, 0, NULL);
    HANDLE b_square_thread_handle = CreateThread(NULL, 0, &square_thread, &second, 0, NULL);
    HANDLE sum_thread_handle = CreateThread(NULL, 0, &sum_thread, 0, 0, NULL);
    HANDLE root_thread_handle = CreateThread(NULL, 0, &root_thread, 0, 0, NULL);

    //Open and map file to memory
    HANDLE channel = CreateFile(".\\channel.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    cout << "Creating file: " << GetLastError() << endl;

    BYTE buf = '\0';
    DWORD dwWritten = 0;
    WriteFile(channel, &buf, sizeof(buf), &dwWritten, NULL);

    HANDLE mapped_channel = CreateFileMappingA(channel, 0, PAGE_READWRITE, 0, 0, NULL);
    cout << "Mapping file: " << GetLastError() << endl;

    //Write input values to memory
    float* values = (float*) MapViewOfFile(mapped_channel, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    cout << "Writing values to memory: " << GetLastError() << endl;

    values[0] = a;
    values[1] = b;

    //Close and unmap file
    CloseHandle(mapped_channel);
    cout << "Closing and unmapping file: " << GetLastError() << endl;

    //Release the semaphore -> square threads(2)
    ReleaseSemaphore(semaphore_square, 1, 0);
    cout << "Releasing the semaphore: " << GetLastError() << endl;

    WaitForSingleObject(semaphore_square, INFINITE);
    cout << "Wating for thread(a) to end: " << GetLastError() << endl;

    //Next threads -> sum thread, root thread
    ReleaseSemaphore(semaphore_sum, 1, 0);
    WaitForSingleObject(semaphore_sum, INFINITE);

    ReleaseSemaphore(semaphore_root, 1, 0);
    WaitForSingleObject(semaphore_root, INFINITE);

    //Open and map file to memory
    channel = CreateFile(".\\channel.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    cout << "Opening file: " << GetLastError() << endl;

    mapped_channel = CreateFileMappingA(channel, 0, PAGE_READWRITE, 0, 0, NULL);
    cout << "Mapping file: " << GetLastError() << endl;

    //Write input values to memory
    values = (float*) MapViewOfFile(mapped_channel, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    cout << "Reading result from memory: " << GetLastError() << endl;

    result = values[0];

    //Close and unmap file
    CloseHandle(mapped_channel);
    cout << "Closing and unmapping file: " << GetLastError() << endl;
    CloseHandle(channel);
    cout << "Closing file: " << GetLastError() << endl;

    cout << "\na = " << a << "; b = " << b << endl << "result = " << result << endl;

    _getch();
    return 0;
}