#ifndef __FPS_H__
#define __FPS_H__


#define uint8   unsigned char
#define int8    char
#define uint16  unsigned short int
#define int16   short int
#define uint32  unsigned int
#define int32   int
#define uint64  unsigned long
#define int63   long

/*Command Type*/
typedef enum {
  FPS_OPEN = 1,
  FPS_CLOSE,
  FPS_USB_INTERNAL_CHECK,
  FPS_CHANGE_BAUDRATE,
  /*For upgrading the Firmware of FPS - Finger Print Scanner*/
  FPS_SET_IAP_MODE,
  FPS_CMOS_LED = 0x12,
  FPS_GET_ENROLL_COUNT = 0x20,
  FPS_CHECK_ENROLLED,
  FPS_ENROLL_START,
  FPS_ENROLL1,
  FPS_ENROLL2,
  FPS_ENROLL3,
  FPS_IS_PRESS_FINGER,
  FPS_DELETE_ID = 0x40,
  FPS_DELETE_ALL,
  FPS_VERIFY = 0x50,
  FPS_IDENTIFY,
  FPS_VERIFY_TEMPLATE,
  FPS_IDENTIFY_TEMPLATE,
  FPS_CAPTURE_FINGER = 0x60,
  FPS_MAKE_TEMPLATE,
  FPS_GET_IMAGE,
  FPS_GET_RAW_IMAGE,
  FPS_GET_TEMPLATE = 0x70,
  FPS_SET_TEMPLATE,
  FPS_ACK = 0x30,
  FPS_NACK
  
}fps_command_type_t;
/*Error code*/

typedef enum {
  FPS_NACK_TIMEOUT = 0x1001,
  FPS_NACK_INVALID_BAUDRATE,
  /*The Specified ID is not between 0 - 199*/
  FPS_NACK_INVALID_POS,
  FPS_NACK_IS_NOT_USED,
  FPS_NACK_IS_ALREADY_USED,
  FPS_NACK_COMM_ERR,
  FPS_NACK_VERIFY_FAILED,
  FPS_NACK_IDENTIFY_FAILED,
  FPS_NACK_DB_IS_FULL,
  FPS_NACK_DB_IS_EMPTY,
  FPS_NACK_TURN_ERR,
  FPS_BAD_FINGER,
  FPS_NACK_ENROLL_FAILED,
  FPS_NACK_IS_NOT_SUPPORTED,
  /*Device Error, Especially If crypto-chip is trouble*/
  FPS_NACK_DEV_ERR,
  FPS_NACK_CAPTURE_CANCELLED,
  FPS_NACK_INVALID_PARAM,
  FPS_NACK_FINGER_IS_NOT_PRESSED,
  
}fps_err_code_t;


typedef struct {
  /*Value 0x55 is Fixed*/
  uint8   start_code1;
  /*Value 0xAA is Fixed*/
  uint8   start_code2;
  /*Device ID shall be fixed as = 0x0001*/
  uint8   device_id[2];

}fps_header_t;

/*Command/Request format*/

typedef struct {
  fps_header_t hdr;
  /*Parameter of command or response*/
  uint8   param[4];
  /*Command code or response code (ACK/NACK)*/
  uint8   code[2];
  /*Check sum is the addition/sum of all above data member.*/
  uint8   chk_sum[2]; 
  
}fps_generic_packet_t;

typedef struct {
  fps_header_t hdr;
  uint8        data_length;
  /*The maximum data packet size could be 52,116 bytes*/
  uint8        data[55000];
  /*Check sum of all above bytes/data member*/
  uint8        chk_sum[2];
 
}fps_generic_data_packet_t;

/*Function Prototype*/
uint8 *fps_open(uint8 with_data_packet, uint16 device_id, uint8 *out_len);

uint16 fps_checksum(uint8 *byte_ptr, uint16 len);

uint8 *fps_sensor_ctl(uint8 sensor_on_off, uint16 device_id, uint8 *out_len);

#endif
