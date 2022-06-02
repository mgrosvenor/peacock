#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>

static int serial_fd = -1; 

int open_serial_port(const char* device)
{
    fprintf(stderr,"Configuring serial port...\n");
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open serial port device \"%s\"\n", device);
        return -1;
    }

    // Flush away any bytes previously read or written.
    int result = tcflush(fd, TCIOFLUSH);
    if (result)
    {
        fprintf(stderr, "Warning: tcflush failed"); // just a warning, not a fatal error
    }

    // Get the current configuration of the serial port.
    struct termios options;
    result = tcgetattr(fd, &options);
    if (result)
    {
        fprintf(stderr, "tcgetattr failed");
        close(fd);
        return -1;
    }

    // Turn off any options that might interfere with our ability to send and
    // receive raw binary bytes.
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_cflag |= CREAD | CLOCAL;

    // Set up timeouts: Calls to read() will return as soon as there is
    // at least one byte available or when 100 ms has passed.
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 0;

    cfsetospeed(&options, B115200);
    
    result = tcsetattr(fd, TCSANOW, &options);
    if (result)
    {
        fprintf(stderr, "tcsetattr failed");
        close(fd);
        return -1;
    }

    // Tell the downstream device we're ready!
    int flags;
    flags = TIOCM_RTS | TIOCM_DTR;
    ioctl(fd, TIOCMBIS, &flags); // Set RTS pin

    if(!fcntl(fd, F_SETFL, 0))
    {
        fprintf(stderr, "Could not set RTS/DTR\n");
        close(fd);
        return -1;
    }

    fprintf(stderr, "Done serial config\n");

    serial_fd = fd;
    return fd;
}


//Poll waiting for more
static inline int wait_response(char* response, int len)
{
    if(serial_fd < 0)
    {
        fprintf(stderr, "Serial port is not yet open\n");
    }

    int r = read(serial_fd, response, len);
    for (; r <= 0; r = read(serial_fd, response, len))
    {
        usleep(500);
    }
    return r;
}


#define RESP_MAX 1024
static char response_buf[RESP_MAX] = { 0 };
static char* response = response_buf;
static int response_rem = 0;

//Buffered getchar from the serial port
char serial_getchar()
{
    if (response_rem <= 0)
    {
        response_rem = wait_response(response, RESP_MAX);
        response = response_buf;
    }

    const char result = response[0];
    response_rem--;
    response++;
    return result;
}

int serial_vsend_str(const char* fmt, va_list args)
{
    return vdprintf(serial_fd, fmt, args);
}