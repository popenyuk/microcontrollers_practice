## Team members
- Zenovii Popenyuk
- Yevhenii Molodtsov

## Description
With the help of the library, the initialization of the accelerometer and gyroscope is starting: if an error appears, then the corresponding screen appears message. If everything is ok, then the data is read from the accelerometer and the gyroscope, and then sent to the screen through the library.

To operate the device, it should be connected to the power supply. It outputs the data in turn - first the current ones - x, y, z and the length of the vector, then maximize in the same sequence. He calculates the maximum data by comparing the maximum lengths of the vectors. A delay of 1000 set if you put a smaller screen does not have time to sketch data. Sometimes the display can "trick," you need to make a hard reset, because some data may not fit.
