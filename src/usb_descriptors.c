/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2023 sorasen2020
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "usb_descriptors.h"

#define USB_PID   (0x0092)
#define USB_VID   (0x0f0d)
#define USB_BCD   (0x0200)

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID1   (0x81)
#define EPNUM_HID2   (0x82)



//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_report1[] =
{
  0x05, 0x01,			//	Usage Page (Generic Desktop Ctrls)
	0x09, 0x05,			//	Usage (Game Pad)
	0xA1, 0x01,			//	Collection (Application)

	0x15, 0x00,			//		Logical Minimum (0)
	0x25, 0x01,			//		Logical Maximum (1)
	0x35, 0x00,			//		Physical Minimum (0)
	0x45, 0x01,			//		Physical Maximum (1)
	0x75, 0x01,			//		Report Size (1)
	0x95, 0x10,			//		Report Count (16)
	0x05, 0x09,			//		Usage Page (Button)
	0x19, 0x01,			//		Usage Minimum (0x01)
	0x29, 0x10,			//		Usage Maximum (0x10)
	0x81, 0x02,			//		Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

	0x05, 0x01,			//		Usage Page (Generic Desktop Ctrls)
	0x25, 0x07,			//		Logical Maximum (7)
	0x46, 0x3B, 0x01,	//		Physical Maximum (315)
	0x75, 0x04,			//		Report Size (4)
	0x95, 0x01,			//		Report Count (1)
	0x65, 0x14,			//		Unit (System: English Rotation, Length: Centimeter)
	0x09, 0x39,			//		Usage (Hat switch)
	0x81, 0x42,			//		Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)

	0x65, 0x00,			//		Unit (None)
	0x95, 0x01,			//		Report Count (1)
	0x81, 0x01,			//		Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

	0x26, 0xFF, 0x00,	//		Logical Maximum (255)
	0x46, 0xFF, 0x00,	//		Physical Maximum (255)
	0x09, 0x30,			//		Usage (X)
	0x09, 0x31,			//		Usage (Y)
	0x09, 0x32,			//		Usage (Z)
	0x09, 0x35,			//		Usage (Rz)
	0x75, 0x08,			//		Report Size (8)
	0x95, 0x04,			//		Report Count (4)
	0x81, 0x02,			//		Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

	0x06, 0x00, 0xFF,	//		Usage Page (Vendor Defined 0xFF00)
	0x09, 0x20,			//		Usage (0x20)
	0x95, 0x01,			//		Report Count (1)
	0x81, 0x02,			//		Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

	0x0A, 0x21, 0x26,	//		Usage (0x2621)
	0x95, 0x08,			//		Report Count (8)
	0x91, 0x02,			//		Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

	0xC0,				//	End Collection
};

uint8_t const desc_hid_report2[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
  if (itf == ITF_NUM_HID1)
  {
    return desc_hid_report1;
  }
  else if (itf == ITF_NUM_HID2)
  {
    return desc_hid_report2;
  }

  return NULL;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+


uint8_t const desc_configuration[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

  // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_HID1, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report1), EPNUM_HID1, CFG_TUD_HID_EP_BUFSIZE, 5),
  TUD_HID_DESCRIPTOR(ITF_NUM_HID2, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report2), EPNUM_HID2, CFG_TUD_HID_EP_BUFSIZE, 5)
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations

  // This example use the same configuration for both high and full speed mode
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr [] =
{
  (const char[]) {0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "HORI,Co.,LTD,",              // 1: Manufacturer
  "POKKEN CONTROLLER",          // 2: Product
  "0",                          // 3: Serials, should use chip ID
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

  return _desc_str;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // TODO set LED based on CAPLOCK, NUMLOCK etc...
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}

int32_t tud_msc_read10_cb (uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	return -1;
}

int32_t tud_msc_write10_cb (uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	return -1;
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
}

bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	return false;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
}

int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	return -1;
}
