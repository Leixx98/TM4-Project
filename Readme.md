# Tm4123G Project
This is a repositories for Tm4123G project written in C.
## Now includes:
* Tm4123g communicates with Fpga and manage data transferred from Fpga. Then send managed data into HMI displayer.
*  Tm4123G get voltage from inside AD. And Communicating with HMI diplayer which send command to control TM4 to send command to Fpga to control phase. It's a part of lock-in amplifier. The voltage is the lock-in amplifier's result. The amplifier is used to detect wee signal.
## Note
*  All the project is written with MDK keil5
* Use the project you need a Tm4 and other devices to download into the MCU
* Some project may use the FreeRtos 

## ThethirdContest:
* It's a project for the third contest of njupt

## TM4内部AD采样：
### It's a project for the third contest of njupt
### Function:
* It uses TM4123G inside AD to get the voltage of the DC
* It transform the DC voltage to the micro signal
* It send the data of phase to change the FPGA controling the phase

## 第四次积分赛：
### It's a project for the forth contest of njupt
### The First Commit Function:
* Communicating with the HMI by UART to change the amplifier and frequency value
* Sending the amplifier and frequency value into the DDS AD9959 to change the signal
* Showing the amplifier and frequency value in the screen
### The Final Commit:
* It includes two commits,the first is presented by simulation and the second is presented by FPGA
* Communicating with the HMI by UART to change the amplifier and frequency value
* Sending the amplifier and frequency value into the DDS AD9959 to change the signal
* Showing the  amplitude-frequency characteristic and phase-frequency characteristic value in the screen
* Draw the amplitude-frequency characteristic and phase-frequency characteristic in the screen with the frequency bewteen the 10k to 400k





 