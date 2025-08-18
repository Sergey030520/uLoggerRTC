#!/bin/bash

st-flash erase
st-flash write build/uLoggerRTC.bin 0x8000000