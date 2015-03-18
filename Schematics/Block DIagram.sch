EESchema Schematic File Version 2
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
LIBS:special
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
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 5
Title "Pain Box Block Diagram"
Date "28 apr 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 6350 4250 1200 650 
U 535DC898
F0 "HV Power supply" 50
F1 "PSU.sch" 50
$EndSheet
$Sheet
S 5650 1450 1800 700 
U 535DC8C6
F0 "Audio Amp" 50
F1 "Audio.sch" 50
$EndSheet
$Sheet
S 2650 3650 2050 1300
U 535DC8E7
F0 "Controller" 50
F1 "Arduino + Wave + Proto.sch" 50
$EndSheet
$Sheet
S 8350 2650 1800 900 
U 535DC90E
F0 "Hand electrodes + Sensor" 50
F1 "Electrodes.sch" 50
$EndSheet
$Comp
L ST_TYPE_RELAY K2
U 1 1 535DC977
P 8300 4600
F 0 "K2" V 8700 4850 60  0000 C CNN
F 1 "ST_TYPE_RELAY" V 8550 5150 60  0000 C CNN
F 2 "~" H 8300 4600 60  0000 C CNN
F 3 "~" H 8300 4600 60  0000 C CNN
	1    8300 4600
	0    1    1    0   
$EndComp
Text HLabel 7550 4450 0    60   UnSpc ~ 0
HV +
Text HLabel 7550 4700 0    60   UnSpc ~ 0
HV -
Text HLabel 9750 3550 1    60   UnSpc ~ 0
ELECTRODE
Text HLabel 9950 3550 1    60   UnSpc ~ 0
ELECTRODE
Wire Wire Line
	7550 4450 8050 4450
Wire Wire Line
	7550 4700 8050 4700
Wire Wire Line
	8550 4450 9750 4450
Wire Wire Line
	9750 4450 9750 3550
Wire Wire Line
	8550 4700 9950 4700
Wire Wire Line
	9950 4700 9950 3550
$Comp
L SPEAKER SP1
U 1 1 535DCA14
P 8150 1800
F 0 "SP1" H 8050 2050 70  0000 C CNN
F 1 "SPEAKER" H 8050 1550 70  0000 C CNN
F 2 "~" H 8150 1800 60  0000 C CNN
F 3 "~" H 8150 1800 60  0000 C CNN
	1    8150 1800
	1    0    0    -1  
$EndComp
Text HLabel 7450 1700 0    60   UnSpc ~ 0
SPK_OUT +
Text HLabel 7450 1900 0    60   UnSpc ~ 0
SPK_OUT -
Wire Wire Line
	7850 1700 7450 1700
Wire Wire Line
	7450 1900 7850 1900
Text HLabel 5650 1900 2    60   UnSpc ~ 0
AUD_IN+
Text HLabel 5650 2000 2    60   UnSpc ~ 0
AUD_IN-
Text HLabel 8350 3150 2    60   UnSpc ~ 0
CDS A
Text HLabel 8350 3250 2    60   UnSpc ~ 0
CDS_B
Text HLabel 8350 2750 2    60   UnSpc ~ 0
+5V
Text HLabel 8350 2950 2    60   UnSpc ~ 0
LIGHT_ENABLE
Text HLabel 5650 1550 2    60   UnSpc ~ 0
+12V
Text HLabel 5650 1650 2    60   UnSpc ~ 0
GND
Text HLabel 6350 4350 2    60   UnSpc ~ 0
+12V
Text HLabel 6350 4800 2    60   UnSpc ~ 0
GND
$Comp
L ST_TYPE_RELAY K1
U 1 1 535DCBAF
P 2850 1700
F 0 "K1" V 3250 1950 60  0000 C CNN
F 1 "ST_TYPE_RELAY" V 3100 2250 60  0000 C CNN
F 2 "~" H 2850 1700 60  0000 C CNN
F 3 "~" H 2850 1700 60  0000 C CNN
	1    2850 1700
	0    1    1    0   
$EndComp
Wire Wire Line
	3100 1550 5650 1550
Wire Wire Line
	5200 1550 5200 4350
Wire Wire Line
	5200 4350 6350 4350
Connection ~ 5200 1550
Wire Wire Line
	5650 1650 5250 1650
Wire Wire Line
	5250 1650 5250 4800
Wire Wire Line
	4700 4800 6350 4800
Wire Wire Line
	3400 1550 3400 1800
Wire Wire Line
	3400 1800 3100 1800
Connection ~ 3400 1550
Wire Wire Line
	1400 1550 2600 1550
Wire Wire Line
	2300 1550 2300 1800
Wire Wire Line
	2300 1800 2600 1800
Connection ~ 2300 1550
Text Label 1900 1550 2    60   ~ 0
12V INPUT
Wire Wire Line
	2750 2250 2750 2550
Wire Wire Line
	7950 5350 8200 5350
Wire Wire Line
	8200 5350 8200 5150
$Comp
L DIODE D1
U 1 1 535DCC2E
P 3100 2550
F 0 "D1" H 3100 2650 40  0000 C CNN
F 1 "DIODE" H 3100 2450 40  0000 C CNN
F 2 "~" H 3100 2550 60  0000 C CNN
F 3 "~" H 3100 2550 60  0000 C CNN
	1    3100 2550
	-1   0    0    1   
$EndComp
$Comp
L DIODE D2
U 1 1 535DCC52
P 8300 5650
F 0 "D2" H 8300 5750 40  0000 C CNN
F 1 "DIODE" H 8300 5550 40  0000 C CNN
F 2 "~" H 8300 5650 60  0000 C CNN
F 3 "~" H 8300 5650 60  0000 C CNN
	1    8300 5650
	-1   0    0    1   
$EndComp
Wire Wire Line
	7950 6350 7950 5350
Wire Wire Line
	7950 5650 8100 5650
Wire Wire Line
	8400 5150 8400 5350
Wire Wire Line
	8400 5350 8650 5350
Wire Wire Line
	8650 5350 8650 5950
Wire Wire Line
	8650 5950 2350 5950
Wire Wire Line
	8500 5650 8650 5650
Connection ~ 8650 5650
Wire Wire Line
	2150 2550 2900 2550
Wire Wire Line
	2950 2250 2950 2350
Wire Wire Line
	2950 2350 3450 2350
Wire Wire Line
	3450 2350 3450 3650
Wire Wire Line
	3450 2550 3300 2550
Connection ~ 3450 2550
Text HLabel 4700 4650 0    60   UnSpc ~ 0
+5V
Text HLabel 4700 4800 0    60   UnSpc ~ 0
GND
Connection ~ 5250 4800
Wire Wire Line
	5650 1900 4800 1900
Wire Wire Line
	4800 1900 4800 2250
Wire Wire Line
	4800 2250 4450 2250
Wire Wire Line
	4450 2250 4450 3650
Wire Wire Line
	5650 2000 4900 2000
Wire Wire Line
	4900 2000 4900 2350
Wire Wire Line
	4900 2350 4550 2350
Wire Wire Line
	4550 2350 4550 3650
Wire Wire Line
	8350 2750 6000 2750
Wire Wire Line
	6000 2750 6000 6350
Wire Wire Line
	6000 4650 4700 4650
Text HLabel 4450 3650 3    60   UnSpc ~ 0
AUD_IN+
Text HLabel 4550 3650 3    60   UnSpc ~ 0
AUD_IN-
Text HLabel 4700 4400 0    60   UnSpc ~ 0
CDS A
Text HLabel 4700 4500 0    60   UnSpc ~ 0
CDS_B
Text HLabel 4700 4200 0    60   UnSpc ~ 0
LIGHT_ENABLE
Wire Wire Line
	8350 2950 5850 2950
Wire Wire Line
	5850 2950 5850 4200
Wire Wire Line
	5850 4200 4700 4200
Wire Wire Line
	8350 3150 4900 3150
Wire Wire Line
	4900 3150 4900 4400
Wire Wire Line
	4900 4400 4700 4400
Wire Wire Line
	4700 4500 4950 4500
Wire Wire Line
	4950 4500 4950 3250
Wire Wire Line
	4950 3250 8350 3250
Wire Wire Line
	2150 2550 2150 6350
Wire Wire Line
	2150 6350 7950 6350
Connection ~ 7950 5650
Connection ~ 2750 2550
Connection ~ 6000 6350
Connection ~ 6000 4650
Text HLabel 3450 3650 3    60   Input ~ 0
12V_Enable
Text HLabel 3650 3650 3    60   Input ~ 0
PSU_CONNECT
Wire Wire Line
	2350 5950 2350 2800
Wire Wire Line
	2350 2800 3650 2800
Wire Wire Line
	3650 2800 3650 3650
$EndSCHEMATC
