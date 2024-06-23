# Lie Detector

### Lie Detector is a desktop GUI app written in Qt Creator. It was created to display real time data gathered by STM32F411RE microcontroller as a part of lie detector project. 
Its specific design enables the user to search for connected via serial port devices, open port with given parameters and read/transmit data.

## First Window - Serial port connection menu
![mojGif](https://github.com/Matii178/Lie-Detector/assets/62108776/32e7e8ff-8498-410b-be59-e9d83711da38)

After starting application first window pops up.
<img src= "https://github.com/Matii178/Lie-Detector/assets/62108776/4ad9154d-51d8-44c3-beb3-e5cb0773a74b" width = "800" height = "400">

It contains 5 user buttons, one dialog window, and one dialog bar.
* 'Search for available devices' - Refreshes the list of connected devices and enables to choose the specific one.
* 'Connect with selected port' - Tries to connect with selected device. If successful displays a message on dialog window.
* 'Disconnect' - Disconnects and closes already opened port.
* 'Try connection' - Send 1 byte of data (0b10100101) via opened port. (in my application I used it to blink with diode to signalize connection)
* 'START' - It proceeds to open main plot window.

Almost every performed action is followed up by various notification displayed on dialog window. For example if user tries to access unaccesable port it will display "The serial port was unreachable" message.
<img src= "https://github.com/Matii178/Lie-Detector/assets/62108776/2d1eee20-14db-47b1-ac1a-fc7fee6b268f" width = "800" height = "400">

After clicking the start button, main window opens up.

## Second Window - Real time data presentation
At first it is completely empty.
<img src= "https://github.com/Matii178/Lie-Detector/assets/62108776/a7a73d25-e766-430f-a49c-6ec478cc1812" width = "800" height = "400">

Once again, there are 5 user buttons on the right side.
* 'Pause' - Pauses the plots.
* 'Resume' - Starts or resumes displaying the data being sent.
* 'Clean' - Completely cleans the plots.
* 'Set mark' - Marks specific time moment with a red line.
* 'Save to File' - Enables to save already displayed data as a .csv file.

As an example, below is presented approx. 18 seconds of gathered data. The window slides to the right as new data arrives presenting only 15 seconds of latest data points.

<img src= "https://github.com/Matii178/Lie-Detector/assets/62108776/0ed04899-26dd-44e8-860f-0645f57bb2c3" width = "800" height = "400">

There are also 2 markers being visible at 11th and 17th second.

