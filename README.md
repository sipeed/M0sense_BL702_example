# M0sense_BL702_example

## Foreword

**Structure Of This Project**

```shell
❯ tree -d -L 2
.
├── bl_mcu_sdk      # bl_mcu_sdk freezed at v1.4.5 
│   ├── bsp
│   ├── common
│   ├── components
│   ├── docs
│   ├── drivers
│   ├── examples
│   ├── tools
│   └── zephyr
├── m0sense_apps    # m0sense_apps(demo) list inside
│   ├── blink           # rgb led blink
│   ├── hello_world     # echo hello, world on your usb
│   ├── lcd_flush       # flush lcd with random color
│   └── rtos_demos      # some 
├── misc            # some other not categorized 
│   ├── firmware        # base firmware support drag-and-drop operation
│   ├── sdk_patch       # patch of bl_mcu_sdk@v1.4.5 for further support
│   └── utils           # some utils for easy use
└── uf2_demos       # much pre-converted uf2 files just for your drag-and-drop
```

We recommend applying patch on `bl_mcu_sdk` for further feature support.
So make sure your app is developed with completely patched `bl_mcu_sdk` if necessary.

Now you can just jump to *[0. Clone this repository and have try](#0-clone-this-repository-and-have-try)* and try these pre-compiled apps.

## 0. Clone this repository and have try

Go to any directory you want and type these below:

```shell
git clone https://github.com/sipeed/M0sense_BL702_example.git
cd M0sense_BL702_example
```

And there are several compiled apps under `uf2_demos`.

You can just hold the `Boot` button near the `Reset` button and then reset the board, Tap `Reset` button or repower just due to you.

And then there will be a REMOVABLE UDISK labeled `M0SENSE` appear on your PC where you can put the uf2 apps.

**THEN JUST DROP ONE OF THEM INTO THE DISK AND IT WILL SOON AUTO REBOOT TO YOUR CHOSEN APP, ENJOY IT.**

If you want to compile it on yourself or fethermore develop your app based on M0Sense.

**SO KEEP GOING**

## 1. Prepare bl_mcu_sdk (may need much time for some reson)

We take the advice from Bouffalolab to import the [bl_mcu_sdk](https://github.com/bouffalolab/bl_mcu_sdk/blob/release_v1.4.5/README.md#how-to-make-sdk-as-submodule) as submodule.

You just need to type this command below after you cloned this repository.

```shell
$ git submodule update --init
```
And then wait for it to finish.

After that, you have to APPLY patch needed.

```bash
./build.sh patch
```

If you have not used `GIT`, maybe run this command below first. If you understand what this is, you should know whether to do it.

```bash
git config user.email "m0sense@sipeed.com"
git config user.name "tinymaix"
```

## 2. Compile in yourself

There are several demos available under `m0sense_apps`. We also provide a shell script named `build.sh`.

Simply type these below and press Enter, there will be the newest `blink_baremetal.uf2` under `uf2_demos` waiting for darg-and-drop.

```shell
./build.sh m0sense_apps/blink/blink_baremetal
``` 

Just type `./build.sh m0sense_apps` and press `Tab` more than once, then you can choose what to be compiled after many auto completion until it's directory just like above.
