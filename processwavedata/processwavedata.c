 //*****************************************************************************
// processwavedata.c
//
// 
//******************************************************************************  

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_BUFF_LEN 128

// These are the offsets from the searched for strings to the wanted data.
//#define WVHT_OFFSET 40
//#define SWP_OFFSET  28

void printHelp(void) {
  printf("something went wrong somewhere!\n");
}

int main (int argc, char *argv[])
{
  FILE *ifp;
  FILE *ofp;
  char DataBuffer[DATA_BUFF_LEN];
  char *BuffPtr;
  char charWVHT[7];
  char charSwP[7];
  float wvht;
  float swp;
  float dist;
  float vel;

  if (argc > 1) {
    if ((strcmp(argv[1], "-h") == 0) || 
        (strcmp(argv[1], "--help") == 0)) {
      printHelp();
      exit(0);
    }

    if ((strcmp(argv[1], "-") == 0) ||
        (strcmp(argv[1], "--") == 0)) {
      ifp = stdin;
    } else {
      // Try to open the input file name.
      ifp = fopen(argv[1], "r");

      // Open failed.  Inform user and quit.
      if (ifp == NULL) {
        perror("Error while input opening file.\n");
        exit(-1);
      }
    }
 
    if (argc > 2) {
      if ((strcmp(argv[2], "-") == 0) ||
          (strcmp(argv[2], "--") == 0)) {
        ofp = stdout;
      } else {
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
  } else {
    ifp = stdin;
    ofp = stdout;
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
    if ((BuffPtr = strstr(DataBuffer, "WVHT =")) != NULL) {
      strncpy(charWVHT, BuffPtr + 6, 6);  // Found the keyword, move it to the output buffer.
      *(charWVHT + 6) = 0;  // Terminate the string.
      continue; // Go back and look for more.
    }

    // Scan the line looking for keyword.
    if ((BuffPtr = strstr(DataBuffer, "SwP =")) != NULL) {
      strncpy(charSwP, BuffPtr + 5, 6);  // Found the keyword, move it to the output buffer.
      *(charSwP + 6) = 0; // Terminate the string.
      continue; // Go back and look for more.
    }
  }

  // Convert both values to float.
  wvht = strtof(charWVHT, NULL);
  swp = strtof(charSwP, NULL);

  // Cap the height to 10.99 feet.
  if (wvht > 10.99) {
    wvht = 10.99;
  }

//  if (swp > 4.375) {
//    swp = 4.375;
//  }

  // Formula from Cy's spec "distance = (SwH / 2)*1350"
  dist = (wvht / 2) * 1350;

  // Formula from Cy's spec "velocity = -216.7(SwP/SwH)+ 1247"
  vel = (-216.7 * (swp / wvht)) + 1247; 

  // Print the output values.  
  fprintf(ofp, "dist = %f\n", dist);
  fprintf(ofp, "vel = %f\n", vel);
  fclose(ifp);
  fclose(ofp);
  return(0);
}

                   
