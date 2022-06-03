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
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening \"%s\"\n", device);
        return -1;
    }

    // Flush away any bytes previously read or written.
    int result = tcflush(fd, TCIOFLUSH);
    if (result)
    {
        fprintf(stderr, "tcflush failed"); // just a warning, not a fatal error
    }

    // Get the current configuration of the serial port.
    struct termios options;
    result = tcgetattr(fd, &options);
    if (result)
    {
        perror("tcgetattr failed");
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

    // This code only supports certain standard baud rates. Supporting
    // non-standard baud rates should be possible but takes more work.
    int baud_rate = 115200;
    switch (baud_rate)
    {
    case 4800:
        cfsetospeed(&options, B4800);
        break;
    case 9600:
        cfsetospeed(&options, B9600);
        break;
    case 19200:
        cfsetospeed(&options, B19200);
        break;
    case 38400:
        cfsetospeed(&options, B38400);
        break;
    case 115200:
        cfsetospeed(&options, B115200);
        break;
    default:
        fprintf(stderr, "warning: baud rate %u is not supported, using 9600.\n",
            baud_rate);
        cfsetospeed(&options, B9600);
        break;
    }
    cfsetispeed(&options, cfgetospeed(&options));

    result = tcsetattr(fd, TCSANOW, &options);
    if (result)
    {
        perror("tcsetattr failed");
        close(fd);
        return -1;
    }

    // Tell the downstream device we're ready!
    int flags;
    flags = TIOCM_RTS | TIOCM_DTR;
    ioctl(fd, TIOCMBIS, &flags); // Set RTS pin

    fcntl(fd, F_SETFL, 0);

    serial_fd = fd;
    return fd;
}


//Poll waiting for more
static inline int wait_response(char* response_buff, int len)
{
    if(serial_fd < 0)
    {
        fprintf(stderr, "Serial port is not yet open\n");
    }

    int r = read(serial_fd, response_buff, len);
    for (; r <= 0; r = read(serial_fd, response_buff, len))
    {
        usleep(500);
    }
    return r;
}


#define RESP_MAX 1024
static char response_buf[RESP_MAX] = { 0 };
static char* response = response_buf;
static int response_rem = 0;

//Are there more bytes to consume, return +ve if yes, 0 if no
int serial_peek()
{
    if (serial_fd < 0)
    {
        fprintf(stderr, "Serial port is not yet open\n");
    }

    if(response_rem > 0)
    {
        return response_rem;
    }

    int r = read(serial_fd, response_buf, RESP_MAX);
    if(r <= 0)
    {
        return 0;
    }
    response = response_buf;
    response_rem = r;

    return r;
}



//Buffered getchar from the serial port
int serial_getchar()
{
    if (response_rem <= 0)
    {
        response_rem = wait_response(response_buf, RESP_MAX);
        response = response_buf;
    }

    const char result = response[0];
    response_rem--;
    response++;
    return result;
}

int serial_vsend_str(const char* fmt, va_list args)
{
    int ret = vdprintf(serial_fd, fmt, args);
    fsync(serial_fd);
    return ret;
}