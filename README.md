# M0sense_BL702_example

## 0. Clone this repository

Go to any directory you want and type these below:

```shell
git clone https://github.com/sipeed/M0sense_BL702_example.git
cd M0sense_BL702_example
```

And there are several compiled apps under `uf2_demos`.

You can just hold the `BOOT` btn near the `RESET` and then reset the board. Tap `RESET` or repower due to you.

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

## 2. Update later and later~~ Sorry:)