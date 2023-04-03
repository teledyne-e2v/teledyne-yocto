# teledyne-yocto
This is a delivery of the Topaz2m driver for IMX8M Mini

## v0.6.0
•	Additional test patterns.
•	Trigger mode control.
•	Image offset control.
•	Fix to SIMR mode not being disabled.

## v0.5.1
This driver version adds the following features:
* Modifies the minimum value of the exposure control to 5
* Modifies the default ROI 1 height value to 1080


# Driver specifications
Feature:	Value
Platform:	Verdin imx8m mini
Sensor:	TOPAZ2M
Max Image Size: 1920x1080
Frame Rate @Max Image Size: 65FPS @RAW10 / 100FPS @RAW8
Colorspace format: Monochrome
Pixel Depth:	8-10 bit
Interface:	CSI-2 MIPI x2 Lanes
Yocto Release:	Dunfell
Kernel version:	5.4.193
Yocto environment version:	0.6.0
Driver version:	0.6

# Build the Teledyne custom Yocto Image
A custom Yocto environment with a custom meta-layer was prepared. This meta-layer called meta-teledyne already has a patch to include the Topaz2m sensor driver.
- **Note:** Yocto is a powerful tool for building custom images, however, it consumes a lot of resources of the host computer when building the image, so it is recommended to use linux natively instead of using virtual machines. In addition, the build process can take several hours.

## Build the Image using a previous build
If you already had a Yocto environment from a previous delivery, you can follow the next procedure to avoid setting up a new environment:
1. Move to your previous **teledyne-yocto** directory and replace all files in that directory with the files in the new provided environment.
2. Execute steps 1 and 2 from the **Setup the Yocto environment** subsection. These will set the required **TELEDYNE_DEVDIR** and **YOCTO_DIR** environment variables. Make sure to use the same paths that you used for your previous build.
3. Go to the **Pre-build configurations*** subsection and continue from there.


# Get the Yocto project environment
## Option A: Use the attached .tar file
1. Download the attached teledyne-yocto.tar file.
2. Move into the path where the file is placed and extract its contents:
tar -xvf teledyne-yocto.tar

## Option B: Clone the project from the RidgeRun repository
1. If you have access to the project repository, you can clone the project with the following command:

  git clone -b v0.6.0 https://gitlab.ridgerun.com/RidgeRun/customers/teledyne/teledyne-yocto.git

# Setup the Yocto environment
1. Move into the project directory and set an environment variable for this path:

  cd teledyne-yocto
  TELEDYNE_DEVDIR=${PWD}
  
2. Define a directory path to hold your Yocto build:

YOCTO_DIR=<path-to-yocto-dir>

for example:

  YOCTO_DIR=~/yocto-dir
  
3. Run the setup script:
  
  cd ${TELEDYNE_DEVDIR}
  ./scripts/setup-repo.sh $YOCTO_DIR
  
# Pre-build configurations (optional)
## Set a common path for Yocto downloads
You can modify the default Yocto downloads path, used during the building process. The default path for this is ```${YOCTO_DIR}/build/downloads```.
  
  
In order to avoid re-downloads in future builds it is possible to change this directory by replacing the default path with one of your preference, that can be used as a common downloads directory for multiple Yocto projects. For this, modify the **DL_DIR** variable provided in the ```${YOCTO_DIR}/meta-teledyne/conf/local.conf.sample``` file.

 ## Configure the driver to be built as a Module
By default, the driver is configured as built-in, so it will be compiled as a part of the kernel. Configuring the driver as a module allows to do easier updates, just by replacing the module file, without having to replace the kernel or flashing the board.
  
  
To configure the kernel as a module:
  
  
1. Open the file ```${TELEDYNE_DEVDIR}/meta-teledyne/recipes-kernel/linux/linux-toradex/topaz2m.cfg``` with any text editor of your preference. This file has the following contents:
  
  CONFIG_VIDEO_TELEDYNE_TOPAZ2M=y

The previous line of code enables the driver as built-in. To enable the driver as a module, modify this line so it looks like the following:
  
  CONFIG_VIDEO_TELEDYNE_TOPAZ2M=m

By modifying this file, the driver will be then compiled as a module.
  
# Build the image
1. Initialize the Yocto environment:
cd ${YOCTO_DIR}
export TEMPLATECONF=${PWD}/meta-teledyne/conf
source poky/oe-init-build-env
2. Build the custom image:
bitbake core-image-teledyne
Flash the board with Toradex Easy Installer
To flash the board you need to use the Toradex easy installer.
If you are using the module for the first time, you probably won't need to load the Installer since it will come preinstalled and you'll just need to power on the board to use it.
Loading the Toradex Easy Installer
1. Download the installer for your module from the Toradex Easy Installer Latest Releases
•	Note: The following version is recommended as it was previously tested:
o	Verdin iMX8M Mini (2022-03-30 | 5.6.0+build.9 | 47.07 MB)
2. Extract the downloaded file:
unzip Verdin-iMX8MM_ToradexEasyInstaller_5.6.0+build.9.zip
3. Prepare the board:
•	Connect the board to your PC using a USB-C to USB-A cable on the X34 USB OTG port. Avoid using USB Hubs to connect the board to your PC as recommended by Toradex.
•	Put the board in recovery mode by pressing the Recovery button and while holding it, press the On/Off button, then release both buttons.
4. Go to the extracted directory and run the script:
cd Verdin-iMX8MM_ToradexEasyInstaller_5.6.0+build.9
./recovery-linux.sh
When the script finishes it will display the message "Successfully downloaded Toradex Easy Installer."
5. Once the Toradex Easy Installer is loaded, choose one of the following options to interact with it:
Option A: Use an HDMI Display
If you have a DSI to HDMI adapter on your board:
1. Connect a display with an HDMI cable to the DSI to HDMI adapter.
2. Connect an USB mouse and keyboard to the board, using connector X53.
Option B: Use a VNC client
If you can't use a display, you can access the user interface remotely. For this:
1. Connect the board to your PC using a USB-C to USB-A cable on the X34 USB OTG port, same port used to load the Toradex Easy Installer. The module provides USB connection with the default IP address 192.168.11.1.
2. Install a VNC client on your PC. For this example, xtightvncviewer will be used:
sudo apt install -y xtightvncviewer
3. Run the VNC client with the provided IP:
vncviewer 192.168.11.1
A window showing the Toradex Easy Installer menu will open. You can use your PC's mouse to interact with this menu.
Installing your Yocto Image with Toradex Easy Installer
Once you build a Yocto Image for the IMX module:
1. Get an SD card in fat32 format. You can use gparted to format it.
2. Go to the directory were the Yocto image is placed:
# for Verdin iMX8M Mini
cd ${YOCTO_DIR}/build/deploy/images/verdin-imx8mm
There you should see a .tar a file similar to the following:
core-image-teledyne-verdin-imx8mm-20220823195236-Tezi_5.7.0-devel-20220823195236+build.0.tar
•	Note: The file name may change depending on the build date.
3. Copy the previous file into the SD card and extract it.
4. Prepare the board:
•	Load the Toradex Easy Installer into the board.
•	Access the Toradex Easy Installer using an HDMI Display or VNC client.
•	Plug in the SD Card on the X55 port.
5. The image that you copied to the SD card should appear on the Toradex Easy Installer menu. Click it and install it.
6. Once the image is installed, you can reboot or power off the board.
•	Note: If you are using a VNC client during the installation, choosing the Power off or Restart options might cause the VNC client window to freeze. You can just close the client window.
7. To test the installation, power on the board. You will be prompted for the login information:
verdin-imx8mm login:
Type the word root and press Enter.
Update the driver module
If you built a new version of the driver using bitbake, you can replace the old module with the new one by following the next steps.
•	Note: To update the driver module, the board had to be previously flashed with the driver configured as a module, otherwise there won't be a module file to replace.
1. Verify that the current driver is installed as a module on the IMX8 board:
find / -iname "teledyne-topaz2m.ko"
The previous command will search for the driver module. If the module exists, you should see an output similar to the following:
root@verdin-imx8mm:~# find / -iname "teledyne-topaz2m.ko"
/lib/modules/5.4.193-5.7.0-devel+git.5a24da287b86/kernel/drivers/media/i2c/teledyne-topaz2m.ko
If no file is found, then you will have to flash the complete image before trying to update the driver as a module.
2. Go to the directory where the built module is:
cd ${YOCTO_DIR}/build/tmp/work/verdin_imx8mm-tdx-linux/linux-toradex/5.4.193+gitAUTOINC+5a24da287b-r0/build/drivers/media/i2c
You will find the driver module named teledyne-topaz2m.ko on this directory.
3. Copy the previous file into the board, replacing the file found on step 1. You can do this via SSH, for example:
scp teledyne-topaz2m.ko root@<board-ip>:/lib/modules/5.4.193-5.7.0-devel+git.5a24da287b86/kernel/drivers/media/i2c/
You have to replace <board-ip> with the IP address of your board on the previous command. You can find this value by running the command ifconfig on the board while having an Ethernet cable plugged in.
4. Reboot the IMX8 board, the new driver module will be loaded.
Add packages to the Yocto image
The image recipe file
The yocto image does not include a package manager like apt to install debian packages. Instead of this, packages must be previously included in the image recipe, then the image has to be compiled and flashed.
You will find the custom Yocto image recipe (.bb file) at the path:
${TELEDYNE_DEVDIR}/meta-teledyne/recipes-images/images/core-image-teledyne.bb
In the previous file, the variable IMAGE_INSTALL will contain the main packages that will be included.
Adding packages to the recipe
To add a package to the recipe file, the easiest way is to add them to the list of packages in the IMAGE_INSTALL variable. For example, if you want to add the packages package1 and package2:
IMAGE_INSTALL += " \
    ${CONMANPKGS} \
    ${GRAPHICAL} \
    ${GSTREAMER} \
    i2c-tools \
    v4l-utils \
    xauth \
    package1 \
    package2 \
    "
However, a more recommended practice when you want to modify a recipe, is to create a .bbappend file:
1. Create a file at the same location as the core-image-teledyne.bb file, with the same name, but using the .bbappend extension instead of .bb. Your directory should look like the following:
ls ${TELEDYNE_DEVDIR}/meta-teledyne/recipes-images/images/
core-image-teledyne.bb  core-image-teledyne.bbappend 
2. In the new core-image-teledyne.bbappend file, add the variable IMAGE_INSTALL_append, and assign to this variable the list of packages that you want to add. For example, if you want to add the packages package1 and package2, your file should look like the following:
IMAGE_INSTALL_append = " \
    package1 \
    package2 \
    "
Check for available packages
For a package to be available to install, its recipe has to be included in one of the current meta directories of the Yocto environment. Also, you need to use a specific name for this package, otherwise its recipe will not be found.
If you want to know which name to use, or want to check if a package is available:
1. Go to the Yocto directory and initialize the Yocto environment:
cd ${YOCTO_DIR}
source poky/oe-init-build-env
1. Run the following command, replacing <package-name> with the name of your package or a part of the name.
bitbake-layers show-recipes "*<package-name>*"
For example, if you want to search for a nano related package, run:
bitbake-layers show-recipes "*nano*"
You will get the following output:
=== Matching recipes: ===
nano:
  meta-oe              4.9.3
nanopb:
  meta-oe              0.4.0 (skipped: Recipe is blacklisted: Needs forward porting to use python3)
Here, you can see that there are two packages that match your search, nano and nanopb, its versions and the meta-layers in which their recipes are, in this case, both recipes are included in meta-oe.
Graphic user interface (GUI) usage
Once you power on the board, you can connect an HDMI display using the DSI to HDMI adapter. Once you do, you will see the main green screen of the GUI.
Open a new terminal
To open a new terminal, click the terminal icon at the top left corner of the screen.
Note: If required, you can run the command login to login into the root user.
Virtual terminals
You can have up to 7 independent login sessions. To change between virtual terminals press the keys CTRL + ALT + F<n>, where n can be a number from 1 to 7. By default, the board will use the virtual terminal #2.
Whenever you switch to a new virtual terminal, you will be asked to login and won't have a GUI. To run a GUI on that virtual terminal, run the following command:
weston-launch
Driver controls
The maximum values of some of the following controls depend on the resolution and format used. These maximum values are indicated for each resolution.
Exposure
The exposure time ranges from 0.001 ms to 500 ms. To be able to input these values as integers, the control input values units are in microseconds (μs).
Resolution	Minimum Value	Maximum Value	Default Value	Step
1920x1080 RAW10	5	500000	14040	1
1920x1080 RAW8	5	500000	9180	1
•	Note: Exposure has a limit between the default and the maximum value. If passed, the frame rate of the capture will be reduced and the sensor will raise a flag in register 0x54.
Flip
Horizontal flip
Value	Description
0	Flip disabled
1	Horizontal flip enabled
Vertical flip
Value	Description
0	Flip disabled
1	Vertical flip enabled
Frame rate
•	Note: The maximum frame rate values can only be achieved by using low image sizes and exposure values.
Resolution	Minimum Value	Maximum Value	Default Value	Step
1920x1080 RAW10	5	646	65	1
1920x1080 RAW8	5	988	100	1
Gain
Analog gain
Analog gain ranges from x1 to x16. The control input values go from 0 to 15, with a default value of 0.
Input values	Gain factor	Gain in dB
0	1.00	0.00
1	1.20	1.58
2	1.45	3.23
3	1.71	4.66
4	2.00	6.02
5	2.40	7.60
6	3.00	9.54
7	3.43	10.71
8	4.00	12.04
9	4.80	13.62
10	6.00	15.56
11	6.86	16.73
12	8.00	18.06
13	9.60	19.65
14	12.00	21.58
15	16.00	24.08
Digital gain
Digital gain ranges from x0.004 to x16. The control input values go from 1 to 4096. The gain factor can be calculated as input / 256.
Minimum Value	Maximum Value	Default Value	Step
1	4096	256	1
•	Note: The total gain can be seen as a multiplication of both gains (analog and digital).
Idle enable
This control is used to improve resolution switching while streaming. When changing between ROI1 and ROI2 which might have different heights, it is recommended to set the sensor to Idle before the switch and then set to acquisition again.
Value	Description
0	Disable Idle mode (set sensor to acquisition)
1	Enable Idle mode

Image offset
Mode	Minimum Value	Maximum Value	Default Value	Step
RAW8	-256	255	1	0
RAW10	-512	511	1	0
Region of interest (ROI)
ROI controls allow the user to set the size and position of the region of interest for both: Single ROI and Single integration multiple ROI (SIMR).
To use single ROI, only ROI 1 controls have to be modified. To use SIMR, both ROI 1 and ROI 2 controls have to be modified.
Notes:
•	To enable single ROI or SIMR, the user has to capture the resolution for that ROI mode. See the Before capturing section for more information.
•	For SIMR, the total height of the image will be equal to ROI 1 height + ROI 2 height. It is the responsibility of the user to verify that this sum does not exceed the maximum height that the sensor can capture (1080).
ROI width
The ROI width is shared for both ROI 1 and ROI 2, so there is a single width control.
Minimum Value	Maximum Value	Default Value	Step
64	1920	1920	2
ROI 1 height
This control allows the user to modify the height of ROI 1.
Minimum Value	Maximum Value	Default Value	Step
32	1080	1080	2
ROI 2 height
This control allows the user to modify the height of ROI 2. For SIMR mode, the total height will be ROI 1 height + ROI 2 height.
Minimum Value	Maximum Value	Default Value	Step
0	1080	0	2
ROI column offset
The ROI column offset is shared for both ROI 1 and ROI 2, so there is a single column offset control.
Minimum Value	Maximum Value	Default Value	Step
0	1920	0	1
ROI 1 row offset
This control allows the user to modify the offset in rows for ROI 1.
Minimum Value	Maximum Value	Default Value	Step
0	1080	0	1
ROI 2 row offset
This control allows the user to modify the offset in rows for ROI 2.
•	Note: For SIMR to work, the ROI 2 row offset must be higher than ROI 1 height. Otherwise, the ROI 2 section of the captured image will be black.
Minimum Value	Maximum Value	Default Value	Step
0	1080	0	1
ROI select
Allows the user to select which ROI to stream.
Value	Description
1	Select ROI 1
2	Select ROI 2
Sub sampling
Sub sampling ratio ranges from 1 to 32.
•	Note: Although the sensor supports a sub sampling ratio of 64, this was not included as an option because it produces very small resolutions that are non-standard and can be unstable.
Horizontal sub sampling
Value	Sub sampling ratio
0	1
1	2
2	4
3	8
4	16
5	32
Vertical sub sampling
Value	Sub sampling ratio
0	1
1	2
2	4
3	8
4	16
5	32
Test pattern
Pattern mode	Description
0	Test pattern disabled
1	Front pattern - Diagonal
2	Rear pattern: Diagonal - Fixed
3	Column increment - Fixed
4	Line increment - Fixed
5	Diagonal - Moving
6	Column increment - Moving
7	Line increment - Moving
8	Uniform 0
9	Uniform 20
10	Uniform 127
11	Uniform 255
12	Uniform 511
13	Uniform 1023
Trigger mode
Trigger mode	Description
0	Internal self-trigger
1	External Frame Trigger
2	External ITC Trigger
Driver testing
Check installation
1. Verify the driver was loaded correctly:
dmesg | grep topaz2m
You should get an output similar to the following:
root@verdin-imx8mm:~# dmesg | grep topaz2m
[   80.434668] topaz2m 2-0010: TOPAZ2M sensor found
[   80.439330] mxc_mipi-csi 32e30000.mipi_csi: Registered sensor subdevice: topaz2m 2-0010
2. Check the kernel version and build time:
uname -a
The output should state the kernel version, the date and time the image was compiled:
root@verdin-imx8mm:~# uname -a
Linux verdin-imx8mm 5.4.193-5.7.0-devel+git.5a24da287b86 #1 SMP PREEMPT Wed Aug 24 17:37:44 UTC 2022 aarch64 GNU/Linux
Controls testing and commands
On the IMX8M mini platform, controls can be modified before or after starting the capture. The values assigned to the controls will be kept even after the capture is stopped and will be used in the next capture.
•	Note: Modifying the following controls values once the capture starts is not recommended, since they affect the capture resolution:
o	ROI width
o	ROI 1 height
o	ROI 1 horizontal sub sampling
o	ROI 1 vertical sub sampling
o	ROI 2 height
o	ROI 2 horizontal sub sampling
o	ROI 2 vertical sub sampling
List the controls
1. Display controls and values:
v4l2-ctl -l
The output should be similar to:
User Controls

                     frame_rate 0x0098092b (int)    : min=5 max=988 step=1 default=100 value=100
                 roi_offset_col 0x0098092c (int)    : min=0 max=1920 step=1 default=0 value=0
                     roi_select 0x0098092d (int)    : min=1 max=2 step=1 default=1 value=1
                      roi_width 0x0098092e (int)    : min=64 max=1920 step=2 default=1920 value=1920
              roi_1_analog_gain 0x0098092f (menu)   : min=0 max=15 default=0 value=0
             roi_1_digital_gain 0x00980930 (int)    : min=1 max=4096 step=1 default=256 value=256
                 roi_1_exposure 0x00980931 (int)    : min=5 max=500000 step=1 default=9180 value=9180
                   roi_1_height 0x00980932 (int)    : min=32 max=1080 step=2 default=1080 value=1080
          roi_1_horizontal_flip 0x00980933 (bool)   : default=0 value=0
            roi_1_vertical_flip 0x00980934 (bool)   : default=0 value=0
               roi_1_offset_row 0x00980935 (int)    : min=0 max=1080 step=1 default=0 value=0
  roi_1_horizontal_sub_sampling 0x00980936 (menu)   : min=0 max=5 default=0 value=0
    roi_1_vertical_sub_sampling 0x00980937 (menu)   : min=0 max=5 default=0 value=0
              roi_2_analog_gain 0x00980938 (menu)   : min=0 max=15 default=0 value=0
             roi_2_digital_gain 0x00980939 (int)    : min=1 max=4096 step=1 default=256 value=256
                 roi_2_exposure 0x0098093a (int)    : min=5 max=500000 step=1 default=9180 value=9180
                   roi_2_height 0x0098093b (int)    : min=0 max=1080 step=2 default=0 value=0
          roi_2_horizontal_flip 0x0098093c (bool)   : default=0 value=0
            roi_2_vertical_flip 0x0098093d (bool)   : default=0 value=0
               roi_2_offset_row 0x0098093e (int)    : min=0 max=1080 step=1 default=0 value=0
  roi_2_horizontal_sub_sampling 0x0098093f (menu)   : min=0 max=5 default=0 value=0
    roi_2_vertical_sub_sampling 0x00980940 (menu)   : min=0 max=5 default=0 value=0
                   test_pattern 0x00980941 (menu)   : min=0 max=13 default=0 value=0
                    idle_enable 0x00980942 (int)    : min=0 max=1 step=1 default=0 value=0
                   trigger_mode 0x00980943 (menu)   : min=0 max=2 default=0 value=0
                   image_offset 0x00980944 (int)    : min=-256 max=255 step=1 default=0 value=0
2. Display controls and menus:
v4l2-ctl -L
The output should be similar to:
User Controls

                     frame_rate 0x0098092b (int)    : min=5 max=988 step=1 default=100 value=100
                 roi_offset_col 0x0098092c (int)    : min=0 max=1920 step=1 default=0 value=0
                     roi_select 0x0098092d (int)    : min=1 max=2 step=1 default=1 value=1
                      roi_width 0x0098092e (int)    : min=64 max=1920 step=2 default=1920 value=1920
              roi_1_analog_gain 0x0098092f (menu)   : min=0 max=15 default=0 value=0
				0: 1.00 (0.00 dB)
				1: 1.20 (1.58 dB)
				2: 1.45 (3.23 dB)
				3: 1.71 (4.66 dB)
				4: 2.00 (6.02 dB)
				5: 2.40 (7.60 dB)
				6: 3.00 (9.54 dB)
				7: 3.43 (10.71 dB)
				8: 4.00 (12.04 dB)
				9: 4.80 (13.62 dB)
				10: 6.00 (15.56 dB)
				11: 6.86 (16.73 dB)
				12: 8.00 (18.06 dB)
				13: 9.60 (19.65 dB)
				14: 12.00 (21.58 dB)
				15: 16.00 (24.08 dB)
             roi_1_digital_gain 0x00980930 (int)    : min=1 max=4096 step=1 default=256 value=256
                 roi_1_exposure 0x00980931 (int)    : min=5 max=500000 step=1 default=9180 value=9180
                   roi_1_height 0x00980932 (int)    : min=32 max=1080 step=2 default=1080 value=1080
          roi_1_horizontal_flip 0x00980933 (bool)   : default=0 value=0
            roi_1_vertical_flip 0x00980934 (bool)   : default=0 value=0
               roi_1_offset_row 0x00980935 (int)    : min=0 max=1080 step=1 default=0 value=0
  roi_1_horizontal_sub_sampling 0x00980936 (menu)   : min=0 max=5 default=0 value=0
				0: No sub sampling
				1: Sub sampling 2
				2: Sub sampling 4
				3: Sub sampling 8
				4: Sub sampling 16
				5: Sub sampling 32
    roi_1_vertical_sub_sampling 0x00980937 (menu)   : min=0 max=5 default=0 value=0
				0: No sub sampling
				1: Sub sampling 2
				2: Sub sampling 4
				3: Sub sampling 8
				4: Sub sampling 16
				5: Sub sampling 32
              roi_2_analog_gain 0x00980938 (menu)   : min=0 max=15 default=0 value=0
				0: 1.00 (0.00 dB)
				1: 1.20 (1.58 dB)
				2: 1.45 (3.23 dB)
				3: 1.71 (4.66 dB)
				4: 2.00 (6.02 dB)
				5: 2.40 (7.60 dB)
				6: 3.00 (9.54 dB)
				7: 3.43 (10.71 dB)
				8: 4.00 (12.04 dB)
				9: 4.80 (13.62 dB)
				10: 6.00 (15.56 dB)
				11: 6.86 (16.73 dB)
				12: 8.00 (18.06 dB)
				13: 9.60 (19.65 dB)
				14: 12.00 (21.58 dB)
				15: 16.00 (24.08 dB)
             roi_2_digital_gain 0x00980939 (int)    : min=1 max=4096 step=1 default=256 value=256
                 roi_2_exposure 0x0098093a (int)    : min=5 max=500000 step=1 default=9180 value=9180
                   roi_2_height 0x0098093b (int)    : min=0 max=1080 step=2 default=0 value=0
          roi_2_horizontal_flip 0x0098093c (bool)   : default=0 value=0
            roi_2_vertical_flip 0x0098093d (bool)   : default=0 value=0
               roi_2_offset_row 0x0098093e (int)    : min=0 max=1080 step=1 default=0 value=0
  roi_2_horizontal_sub_sampling 0x0098093f (menu)   : min=0 max=5 default=0 value=0
				0: No sub sampling
				1: Sub sampling 2
				2: Sub sampling 4
				3: Sub sampling 8
				4: Sub sampling 16
				5: Sub sampling 32
    roi_2_vertical_sub_sampling 0x00980940 (menu)   : min=0 max=5 default=0 value=0
				0: No sub sampling
				1: Sub sampling 2
				2: Sub sampling 4
				3: Sub sampling 8
				4: Sub sampling 16
				5: Sub sampling 32
                   test_pattern 0x00980941 (menu)   : min=0 max=13 default=0 value=0
				0: Disabled
				1: Front pattern - Diagonal
				2: Rear pattern: Diagonal - Fixed
				3: Column increment - Fixed
				4: Line increment - Fixed
				5: Diagonal - Moving
				6: Column increment - Moving
				7: Line increment - Moving
				8: Uniform 0
				9: Uniform 20
				10: Uniform 127
				11: Uniform 255
				12: Uniform 511
				13: Uniform 1023
                    idle_enable 0x00980942 (int)    : min=0 max=1 step=1 default=0 value=0
                   trigger_mode 0x00980943 (menu)   : min=0 max=2 default=0 value=0
				0: Internal self-trigger
				1: External Frame Trigger
				2: External ITC Trigger
                   image_offset 0x00980944 (int)    : min=-256 max=255 step=1 default=0 value=0
Analog gain
v4l2-ctl -d /dev/video0 -c roi_1_analog_gain=<value>
v4l2-ctl -d /dev/video0 -c roi_2_analog_gain=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_analog_gain=10
v4l2-ctl -d /dev/video0 -c roi_2_analog_gain=10
Digital gain
v4l2-ctl -d /dev/video0 -c roi_1_digital_gain=<value>
v4l2-ctl -d /dev/video0 -c roi_2_digital_gain=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_digital_gain=2000
v4l2-ctl -d /dev/video0 -c roi_2_digital_gain=2000
Exposure
v4l2-ctl -d /dev/video0 -c roi_1_exposure=<value>
v4l2-ctl -d /dev/video0 -c roi_2_exposure=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_exposure=15000
v4l2-ctl -d /dev/video0 -c roi_2_exposure=15000
Frame rate
v4l2-ctl -d /dev/video0 -c frame_rate=<value>
Example:
v4l2-ctl -d /dev/video0 -c frame_rate=60
•	Note: The maximum frame rate value changes depending on the resolution. Check the Driver controls section for more details on maximum values.
Horizontal flip
v4l2-ctl -d /dev/video0 -c roi_1_horizontal_flip=<value>
v4l2-ctl -d /dev/video0 -c roi_2_horizontal_flip=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_horizontal_flip=1
v4l2-ctl -d /dev/video0 -c roi_2_horizontal_flip=1
Vertical flip
v4l2-ctl -d /dev/video0 -c roi_1_vertical_flip=<value>
v4l2-ctl -d /dev/video0 -c roi_2_vertical_flip=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_vertical_flip=1
v4l2-ctl -d /dev/video0 -c roi_2_vertical_flip=1
Width
v4l2-ctl -d /dev/video0 -c roi_width=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_width=400
Height
v4l2-ctl -d /dev/video0 -c roi_1_height=<value>
v4l2-ctl -d /dev/video0 -c roi_2_height=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_height=200
v4l2-ctl -d /dev/video0 -c roi_2_height=200
Column offset
v4l2-ctl -d /dev/video0 -c roi_offset_col=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_offset_col=450
Row offset
v4l2-ctl -d /dev/video0 -c roi_1_offset_row=<value>
v4l2-ctl -d /dev/video0 -c roi_2_offset_row=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_offset_row=425
v4l2-ctl -d /dev/video0 -c roi_2_offset_row=425
Horizontal sub sampling
v4l2-ctl -d /dev/video0 -c roi_1_horizontal_sub_sampling=<value>
v4l2-ctl -d /dev/video0 -c roi_2_horizontal_sub_sampling=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_horizontal_sub_sampling=1
v4l2-ctl -d /dev/video0 -c roi_2_horizontal_sub_sampling=1

•	Note: The capture width will change if a horizontal sub sampling ratio is applied. Use the command v4l2-ctl -d /dev/video0 --list-formats-ext to check for the image size when changing the control value.
Vertical sub sampling
v4l2-ctl -d /dev/video0 -c roi_1_vertical_sub_sampling=<value>
v4l2-ctl -d /dev/video0 -c roi_2_vertical_sub_sampling=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_1_vertical_sub_sampling=1
v4l2-ctl -d /dev/video0 -c roi_2_vertical_sub_sampling=1

•	Note: The capture height will change if a vertical sub sampling ratio is applied. Use the command v4l2-ctl -d /dev/video0 --list-formats-ext to check for the image size when changing the control value.
Test pattern
v4l2-ctl -d /dev/video0 -c test_pattern=<value>
Example:
v4l2-ctl -d /dev/video0 -c test_pattern=2
ROI select
v4l2-ctl -d /dev/video0 -c roi_select=<value>
Example:
v4l2-ctl -d /dev/video0 -c roi_select=2
Idle enable
v4l2-ctl -d /dev/video0 -c idle_enable=<value>
Example:
v4l2-ctl -d /dev/video0 -c idle_enable=0
Image offset
v4l2-ctl -d /dev/video0 -c image_offset=<value>
Example:
v4l2-ctl -d /dev/video0 -c image_offset=100
Before capturing
Verify supported formats
Command to list the formats:
v4l2-ctl -d /dev/video0 --list-formats-ext
Expected output:
root@verdin-imx8mm:~# v4l2-ctl -d /dev/video0 --list-formats-ext

ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture

	[0]: 'GREY' (8-bit Greyscale)
		Size: Discrete 1920x1080  # ROI 1 resolution
		Size: Discrete 1920x0     # ROI 2 resolution
		Size: Discrete 1920x1080  # SIMR resolution
	[1]: 'Y10 ' (10-bit Greyscale)
		Size: Discrete 1920x1080  # ROI 1 resolution
		Size: Discrete 1920x0     # ROI 2 resolution
		Size: Discrete 1920x1080  # SIMR resolution
Considerations for the user
Some considerations for the user before starting the capture are listed below:
Resolution limitation
•	The IMX8M Mini platform has the following resolution limitation:
o	When using an 8-bit/pixel format the IMAGE WIDTH should be a multiple of 8 pixels.
o	When using a 10-bit/pixel format the IMAGE_WIDTH should be a multiple of 4 pixels.
You can check for this information at section 13.4.4.14 CSI Image Parameter Register of the i.MX 8M Mini Applications Processor Reference Manual Rev. 3.
Checking for supported resolutions
•	The supported resolutions will change if a sub sampling ratio is applied or ROI control values are modified. It is recommended to check for the current supported resolutions with the command v4l2-ctl -d /dev/video0 --list-formats-ext before capturing.
Enable single ROI or SIMR
To stream from ROI 1 or ROI 2:
•	Select the ROI to capture from using the roi_select control.
•	Run the capture command using the current supported resolution of the selected ROI.
To stream from SIMR:
•	Run the capture command using the current supported SIMR resolution, no controls need to be set.

For example, if an ROI 1 of 1920x500 is set, and an ROI 2 of 1920x200 is set, the suported resolutions will change to:
ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture

	[0]: 'GREY' (8-bit Greyscale)
		Size: Discrete 1920x500
		Size: Discrete 1920x200
		Size: Discrete 1920x700
	[1]: 'Y10 ' (10-bit Greyscale)
		Size: Discrete 1920x500
		Size: Discrete 1920x200
		Size: Discrete 1920x700
•	In the previous example, to capture from ROI 1, the capture resolution should be 1920x500. To enable SIMR, the capture resolution should be 1920x700.
Capture with v4l2-ctl
Test the capture framerate
1920x800 at 80 fps, RAW10
Set the resolution:
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=800
Capture command:
v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=800,pixelformat='Y10 ' --stream-mmap
Expected output:
root@verdin-imx8mm:~# v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=800,pixelformat='Y10 ' --stream-mmap
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 80.04 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 80.04 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 80.04 fps
1920x1080 at 65 fps, RAW10
Set the resolution:
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=1080
Capture command:
v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='Y10 ' --stream-mmap
Expected output:
root@verdin-imx8mm:~# v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='Y10 ' --stream-mmap
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 65.02 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 65.02 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 65.02 fps
1920x800 at 130 fps, RAW8
Set the resolution:
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=800
Capture command:
v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=800,pixelformat='GREY' --stream-mmap
Expected output:
root@verdin-imx8mm:~# v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=800,pixelformat='GREY' --stream-mmap
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 130.00 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 130.00 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 130.00 fps
1920x1080 at 100 fps, RAW8
Set the resolution:
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=1080
Capture command:
v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='GREY' --stream-mmap
Expected output:
root@verdin-imx8mm:~# v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='GREY' --stream-mmap
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 100.04 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 100.04 fps
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 100.04 fps
Capture a single frame
To capture a single frame, set the desired resolution and pixel format, for example, for 1920x800 RAW10:
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=800 --set-fmt-video=width=1920,height=800,pixelformat='Y10 '
Then run the capture command:
v4l2-ctl -d /dev/video0 --stream-mmap --stream-count=1 --stream-to=test_frame_1920x800_raw10.raw
Notes:
•	You can choose any number of frames by setting the --stream-count variable and the name of the output file by setting the --stream-to variable.
•	These captured frames can be visualized with rawpixels or vooya in your host PC. Please consider the following settings to be able to view them correctly.
rawpixels configuration for 1920x800 at 80 fps, RAW10
•	width: 1920
•	height: 800
•	Predefined format: Grayscale 8bit
•	Pixel format: Grayscale
•	bpp1: 16
rawpixels configuration for 1920x1080 at 65 fps, RAW10
•	width: 1920
•	height: 1080
•	Predefined format: Grayscale 8bit
•	Pixel format: Grayscale
•	bpp1: 16
rawpixels configuration for 1920x800 at 130 fps, RAW8
•	width: 1920
•	height: 800
•	Predefined format: Grayscale 8bit
•	Pixel format: Grayscale
•	bpp1: 8
rawpixels configuration for 1920x1080 at 100 fps, RAW8
•	width: 1920
•	height: 1080
•	Predefined format: Grayscale 8bit
•	Pixel format: Grayscale
•	bpp1: 8
vooya configuration for 1920x800 at 80 fps, RAW10
•	width: 1920
•	height: 800
•	Frames/Second: 80
•	Color Space: Single Channel
•	Data Container: Single Integer
•	Bit Depth (Value): 10bit
vooya configuration for 1920x1080 at 65 fps, RAW10
•	width: 1920
•	height: 1080
•	Frames/Second: 65
•	Color Space: Single Channel
•	Data Container: Single Integer
•	Bit Depth (Value): 10bit
vooya configuration for 1920x800 at 130 fps, RAW8
•	width: 1920
•	height: 800
•	Frames/Second: 130
•	Color Space: Single Channel
•	Data Container: Single Integer
•	Bit Depth (Value): 8bit
vooya configuration for 1920x1080 at 100 fps, RAW8
•	width: 1920
•	height: 1080
•	Frames/Second: 100
•	Color Space: Single Channel
•	Data Container: Single Integer
•	Bit Depth (Value): 8bit
Capture with GStreamer
•	Note: Y10 is not supported by v4l2src. For this reason, the following pipeline is for testing the RAW8 configuration.
1920x800 at 130 fps, RAW8
1. Before executing the pipeline, set the ROI size and the format.
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=800
v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=800,pixelformat='GREY'
2. Run the capture pipeline:
gst-launch-1.0 v4l2src ! "video/x-raw,width=1920,height=800,format=GRAY8" ! queue ! videoconvert ! queue ! ximagesink sync=false
1920x1080 at 100 fps, RAW8
1. Before executing the pipeline, set the ROI size and the format.
v4l2-ctl -d /dev/video0 -c roi_width=1920,roi_1_height=1080
v4l2-ctl -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='GREY'
2. Run the capture pipeline:
gst-launch-1.0 v4l2src ! "video/x-raw,width=1920,height=1080,format=GRAY8" ! queue ! videoconvert ! queue ! ximagesink sync=false
Reading I2C bus address registers
For reading and writing the sensor registers, the sensor has to be capturing video using v4l2-ctl or GStreamer. Since the addresses to be read are 8 bits and need to give back a value of 16 bits, the i2ctransfer command can be used to read these addresses as follows:
i2ctransfer -f -y [BUS NUMBER] w[BYTES]@[ADDRESS] [REGISTER] r[BYTES]
For example, to read the register 0x7F (chip ID) of the 0x10 address in i2c 2 bus, it is needed to write 1 byte (0x7F) to the 0x10 address and read 2 bytes (r[BYTES]) from this register. So, the full command is:
i2ctransfer -f -y 2 w1@0x10 0x7F r2
The register gives back:
root@verdin-imx8mm:~# i2ctransfer -f -y 2 w1@0x10 0x7F r2
0x80 0x36
Writing I2C bus address
For reading, and also writing, I2C bus address registers the sensor has to be capturing video using v4l2-ctl or GStreamer.
i2ctransfer -f -y [BUS NUMBER] w[BYTES]@[ADDRESS] [REGISTER] [MSB VALUE TO WRITE] [MSB VALUE TO READ]
For example, to write to the register 0x0B (Integration Time) of the 0x10 address in i2c 2 bus, it is needed to write 3 bytes (w[BYTES]) to the register 0x0B of the 0x10 address. 1 byte is for the register to be written (0x0B) and the other 2 bytes are the value to be written in this register. So, the full command for this example is:
For example:
i2ctransfer -f -y 2 w3@0x10 0x0b 0x30 0x00
If the register 0x0B is read:
root@verdin-imx8mm:~# i2ctransfer -f -y 2 w1@0x10 0x0b r2
0x30 0x00
Reading pixel timing values
The pixel timing values have to be read when the sensor is capturing the video using v4l2-ctl or GStreamer.
1. To read the pixel timing registers it is required to place the sensor in standby mode:
i2ctransfer -f -y 2 w3@0x10 0x02 0x00 0x01
2. Change the offset register value, for example:
i2ctransfer -f -y 2 w3@0x10 0x4f 0x00 0x00
3. Read the value in the RAM:
i2ctransfer -f -y 2 w1@0x10 0x51 r2
root@verdin-imx8mm:~# i2ctransfer -f -y 2 w1@0x10 0x51 r2
0x05 0x52
4. When finished reading the pixel timing values, place the sensor in acquisition mode:
i2ctransfer -f -y 2 w3@0x10 0x02 0x00 0x08
