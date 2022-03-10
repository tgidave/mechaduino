#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_BUFF_LEN 1024

#define WVHT_OFFSET 40
#define SWP_OFFSET  28

int main (int argc, char *argv[])
{
  FILE *fp;
  char DataBuffer[DATA_BUFF_LEN];
  char *BuffPtr;
  char WVHT[6];
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

    if ((BuffPtr = strstr(DataBuffer, "Significant Wave Height (WVHT):")) != NULL) {
      strncpy(WVHT, (BuffPtr + WVHT_OFFSET), 5);
      *(WVHT + 5) = 0;
      continue;
    }

    if ((BuffPtr = strstr(DataBuffer, "Swell Period (SwP):")) != NULL) {
      strncpy(SwP, (BuffPtr + SWP_OFFSET), 5);
      *(SwP + 5) = 0;
      continue;
    }
  }

  printf("WVHT = %s\n", WVHT);
  printf("SwP = %s\n", SwP);

  return(0);
}

