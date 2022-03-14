// C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#define ARG_MAX_STRING_LEN  6
#define FN_MAX_STRING_LEN   1023
#define SERIAL_DATA_STREAM_LEN  16

void printHelp(void) {
  printf("Didn't work...\n");
}

int main(int argc, char *argv[]) {

  int argIx;  // argument index
  int operandLen; // Operand length temporary storage.
  char waveHeight[ARG_MAX_STRING_LEN + 1];  // string storage for wave heigth
  char wavePeriod[ARG_MAX_STRING_LEN + 1];  // string storage for wave period
  char fn[FN_MAX_STRING_LEN + 1]; // string storage for file name
  char serialDataString[SERIAL_DATA_STREAM_LEN];  // string storage for serial data string.
  struct termios tty; // termios struct, we call it 'tty' for convention
  int serialPort; // file descriptor index. 


// We are expecting at least one argument.
  if (argc == 1) {
    printf("\nError: No arguments specified!!!\n\n");
    printHelp();
    exit(1);
  }

  // Initialize the local variables.
  argIx = 1;
  waveHeight[0] = '\0';
  wavePeriod[0] = '\0';
  fn[0] = '\0';
  serialDataString[0] = '\0';

  // check the arguments.
  while (argIx < argc) {
    if (argv[argIx][0] == '-') {
      switch (argv[argIx][1]) {

        case 'w':
          if ((operandLen = strlen(argv[++argIx])) > ARG_MAX_STRING_LEN) {
            printf("\nError: Wave heigth value too long, should be no more than 6 characters!\n\n");
            printHelp();
            exit (1);
          }

          strcpy(waveHeight, argv[argIx]);
          ++argIx;
          continue;

        case 'p':

          if ((operandLen = strlen(argv[++argIx])) > ARG_MAX_STRING_LEN) {
            printf("\nError: Wave period value too long, should be no more than 6 characters!\n\n");
            printHelp();
            exit (1);
          }

          strcpy(wavePeriod, argv[argIx]);
          ++argIx;
          continue;
       
        case 'f':
          if (strlen(argv[++argIx]) > FN_MAX_STRING_LEN ) {
            printf("\nError: Serial port file name too long.  Must be less than or equal to %i.\n\n", FN_MAX_STRING_LEN );
            printHelp();
            exit(1);
          }

          strcpy(fn, argv[argIx]);
          ++argIx;
          continue;

        case 'h':

          printHelp();
          exit(0);

        default:
          printf("Error: Invalid switch %c!!!\n\n", argv[argIx][1]);
          printHelp();
          exit(1);
      }
    } else {
      if (argv[argIx][0] != '\0') {
        printf("Error: Too many arguments received!!!\n\n");
        printHelp();
        exit(1);
      }
    }
  } 

  if (waveHeight[0] == '\0') {
    printf("Error: Wave heigth argument not received!!!\n\n");
    printHelp();
    exit(1);
  }

  if (wavePeriod[0] == '\0') {
    printf("Error: Wave period argument not received!!!\n\n");
    printHelp();
    exit(1);
  }

  if (fn[0] == '\0') {
    printf("Error: Serial file name not received!!!\n\n");
    printHelp();
    exit(1);
  }

  strcat(serialDataString, "W");
  strcat(serialDataString, waveHeight);
  strcat(serialDataString, ",");
  strcat(serialDataString, wavePeriod);
  strcat(serialDataString, ";");

  // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
  serialPort = open(fn, O_RDWR);

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
sd
  // Save tty settings, also checking for error
  if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Write to serial port
  write(serialPort, serialDataString, strlen(serialDataString));
  close(serialPort);
  return 0; // success
};
