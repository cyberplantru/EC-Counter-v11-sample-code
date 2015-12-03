##CyberPlant E.C. Reader

The E.C. Reader based on the precision timer NE555DR. The Conductivity circuit provides the AC excitation. The output signal then is a square wave. Its frequency varies from 240 Hz when the sensor is bone dry, up to 17255 Hz at the E.C. 80.00 μS/cm. This output can measured using the Interrupts. For temperature compensation can used Waterproof DS18B20 temperature sensor. The E.C. Reader provides high accuracy readings of conductivity measurement.

##Features:

- Power Supply: 5V - 15V
- Measuring Range EC: 0 - 80.000 uS
- Accurate EC readings: ±2%
- Temp sensor support: ds18b20
- Accurate temperature readings:  ±0.3°C
- BNC Connector
- Grove Connector
- PCB Size : 36.8 mm×20.3 mm


## EC measurement

Connect the module to D2/D3 pins Arduino Uno via Grove cable.
Connect the EC electrode. For temperature compensation connect the ds18b20 sensor and solder a resistor 4k7. Measurements without temperature compensation may lead to less accuracy. Provide supply least 200 mA to measure the full range of conductivity.

![EC Reader](http://image.cyber-plant.com/var/albums/ECReaderConnect.jpg?m=1449111914)



See the sample code *ECMeasurementSerial.ino*

The sample code uses the sensor calibration feature with one touch.
![pH to I2C](http://image.cyber-plant.com/var/resizes/PhSerial.png?m=1447486499)

## Isolated sensor
If you need to isolate the EC Reader can use I2Ciso. When reading the pH and Conductivity together, recommended to isolate the pH circuit and leave the E.C. unisolated.

![pH to I2C](http://image.cyber-plant.com/var/resizes/pHtoI2C_iso.jpg?m=1449086266)
