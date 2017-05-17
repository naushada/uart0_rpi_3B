#ifndef __FPS_C__
#define __FPS_C__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fps.h"

/*Global Data structure initialization*/

fps_generic_packet_t      command_packet_g;
fps_generic_packet_t      response_packet_g;
fps_generic_data_packet_t response_data_packet_g;

uint16 fps_checksum(uint8 *byte_ptr, uint16 len) {
  uint8 idx = 0;
  uint16 chk_sum = 0;

  for(idx = 0; idx < len; idx++) {
    chk_sum += byte_ptr[idx];
  }
  return(chk_sum);

}/*fps_checksum*/


uint8 *fps_open(uint8 with_data_packet, uint16 device_id, uint8 *out_len) {
  uint8 *byte_ptr = NULL;
  uint16 chk_sum = 0x00;
  
  fps_generic_packet_t *command_ptr = &command_packet_g;
  byte_ptr = (uint8 *)command_ptr;

  /*Preparing the command*/
  command_ptr->hdr.start_code1 = 0x55;
  *out_len += 1;
  command_ptr->hdr.start_code2 = 0xAA;
  *out_len += 1;

  /*Packing of data as little endian*/
  command_ptr->hdr.device_id[0] = device_id & 0xFF;
  *out_len += 1;
  command_ptr->hdr.device_id[1] = (device_id >> 8) & 0xFF;
  *out_len += 1;
  memset((void *)command_ptr->param, 0, sizeof(command_ptr->param));

  /*if with_data_packet is zero, then response data will not be present in response*/
  command_ptr->param[0] = with_data_packet & 0xFF;
  *out_len += 4;

  /*Populating command code in little endian fashion*/
  memset((void *)command_ptr->code, 0, sizeof(command_ptr->code));
  command_ptr->code[0] = FPS_OPEN & 0xFF;
  *out_len += 1;
  command_ptr->code[1] = (FPS_OPEN >> 8) & 0xFF;
  *out_len += 1;

  chk_sum = fps_checksum(byte_ptr, *out_len);
  command_ptr->chk_sum[0] = chk_sum & 0xFF;
  *out_len += 1;
  command_ptr->chk_sum[1] = (chk_sum >> 8) & 0xFF;  
  *out_len += 1;

  return((uint8 *)command_ptr);
  
}/*fps_open*/

uint8 *fps_sensor_ctl(uint8 sensor_on_off, uint16 device_id, uint8 *out_len) {
  uint8 *byte_ptr = NULL;
  uint16 chk_sum = 0x00;
  
  fps_generic_packet_t *command_ptr = &command_packet_g;
  byte_ptr = (uint8 *)command_ptr;

  /*Preparing the command*/
  command_ptr->hdr.start_code1 = 0x55;
  *out_len += 1;
  command_ptr->hdr.start_code2 = 0xAA;
  *out_len += 1;

  /*Packing of data as little endian*/
  command_ptr->hdr.device_id[0] = device_id & 0xFF;
  *out_len += 1;
  command_ptr->hdr.device_id[1] = (device_id >> 8) & 0xFF;
  *out_len += 1;
  memset((void *)command_ptr->param, 0, sizeof(command_ptr->param));
  
  /*if with_data_packet is zero, then response data will not be present in response*/
  command_ptr->param[0] = sensor_on_off & 0xFF;
  *out_len += 1;
  command_ptr->param[1] = (sensor_on_off >> 8) & 0xFF;
  *out_len += 1;
  command_ptr->param[2] = (sensor_on_off >> 16) & 0xFF;
  *out_len += 1;
  command_ptr->param[3] = (sensor_on_off >> 24) & 0xFF;
  *out_len += 1;

  /*Populating command code in little endian fashion*/
  memset((void *)command_ptr->code, 0, sizeof(command_ptr->code));
  command_ptr->code[0] = FPS_CMOS_LED & 0xFF;
  *out_len += 1;
  command_ptr->code[1] = (FPS_CMOS_LED >> 8) & 0xFF;
  *out_len += 1;
  chk_sum = fps_checksum(byte_ptr, *out_len);
  command_ptr->chk_sum[0] = chk_sum & 0xFF;
  *out_len += 1;
  command_ptr->chk_sum[1] = (chk_sum >> 8) & 0xFF;  
  *out_len += 1;

  return((uint8 *)command_ptr);
  
}/*fps_sensor_ctl*/



#endif
