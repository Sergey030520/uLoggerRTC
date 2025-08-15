#!/bin/bash

st-flash erase
st-flash write build/rtc_prg.bin 0x8000000