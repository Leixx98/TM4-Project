# Tm4123G Project
This is a repositories for Tm4123G project written in C.
## Now includes:
* Tm4123g communicates with Fpga and manage data transferred from Fpga. Then send managed data into HMI displayer.
*  Tm4123G get voltage from inside AD. And Communicating with HMI diplayer which send command to control TM4 to send command to Fpga to control phase. It's a part of lock-in amplifier. The voltage is the lock-in amplifier's result. The amplifier is used to detect wee signal.
## Note
*  All the project is written with MDK keil5
* Use the project you need a Tm4 and other devices to download into the MCU
* Some project may use the FreeRtos 
 