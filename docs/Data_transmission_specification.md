# Data transmission and storage specification

## 1. Data transmission specification

Data is transmitted in the form of a packed structure ```sensor_controller_data_t```. Structure is defined in sensor_controller.h file. **Note**: while in the struct there may exist tables of data such as multiple readings from accelerometer, only the last reading is sent.

On the reception of ```data``` command, the controller sends the data as a packed structure through UART. There is an added ACK at the beginning of the response.

## 2. Data storage specification

Data is once again stored in a ring buffer of ```sensor_controller_data_t```, and then saved into the SD card. There are multiple files in the SD card:
- ```single_shot_sensor_data.txt``` - will contain the data from the single shot mode. The data is stored as a string. The format is as follows:
    ```
    <timestamp>;<sensor_data_1>;...;<sensor_data_N>
    ```
    where the timestamp is in milliseconds since the start of the program, and the sensor data is the data from the sensor in the form of a packed structure.
- ```continuous_sensor_data.txt``` - will contain the data from the continuous mode. The data is stored as a string. The format is as follows:
    ```
    <timestamp>;<sensor_data_1_1>;...;<sensor_data_N_1>
    <timestamp>;<sensor_data_1_2>;...;<sensor_data_N_2>
    ```
    where the timestamp is in milliseconds since the start of the program, and the sensor data is the data from the sensor in the form of a string.