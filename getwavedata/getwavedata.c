#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_BUFF_LEN 1024

#define SWH_OFFSET 28
#define SWP_OFFSET 28

int main (int argc, char *argv[])
{
  FILE *fp;
  char DataBuffer[DATA_BUFF_LEN];
  char *BuffPtr;
  char *EndPtr;
  char SwH[6];
  char SwP[6];

  if (argc != 2) {
    printf("No file name specified!\n");
    exit;
  }

  fp = fopen(argv[1], "r");

  if (fp == NULL) {
    perror("Error while opening file.\n");
    exit(-1);
  }

  while (1) {
    if (fgets(DataBuffer, DATA_BUFF_LEN, fp) == NULL) {
      if (feof(fp)) {
        break;
      }
      else {
        perror("Error while reading file.\n");
        exit(-1);
      }
    }

    if ((BuffPtr = strstr(DataBuffer, "Swell Height (SwH):")) != NULL) {
      strncpy(SwH, (BuffPtr + SWH_OFFSET), 5);
      *(SwH + 5) = 0;
      continue;
    }

    if ((BuffPtr = strstr(DataBuffer, "Swell Period (SwP):")) != NULL) {
      strncpy(SwP, (BuffPtr + SWP_OFFSET), 5);
      *(SwP + 5) = 0;
      continue;
    }
  }

  printf("SwH = %s\n", SwH);
  printf("SwP = %s\n", SwP);



  return(0);
}

