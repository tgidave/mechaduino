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

int main (int argc, char *argv[])
{
  FILE *fp;
  char DataBuffer[DATA_BUFF_LEN];
  char *BuffPtr;
  char WVHT[6];
  char SwP[6];

  // Make sure there is only one argument was specified.
  if (argc != 2) {
    printf("No file name specified!\n");
    exit;
  }

  // Try to open the input file name.
  fp = fopen(argv[1], "r");

  // Open failed.  Inform user and quit.
  if (fp == NULL) {
    perror("Error while opening file.\n");
    exit(-1);
  }

  
  while (1) {   // Scan the file until end of file is received.
    // Get a line from the data file
    if (fgets(DataBuffer, DATA_BUFF_LEN, fp) == NULL) {
      if (feof(fp)) {   // Check for end of file condition 
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
  printf("WVHT = %s\n", WVHT);
  printf("SwP = %s\n", SwP);
// add code to output data to a file to be passed to the next program.
  return(0);
}

