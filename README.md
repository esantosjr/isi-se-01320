## FreeRTOS project for 01320

This branch contains the software presented ...

#### Folders structure

    ├── Docs              # Documents for presentation.
    ├── FreeRTOS          # Contains the FreeRTOS real time kernel source files.
    ├── FreeRTOS-Plus     # Contains FreeRTOS+ components.
    ├── tools             # FreeRTOS complementary files.
    ├── Applications      # Main source code and Makefile.
            |
            +- MyApp
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

Expected output:

```sh
AAAAAH
```

#### Updating the code

The most relevant files are:

* Application/My_App/src/**main_app.c**: project main application.
* Application/My_App/inc/**FreeRTOSConfig.h**: FreeRTOS configuration file.
* Application/My_App/**Makefile**: contains the paths to files to be compiled, linked and generated.
