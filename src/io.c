#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "types.h"
#include "errors.h"
#include "methods.h"

char* read_file(char* fileName) {
    // initialize error codes and string buffer for html code
    FileError err = ERR_NONE;
    char *buffer = NULL;

    // opened in binary mode to read \n characters
    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        file_log(ERROR, "File read failed");
        err = ERR_OPEN_FAILED;
        goto handle_errors;
    }
    
    // move file pointer to the end of the file and calculate size
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    if (size <= 0) {
        file_log(ERROR, "Invalid file size");
        err = ERR_INVALID_SIZE;
        goto handle_errors;
    }

    rewind(fp);
    // allocate memory for the detected file size+1 ('\0' at end)
    buffer = (char *)malloc(size+1);
    if (!buffer) {
        file_log(ERROR, "Memory allocation failed");
        err = ERR_ALLOC_FAILED;
        goto handle_errors;
    }

    // check if size read is equal to the previously detected size
    size_t readSize = fread(buffer, 1, size, fp);
    if(readSize != (size_t)size) {
        file_log(ERROR, "File Reading interrupted");
        err = ERR_READ_FAILED;
        goto handle_errors;
    } 

    // add string terminator to end of buffer
    buffer[size] = '\0';
    file_log(INFO, "file [%s] successfully read", fileName);

handle_errors:
    switch(err) {
        case ERR_READ_FAILED:
            free(buffer);
            // reset buffer after incomplete file reads
            buffer = NULL;

        case ERR_ALLOC_FAILED:
        case ERR_INVALID_SIZE: fclose(fp);
        case ERR_OPEN_FAILED:
        case ERR_NONE:

        default: return buffer;
    }
}

void file_log(LogType type, char* message, ...) {
    /* error logging is not used for this function since can cause recursive loops */
    va_list args;
    va_list argsCopy;
    time_t now;
    char timeBuffer[100];
    // struct with y-m-d-h-m-s format from time.h
    struct tm *timeInfo;

    FILE *fp = fopen("logs/server.log", "a");
    if (!fp) {
        perror("file_logERROR: File read failed");
        return;
    }

    // get time info from system
    time(&now);
    timeInfo = localtime(&now);
    if (!timeInfo) {
        perror("file_logERROR: Cannot extract timezone information");
        return;
    }

    // format localtime
    if (!strftime(timeBuffer, sizeof(timeBuffer),
                "%Y-%m-%d %H:%M:%S", timeInfo)) {
        perror("file_logERROR: formatted time output not written");
        return;
    }

    // print formatted local time to file and stdout
    fprintf(fp, "[%s]", timeBuffer);
    printf("[%s]", timeBuffer);

    // use the arguments in the variable list and format them to output
    va_start(args, message);
    va_copy(argsCopy, args);

    vfprintf(fp, message, args);
    vprintf(message, argsCopy);

    va_end(argsCopy);
    va_end(args);

    fputc('\n', fp);
    putchar('\n');

    // flush all buffers
    if (fflush(stdout) == EOF) {
        perror("fflush failed");
    }
}
