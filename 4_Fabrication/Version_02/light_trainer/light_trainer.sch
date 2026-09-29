EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Light_trianer"
Date "2026-06-30"
Rev "1"
Comp ""
Comment1 "Robin Forestier"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:R R?
U 1 1 6A437E7C
P 2450 2000
F 0 "R?" H 2520 2046 50  0000 L CNN
F 1 "10K" H 2520 1955 50  0000 L CNN
F 2 "" V 2380 2000 50  0001 C CNN
F 3 "~" H 2450 2000 50  0001 C CNN
	1    2450 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 6A438193
P 2750 2000
F 0 "R?" H 2820 2046 50  0000 L CNN
F 1 "10K" H 2820 1955 50  0000 L CNN
F 2 "" V 2680 2000 50  0001 C CNN
F 3 "~" H 2750 2000 50  0001 C CNN
	1    2750 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT?
U 1 1 6A438A35
P 1650 2050
F 0 "BT?" H 1758 2096 50  0000 L CNN
F 1 "Battery" H 1758 2005 50  0000 L CNN
F 2 "" V 1650 2110 50  0001 C CNN
F 3 "~" V 1650 2110 50  0001 C CNN
	1    1650 2050
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D?
U 1 1 6A4399BF
P 2750 3150
F 0 "D?" H 3094 3196 50  0000 L CNN
F 1 "WS2812B" H 3094 3105 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 2800 2850 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 2850 2775 50  0001 L TNN
	1    2750 3150
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Distance:VL53L1CXV0FY1 U?
U 1 1 6A43A4CD
P 5150 2300
F 0 "U?" H 5480 2346 50  0000 L CNN
F 1 "VL53L1CXV0FY1" H 5480 2255 50  0000 L CNN
F 2 "Sensor_Distance:ST_VL53L1x" H 5825 1750 50  0001 C CNN
F 3 "https://www.st.com/resource/en/datasheet/vl53l1x.pdf" H 5250 2300 50  0001 C CNN
	1    5150 2300
	1    0    0    -1  
$EndComp
$EndSCHEMATC
