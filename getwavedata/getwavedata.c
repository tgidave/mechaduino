//*****************************************************************************
// getwavedata.c
//
// This program opens a file which is the output of a wget command and then
// scans the file line by line searching for two strings and then outputs
// the data associated with that character string.
// 
// The file name of the wget output file is the only parameter required.
// 
//******************************************************************************  

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_BUFF_LEN 1024

// These are the offsets from the searched for strings to the wanted data.
#define WVHT_OFFSET 40
#define SWP_OFFSET  28

void printHelp(void) {
  printf("something went wrong somewhere!\n");
}

int main (int argc, char *argv[])
{
  FILE *ifp;
  FILE *ofp;
  char DataBuffer[DATA_BUFF_LEN];
  char *BuffPtr;
  char WVHT[6];
  char SwP[6];

  ifp = stdin;
  ofp = stdout;

  if (argc > 1) {
    if ((strcmp(argv[1], "-h") == 0) || 
        (strcmp(argv[1], "--help") == 0)) {
      printHelp();
      exit(0);
    }

    if (!((strcmp(argv[1], "-") == 0) ||
        (strcmp(argv[1], "--") == 0))) {
      // Try to open the input file name.
      ifp = fopen(argv[1], "r");

      // Open failed.  Inform user and quit.
      if (ifp == NULL) {
        perror("Error while opening input file.\n");
        exit(-1);
      }
    }
 
    if (argc > 2) {
      if (!((strcmp(argv[2], "-") == 0) ||
          (strcmp(argv[2], "--") == 0))) {
        // Try to open the output file name.
        ofp = fopen(argv[2], "w");

        // Open failed.  Inform user and quit.
        if (ifp == NULL) {
          perror("Error while opening output file.\n");
          exit(-1);
        }
      }
    }

    if (argc > 3) {
      printf("Too many arguments specified.  Only a maximum of 2 may be specified!\n");
      printHelp();
    }
  }
  
  while (1) {   // Scan the file until end of file is received.
    // Get a line from the data file
    if (fgets(DataBuffer, DATA_BUFF_LEN, ifp) == NULL) {
      if (feof(ifp)) {   // Check for end of file condition 
        break;  // if EOF received break out of the loop.
      }
      else {  // If not EOF, some other error was received.
        perror("Error while reading file.\n");
        exit(-1);  // exit with an error condition.
      }
    }

    // Scan the line looking for keyword.
    if ((BuffPtr = strstr(DataBuffer, "Significant Wave Height (WVHT):")) != NULL) {
      strncpy(WVHT, (BuffPtr + WVHT_OFFSET), 5);  // Found the keyword, move it to the output buffer.
      *(WVHT + 5) = 0;  // Terminate the string.
      continue; // Go back and look for more.
    }

    // Scan the line looking for keyword.
    if ((BuffPtr = strstr(DataBuffer, "Swell Period (SwP):")) != NULL) {
      strncpy(SwP, (BuffPtr + SWP_OFFSET), 5);  // Found the keyword, move it to the output buffer.
      *(SwP + 5) = 0; // Terminate the string.
      continue; // Go back and look for more.
    }
  }

  // Print the output values.  
  fprintf(ofp, "WVHT = %s\n", WVHT);
  fprintf(ofp, "SwP = %s\n", SwP);
// add code to output data to a file to be passed to the next program.
  fclose(ifp);
  fclose(ofp);
  return(0);
}

