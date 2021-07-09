## EEL7415/7515 - IoT LoRa

This branch contains the software presented ...

#### Folders structure

    ├── Bin                   # Binary files.
    ├── Docs                  # Boards usefull files.
    ├── Drivers               # Sensors and HAL drivers.
    ├── Middlewares           # LoRaWAN stack.
    ├── Projects              # Main source code and Makefile
    └── ...

#### Prerequisites

The tests were performed in a Linux machine with Ubuntu version 20.04.2 LTS

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
$ cd Applications/MyApp/
```

To compile:

```sh
$ make
```

The binary file will be located in a folder named ```build```. To clean the build files, use: ```make clean```.

#### Executing the application

Execute the following command:

```sh
$ ./build/my_app
```

#### Updating the code

The most relevant files are:

* Application/My_App/src/**main.c**: project main application.
* Application/My_App/**Makefile**: contains the paths to files to be compiled and generated.
