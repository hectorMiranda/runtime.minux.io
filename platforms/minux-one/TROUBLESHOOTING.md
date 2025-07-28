# SSD1306 Display Troubleshooting Guide

## Error: "SSD1306 allocation failed"

This error occurs when the Arduino cannot communicate with the OLED display. Here are the most common causes and solutions:

### 1. **Check Wiring** (Most Common Issue)

**Correct SSD1306 to Arduino Nano Wiring:**
```
SSD1306 Pin    →    Arduino Nano Pin
VCC/VDD        →    5V (or 3.3V if module requires it)
GND            →    GND
SDA            →    A4 (Analog Pin 4)
SCL            →    A5 (Analog Pin 5)
```

**⚠️ Important Notes:**
- Some SSD1306 modules work on 5V, others only on 3.3V - check your module specs
- Make sure connections are solid - loose wires cause intermittent failures
- Use short wires (under 6 inches) for stable I2C communication

### 2. **I2C Address Issues**

Most SSD1306 displays use address `0x3C`, but some use `0x3D`. To find the correct address:

1. Run the diagnostic: `diagnose.bat`
2. Check the I2C scanner output
3. Update the `SCREEN_ADDRESS` in main.cpp if needed

### 3. **Power Supply Problems**

- **Insufficient Power**: Use the 5V pin, not 3.3V (unless module specifically requires 3.3V)
- **Brown-out**: If using USB power, try a powered USB hub or external power supply
- **Bad USB Cable**: Try a different USB cable

### 4. **Module Compatibility**

**Supported SSD1306 Modules:**
- 128x64 OLED displays
- I2C interface (not SPI)
- SSD1306 controller chip

**Common Working Modules:**
- "0.96 inch OLED Display Module"
- "128x64 I2C OLED"
- Most generic SSD1306 modules from electronics suppliers

### 5. **Software Issues**

If hardware is confirmed working:
- Check that the Adafruit SSD1306 library is installed
- Verify I2C pull-up resistors (most modules have them built-in)
- Try different I2C clock speeds

## Quick Diagnostic Steps

1. **Run Hardware Test:**
   ```
   diagnose.bat
   ```

2. **Check Serial Output for:**
   - I2C device detection
   - Successful display initialization
   - Error messages

3. **Visual Confirmation:**
   - The diagnostic will show a test pattern on the display if successful
   - LED on Arduino should blink to confirm code is running

## Common Solutions

### Solution 1: Wrong I2C Address
If I2C scan finds device at different address (like 0x3D):
```cpp
#define SCREEN_ADDRESS 0x3D  // Change from 0x3C to 0x3D
```

### Solution 2: Power Issue
Try connecting VCC to 3.3V instead of 5V:
```
SSD1306 VCC → Arduino 3.3V (instead of 5V)
```

### Solution 3: Loose Connections
- Re-seat all wires
- Check continuity with multimeter
- Use breadboard or soldering for permanent connections

### Solution 4: Bad Module
If all else fails, the display module might be defective. Try:
- Different SSD1306 module
- Testing module with simple Arduino sketch

## Still Having Issues?

1. Take photos of your wiring setup
2. Run `diagnose.bat` and capture the serial output
3. Check that the Arduino Nano is working with a simple blink sketch
4. Verify the SSD1306 module with a basic Adafruit example

The diagnostic tool will help identify whether the issue is:
- ❌ Hardware connection problem
- ❌ Wrong I2C address  
- ❌ Power supply issue
- ❌ Defective module
- ✅ Working correctly
