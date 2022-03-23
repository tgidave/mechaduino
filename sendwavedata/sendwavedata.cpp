// C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#define DATA_LEN  7
#define DATA_BUFFER_LEN  128
#define SERIAL_DATA_LEN  18
#define DIST_OFFSET 6
#define VEL_OFFSET 5

void printHelp(void) {
  printf("Didn't work...\n");
}

int main(int argc, char *argv[]) {

  int argIx;  // argument index

  char *buffPtr;

  char chardist[DATA_LEN + 1];  // string storage for wave heigth
  char charvel[DATA_LEN + 1];  // string storage for wave period
  char dataBuffer[DATA_BUFFER_LEN];  // string storage for serial data string.
  char serialData[SERIAL_DATA_LEN];

  float dist;
  float vel;

  FILE * inf = stdin;
  FILE * otf = stdout;

  int useSerialPort = false;

  int serialPort;  

  struct termios tty; // termios struct, we call it 'tty' for convention

  // Initialize the local variables.
  argIx = 1;

  // check the arguments.
  if (argc > 1) {
    if ((strcmp(argv[1], "-h") == 0) || 
        (strcmp(argv[1], "--help") == 0)) {
      printHelp();
      exit(0);
    }

    if (!((strcmp(argv[1], "-") == 0) ||
        (strcmp(argv[1], "--") == 0))) {

      // Try to open the input file name.
      if ((inf = fopen(argv[1], "r")) == NULL) {
          perror("Error while opening input file.\n");
          exit(-1);
      }
    }
 
    if (argc > 2) {
      if (!((strcmp(argv[2], "-") == 0) ||
           (strcmp(argv[2], "--") == 0))) {
        // Try to open the output file name.
        if((serialPort = open(argv[2], O_RDWR)) == -1) {
          // Open failed.  Inform user and quit.
          perror("Error while opening serial port.\n");
          exit(-1);
        }
        useSerialPort = true;
      }
    }

    if (argc > 3) {
      printf("Too many arguments specified.  Only a maximum of 2 may be specified!\n");
      printHelp();
    }
  } 

  while (1) {   // Scan the file until end of file is received.
    // Get a line from the data file
    if (fgets(dataBuffer, DATA_BUFFER_LEN, inf) == NULL) {
      if (feof(inf)) {   // Check for end of file condition 
        break;  // if EOF received break out of the loop.
      }
      else {  // If not EOF, some other error was received.
        perror("Error while reading file.\n");
        exit(-1);  // exit with an error condition.
      }
    }

    // Scan the line looking for keyword.
    if ((buffPtr = strstr(dataBuffer, "dist = ")) != NULL) {
      strncpy(chardist, buffPtr + DIST_OFFSET, DATA_LEN);  // Found the keyword, move it to the output buffer.
      dist = strtof(chardist, NULL);
      continue; // Go back and look for more.
    }

    // Scan the line looking for keyword.
    if ((buffPtr = strstr(dataBuffer, "vel = ")) != NULL) {
      strncpy(charvel, buffPtr + VEL_OFFSET, DATA_LEN);  // Found the keyword, move it to the output buffer.
      vel = strtof(charvel, NULL);
      continue; // Go back and look for more.
    }
  }

  strcat(serialData, "W");
  sprintf(dataBuffer, "%4.2f", dist);
  strcat(serialData, dataBuffer);
  strcat(serialData, ",");
  sprintf(dataBuffer, "%4.2f", vel);
  strcat(serialData, dataBuffer);
  strcat(serialData, ";");

  if (useSerialPort == false) {
    fprintf(otf, "%s\n", serialData);
    return(0);
  }

  // Read in existing settings, and handle any error
  if(tcgetattr(serialPort, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 115200
  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  // Save tty settings, also checking for error
  if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Write to serial port
  write(serialPort, serialData, strlen(serialData));
  close(serialPort);
  return 0; // success
};
