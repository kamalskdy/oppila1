DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT += test-ldr-sensor test-bmp180 test-adxl345 photo_sensor sound-sensor touch_sensor resistor

CONTIKI_TARGET_SOURCEFILES += owinos-adc.c bmp180.c adxl345.c

all: $(CONTIKI_PROJECT)

CONTIKI = ../../
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
