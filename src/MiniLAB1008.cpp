#include "Controller.h"
#include "MiniLAB1008.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#include <libhid/pmd.h>
#include <libhid/minilab-1008.h>

MiniLAB1008::MiniLAB1008(Controller *c) : Pedal(c)
{
  ret = hid_init();
  hid = NULL;
}

void MiniLAB1008::run() {
  int flag;
  signed short svalue;
  int temp;
  __u8 channel, gain;
  int interface;
  
  while (ret != HID_RET_SUCCESS) {
	std::cout << "Waiting for HID_RED_SUCCESS..." << std::endl;
        sleep(1000);
  }

  while ((interface = PMD_Find_Interface(&hid, 0, MINILAB1008_PID)) < 0) {
	std::cout << "Searching for MiniLab 1008 (PMD_Find_Interface returned "
                  << interface << ")" << std::endl;
        sleep(1000);
  } 
  temp = 0;
  channel = (__u8) temp;
  gain = SE_10_00V;
  flag = fcntl(fileno(stdin), F_GETFL);
  fcntl(0, F_SETFL, flag | O_NONBLOCK);
  svalue = 0;
  while (1) {
    sleep(10);
    signed short last = svalue;
    svalue = usbAIn_miniLAB1008(hid, channel, gain);
    // Pedal seems to be generating numbers in the range 0 to 512
    if (last != svalue) {
      value = (double)svalue/512.0;
      controller->speedChange(value);
      // std::cout << "Sent new speed = " << value << std::endl;
    }

  } // end forever
}
