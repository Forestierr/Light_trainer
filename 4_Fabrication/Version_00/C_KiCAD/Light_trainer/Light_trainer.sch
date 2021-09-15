EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Light Trainer"
Date "2020-12-09"
Rev ""
Comp ""
Comment1 "©2021 Robin Forestier"
Comment2 ""
Comment3 "V_00"
Comment4 "Robin Forestier"
$EndDescr
$Comp
L Light_trainer-rescue:SW_Push-Switch SW1
U 1 1 5FD2E185
P 1600 4000
F 0 "SW1" H 1600 4285 50  0000 C CNN
F 1 "SW_Push" H 1600 4194 50  0000 C CNN
F 2 "Work_Forestier:SW_Cherry_MX_1.00u_PCB_3D" H 1600 4200 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1600 4200 50  0001 C CNN
F 4 "Digikey" H 1600 4000 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1600 4000 50  0001 C CNN "Prix total"
F 6 "1,32" H 1600 4000 50  0001 C CNN "prix unité"
	1    1600 4000
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SW_Push-Switch SW2
U 1 1 5FD62CD6
P 1600 4350
F 0 "SW2" H 1600 4635 50  0000 C CNN
F 1 "SW_Push" H 1600 4544 50  0000 C CNN
F 2 "Work_Forestier:SW_Cherry_MX_1.00u_PCB_3D" H 1600 4550 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1600 4550 50  0001 C CNN
F 4 "Digikey" H 1600 4350 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1600 4350 50  0001 C CNN "Prix total"
F 6 "1,32" H 1600 4350 50  0001 C CNN "prix unité"
	1    1600 4350
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SW_Push-Switch SW3
U 1 1 5FD65B5A
P 1600 4700
F 0 "SW3" H 1600 4985 50  0000 C CNN
F 1 "SW_Push" H 1600 4894 50  0000 C CNN
F 2 "Work_Forestier:SW_Cherry_MX_1.00u_PCB_3D" H 1600 4900 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1600 4900 50  0001 C CNN
F 4 "Digikey" H 1600 4700 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1600 4700 50  0001 C CNN "Prix total"
F 6 "1,32" H 1600 4700 50  0001 C CNN "prix unité"
	1    1600 4700
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SW_Push-Switch SW4
U 1 1 5FD68A79
P 1600 5050
F 0 "SW4" H 1600 5335 50  0000 C CNN
F 1 "SW_Push" H 1600 5244 50  0000 C CNN
F 2 "Work_Forestier:SW_Cherry_MX_1.00u_PCB_3D" H 1600 5250 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1600 5250 50  0001 C CNN
F 4 "Digikey" H 1600 5050 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1600 5050 50  0001 C CNN "Prix total"
F 6 "1,32" H 1600 5050 50  0001 C CNN "prix unité"
	1    1600 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 3900 1300 4000
Wire Wire Line
	1300 4000 1400 4000
Wire Wire Line
	1300 4000 1300 4350
Wire Wire Line
	1300 4350 1400 4350
Connection ~ 1300 4000
Wire Wire Line
	1300 4350 1300 4700
Wire Wire Line
	1300 4700 1400 4700
Connection ~ 1300 4350
Wire Wire Line
	1300 4700 1300 5050
Wire Wire Line
	1300 5050 1400 5050
Connection ~ 1300 4700
$Comp
L Light_trainer-rescue:R-Device R1
U 1 1 5FD7C917
P 1950 5300
F 0 "R1" H 2020 5346 50  0000 L CNN
F 1 "10k" H 2020 5255 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 1880 5300 50  0001 C CNN
F 3 "~" H 1950 5300 50  0001 C CNN
F 4 "Digikey" H 1950 5300 50  0001 C CNN "Fournisseur"
	1    1950 5300
	-1   0    0    1   
$EndComp
Wire Wire Line
	1950 5050 1950 5150
Wire Wire Line
	1950 5050 1800 5050
Wire Wire Line
	1950 5050 1950 4700
Wire Wire Line
	1950 4700 1800 4700
Connection ~ 1950 5050
Wire Wire Line
	1950 4700 1950 4350
Wire Wire Line
	1950 4350 1800 4350
Connection ~ 1950 4700
Wire Wire Line
	1800 4000 1950 4000
Wire Wire Line
	1950 4000 1950 4350
Connection ~ 1950 4350
Text GLabel 2150 4000 2    50   Output ~ 0
switch
Wire Wire Line
	1950 4000 2150 4000
Connection ~ 1950 4000
Wire Notes Line
	1000 5900 2550 5900
Wire Notes Line
	2550 5900 2550 3500
Wire Notes Line
	2550 3500 1000 3500
Wire Notes Line
	1000 3500 1000 5900
Text Notes 1000 3450 0    50   ~ 0
Switch
Wire Wire Line
	1950 5500 1950 5450
$Comp
L Light_trainer-rescue:+3.3V-power #PWR01
U 1 1 6047DC4D
P 1300 3900
F 0 "#PWR01" H 1300 3750 50  0001 C CNN
F 1 "+3.3V" H 1315 4073 50  0000 C CNN
F 2 "" H 1300 3900 50  0001 C CNN
F 3 "" H 1300 3900 50  0001 C CNN
	1    1300 3900
	1    0    0    -1  
$EndComp
Text Notes 4400 1050 0    236  ~ 47
LightTrainer V00
$Comp
L Light_trainer-rescue:SK6812-LED D1
U 1 1 5FDB63B9
P 1800 6750
F 0 "D1" H 1550 7100 50  0000 L CNN
F 1 "SK6812" H 1450 7000 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 1850 6450 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 1900 6375 50  0001 L TNN
F 4 "Digikey" H 1800 6750 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 1800 6750 50  0001 C CNN "Prix total"
F 6 "4,1" H 1800 6750 50  0001 C CNN "prix unité"
	1    1800 6750
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D2
U 1 1 5FEF28E1
P 2450 6750
F 0 "D2" H 2200 7100 50  0000 L CNN
F 1 "SK6812" H 2100 7000 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 2500 6450 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 2550 6375 50  0001 L TNN
F 4 "Digikey" H 2450 6750 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 2450 6750 50  0001 C CNN "Prix total"
F 6 "4,1" H 2450 6750 50  0001 C CNN "prix unité"
	1    2450 6750
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D3
U 1 1 5FF375C6
P 3100 6750
F 0 "D3" H 2850 7100 50  0000 L CNN
F 1 "SK6812" H 2750 7000 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 3150 6450 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 3200 6375 50  0001 L TNN
F 4 "Digikey" H 3100 6750 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 3100 6750 50  0001 C CNN "Prix total"
F 6 "4,1" H 3100 6750 50  0001 C CNN "prix unité"
	1    3100 6750
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D4
U 1 1 5FF375CF
P 3750 6750
F 0 "D4" H 3500 7100 50  0000 L CNN
F 1 "SK6812" H 3400 7000 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 3800 6450 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 3850 6375 50  0001 L TNN
F 4 "Digikey" H 3750 6750 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 3750 6750 50  0001 C CNN "Prix total"
F 6 "4,1" H 3750 6750 50  0001 C CNN "prix unité"
	1    3750 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 6750 2150 6750
Wire Wire Line
	2750 6750 2800 6750
Wire Wire Line
	3400 6750 3450 6750
Wire Wire Line
	4050 6750 4100 6750
Wire Wire Line
	1800 7100 1800 7050
Wire Wire Line
	2450 7100 2450 7050
Wire Wire Line
	3100 7100 3100 7050
Wire Wire Line
	3750 7100 3750 7050
Wire Wire Line
	1800 6400 1800 6450
Wire Wire Line
	2450 6400 2450 6450
Wire Wire Line
	3750 6400 3750 6450
Wire Wire Line
	3100 6400 3100 6450
Text GLabel 1250 6750 0    50   Input ~ 0
LED
Text Notes 1000 6100 0    50   ~ 0
LED néo-pixel
$Comp
L Light_trainer-rescue:GND-power #PWR014
U 1 1 603F42F0
P 4400 7100
F 0 "#PWR014" H 4400 6850 50  0001 C CNN
F 1 "GND" H 4405 6927 50  0000 C CNN
F 2 "" H 4400 7100 50  0001 C CNN
F 3 "" H 4400 7100 50  0001 C CNN
	1    4400 7100
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR03
U 1 1 6043A369
P 1800 7100
F 0 "#PWR03" H 1800 6850 50  0001 C CNN
F 1 "GND" H 1805 6927 50  0000 C CNN
F 2 "" H 1800 7100 50  0001 C CNN
F 3 "" H 1800 7100 50  0001 C CNN
	1    1800 7100
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR07
U 1 1 6044BBB6
P 2450 7100
F 0 "#PWR07" H 2450 6850 50  0001 C CNN
F 1 "GND" H 2455 6927 50  0000 C CNN
F 2 "" H 2450 7100 50  0001 C CNN
F 3 "" H 2450 7100 50  0001 C CNN
	1    2450 7100
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR010
U 1 1 6045D3E7
P 3100 7100
F 0 "#PWR010" H 3100 6850 50  0001 C CNN
F 1 "GND" H 3105 6927 50  0000 C CNN
F 2 "" H 3100 7100 50  0001 C CNN
F 3 "" H 3100 7100 50  0001 C CNN
	1    3100 7100
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR012
U 1 1 6046ECE6
P 3750 7100
F 0 "#PWR012" H 3750 6850 50  0001 C CNN
F 1 "GND" H 3755 6927 50  0000 C CNN
F 2 "" H 3750 7100 50  0001 C CNN
F 3 "" H 3750 7100 50  0001 C CNN
	1    3750 7100
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR04
U 1 1 60491CDF
P 1950 5500
F 0 "#PWR04" H 1950 5250 50  0001 C CNN
F 1 "GND" H 1955 5327 50  0000 C CNN
F 2 "" H 1950 5500 50  0001 C CNN
F 3 "" H 1950 5500 50  0001 C CNN
	1    1950 5500
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR02
U 1 1 604E998D
P 1800 6400
F 0 "#PWR02" H 1800 6250 50  0001 C CNN
F 1 "+5V" H 1950 6450 50  0000 C CNN
F 2 "" H 1800 6400 50  0001 C CNN
F 3 "" H 1800 6400 50  0001 C CNN
	1    1800 6400
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR06
U 1 1 604FB129
P 2450 6400
F 0 "#PWR06" H 2450 6250 50  0001 C CNN
F 1 "+5V" H 2600 6450 50  0000 C CNN
F 2 "" H 2450 6400 50  0001 C CNN
F 3 "" H 2450 6400 50  0001 C CNN
	1    2450 6400
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR09
U 1 1 6050C9D7
P 3100 6400
F 0 "#PWR09" H 3100 6250 50  0001 C CNN
F 1 "+5V" H 3250 6450 50  0000 C CNN
F 2 "" H 3100 6400 50  0001 C CNN
F 3 "" H 3100 6400 50  0001 C CNN
	1    3100 6400
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR011
U 1 1 6051E1D9
P 3750 6400
F 0 "#PWR011" H 3750 6250 50  0001 C CNN
F 1 "+5V" H 3900 6450 50  0000 C CNN
F 2 "" H 3750 6400 50  0001 C CNN
F 3 "" H 3750 6400 50  0001 C CNN
	1    3750 6400
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:TestPoint-Connector TP4
U 1 1 606B57C6
P 1950 3850
F 0 "TP4" H 2008 3968 50  0000 L CNN
F 1 "TestPoint" H 2008 3877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 2150 3850 50  0001 C CNN
F 3 "~" H 2150 3850 50  0001 C CNN
F 4 "~" H 1950 3850 50  0001 C CNN "Fournisseur"
F 5 "~" H 1950 3850 50  0001 C CNN "Prix total"
F 6 " ~" H 1950 3850 50  0001 C CNN "prix unité"
	1    1950 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3850 1950 4000
$Comp
L Light_trainer-rescue:TestPoint-Connector TP1
U 1 1 6070C247
P 1400 6700
F 0 "TP1" H 1050 7000 50  0000 L CNN
F 1 "TestPoint" H 1050 6900 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 1600 6700 50  0001 C CNN
F 3 "~" H 1600 6700 50  0001 C CNN
F 4 "~" H 1400 6700 50  0001 C CNN "Fournisseur"
F 5 "~" H 1400 6700 50  0001 C CNN "Prix total"
F 6 " ~" H 1400 6700 50  0001 C CNN "prix unité"
	1    1400 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 6750 1400 6750
Wire Wire Line
	1400 6750 1400 6700
Connection ~ 1400 6750
Wire Wire Line
	1400 6750 1500 6750
$Comp
L Light_trainer-rescue:ESP32-DevKitC-32D-Work_Forestier U2
U 1 1 6148669D
P 5700 2900
F 0 "U2" H 5700 4065 50  0000 C CNN
F 1 "ESP32-DevKitC-32D" H 5700 3974 50  0000 C CNN
F 2 "Work_Forestier:ESP32-DevKitC-32D" H 5350 4100 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Espressif%20PDFs/ESP32-DevKitC_GSG_Ver1.4_2017.pdf" H 5350 4100 50  0001 C CNN
	1    5700 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT1
U 1 1 613C716D
P 1000 2450
F 0 "BT1" H 1108 2496 50  0000 L CNN
F 1 "Battery" H 1108 2405 50  0000 L CNN
F 2 "Work_Forestier:BatteryHolder_Keystone_2464_3xAA" V 1000 2510 50  0001 C CNN
F 3 "~" V 1000 2510 50  0001 C CNN
	1    1000 2450
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Switching:LTC3525-5 U1
U 1 1 613E4BD4
P 2800 2300
F 0 "U1" H 2900 2650 50  0000 C CNN
F 1 "LTC3525-5" H 3050 2550 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-363_SC-70-6" H 2850 2050 50  0001 L CNN
F 3 "https://www.analog.com/media/en/technical-documentation/data-sheets/3525fc.pdf" H 2800 2300 50  0001 C CNN
	1    2800 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 2300 2400 2300
$Comp
L Device:L L1
U 1 1 613E7FA8
P 3100 1800
F 0 "L1" V 3290 1800 50  0000 C CNN
F 1 "10uH" V 3199 1800 50  0000 C CNN
F 2 "Inductor_SMD:L_1210_3225Metric" H 3100 1800 50  0001 C CNN
F 3 "~" H 3100 1800 50  0001 C CNN
	1    3100 1800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3350 1800 3350 2200
Wire Wire Line
	3350 2200 3200 2200
$Comp
L Device:C C2
U 1 1 613EB1F1
P 3400 2600
F 0 "C2" H 3515 2646 50  0000 L CNN
F 1 "10uF" H 3515 2555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3438 2450 50  0001 C CNN
F 3 "~" H 3400 2600 50  0001 C CNN
	1    3400 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 2400 3400 2450
Wire Wire Line
	3200 2400 3400 2400
Wire Wire Line
	3400 2400 3500 2400
Connection ~ 3400 2400
Wire Wire Line
	2800 2600 2800 2800
Wire Wire Line
	2800 2800 3400 2800
Wire Wire Line
	3400 2800 3400 2750
$Comp
L Device:C C1
U 1 1 613F1363
P 2300 2600
F 0 "C1" H 2415 2646 50  0000 L CNN
F 1 "1uF" H 2415 2555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2338 2450 50  0001 C CNN
F 3 "~" H 2300 2600 50  0001 C CNN
	1    2300 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2800 2300 2800
Wire Wire Line
	2300 2800 2300 2750
Connection ~ 2800 2800
Wire Wire Line
	2300 2450 2300 2300
Wire Wire Line
	2800 1800 2950 1800
Wire Wire Line
	3250 1800 3350 1800
$Comp
L Light_trainer-rescue:+5V-power #PWR013
U 1 1 60552CBF
P 4400 6400
F 0 "#PWR013" H 4400 6250 50  0001 C CNN
F 1 "+5V" H 4550 6450 50  0000 C CNN
F 2 "" H 4400 6400 50  0001 C CNN
F 3 "" H 4400 6400 50  0001 C CNN
	1    4400 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 6700 4750 6750
$Comp
L Light_trainer-rescue:TestPoint-Connector TP5
U 1 1 60377721
P 4750 6700
F 0 "TP5" H 4808 6818 50  0000 L CNN
F 1 "TestPoint" H 4808 6727 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 4950 6700 50  0001 C CNN
F 3 "~" H 4950 6700 50  0001 C CNN
F 4 "~" H 4750 6700 50  0001 C CNN "Fournisseur"
F 5 "~" H 4750 6700 50  0001 C CNN "Prix total"
F 6 " ~" H 4750 6700 50  0001 C CNN "prix unité"
	1    4750 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 6400 4400 6450
Wire Wire Line
	4400 7100 4400 7050
Wire Wire Line
	4750 6750 4700 6750
$Comp
L Light_trainer-rescue:SK6812-LED D5
U 1 1 5FF5112C
P 4400 6750
F 0 "D5" H 4150 7100 50  0000 L CNN
F 1 "SK6812" H 4050 7000 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 4450 6450 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 4500 6375 50  0001 L TNN
F 4 "Digikey" H 4400 6750 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 4400 6750 50  0001 C CNN "Prix total"
F 6 "4,1" H 4400 6750 50  0001 C CNN "prix unité"
	1    4400 6750
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR08
U 1 1 61414884
P 3500 2350
F 0 "#PWR08" H 3500 2200 50  0001 C CNN
F 1 "+5V" H 3650 2400 50  0000 C CNN
F 2 "" H 3500 2350 50  0001 C CNN
F 3 "" H 3500 2350 50  0001 C CNN
	1    3500 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 2350 3500 2400
$Comp
L Light_trainer-rescue:GND-power #PWR05
U 1 1 61415E58
P 2800 2850
F 0 "#PWR05" H 2800 2600 50  0001 C CNN
F 1 "GND" H 2805 2677 50  0000 C CNN
F 2 "" H 2800 2850 50  0001 C CNN
F 3 "" H 2800 2850 50  0001 C CNN
	1    2800 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2850 2800 2800
Wire Notes Line
	3800 1500 3800 3150
Wire Notes Line
	800  1500 800  3150
$Comp
L Light_trainer-rescue:+5V-power #PWR017
U 1 1 61421D85
P 4900 3700
F 0 "#PWR017" H 4900 3550 50  0001 C CNN
F 1 "+5V" H 5050 3750 50  0000 C CNN
F 2 "" H 4900 3700 50  0001 C CNN
F 3 "" H 4900 3700 50  0001 C CNN
	1    4900 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3700 4900 3750
Wire Wire Line
	5350 3750 4900 3750
$Comp
L Light_trainer-rescue:GND-power #PWR021
U 1 1 61424DCF
P 6350 2000
F 0 "#PWR021" H 6350 1750 50  0001 C CNN
F 1 "GND" H 6355 1827 50  0000 C CNN
F 2 "" H 6350 2000 50  0001 C CNN
F 3 "" H 6350 2000 50  0001 C CNN
	1    6350 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 2000 6350 1950
Wire Wire Line
	6050 1950 6350 1950
$Comp
L Light_trainer-rescue:GND-power #PWR022
U 1 1 61427B71
P 6350 2600
F 0 "#PWR022" H 6350 2350 50  0001 C CNN
F 1 "GND" H 6355 2427 50  0000 C CNN
F 2 "" H 6350 2600 50  0001 C CNN
F 3 "" H 6350 2600 50  0001 C CNN
	1    6350 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 2600 6350 2550
Wire Wire Line
	6050 2550 6350 2550
$Comp
L Light_trainer-rescue:GND-power #PWR016
U 1 1 6142C078
P 4900 3300
F 0 "#PWR016" H 4900 3050 50  0001 C CNN
F 1 "GND" H 4905 3127 50  0000 C CNN
F 2 "" H 4900 3300 50  0001 C CNN
F 3 "" H 4900 3300 50  0001 C CNN
	1    4900 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3300 4900 3250
Wire Wire Line
	5350 3250 4900 3250
$Comp
L Connector:TestPoint TP6
U 1 1 6142F575
P 4800 4850
F 0 "TP6" H 4858 4968 50  0000 L CNN
F 1 "TestPoint" H 4858 4877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 5000 4850 50  0001 C CNN
F 3 "~" H 5000 4850 50  0001 C CNN
F 4 "~" H 4800 4850 50  0001 C CNN "Fournisseur"
F 5 "~" H 4800 4850 50  0001 C CNN "Prix total"
F 6 "~" H 4800 4850 50  0001 C CNN "prix unité"
	1    4800 4850
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR015
U 1 1 61430156
P 4800 4900
F 0 "#PWR015" H 4800 4650 50  0001 C CNN
F 1 "GND" H 4805 4727 50  0000 C CNN
F 2 "" H 4800 4900 50  0001 C CNN
F 3 "" H 4800 4900 50  0001 C CNN
	1    4800 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 4900 4800 4850
$Comp
L Connector:TestPoint TP7
U 1 1 61436086
P 5350 4850
F 0 "TP7" H 5408 4968 50  0000 L CNN
F 1 "TestPoint" H 5408 4877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 5550 4850 50  0001 C CNN
F 3 "~" H 5550 4850 50  0001 C CNN
F 4 "~" H 5350 4850 50  0001 C CNN "Fournisseur"
F 5 "~" H 5350 4850 50  0001 C CNN "Prix total"
F 6 "~" H 5350 4850 50  0001 C CNN "prix unité"
	1    5350 4850
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR019
U 1 1 61439438
P 5350 4950
F 0 "#PWR019" H 5350 4800 50  0001 C CNN
F 1 "+5V" H 5350 5100 50  0000 C CNN
F 2 "" H 5350 4950 50  0001 C CNN
F 3 "" H 5350 4950 50  0001 C CNN
	1    5350 4950
	-1   0    0    1   
$EndComp
Wire Wire Line
	5350 4850 5350 4950
$Comp
L Connector:TestPoint TP2
U 1 1 6143AF5D
P 2300 1900
F 0 "TP2" H 2358 2018 50  0000 L CNN
F 1 "TestPoint" H 2358 1927 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 2500 1900 50  0001 C CNN
F 3 "~" H 2500 1900 50  0001 C CNN
F 4 "~" H 2300 1900 50  0001 C CNN "Fournisseur"
F 5 "~" H 2300 1900 50  0001 C CNN "Prix total"
F 6 "~" H 2300 1900 50  0001 C CNN "prix unité"
	1    2300 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 6143D663
P 2300 2850
F 0 "TP3" H 2100 3000 50  0000 L CNN
F 1 "TestPoint" H 1900 2900 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 2500 2850 50  0001 C CNN
F 3 "~" H 2500 2850 50  0001 C CNN
F 4 "~" H 2300 2850 50  0001 C CNN "Fournisseur"
F 5 "~" H 2300 2850 50  0001 C CNN "Prix total"
F 6 "~" H 2300 2850 50  0001 C CNN "prix unité"
	1    2300 2850
	-1   0    0    1   
$EndComp
Wire Wire Line
	2300 2800 2300 2850
Wire Wire Line
	6050 2950 6250 2950
Wire Wire Line
	6050 3050 6250 3050
Wire Wire Line
	5100 1850 5100 1950
$Comp
L Light_trainer-rescue:+3.3V-power #PWR018
U 1 1 6144B410
P 5100 1850
F 0 "#PWR018" H 5100 1700 50  0001 C CNN
F 1 "+3.3V" H 5115 2023 50  0000 C CNN
F 2 "" H 5100 1850 50  0001 C CNN
F 3 "" H 5100 1850 50  0001 C CNN
	1    5100 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 1950 5100 1950
Text GLabel 6250 3050 2    50   Output ~ 0
LED
Text GLabel 6250 2950 2    50   Input ~ 0
switch
Wire Notes Line
	1000 6150 1000 7500
Wire Notes Line
	5300 6150 5300 7500
Wire Notes Line
	1000 7500 5300 7500
Wire Notes Line
	1000 6150 5300 6150
Wire Notes Line
	6850 1500 6850 4050
Wire Notes Line
	6850 4050 4450 4050
Wire Notes Line
	4450 4050 4450 1500
Wire Notes Line
	4450 1500 6850 1500
NoConn ~ 5350 3650
NoConn ~ 5350 3550
NoConn ~ 5350 3450
NoConn ~ 5350 3350
NoConn ~ 5350 3150
NoConn ~ 5350 3050
NoConn ~ 5350 2950
NoConn ~ 5350 2850
NoConn ~ 5350 2750
NoConn ~ 5350 2650
NoConn ~ 5350 2550
NoConn ~ 5350 2450
NoConn ~ 5350 2350
NoConn ~ 5350 2250
NoConn ~ 5350 2150
NoConn ~ 5350 2050
NoConn ~ 6050 2050
NoConn ~ 6050 2150
NoConn ~ 6050 2250
NoConn ~ 6050 2350
NoConn ~ 6050 2450
NoConn ~ 6050 2650
NoConn ~ 6050 2750
NoConn ~ 6050 2850
NoConn ~ 6050 3150
NoConn ~ 6050 3250
NoConn ~ 6050 3350
NoConn ~ 6050 3450
NoConn ~ 6050 3550
NoConn ~ 6050 3650
NoConn ~ 6050 3750
Text Notes 4450 1450 0    50   ~ 0
ESP DEVKIT C\n
Text Notes 1500 1450 0    50   ~ 0
Alimentation 3 pile AA avec régulateur 5V\n
Text Notes 4500 4550 0    50   ~ 0
Test point GND/5V\n
$Comp
L Connector:TestPoint TP8
U 1 1 614C7562
P 5900 4850
F 0 "TP8" H 5958 4968 50  0000 L CNN
F 1 "TestPoint" H 5958 4877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 6100 4850 50  0001 C CNN
F 3 "~" H 6100 4850 50  0001 C CNN
F 4 "~" H 5900 4850 50  0001 C CNN "Fournisseur"
F 5 "~" H 5900 4850 50  0001 C CNN "Prix total"
F 6 "~" H 5900 4850 50  0001 C CNN "prix unité"
	1    5900 4850
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+3.3V-power #PWR020
U 1 1 614C9C11
P 5900 4950
F 0 "#PWR020" H 5900 4800 50  0001 C CNN
F 1 "+3.3V" H 5900 5100 50  0000 C CNN
F 2 "" H 5900 4950 50  0001 C CNN
F 3 "" H 5900 4950 50  0001 C CNN
	1    5900 4950
	-1   0    0    1   
$EndComp
Wire Wire Line
	5900 4850 5900 4950
Wire Notes Line
	4500 4600 6400 4600
Wire Notes Line
	6400 5250 6400 4600
Wire Notes Line
	4500 4600 4500 5250
Wire Notes Line
	4500 5250 6400 5250
$Comp
L Device:Jumper JP1
U 1 1 614DCE52
P 1900 1950
F 0 "JP1" H 1900 2214 50  0000 C CNN
F 1 "Jumper" H 1900 2123 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_Pad1.0x1.5mm" H 1900 1950 50  0001 C CNN
F 3 "~" H 1900 1950 50  0001 C CNN
F 4 "~" H 1900 1950 50  0001 C CNN "Fournisseur"
F 5 "~" H 1900 1950 50  0001 C CNN "Prix total"
F 6 "~" H 1900 1950 50  0001 C CNN "prix unité"
	1    1900 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 1800 2800 1950
Wire Wire Line
	2300 2300 2300 1950
Connection ~ 2300 2300
Wire Wire Line
	2300 1950 2300 1900
Connection ~ 2300 1950
Wire Wire Line
	2300 1950 2800 1950
Connection ~ 2800 1950
Wire Wire Line
	2800 1950 2800 2000
Connection ~ 2300 2800
$Comp
L Switch:SW_SPDT SW5
U 1 1 61410395
P 1300 2050
F 0 "SW5" H 1300 2335 50  0000 C CNN
F 1 "SW_SPDT" H 1300 2244 50  0000 C CNN
F 2 "Work_Forestier:C&K-1101M2S5CQE2" H 1300 2050 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1429/1000.pdf" H 1300 2050 50  0001 C CNN
	1    1300 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2800 2300 2800
Wire Wire Line
	1000 2050 1100 2050
NoConn ~ 1500 2150
Wire Notes Line
	800  1500 3800 1500
Wire Notes Line
	800  3150 3800 3150
Wire Wire Line
	2200 1950 2300 1950
Wire Wire Line
	1600 1950 1500 1950
Wire Wire Line
	1000 2650 1000 2800
Wire Wire Line
	1000 2250 1000 2050
$Comp
L Mechanical:MountingHole H1
U 1 1 6141B071
P 7000 4750
F 0 "H1" H 7100 4796 50  0000 L CNN
F 1 "MountingHole" H 7100 4705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 7000 4750 50  0001 C CNN
F 3 "~" H 7000 4750 50  0001 C CNN
	1    7000 4750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 6141E6D1
P 7800 4750
F 0 "H2" H 7900 4796 50  0000 L CNN
F 1 "MountingHole" H 7900 4705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 7800 4750 50  0001 C CNN
F 3 "~" H 7800 4750 50  0001 C CNN
	1    7800 4750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 6141E94E
P 7800 5150
F 0 "H4" H 7900 5196 50  0000 L CNN
F 1 "MountingHole" H 7900 5105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 7800 5150 50  0001 C CNN
F 3 "~" H 7800 5150 50  0001 C CNN
	1    7800 5150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 6141EBEB
P 7000 5150
F 0 "H3" H 7100 5196 50  0000 L CNN
F 1 "MountingHole" H 7100 5105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 7000 5150 50  0001 C CNN
F 3 "~" H 7000 5150 50  0001 C CNN
	1    7000 5150
	1    0    0    -1  
$EndComp
Wire Notes Line
	6800 5400 8550 5400
Wire Notes Line
	8550 5400 8550 4550
Wire Notes Line
	8550 4550 6800 4550
Wire Notes Line
	6800 4550 6800 5400
Text Notes 6800 4500 0    50   ~ 0
Trous de fixation\n
$EndSCHEMATC
