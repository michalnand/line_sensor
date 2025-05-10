# i2c sensor for line follower

![image](doc/images/top_3d.png)
![image](doc/images/top.png)
![image](doc/images/bottom.png)
![image](doc/images/board.png)


# usage

## compiling firmware

- arm-none-eabi-gcc is required

```bash
cd firmware
make clean
make
```

## writing into flash

- use st-link for write firmware
- connect gnd, swclk, swdio and reset to debug connector
- execture **st-flash --connect-under-reset write build.bin 0x8000000**

```bash
cd firmware/bin
./arm_burn
```