EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Light Trainer"
Date "2022-06-14"
Rev ""
Comp ""
Comment1 "©2022 Robin Forestier"
Comment2 ""
Comment3 "V_01"
Comment4 "Robin Forestier"
$EndDescr
$Comp
L archive:Light_trainer-rescue_SW_Push-Switch SW1
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
L archive:Light_trainer-rescue_SW_Push-Switch SW2
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
L archive:Light_trainer-rescue_SW_Push-Switch SW3
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
L archive:Light_trainer-rescue_SW_Push-Switch SW4
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
L archive:Light_trainer-rescue_R-Device R1
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
L archive:Light_trainer-rescue_+3.3V-power #PWR01
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
L archive:Light_trainer-rescue_SK6812-LED D1
U 1 1 5FDB63B9
P 4350 5200
F 0 "D1" H 4100 5550 50  0000 L CNN
F 1 "SK6812" H 4000 5450 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 4400 4900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 4450 4825 50  0001 L TNN
F 4 "Digikey" H 4350 5200 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 4350 5200 50  0001 C CNN "Prix total"
F 6 "4,1" H 4350 5200 50  0001 C CNN "prix unité"
	1    4350 5200
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_SK6812-LED D2
U 1 1 5FEF28E1
P 5000 5200
F 0 "D2" H 4750 5550 50  0000 L CNN
F 1 "SK6812" H 4650 5450 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 5050 4900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 5100 4825 50  0001 L TNN
F 4 "Digikey" H 5000 5200 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 5000 5200 50  0001 C CNN "Prix total"
F 6 "4,1" H 5000 5200 50  0001 C CNN "prix unité"
	1    5000 5200
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_SK6812-LED D3
U 1 1 5FF375C6
P 5650 5200
F 0 "D3" H 5400 5550 50  0000 L CNN
F 1 "SK6812" H 5300 5450 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 5700 4900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 5750 4825 50  0001 L TNN
F 4 "Digikey" H 5650 5200 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 5650 5200 50  0001 C CNN "Prix total"
F 6 "4,1" H 5650 5200 50  0001 C CNN "prix unité"
	1    5650 5200
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_SK6812-LED D4
U 1 1 5FF375CF
P 6300 5200
F 0 "D4" H 6050 5550 50  0000 L CNN
F 1 "SK6812" H 5950 5450 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 6350 4900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 6400 4825 50  0001 L TNN
F 4 "Digikey" H 6300 5200 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 6300 5200 50  0001 C CNN "Prix total"
F 6 "4,1" H 6300 5200 50  0001 C CNN "prix unité"
	1    6300 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 5200 4700 5200
Wire Wire Line
	5300 5200 5350 5200
Wire Wire Line
	5950 5200 6000 5200
Wire Wire Line
	4350 5550 4350 5500
Wire Wire Line
	5000 5550 5000 5500
Wire Wire Line
	5650 5550 5650 5500
Wire Wire Line
	6300 5550 6300 5500
Wire Wire Line
	4350 4850 4350 4900
Wire Wire Line
	5000 4850 5000 4900
Wire Wire Line
	6300 4850 6300 4900
Wire Wire Line
	5650 4850 5650 4900
Text GLabel 3800 5200 0    50   Input ~ 0
LED
Text Notes 3550 4550 0    50   ~ 0
LED néo-pixel
$Comp
L archive:Light_trainer-rescue_GND-power #PWR03
U 1 1 6043A369
P 4350 5550
F 0 "#PWR03" H 4350 5300 50  0001 C CNN
F 1 "GND" H 4355 5377 50  0000 C CNN
F 2 "" H 4350 5550 50  0001 C CNN
F 3 "" H 4350 5550 50  0001 C CNN
	1    4350 5550
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_GND-power #PWR07
U 1 1 6044BBB6
P 5000 5550
F 0 "#PWR07" H 5000 5300 50  0001 C CNN
F 1 "GND" H 5005 5377 50  0000 C CNN
F 2 "" H 5000 5550 50  0001 C CNN
F 3 "" H 5000 5550 50  0001 C CNN
	1    5000 5550
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_GND-power #PWR010
U 1 1 6045D3E7
P 5650 5550
F 0 "#PWR010" H 5650 5300 50  0001 C CNN
F 1 "GND" H 5655 5377 50  0000 C CNN
F 2 "" H 5650 5550 50  0001 C CNN
F 3 "" H 5650 5550 50  0001 C CNN
	1    5650 5550
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_GND-power #PWR012
U 1 1 6046ECE6
P 6300 5550
F 0 "#PWR012" H 6300 5300 50  0001 C CNN
F 1 "GND" H 6305 5377 50  0000 C CNN
F 2 "" H 6300 5550 50  0001 C CNN
F 3 "" H 6300 5550 50  0001 C CNN
	1    6300 5550
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_GND-power #PWR04
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
L archive:Light_trainer-rescue_+5V-power #PWR02
U 1 1 604E998D
P 4350 4850
F 0 "#PWR02" H 4350 4700 50  0001 C CNN
F 1 "+5V" H 4500 4900 50  0000 C CNN
F 2 "" H 4350 4850 50  0001 C CNN
F 3 "" H 4350 4850 50  0001 C CNN
	1    4350 4850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_+5V-power #PWR06
U 1 1 604FB129
P 5000 4850
F 0 "#PWR06" H 5000 4700 50  0001 C CNN
F 1 "+5V" H 5150 4900 50  0000 C CNN
F 2 "" H 5000 4850 50  0001 C CNN
F 3 "" H 5000 4850 50  0001 C CNN
	1    5000 4850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_+5V-power #PWR09
U 1 1 6050C9D7
P 5650 4850
F 0 "#PWR09" H 5650 4700 50  0001 C CNN
F 1 "+5V" H 5800 4900 50  0000 C CNN
F 2 "" H 5650 4850 50  0001 C CNN
F 3 "" H 5650 4850 50  0001 C CNN
	1    5650 4850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_+5V-power #PWR011
U 1 1 6051E1D9
P 6300 4850
F 0 "#PWR011" H 6300 4700 50  0001 C CNN
F 1 "+5V" H 6450 4900 50  0000 C CNN
F 2 "" H 6300 4850 50  0001 C CNN
F 3 "" H 6300 4850 50  0001 C CNN
	1    6300 4850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_TestPoint-Connector TP4
U 1 1 606B57C6
P 1950 3850
F 0 "TP4" H 2008 3968 50  0000 L CNN
F 1 "Switch" H 2008 3877 50  0000 L CNN
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
L archive:Light_trainer-rescue_TestPoint-Connector TP1
U 1 1 6070C247
P 3950 5150
F 0 "TP1" H 3600 5450 50  0000 L CNN
F 1 "Led-in" H 3600 5350 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 4150 5150 50  0001 C CNN
F 3 "~" H 4150 5150 50  0001 C CNN
F 4 "~" H 3950 5150 50  0001 C CNN "Fournisseur"
F 5 "~" H 3950 5150 50  0001 C CNN "Prix total"
F 6 " ~" H 3950 5150 50  0001 C CNN "prix unité"
	1    3950 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 5200 3950 5200
Wire Wire Line
	3950 5200 3950 5150
Connection ~ 3950 5200
Wire Wire Line
	3950 5200 4050 5200
$Comp
L archive:Light_trainer-rescue_ESP32-DevKitC-32D-Work_Forestier U2
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
L archive:Device_Battery BT1
U 1 1 613C716D
P 1250 2450
F 0 "BT1" H 1358 2496 50  0000 L CNN
F 1 "Battery" H 1358 2405 50  0000 L CNN
F 2 "Work_Forestier:BatteryHolder_Keystone_593-594" V 1250 2510 50  0001 C CNN
F 3 "~" V 1250 2510 50  0001 C CNN
	1    1250 2450
	1    0    0    -1  
$EndComp
$Comp
L archive:Device_C C2
U 1 1 613EB1F1
P 3000 2500
F 0 "C2" H 3115 2546 50  0000 L CNN
F 1 "0.1uF" H 3115 2455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3038 2350 50  0001 C CNN
F 3 "~" H 3000 2500 50  0001 C CNN
	1    3000 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 2700 3000 2650
$Comp
L archive:Device_C C1
U 1 1 613F1363
P 2050 2500
F 0 "C1" H 2165 2546 50  0000 L CNN
F 1 "0.33uF" H 2165 2455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2088 2350 50  0001 C CNN
F 3 "~" H 2050 2500 50  0001 C CNN
	1    2050 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2700 2050 2700
Wire Wire Line
	2050 2700 2050 2650
Connection ~ 2550 2700
$Comp
L archive:Light_trainer-rescue_TestPoint-Connector TP5
U 1 1 60377721
P 6700 5100
F 0 "TP5" H 6758 5218 50  0000 L CNN
F 1 "Led-out" H 6758 5127 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 6900 5100 50  0001 C CNN
F 3 "~" H 6900 5100 50  0001 C CNN
F 4 "~" H 6700 5100 50  0001 C CNN "Fournisseur"
F 5 "~" H 6700 5100 50  0001 C CNN "Prix total"
F 6 " ~" H 6700 5100 50  0001 C CNN "prix unité"
	1    6700 5100
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_+5V-power #PWR08
U 1 1 61414884
P 3000 2050
F 0 "#PWR08" H 3000 1900 50  0001 C CNN
F 1 "+5V" H 3150 2100 50  0000 C CNN
F 2 "" H 3000 2050 50  0001 C CNN
F 3 "" H 3000 2050 50  0001 C CNN
	1    3000 2050
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_GND-power #PWR05
U 1 1 61415E58
P 2550 2750
F 0 "#PWR05" H 2550 2500 50  0001 C CNN
F 1 "GND" H 2555 2577 50  0000 C CNN
F 2 "" H 2550 2750 50  0001 C CNN
F 3 "" H 2550 2750 50  0001 C CNN
	1    2550 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2750 2550 2700
Wire Notes Line
	3800 1500 3800 3150
Wire Notes Line
	800  1500 800  3150
$Comp
L archive:Light_trainer-rescue_+5V-power #PWR017
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
L archive:Light_trainer-rescue_GND-power #PWR021
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
L archive:Light_trainer-rescue_GND-power #PWR022
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
L archive:Light_trainer-rescue_GND-power #PWR016
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
L archive:Connector_TestPoint TP6
U 1 1 6142F575
P 7550 1850
F 0 "TP6" H 7608 1968 50  0000 L CNN
F 1 "Gnd" H 7608 1877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 7750 1850 50  0001 C CNN
F 3 "~" H 7750 1850 50  0001 C CNN
F 4 "~" H 7550 1850 50  0001 C CNN "Fournisseur"
F 5 "~" H 7550 1850 50  0001 C CNN "Prix total"
F 6 "~" H 7550 1850 50  0001 C CNN "prix unité"
	1    7550 1850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_GND-power #PWR015
U 1 1 61430156
P 7550 1900
F 0 "#PWR015" H 7550 1650 50  0001 C CNN
F 1 "GND" H 7555 1727 50  0000 C CNN
F 2 "" H 7550 1900 50  0001 C CNN
F 3 "" H 7550 1900 50  0001 C CNN
	1    7550 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 1900 7550 1850
$Comp
L archive:Connector_TestPoint TP7
U 1 1 61436086
P 8100 1850
F 0 "TP7" H 8158 1968 50  0000 L CNN
F 1 "5V" H 8158 1877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 8300 1850 50  0001 C CNN
F 3 "~" H 8300 1850 50  0001 C CNN
F 4 "~" H 8100 1850 50  0001 C CNN "Fournisseur"
F 5 "~" H 8100 1850 50  0001 C CNN "Prix total"
F 6 "~" H 8100 1850 50  0001 C CNN "prix unité"
	1    8100 1850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_+5V-power #PWR019
U 1 1 61439438
P 8100 1950
F 0 "#PWR019" H 8100 1800 50  0001 C CNN
F 1 "+5V" H 8100 2100 50  0000 C CNN
F 2 "" H 8100 1950 50  0001 C CNN
F 3 "" H 8100 1950 50  0001 C CNN
	1    8100 1950
	-1   0    0    1   
$EndComp
Wire Wire Line
	8100 1850 8100 1950
$Comp
L archive:Connector_TestPoint TP2
U 1 1 6143AF5D
P 2050 1850
F 0 "TP2" H 2108 1968 50  0000 L CNN
F 1 "9V" H 2108 1877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 2250 1850 50  0001 C CNN
F 3 "~" H 2250 1850 50  0001 C CNN
F 4 "~" H 2050 1850 50  0001 C CNN "Fournisseur"
F 5 "~" H 2050 1850 50  0001 C CNN "Prix total"
F 6 "~" H 2050 1850 50  0001 C CNN "prix unité"
	1    2050 1850
	-1   0    0    -1  
$EndComp
$Comp
L archive:Connector_TestPoint TP3
U 1 1 6143D663
P 2050 2750
F 0 "TP3" H 1850 2900 50  0000 L CNN
F 1 "TestPoint" H 1650 2800 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 2250 2750 50  0001 C CNN
F 3 "~" H 2250 2750 50  0001 C CNN
F 4 "~" H 2050 2750 50  0001 C CNN "Fournisseur"
F 5 "~" H 2050 2750 50  0001 C CNN "Prix total"
F 6 "~" H 2050 2750 50  0001 C CNN "prix unité"
	1    2050 2750
	-1   0    0    1   
$EndComp
Wire Wire Line
	2050 2700 2050 2750
Wire Wire Line
	6050 2950 6250 2950
Wire Wire Line
	6050 3050 6250 3050
Wire Wire Line
	5100 1850 5100 1950
$Comp
L archive:Light_trainer-rescue_+3.3V-power #PWR018
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
	3550 4600 3550 5950
Wire Notes Line
	7850 4600 7850 5950
Wire Notes Line
	3550 5950 7850 5950
Wire Notes Line
	3550 4600 7850 4600
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
Text Notes 7250 1550 0    50   ~ 0
Test point GND/5V\n
$Comp
L archive:Connector_TestPoint TP8
U 1 1 614C7562
P 8650 1850
F 0 "TP8" H 8708 1968 50  0000 L CNN
F 1 "3.3V" H 8708 1877 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 8850 1850 50  0001 C CNN
F 3 "~" H 8850 1850 50  0001 C CNN
F 4 "~" H 8650 1850 50  0001 C CNN "Fournisseur"
F 5 "~" H 8650 1850 50  0001 C CNN "Prix total"
F 6 "~" H 8650 1850 50  0001 C CNN "prix unité"
	1    8650 1850
	1    0    0    -1  
$EndComp
$Comp
L archive:Light_trainer-rescue_+3.3V-power #PWR020
U 1 1 614C9C11
P 8650 1950
F 0 "#PWR020" H 8650 1800 50  0001 C CNN
F 1 "+3.3V" H 8650 2100 50  0000 C CNN
F 2 "" H 8650 1950 50  0001 C CNN
F 3 "" H 8650 1950 50  0001 C CNN
	1    8650 1950
	-1   0    0    1   
$EndComp
Wire Wire Line
	8650 1850 8650 1950
Wire Notes Line
	7250 1600 9150 1600
Wire Notes Line
	9150 2250 9150 1600
Wire Notes Line
	7250 1600 7250 2250
Wire Notes Line
	7250 2250 9150 2250
Connection ~ 2050 2700
$Comp
L archive:Switch_SW_SPDT SW5
U 1 1 61410395
P 1550 2200
F 0 "SW5" H 1550 2485 50  0000 C CNN
F 1 "SW_SPDT" H 1550 2394 50  0000 C CNN
F 2 "Work_Forestier:C&K-1101M2S5CQE2" H 1550 2200 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1429/1000.pdf" H 1550 2200 50  0001 C CNN
	1    1550 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 2200 1350 2200
NoConn ~ 1750 2300
Wire Notes Line
	800  1500 3800 1500
Wire Notes Line
	800  3150 3800 3150
$Comp
L archive:Mechanical_MountingHole H1
U 1 1 6141B071
P 7450 2850
F 0 "H1" H 7550 2896 50  0000 L CNN
F 1 "MountingHole" H 7550 2805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 7450 2850 50  0001 C CNN
F 3 "~" H 7450 2850 50  0001 C CNN
	1    7450 2850
	1    0    0    -1  
$EndComp
$Comp
L archive:Mechanical_MountingHole H2
U 1 1 6141E6D1
P 8250 2850
F 0 "H2" H 8350 2896 50  0000 L CNN
F 1 "MountingHole" H 8350 2805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8250 2850 50  0001 C CNN
F 3 "~" H 8250 2850 50  0001 C CNN
	1    8250 2850
	1    0    0    -1  
$EndComp
$Comp
L archive:Mechanical_MountingHole H4
U 1 1 6141E94E
P 8250 3250
F 0 "H4" H 8350 3296 50  0000 L CNN
F 1 "MountingHole" H 8350 3205 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8250 3250 50  0001 C CNN
F 3 "~" H 8250 3250 50  0001 C CNN
	1    8250 3250
	1    0    0    -1  
$EndComp
$Comp
L archive:Mechanical_MountingHole H3
U 1 1 6141EBEB
P 7450 3250
F 0 "H3" H 7550 3296 50  0000 L CNN
F 1 "MountingHole" H 7550 3205 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 7450 3250 50  0001 C CNN
F 3 "~" H 7450 3250 50  0001 C CNN
	1    7450 3250
	1    0    0    -1  
$EndComp
Wire Notes Line
	7250 3500 9000 3500
Wire Notes Line
	9000 3500 9000 2650
Wire Notes Line
	9000 2650 7250 2650
Wire Notes Line
	7250 2650 7250 3500
Text Notes 7250 2600 0    50   ~ 0
Trous de fixation\n
Wire Wire Line
	1750 2100 2050 2100
Wire Wire Line
	2050 2700 1250 2700
$Comp
L Work_Forestier:L78M05CDT-TR U1
U 1 1 62AB64A4
P 2550 2200
F 0 "U1" H 2550 2565 50  0000 C CNN
F 1 "L78M05CDT-TR" H 2550 2474 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TO-252-2" H 2550 2200 50  0001 C CNN
F 3 "https://octopart.com/datasheet/l78m05cdt-tr-stmicroelectronics-40015935" H 2550 2200 50  0001 C CNN
	1    2550 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 2100 2250 2100
Connection ~ 2050 2100
Wire Wire Line
	2050 2100 2050 2350
Wire Wire Line
	2550 2400 2550 2700
Wire Wire Line
	1250 2200 1250 2250
Wire Wire Line
	1250 2650 1250 2700
Wire Wire Line
	2050 1850 2050 2100
Wire Wire Line
	2850 2100 3000 2100
Wire Wire Line
	3000 2050 3000 2100
Connection ~ 3000 2100
Wire Wire Line
	3000 2100 3000 2350
Wire Wire Line
	2550 2700 3000 2700
Wire Wire Line
	6600 5200 6700 5200
Wire Wire Line
	6700 5100 6700 5200
$EndSCHEMATC
