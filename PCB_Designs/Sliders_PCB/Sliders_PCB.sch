EESchema Schematic File Version 2
LIBS:Custom_Switches
LIBS:mcdts2-4n
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:Slide_Potentiometer
LIBS:Sliders_PCB-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X06 P1
U 1 1 588B4D25
P 6400 2300
F 0 "P1" H 6400 2650 50  0000 C CNN
F 1 "CONN_01X06" V 6500 2300 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x06" H 6400 2300 50  0001 C CNN
F 3 "" H 6400 2300 50  0000 C CNN
	1    6400 2300
	1    0    0    -1  
$EndComp
Text Label 6200 2050 2    60   ~ 0
VDD
Text Label 6200 2150 2    60   ~ 0
GND
Text Label 6200 1250 2    60   ~ 0
VDD
$Comp
L C C1
U 1 1 588B503E
P 6200 1400
F 0 "C1" H 6225 1500 50  0000 L CNN
F 1 "100n" H 6225 1300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6238 1250 50  0001 C CNN
F 3 "" H 6200 1400 50  0000 C CNN
	1    6200 1400
	1    0    0    -1  
$EndComp
Text Label 6200 1550 2    60   ~ 0
GND
$Comp
L PWR_FLAG #VDD01
U 1 1 588B50BE
P 6400 1250
F 0 "#VDD01" H 6400 1345 50  0001 C CNN
F 1 "PWR_FLAG" H 6400 1430 50  0000 C CNN
F 2 "" H 6400 1250 50  0000 C CNN
F 3 "" H 6400 1250 50  0000 C CNN
	1    6400 1250
	1    0    0    -1  
$EndComp
Text Label 4250 1000 2    60   ~ 0
VDD
Text Label 5050 1000 0    60   ~ 0
GND
Wire Wire Line
	6200 1550 6400 1550
Wire Wire Line
	6400 1250 6200 1250
Wire Wire Line
	5050 1000 5050 3250
Connection ~ 5050 1800
Connection ~ 5050 2500
Wire Wire Line
	4250 1000 4250 3250
Connection ~ 4250 1800
Connection ~ 4250 2500
Wire Wire Line
	4650 1200 4650 1500
Wire Wire Line
	4650 1500 5850 1500
Wire Wire Line
	5850 1500 5850 2350
Wire Wire Line
	5750 2050 4650 2050
Wire Wire Line
	4650 3500 6000 3500
$Comp
L PWR_FLAG #GND02
U 1 1 588B6CB3
P 6400 1550
F 0 "#GND02" H 6400 1645 50  0001 C CNN
F 1 "PWR_FLAG" H 6400 1730 50  0000 C CNN
F 2 "" H 6400 1550 50  0000 C CNN
F 3 "" H 6400 1550 50  0000 C CNN
	1    6400 1550
	-1   0    0    1   
$EndComp
$Comp
L CP C2
U 1 1 588B6F6B
P 6400 1400
F 0 "C2" H 6425 1500 50  0000 L CNN
F 1 "CP" H 6425 1300 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_4x4.5" H 6438 1250 50  0001 C CNN
F 3 "" H 6400 1400 50  0000 C CNN
	1    6400 1400
	1    0    0    -1  
$EndComp
$Comp
L PTA6043 R1
U 1 1 589238E9
P 4650 1000
F 0 "R1" H 4775 850 60  0000 C CNN
F 1 "PTA6043" H 4650 1150 60  0000 C CNN
F 2 "Potentiometers_Bourne:Potentiometer_Bourns-PTA6043" H 4600 1375 60  0001 C CNN
F 3 "" H 4600 1375 60  0001 C CNN
	1    4650 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 1000 4350 1000
Wire Wire Line
	4950 1000 5050 1000
$Comp
L PTA6043 R2
U 1 1 589239B0
P 4650 1800
F 0 "R2" H 4775 1650 60  0000 C CNN
F 1 "PTA6043" H 4650 1950 60  0000 C CNN
F 2 "Potentiometers_Bourne:Potentiometer_Bourns-PTA6043" H 4600 2175 60  0001 C CNN
F 3 "" H 4600 2175 60  0001 C CNN
	1    4650 1800
	1    0    0    -1  
$EndComp
$Comp
L PTA6043 R4
U 1 1 589239FB
P 4650 3250
F 0 "R4" H 4775 3100 60  0000 C CNN
F 1 "PTA6043" H 4650 3400 60  0000 C CNN
F 2 "Potentiometers_Bourne:Potentiometer_Bourns-PTA6043" H 4600 3625 60  0001 C CNN
F 3 "" H 4600 3625 60  0001 C CNN
	1    4650 3250
	1    0    0    -1  
$EndComp
$Comp
L PTA6043 R3
U 1 1 58923A9D
P 4650 2500
F 0 "R3" H 4775 2350 60  0000 C CNN
F 1 "PTA6043" H 4650 2650 60  0000 C CNN
F 2 "Potentiometers_Bourne:Potentiometer_Bourns-PTA6043" H 4600 2875 60  0001 C CNN
F 3 "" H 4600 2875 60  0001 C CNN
	1    4650 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 1800 4350 1800
Wire Wire Line
	4650 2050 4650 2000
Wire Wire Line
	4950 1800 5050 1800
Wire Wire Line
	4650 2700 4650 2750
Wire Wire Line
	4250 2500 4350 2500
Wire Wire Line
	4950 2500 5050 2500
Wire Wire Line
	4250 3250 4350 3250
Wire Wire Line
	4650 3500 4650 3450
Wire Wire Line
	5050 3250 4950 3250
Wire Wire Line
	5850 2350 6200 2350
Wire Wire Line
	5750 2050 5750 2250
Wire Wire Line
	5750 2250 6200 2250
Wire Wire Line
	4650 2750 5850 2750
Wire Wire Line
	5850 2750 5850 2450
Wire Wire Line
	5850 2450 6200 2450
Wire Wire Line
	6000 3500 6000 2550
Wire Wire Line
	6000 2550 6200 2550
$EndSCHEMATC
