EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
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
L Light_trainer-rescue:SW_Push-Switch SW3
U 1 1 5FD2E185
P 1800 4600
F 0 "SW3" H 1800 4885 50  0000 C CNN
F 1 "SW_Push" H 1800 4794 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_PTS645" H 1800 4800 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1800 4800 50  0001 C CNN
F 4 "Digikey" H 1800 4600 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1800 4600 50  0001 C CNN "Prix total"
F 6 "1,32" H 1800 4600 50  0001 C CNN "prix unité"
	1    1800 4600
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SW_Push-Switch SW4
U 1 1 5FD62CD6
P 1800 4950
F 0 "SW4" H 1800 5235 50  0000 C CNN
F 1 "SW_Push" H 1800 5144 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_PTS645" H 1800 5150 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1800 5150 50  0001 C CNN
F 4 "Digikey" H 1800 4950 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1800 4950 50  0001 C CNN "Prix total"
F 6 "1,32" H 1800 4950 50  0001 C CNN "prix unité"
	1    1800 4950
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SW_Push-Switch SW5
U 1 1 5FD65B5A
P 1800 5300
F 0 "SW5" H 1800 5585 50  0000 C CNN
F 1 "SW_Push" H 1800 5494 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_PTS645" H 1800 5500 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1800 5500 50  0001 C CNN
F 4 "Digikey" H 1800 5300 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1800 5300 50  0001 C CNN "Prix total"
F 6 "1,32" H 1800 5300 50  0001 C CNN "prix unité"
	1    1800 5300
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SW_Push-Switch SW7
U 1 1 5FD68A79
P 1800 5650
F 0 "SW7" H 1800 5935 50  0000 C CNN
F 1 "SW_Push" H 1800 5844 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_PTS645" H 1800 5850 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 1800 5850 50  0001 C CNN
F 4 "Digikey" H 1800 5650 50  0001 C CNN "Fournisseur"
F 5 "0,22" H 1800 5650 50  0001 C CNN "Prix total"
F 6 "1,32" H 1800 5650 50  0001 C CNN "prix unité"
	1    1800 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 4500 1500 4600
Wire Wire Line
	1500 4600 1600 4600
Wire Wire Line
	1500 4600 1500 4950
Wire Wire Line
	1500 4950 1600 4950
Connection ~ 1500 4600
Wire Wire Line
	1500 4950 1500 5300
Wire Wire Line
	1500 5300 1600 5300
Connection ~ 1500 4950
Wire Wire Line
	1500 5300 1500 5650
Wire Wire Line
	1500 5650 1600 5650
Connection ~ 1500 5300
$Comp
L Light_trainer-rescue:R-Device R17
U 1 1 5FD7C917
P 2150 5900
F 0 "R17" H 2220 5946 50  0000 L CNN
F 1 "10k" H 2220 5855 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2080 5900 50  0001 C CNN
F 3 "~" H 2150 5900 50  0001 C CNN
F 4 "Digikey" H 2150 5900 50  0001 C CNN "Fournisseur"
	1    2150 5900
	-1   0    0    1   
$EndComp
Wire Wire Line
	2150 5650 2150 5750
Wire Wire Line
	2150 5650 2000 5650
Wire Wire Line
	2150 5650 2150 5300
Wire Wire Line
	2150 5300 2000 5300
Connection ~ 2150 5650
Wire Wire Line
	2150 5300 2150 4950
Wire Wire Line
	2150 4950 2000 4950
Connection ~ 2150 5300
Wire Wire Line
	2000 4600 2150 4600
Wire Wire Line
	2150 4600 2150 4950
Connection ~ 2150 4950
Text GLabel 2350 4600 2    50   Output ~ 0
switch
Wire Wire Line
	2150 4600 2350 4600
Connection ~ 2150 4600
Wire Notes Line
	1200 6500 2750 6500
Wire Notes Line
	2750 6500 2750 4100
Wire Notes Line
	2750 4100 1200 4100
Wire Notes Line
	1200 4100 1200 6500
Text Notes 1200 4050 0    50   ~ 0
Switch
Wire Wire Line
	2150 6100 2150 6050
$Comp
L Light_trainer-rescue:+3.3V-power #PWR044
U 1 1 6047DC4D
P 1500 4500
F 0 "#PWR044" H 1500 4350 50  0001 C CNN
F 1 "+3.3V" H 1515 4673 50  0000 C CNN
F 2 "" H 1500 4500 50  0001 C CNN
F 3 "" H 1500 4500 50  0001 C CNN
	1    1500 4500
	1    0    0    -1  
$EndComp
Text Notes 6700 950  0    236  ~ 47
LightTrainer V00
$Comp
L Light_trainer-rescue:SK6812-LED D5
U 1 1 5FDB63B9
P 3750 4650
F 0 "D5" H 3500 5000 50  0000 L CNN
F 1 "SK6812" H 3400 4900 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 3800 4350 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 3850 4275 50  0001 L TNN
F 4 "Digikey" H 3750 4650 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 3750 4650 50  0001 C CNN "Prix total"
F 6 "4,1" H 3750 4650 50  0001 C CNN "prix unité"
	1    3750 4650
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D6
U 1 1 5FEF28E1
P 4400 4650
F 0 "D6" H 4150 5000 50  0000 L CNN
F 1 "SK6812" H 4050 4900 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 4450 4350 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 4500 4275 50  0001 L TNN
F 4 "Digikey" H 4400 4650 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 4400 4650 50  0001 C CNN "Prix total"
F 6 "4,1" H 4400 4650 50  0001 C CNN "prix unité"
	1    4400 4650
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D7
U 1 1 5FF375C6
P 5050 4650
F 0 "D7" H 4800 5000 50  0000 L CNN
F 1 "SK6812" H 4700 4900 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 5100 4350 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 5150 4275 50  0001 L TNN
F 4 "Digikey" H 5050 4650 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 5050 4650 50  0001 C CNN "Prix total"
F 6 "4,1" H 5050 4650 50  0001 C CNN "prix unité"
	1    5050 4650
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D8
U 1 1 5FF375CF
P 5700 4650
F 0 "D8" H 5450 5000 50  0000 L CNN
F 1 "SK6812" H 5350 4900 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 5750 4350 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 5800 4275 50  0001 L TNN
F 4 "Digikey" H 5700 4650 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 5700 4650 50  0001 C CNN "Prix total"
F 6 "4,1" H 5700 4650 50  0001 C CNN "prix unité"
	1    5700 4650
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D13
U 1 1 5FF51111
P 3750 5800
F 0 "D13" H 3500 6150 50  0000 L CNN
F 1 "SK6812" H 3400 6050 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 3800 5500 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 3850 5425 50  0001 L TNN
F 4 "Digikey" H 3750 5800 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 3750 5800 50  0001 C CNN "Prix total"
F 6 "4,1" H 3750 5800 50  0001 C CNN "prix unité"
	1    3750 5800
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D14
U 1 1 5FF5111A
P 4400 5800
F 0 "D14" H 4150 6150 50  0000 L CNN
F 1 "SK6812" H 4050 6050 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 4450 5500 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 4500 5425 50  0001 L TNN
F 4 "Digikey" H 4400 5800 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 4400 5800 50  0001 C CNN "Prix total"
F 6 "4,1" H 4400 5800 50  0001 C CNN "prix unité"
	1    4400 5800
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D15
U 1 1 5FF51123
P 5050 5800
F 0 "D15" H 4800 6150 50  0000 L CNN
F 1 "SK6812" H 4700 6050 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 5100 5500 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 5150 5425 50  0001 L TNN
F 4 "Digikey" H 5050 5800 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 5050 5800 50  0001 C CNN "Prix total"
F 6 "4,1" H 5050 5800 50  0001 C CNN "prix unité"
	1    5050 5800
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:SK6812-LED D16
U 1 1 5FF5112C
P 5700 5800
F 0 "D16" H 5450 6150 50  0000 L CNN
F 1 "SK6812" H 5350 6050 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 5750 5500 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 5800 5425 50  0001 L TNN
F 4 "Digikey" H 5700 5800 50  0001 C CNN "Fournisseur"
F 5 "0,41" H 5700 5800 50  0001 C CNN "Prix total"
F 6 "4,1" H 5700 5800 50  0001 C CNN "prix unité"
	1    5700 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 4650 4100 4650
Wire Wire Line
	4700 4650 4750 4650
Wire Wire Line
	5350 4650 5400 4650
Wire Wire Line
	6000 4650 6050 4650
Wire Wire Line
	6050 5800 6000 5800
Wire Wire Line
	5400 5800 5350 5800
Wire Wire Line
	4750 5800 4700 5800
Wire Wire Line
	4050 5800 4100 5800
Wire Wire Line
	3750 5000 3750 4950
Wire Wire Line
	4400 5000 4400 4950
Wire Wire Line
	5050 5000 5050 4950
Wire Wire Line
	5700 5000 5700 4950
Wire Wire Line
	5700 6150 5700 6100
Wire Wire Line
	5050 6150 5050 6100
Wire Wire Line
	4400 6150 4400 6100
Wire Wire Line
	3750 6150 3750 6100
Wire Wire Line
	3750 4300 3750 4350
Wire Wire Line
	4400 4300 4400 4350
Wire Wire Line
	5700 4300 5700 4350
Wire Wire Line
	5050 4300 5050 4350
Wire Wire Line
	5700 5450 5700 5500
Wire Wire Line
	5050 5450 5050 5500
Wire Wire Line
	3750 5450 3750 5500
Wire Wire Line
	4400 5450 4400 5500
Text GLabel 3350 5800 0    50   Input ~ 0
DOUT
Wire Wire Line
	3350 5800 3450 5800
Text GLabel 6050 4650 2    50   Output ~ 0
DOUT
Text GLabel 3200 4650 0    50   Input ~ 0
LED
Text Notes 2950 4000 0    50   ~ 0
LED néo-pixel
$Comp
L Light_trainer-rescue:TestPoint-Connector TP5
U 1 1 60377721
P 6050 5750
F 0 "TP5" H 6108 5868 50  0000 L CNN
F 1 "TestPoint" H 6108 5777 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 6250 5750 50  0001 C CNN
F 3 "~" H 6250 5750 50  0001 C CNN
F 4 "~" H 6050 5750 50  0001 C CNN "Fournisseur"
F 5 "~" H 6050 5750 50  0001 C CNN "Prix total"
F 6 " ~" H 6050 5750 50  0001 C CNN "prix unité"
	1    6050 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 5750 6050 5800
$Comp
L Light_trainer-rescue:GND-power #PWR060
U 1 1 603F42F0
P 5700 6150
F 0 "#PWR060" H 5700 5900 50  0001 C CNN
F 1 "GND" H 5705 5977 50  0000 C CNN
F 2 "" H 5700 6150 50  0001 C CNN
F 3 "" H 5700 6150 50  0001 C CNN
	1    5700 6150
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR059
U 1 1 60405B0D
P 5050 6150
F 0 "#PWR059" H 5050 5900 50  0001 C CNN
F 1 "GND" H 5055 5977 50  0000 C CNN
F 2 "" H 5050 6150 50  0001 C CNN
F 3 "" H 5050 6150 50  0001 C CNN
	1    5050 6150
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR058
U 1 1 6041735D
P 4400 6150
F 0 "#PWR058" H 4400 5900 50  0001 C CNN
F 1 "GND" H 4405 5977 50  0000 C CNN
F 2 "" H 4400 6150 50  0001 C CNN
F 3 "" H 4400 6150 50  0001 C CNN
	1    4400 6150
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR057
U 1 1 60428BE3
P 3750 6150
F 0 "#PWR057" H 3750 5900 50  0001 C CNN
F 1 "GND" H 3755 5977 50  0000 C CNN
F 2 "" H 3750 6150 50  0001 C CNN
F 3 "" H 3750 6150 50  0001 C CNN
	1    3750 6150
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR045
U 1 1 6043A369
P 3750 5000
F 0 "#PWR045" H 3750 4750 50  0001 C CNN
F 1 "GND" H 3755 4827 50  0000 C CNN
F 2 "" H 3750 5000 50  0001 C CNN
F 3 "" H 3750 5000 50  0001 C CNN
	1    3750 5000
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR046
U 1 1 6044BBB6
P 4400 5000
F 0 "#PWR046" H 4400 4750 50  0001 C CNN
F 1 "GND" H 4405 4827 50  0000 C CNN
F 2 "" H 4400 5000 50  0001 C CNN
F 3 "" H 4400 5000 50  0001 C CNN
	1    4400 5000
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR047
U 1 1 6045D3E7
P 5050 5000
F 0 "#PWR047" H 5050 4750 50  0001 C CNN
F 1 "GND" H 5055 4827 50  0000 C CNN
F 2 "" H 5050 5000 50  0001 C CNN
F 3 "" H 5050 5000 50  0001 C CNN
	1    5050 5000
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR048
U 1 1 6046ECE6
P 5700 5000
F 0 "#PWR048" H 5700 4750 50  0001 C CNN
F 1 "GND" H 5705 4827 50  0000 C CNN
F 2 "" H 5700 5000 50  0001 C CNN
F 3 "" H 5700 5000 50  0001 C CNN
	1    5700 5000
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:GND-power #PWR056
U 1 1 60491CDF
P 2150 6100
F 0 "#PWR056" H 2150 5850 50  0001 C CNN
F 1 "GND" H 2155 5927 50  0000 C CNN
F 2 "" H 2150 6100 50  0001 C CNN
F 3 "" H 2150 6100 50  0001 C CNN
	1    2150 6100
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR039
U 1 1 604E998D
P 3750 4300
F 0 "#PWR039" H 3750 4150 50  0001 C CNN
F 1 "+5V" H 3900 4350 50  0000 C CNN
F 2 "" H 3750 4300 50  0001 C CNN
F 3 "" H 3750 4300 50  0001 C CNN
	1    3750 4300
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR040
U 1 1 604FB129
P 4400 4300
F 0 "#PWR040" H 4400 4150 50  0001 C CNN
F 1 "+5V" H 4550 4350 50  0000 C CNN
F 2 "" H 4400 4300 50  0001 C CNN
F 3 "" H 4400 4300 50  0001 C CNN
	1    4400 4300
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR041
U 1 1 6050C9D7
P 5050 4300
F 0 "#PWR041" H 5050 4150 50  0001 C CNN
F 1 "+5V" H 5200 4350 50  0000 C CNN
F 2 "" H 5050 4300 50  0001 C CNN
F 3 "" H 5050 4300 50  0001 C CNN
	1    5050 4300
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR042
U 1 1 6051E1D9
P 5700 4300
F 0 "#PWR042" H 5700 4150 50  0001 C CNN
F 1 "+5V" H 5850 4350 50  0000 C CNN
F 2 "" H 5700 4300 50  0001 C CNN
F 3 "" H 5700 4300 50  0001 C CNN
	1    5700 4300
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR053
U 1 1 60552CBF
P 5700 5450
F 0 "#PWR053" H 5700 5300 50  0001 C CNN
F 1 "+5V" H 5850 5500 50  0000 C CNN
F 2 "" H 5700 5450 50  0001 C CNN
F 3 "" H 5700 5450 50  0001 C CNN
	1    5700 5450
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR052
U 1 1 60564858
P 5050 5450
F 0 "#PWR052" H 5050 5300 50  0001 C CNN
F 1 "+5V" H 5200 5500 50  0000 C CNN
F 2 "" H 5050 5450 50  0001 C CNN
F 3 "" H 5050 5450 50  0001 C CNN
	1    5050 5450
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR051
U 1 1 60576329
P 4400 5450
F 0 "#PWR051" H 4400 5300 50  0001 C CNN
F 1 "+5V" H 4550 5500 50  0000 C CNN
F 2 "" H 4400 5450 50  0001 C CNN
F 3 "" H 4400 5450 50  0001 C CNN
	1    4400 5450
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:+5V-power #PWR050
U 1 1 60587C84
P 3750 5450
F 0 "#PWR050" H 3750 5300 50  0001 C CNN
F 1 "+5V" H 3900 5500 50  0000 C CNN
F 2 "" H 3750 5450 50  0001 C CNN
F 3 "" H 3750 5450 50  0001 C CNN
	1    3750 5450
	1    0    0    -1  
$EndComp
$Comp
L Light_trainer-rescue:TestPoint-Connector TP3
U 1 1 606B57C6
P 2150 4450
F 0 "TP3" H 2208 4568 50  0000 L CNN
F 1 "TestPoint" H 2208 4477 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 2350 4450 50  0001 C CNN
F 3 "~" H 2350 4450 50  0001 C CNN
F 4 "~" H 2150 4450 50  0001 C CNN "Fournisseur"
F 5 "~" H 2150 4450 50  0001 C CNN "Prix total"
F 6 " ~" H 2150 4450 50  0001 C CNN "prix unité"
	1    2150 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 4450 2150 4600
$Comp
L Light_trainer-rescue:TestPoint-Connector TP4
U 1 1 6070C247
P 3350 4600
F 0 "TP4" H 3000 4900 50  0000 L CNN
F 1 "TestPoint" H 3000 4800 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_2.5x2.5mm_Drill1.2mm" H 3550 4600 50  0001 C CNN
F 3 "~" H 3550 4600 50  0001 C CNN
F 4 "~" H 3350 4600 50  0001 C CNN "Fournisseur"
F 5 "~" H 3350 4600 50  0001 C CNN "Prix total"
F 6 " ~" H 3350 4600 50  0001 C CNN "prix unité"
	1    3350 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 4650 3350 4650
Wire Wire Line
	3350 4650 3350 4600
Connection ~ 3350 4650
Wire Wire Line
	3350 4650 3450 4650
Wire Notes Line
	6550 4050 2950 4050
Wire Notes Line
	2950 4050 2950 6500
Wire Notes Line
	2950 6500 6550 6500
Wire Notes Line
	6550 4050 6550 6500
$Comp
L Work_Forestier:ESP32-DevKitC-32D U1
U 1 1 6148669D
P 12150 2850
F 0 "U1" H 12150 4015 50  0000 C CNN
F 1 "ESP32-DevKitC-32D" H 12150 3924 50  0000 C CNN
F 2 "Work_Forestier:ESP32-DevKitC-32D" H 11800 4050 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Espressif%20PDFs/ESP32-DevKitC_GSG_Ver1.4_2017.pdf" H 11800 4050 50  0001 C CNN
	1    12150 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT?
U 1 1 613C716D
P 1650 2350
F 0 "BT?" H 1758 2396 50  0000 L CNN
F 1 "Battery" H 1758 2305 50  0000 L CNN
F 2 "" V 1650 2410 50  0001 C CNN
F 3 "~" V 1650 2410 50  0001 C CNN
	1    1650 2350
	1    0    0    -1  
$EndComp
$EndSCHEMATC
