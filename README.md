## FreeRTOS project - Practical test

This branch contains the software presented to the practical test of process 01320. The application is programmed in C language running the [FreeRTOS simulator for POSIX/Linux](https://www.freertos.org/FreeRTOS-simulator-for-Linux.html).

#### Folders structure

    ├── Docs              # Documents for presentation.
    ├── FreeRTOS          # Contains the FreeRTOS real time kernel source files.
    ├── FreeRTOS-Plus     # Contains FreeRTOS+ components.
    ├── tools             # FreeRTOS complementary files.
    ├── Applications      # Main source code and Makefile.
            |
            +- MainApp
                |
                +- src      # source code
                +- inc      # includes
                -- Makefile # paths to files to be compiled, linked and generated
    └── ...

#### Prerequisites

The tests were performed in a Linux machine with Ubuntu version 20.04.2 LTS.

* **GCC**

```sh
$ gcc --version
gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
```

* **Make**

```sh
$ make --version
GNU Make 4.2.1
```

#### Compiling the code

Go to the folder that contains the Makefile file:

```sh
$ cd Applications/MainApp/
```

To compile:

```sh
$ make
```

The binary file will be located in a folder named ```build```. To clean the build files, use: ```make clean```.

#### Executing the application

Execute the following command:

```sh
$ ./build/main_app
```

Expected output:

```sh
Starting main application

Creating Queues... 
Queue created... 
Starting scheduling, use Ctrl + C on any moment to finish ... 

******* ADCRead STATS *******
 Task Priority: 4
 Queue Name: Queue-01
 Queue Space Used: 0
 Queue Space Avaliable: 1000
******************************

******* Processing STATS *******
 Task Priority: 3
 Queue Name: Queue-01
 Queue Space Used: 0
 Queue Space Avaliable: 1000
******************************

******* SerialInter STATS *******
 Task Priority: 3
******************************

******* Stats STATS *******
 Task Priority: 3
******************************

...
```

#### Interacting with the application

To verify the processed ADC readings enter the following command on the serial interface:

```sh
obter
```

To clear the buffers, type:

```sh
zerar
```

#### Updating the code

The most relevant files are:

* Application/My_App/src/**main_app.c**: project main application.
* Application/My_App/inc/**FreeRTOSConfig.h**: FreeRTOS configuration file.
* Application/My_App/**Makefile**: contains the paths to files to be compiled, linked and generated.
