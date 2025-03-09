#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  // Get command line arguments: path of assembly file
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <assembly file>\n", argv[0]);
    return 1;
  }
  char* assemblyFilePath = argv[1];

  // Load the contents of the assembly file
  FILE* assemblyFile = fopen(assemblyFilePath, "r");
  if (assemblyFile == NULL) {
    perror("Failed to open assembly file");
    return 1;
  }
  fseek(assemblyFile, 0, SEEK_END);
  size_t assemblyFileSize = ftell(assemblyFile);
  fseek(assemblyFile, 0, SEEK_SET);
  char* assemblyFileContents = malloc(assemblyFileSize + 1);
  if (fread(assemblyFileContents, assemblyFileSize, 1, assemblyFile) != 1) {
    fprintf(stderr, "Failed to read assembly file");
    return 1;
  }
  fclose(assemblyFile);
  assemblyFileContents[assemblyFileSize] = '\0';

  return 0;
}
