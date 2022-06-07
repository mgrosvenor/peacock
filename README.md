# peacock

A Raspberry Pi Pico IO Library.
The library implements RPC over USB (serial) to control I/O functions on the device.
Most of the commands are similar or nearly identical to the Pico SDK.

This is a C library which makes it easy to integrate into other projects.

## building

Peacock uses cmake. The usual process applies

```bash
mkdir build
cd build
cmake ..
make
```

## API

### General

`int pck_init(const char* dev)`

Initialize the library.
`dev` is a string path to the USB tty device which the device presents.
Return 0 on success.

`void pck_close()`

Close the connection to the device.

### GPIO Functions

`int pck_gpio_in(const int pin)`

Fetch input from a given GPIO pin number.
Return -1 on error, 0 for low, 1 for high.

`int pck_gpio_out(const int pin, const int val)`

Set output on GPIO pin to given value.
Valid values are:

- 0 to drive low
- 1 to drive high
- -1 for Hi-Z

Return 0 on success.

`int pck_gpio_pull(int pin, bool up, bool dwn)`

The the I/O pin pull up/down values.
Return 0 on success.

`int pck_gpio_pin_func(const int pin, const char func)`

Set the given pin function mode:

- 'g' for GPIO mode
- 'p' for PWM mode

Return 0 on success.

### PWM Functions

`int pck_pwm_slice_channel_num(const int pin, int* slice_num, int* channel_num)`

Get the slice number and channel number of the given pin. 
Return 0 on success.

`int pck_pwm_config(const int slice_num, const char mode, const int div_int, const int div_frc, const int wrap, bool phase_correct)`

Configure the given PWM slice.
Mode is clock counter mode: 

- 'F' for Free running mode
- 'h' for high on channel B
- 'r' for rising edge on channel B
- 'f' for falling edge on channel B

Other values are as described in the Pico SDK.
Return 0 on success. 

`int pck_pwm_level(const int pin, const int level)`

Set the PWM level (effectively duty cycle as a fraction of wrap) on the given pin.
Return 0 on success.

`int pck_pwm_enable(const int slice, bool enabled)`

Enable or disable PWM generation on the given slice number.
Return 0 on success.

`int pck_pwm_get_counter(const int slice, pwm_count_t* count)`

Get the currnet PWM counter value and the time at which it was fetched.
Return 0 on success.

```C
typedef struct
{
    int count;
    int ts_lo;
    int ts_hi;
} pwm_count_t;
```

Time is broken into high and low components so the full 64bit value can come across.

Use the following to reassemble.

```C
const uint64_t ts = ((uint64_t)ts_hi << 32) + ts_low; 
```

### Util Functions

`int pck_util_sleep(int us)`

Cause the device to sleep for the given number of microseconds.
Return 0 on success.

## License

See LICENSE for full details. TL;DR - BSD 3-clause.
