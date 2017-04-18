// FILE: vscp.h 
//
// This file is part of the VSCP (http://www.vscp.org) 
//
// The MIT License (MIT)
// 
// Copyright (c) 2000-2017 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//            !!!!!!!!!!!!!!!!!!!!  W A R N I N G  !!!!!!!!!!!!!!!!!!!!
// This file may be a copy of the original file. This is because the file is
// copied to other projects as a convinience. Thus editing the copy will not make
// it to the original and will be overwritten.
// The original file can be found in the vscp_softare source tree under 
// src/vscp/common 

#ifndef _VSCP_H_
#define _VSCP_H_

#include <vscp_class.h>
#include <vscp_type.h>
#include <canal.h>
#include <crc.h>
#include <inttypes.h>


// VSCP daemon defines
#define VSCP_MAX_CLIENTS                    4096    // abs. max. is 0xffff
#define VSCP_MAX_DEVICES                    4096    // abs. max. is 0xffff

//
// daemon
//
// Base values for queue or channel id's
//
#define VSCP_CLIENT_ID_BASE                 0x00000000
#define VSCP_DEVICE_ID_BASE                 0x00010000


/// Names of mutex's
#define VSCPD_CLIENT_OBJ_MUTEX              _("____VSCPD_CLIENT_OBJ_MUTEX____")
#define VSCPD_UDP_OBJ_MUTEX                 _("____VSCPD_UDP_OBJ_MUTEX____")
#define VSCPD_SEND_OBJ_MUTEX                _("____VSCPD_SEND_OBJ_MUTEX____")
#define VSCPD_RECEIVE_OBJ_MUTEX             _("____VSCPD_RECEIVE_OBJ_MUTEX____")
#define VSCPD_CLIENT_MUTEX                  _("____VSCPD_CLIENT_MUTEX____")

#define	VSCP_LEVEL2_UDP_PORT                9598
#define	VSCP_LEVEL2_TCP_PORT                9598

#define VSCP_ADDRESS_SEGMENT_CONTROLLER	    0x00
#define VSCP_ADDRESS_NEW_NODE               0xff

#define VSCP_MAX_DATA                       (512-25)

#define VSCP_LEVEL1                         0           // Changed 151104  Was 1/2
#define VSCP_LEVEL2                         1

#ifdef __cplusplus
extern "C" {
#endif

    //          * * * General structure for VSCP * * *

    // This structure is for VSCP Level II
    

    typedef struct {
        uint16_t crc;           // crc checksum (calculated from here to end)
                                // Used for UDP/Ethernet etc
       
        uint32_t obid;          // Used by driver for channel info etc.        
        
        // Time block - Always UTC time
        uint16_t year; 
        uint8_t month;	// 1-12
        uint8_t day;	// 1-31
        uint8_t hour;	// 0-23
        uint8_t minute;	// 0-59
        uint8_t second;	// 0-59
        
        uint32_t timestamp;     // Relative time stamp for package in microseconds
                                // ~71 minutes before roll over
        
        // ----- CRC should be calculated from here to end + data block ----
        
        uint16_t head;          // Bit 15   GUID is IP v.6 address.
                                // Bit 8-14 = Reserved
                                // bit 765  priority, Priority 0-7 where 0 is highest.
                                // bit 4 = hard coded, true for a hard coded device.
                                // bit 3 = Don't calculate CRC, false for CRC usage.
                                //          Just checked when CRC is used.        
                                // bit 2 = Reserved.
                                // bit 1 = Reserved.
                                // bit 0 = Reserved.
        uint16_t vscp_class;    // VSCP class
        uint16_t vscp_type;     // VSCP type
        uint8_t GUID[ 16 ];     // Node globally unique id MSB(0) -> LSB(15)
        uint16_t sizeData;      // Number of valid data bytes
        
        uint8_t *pdata;         // Pointer to data. Max 487 (512- 25) bytes
        
    } /*__attribute__((packed, aligned(1)))*/ vscpEvent;


typedef vscpEvent *PVSCPEVENT;



// This structure is for VSCP Level II with data embedded == big!!!

typedef struct { 
    
    uint16_t crc;                   // crc checksum (calculated from here to end)
                                    // Used for UDP/Ethernet etc
 
    uint32_t obid;                  // Used by driver for channel info etc.    
    
    // Time block - Always UTC time
    uint16_t year; 
    uint8_t month;	// 1-12
    uint8_t day;	// 1-31
    uint8_t hour;	// 0-23
    uint8_t minute;	// 0-59
    uint8_t second;	// 0-59
    
    uint32_t timestamp;             // Relative time stamp for package in microseconds.
                                    // ~71 minutes before roll over
    
    // CRC should be calculated from here to end + datablock
    uint8_t head;                   // Bit 15   GUID is IP v.6 address.
                                    // Bit 8-14 = Reserved
                                    // bit 7,6,5 priority => Priority 0-7 where 0 is highest.
                                    // bit 4 = hard coded, true for a hard coded device.
                                    // bit 3 = Don't calculate CRC, Set to zero to use CRC.
                                    //          Just checked when CRC is used.    
                                    // bit 2 = Reserved.
                                    // bit 1 = Reserved.
                                    // bit 0 = Reserved.
    uint16_t vscp_class;            // VSCP class
    uint16_t vscp_type;             // VSCP type
    uint8_t  GUID[ 16 ];            // Node globally unique id MSB(0) -> LSB(15)
    uint16_t sizeData;              // Number of valid data bytes		

    uint8_t  data[VSCP_MAX_DATA];   // Pointer to data. Max. 487 (512-25) bytes

} /*__attribute__((packed, aligned(1)))*/ vscpEventEx;


typedef vscpEventEx *PVSCPEVENTEX;

// Priorities in the header byte as or'in values
// Priorities goes from 0-7 where 0 is highest 
#define VSCP_PRIORITY_0                     0x00
#define VSCP_PRIORITY_1                     0x20
#define VSCP_PRIORITY_2                     0x40
#define VSCP_PRIORITY_3                     0x60
#define VSCP_PRIORITY_4                     0x80
#define VSCP_PRIORITY_5                     0xA0
#define VSCP_PRIORITY_6                     0xC0
#define VSCP_PRIORITY_7                     0xE0

#define VSCP_PRIORITY_HIGH                  0x00
#define VSCP_PRIORITY_LOW                   0xE0
#define VSCP_PRIORITY_MEDIUM                0xC0
#define VSCP_PRIORITY_NORMAL                0x60

#define VSCP_HEADER_PRIORITY_MASK           0xE0

#define VSCP_HEADER_HARD_CODED              0x10    // If set node nickname is hardcoded
#define VSCP_HEADER_NO_CRC                  0x08    // Don't calculate CRC

#define VSCP_NO_CRC_CALC                    0x08    // If set no CRC is calculated

#define VSCP_MASK_PRIORITY                  0xE0
#define VSCP_MASK_HARDCODED                 0x10
#define VSCP_MASK_NOCRCCALC                 0x08

#define VSCP_LEVEL1_MAXDATA                 8
#define VSCP_LEVEL2_MAXDATA                 (512 - 25)

#define VSCP_CAN_ID_HARD_CODED	            0x02000000  // Hard coded bit in CAN frame id

// GUID byte positions
#define VSCP_GUID_MSB                       0
#define VSCP_GUID_LSB                       15

//
// Filter structure for VSCP Level 2
//

typedef struct  {

    uint8_t filter_priority;        // Priority 
    uint8_t mask_priority;
                
    uint16_t filter_class;          // VSCP class
    uint16_t mask_class;

    uint16_t filter_type;           // VSCP type
    uint16_t mask_type;

    uint8_t filter_GUID[ 16 ];      // Node address MSB -> LSB, LSB is node nickname id
    uint8_t mask_GUID[ 16 ];        // when interfacing the VSCP daemon.
    
} /*__attribute__((packed, aligned(1)))*/ vscpEventFilter;


typedef vscpEventFilter *PVSCPEVENTFILTER;


/*!
    Statistics

    This is the general statistics structure
*/

typedef struct structVSCPStatistics {
    unsigned long cntReceiveFrames;             // # of receive frames
    unsigned long cntTransmitFrames;            // # of transmitted frames
    unsigned long cntReceiveData;               // # of received data bytes
    unsigned long cntTransmitData;              // # of transmitted data bytes	
    unsigned long cntOverruns;                  // # of overruns
    unsigned long x;                            // Currently undefined value 
    unsigned long y;                            // Currently undefined value 
    unsigned long z;                            // Currently undefined value 
} /*__attribute__((packed, aligned(1)))*/ VSCPStatistics;

typedef  VSCPStatistics * PVSCPSTATISTICS;


/*!
    VSCPStatus

    This is the general channel state structure
*/

#define VSCP_STATUS_ERROR_STRING_SIZE       80

typedef struct structVSCPStatus {
    unsigned long channel_status;                       // Current state for channel
    unsigned long lasterrorcode;                        // Last error code
    unsigned long lasterrorsubcode;                     // Last error sub code
    char lasterrorstr[VSCP_STATUS_ERROR_STRING_SIZE];   // Last error string
} /*__attribute__((packed, aligned(1)))*/ VSCPStatus;


typedef  VSCPStatus * PVSCPSTATUS;
 

// VSCP LEVEL II UDP datagram offsets
#define VSCP_UDP_POS_HEAD                   0
#define VSCP_UDP_POS_CLASS                  1
#define VSCP_UDP_POS_TYPE                   3
#define VSCP_UDP_POS_GUID                   5
#define VSCP_UDP_POS_SIZE                   21

#define VSCP_UDP_POS_DATA                   23  // Holder for beginning of data
#define VSCP_UDP_POS_CRC                    25  // dummy: actual is len - 2


/*!
    VSCP Channel Info

    This is the channel VSCP Info structure
*/

typedef struct structVSCPChannelInfo {
    unsigned char channelType;      // Level I, Level II etc. from canal.h 
    unsigned short channel;         // daemon channel number
    char GUID[ 16 ];                // Channel GUID id
    
} /*__attribute__((packed, aligned(1)))*/ VSCPChannelInfo;

typedef  VSCPChannelInfo	*PVSCPCHANNELINFO;

// * * * Multicast on VSCP reserved IP 224.0.23.158

#define VSCP_MULTICAST_IPV4_ADDRESS_STR         "224.0.23.158"

#define VSCP_MULTICAST_ANNNOUNCE_PORT           9598
#define VSCP_MULTICAST_PORT                     33333   

// Packet format type = 0
#define VSCP_MULTICATS_PACKET0_HEADER_LENGTH    28

#define VSCP_MULTICAST_PACKET0_POS_PKTTYPE      0
#define VSCP_MULTICAST_PACKET0_POS_HEAD         1
#define VSCP_MULTICAST_PACKET0_POS_HEAD_MSB     1
#define VSCP_MULTICAST_PACKET0_POS_HEAD_LSB     2
#define VSCP_MULTICAST_PACKET0_POS_TIMESTAMP    3
#define VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS   7
#define VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE    9
#define VSCP_MULTICAST_PACKET0_POS_VSCP_GUID    11
#define VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE    27
#define VSCP_MULTICAST_PACKET0_POS_VSCP_DATA    29

// VSCP multicast packet types
#define VSCP_MULTICAST_TYPE_EVENT               0

// VSCP multicast Encryption types
#define VSCP_MULTICAST_ENCRYPTION_NONE          0

#define SET_VSCP_MULTICAST_TYPE( type, encryption )  ( (type<<4) + encryption )
#define GET_VSCP_MULTICAST_PACKET_TYPE( type)        ( (type>>4) & 0x0f)
#define GET_VSCP_MULTICAST_PACKET_ENCRYPTION( type)  ( (type) & 0x0f)

// Multicast proxy CLASS=1026, TYPE=3 http://www.vscp.org/docs/vscpspec/doku.php?id=class2.information#type_3_0x0003_level_ii_proxy_node_heartbeat
#define VSCP_MULTICAST_PROXY_HEARTBEAT_DATA_SIZE            192
#define VSCP_MULTICAST_PROXY_HEARTBEAT_POS_REALGUID         0       // The real GUID for the node
#define VSCP_MULTICAST_PROXY_HEARTBEAT_POS_IFGUID           32      // GUID for interface node is on
#define VSCP_MULTICAST_PROXY_HEARTBEAT_POS_IFLEVEL          48      // 0=Level I node, 1=Level II node
#define VSCP_MULTICAST_PROXY_HEARTBEAT_POS_NODENAME         64      // Name of node
#define VSCP_MULTICAST_PROXY_HEARTBEAT_POS_IFNAME           128     // Name of interface

// Bootloaders
#define VSCP_BOOTLOADER_VSCP                    0x00	// VSCP boot loader algorithm
#define VSCP_BOOTLOADER_PIC1                    0x01	// PIC algorithm 0
#define VSCP_BOOTLOADER_AVR1                    0x10	// AVR algorithm 0
#define VSCP_BOOTLOADER_LPC1                    0x20	// NXP/Philips LPC algorithm 0
#define VSCP_BOOTLOADER_ST                      0x30	// ST STR algorithm 0
#define VSCP_BOOTLOADER_FREESCALE               0x40	// Freescale Kinetics algorithm 0
#define VSCP_BOOTLOADER_NONE                    0xff


//          * * * Data Coding for VSCP packets * * *

// Data format masks
#define VSCP_MASK_DATACODING_TYPE               0xE0  // Bits 5,6,7
#define VSCP_MASK_DATACODING_UNIT               0x18  // Bits 3,4
#define VSCP_MASK_DATACODING_INDEX              0x07  // Bits 0,1,2

// Theese bits are coded in the three MSB bytes of the first data byte
// in a paket and tells the type of the data that follows.
#define VSCP_DATACODING_BIT                     0x00
#define VSCP_DATACODING_BYTE                    0x20
#define VSCP_DATACODING_STRING                  0x40
#define VSCP_DATACODING_INTEGER                 0x60
#define VSCP_DATACODING_NORMALIZED              0x80
#define VSCP_DATACODING_SINGLE                  0xA0  // single precision float
#define VSCP_DATACODING_RESERVED1               0xC0
#define VSCP_DATACODING_RESERVED2               0xE0

// These bits are coded in the four least significant bits of the first data byte
// in a packet and tells how the following data should be interpreted. For a flow sensor
// the default format can be litres/minute. Other formats such as m3/second can be defined 
// by the node if it which. However it must always be able to report in the default format.
#define VSCP_DATACODING_INTERPRETION_DEFAULT  0

/// Get data coding type
#define VSCP_DATACODING_TYPE( b ) ( VSCP_MASK_DATACODING_TYPE & b )

/// Get data coding unit
#define VSCP_DATACODING_UNIT( b ) ( ( VSCP_MASK_DATACODING_UNIT & b ) >> 3 )

/// Get data coding sensor index
#define VSCP_DATACODING_INDEX( b ) ( VSCP_MASK_DATACODING_INDEX & b )

// CRC8 Constants
#define VSCP_CRC8_POLYNOMIAL                0x18
#define VSCP_CRC8_REMINDER                  0x00

// CRC16 Constants
#define VSCP_CRC16_POLYNOMIAL               0x1021
#define VSCP_CRC16_REMINDER                 0xFFFF

// CRC32 Constants
#define VSCP_CRC32_POLYNOMIAL               0x04C11DB7
#define VSCP_CRC32_REMINDER                 0xFFFFFFFF


// Node data - the required registers are fetched from this 
//	structure
struct myNode {
    unsigned char GUID[ 16 ];
    unsigned char nicknameID;
};

// * * * Standard VSCP registers * * *

// Register defines above 0x7f
#define VSCP_STD_REGISTER_ALARM_STATUS          0x80

#define VSCP_STD_REGISTER_MAJOR_VERSION         0x81
#define VSCP_STD_REGISTER_MINOR_VERSION         0x82
#define VSCP_STD_REGISTER_SUB_VERSION           0x83

// 0x84 - 0x88
#define VSCP_STD_REGISTER_USER_ID               0x84

// 0x89 - 0x8C
#define VSCP_STD_REGISTER_USER_MANDEV_ID        0x89

// 0x8D -0x90
#define VSCP_STD_REGISTER_USER_MANSUBDEV_ID     0x8D

#define VSCP_STD_REGISTER_NICKNAME_ID           0x91

#define VSCP_STD_REGISTER_PAGE_SELECT_MSB       0x92
#define VSCP_STD_REGISTER_PAGE_SELECT_LSB       0x93

#define VSCP_STD_REGISTER_FIRMWARE_MAJOR        0x94
#define VSCP_STD_REGISTER_FIRMWARE_MINOR        0x95
#define VSCP_STD_REGISTER_FIRMWARE_SUBMINOR     0x96

#define VSCP_STD_REGISTER_BOOT_LOADER           0x97
#define VSCP_STD_REGISTER_BUFFER_SIZE           0x98
#define VSCP_STD_REGISTER_PAGES_COUNT           0x99

// 0xd0 - 0xdf
#define VSCP_STD_REGISTER_GUID                  0xD0

// 0xe0 - 0xff
#define VSCP_STD_REGISTER_DEVICE_URL            0xE0

// Level I Decision Matrix
#define VSCP_LEVEL1_DM_ROW_SIZE                 8

#define VSCP_LEVEL1_DM_OFFSET_OADDR             0
#define VSCP_LEVEL1_DM_OFFSET_FLAGS             1
#define VSCP_LEVEL1_DM_OFFSET_CLASS_MASK        2
#define VSCP_LEVEL1_DM_OFFSET_CLASS_FILTER      3
#define VSCP_LEVEL1_DM_OFFSET_TYPE_MASK         4
#define VSCP_LEVEL1_DM_OFFSET_TYPE_FILTER       5
#define VSCP_LEVEL1_DM_OFFSET_ACTION            6
#define VSCP_LEVEL1_DM_OFFSET_ACTION_PARAM      7

// Bits for VSCP server 16-bit capability code
// used by CLASS1.PROTOCOL, HIGH END SERVER RESPONSE
// and low end 16-bits for
// CLASS2.PROTOCOL, HIGH END SERVER HEART BEAT

#define VSCP_SERVER_CAPABILITY_MULTICAST            (1<<16)
#define VSCP_SERVER_CAPABILITY_TCPIP                (1<<15)
#define VSCP_SERVER_CAPABILITY_UDP                  (1<<14)
#define VSCP_SERVER_CAPABILITY_MULTICAST_ANNOUNCE   (1<<13)
#define VSCP_SERVER_CAPABILITY_RAWETH               (1<<12)
#define VSCP_SERVER_CAPABILITY_WEB                  (1<<11)
#define VSCP_SERVER_CAPABILITY_WEBSOCKET            (1<<10)
#define VSCP_SERVER_CAPABILITY_REST                 (1<<9)
#define VSCP_SERVER_CAPABILITY_MQTT                 (1<<8)
#define VSCP_SERVER_CAPABILITY_COAP                 (1<<7)
#define VSCP_SERVER_CAPABILITY_IP6                  (1<<6)
#define VSCP_SERVER_CAPABILITY_IP4                  (1<<5)
#define VSCP_SERVER_CAPABILITY_SSL                  (1<<4)
#define VSCP_SERVER_CAPABILITY_TWO_CONNECTIONS      (1<<3)


/// Error Codes
#define VSCP_ERROR_SUCCESS                      0       // All is OK
#define VSCP_ERROR_ERROR                        -1      // Error
#define VSCP_ERROR_CHANNEL                      7       // Invalid channel
#define VSCP_ERROR_FIFO_EMPTY                   8       // FIFO is empty
#define VSCP_ERROR_FIFO_FULL                    9       // FIFI is full
#define VSCP_ERROR_FIFO_SIZE                    10      // FIFO size error
#define VSCP_ERROR_FIFO_WAIT                    11      
#define VSCP_ERROR_GENERIC                      12      // Generic error
#define VSCP_ERROR_HARDWARE                     13      // Hardware error
#define VSCP_ERROR_INIT_FAIL                    14      // Initialization failed
#define VSCP_ERROR_INIT_MISSING                 15
#define VSCP_ERROR_INIT_READY                   16
#define VSCP_ERROR_NOT_SUPPORTED                17      // Not supported
#define VSCP_ERROR_OVERRUN                      18      // Overrun
#define VSCP_ERROR_RCV_EMPTY                    19      // Receive buffer empty
#define VSCP_ERROR_REGISTER                     20      // Register value error
#define VSCP_ERROR_TRM_FULL                     21      // Transmit buffer full
#define VSCP_ERROR_LIBRARY                      28      // Unable to load library
#define VSCP_ERROR_PROCADDRESS                  29      // Unable get library proc. address
#define VSCP_ERROR_ONLY_ONE_INSTANCE            30      // Only one instance allowed
#define VSCP_ERROR_SUB_DRIVER                   31      // Problem with sub driver call
#define VSCP_ERROR_TIMEOUT                      32      // Time-out
#define VSCP_ERROR_NOT_OPEN                     33      // The device is not open.
#define VSCP_ERROR_PARAMETER                    34      // A parameter is invalid.
#define VSCP_ERROR_MEMORY                       35      // Memory exhausted.
#define VSCP_ERROR_INTERNAL                     36      // Some kind of internal program error
#define VSCP_ERROR_COMMUNICATION                37      // Some kind of communication error
#define VSCP_ERROR_USER                         38      // Login error user name
#define VSCP_ERROR_PASSWORD                     39      // Login error password
#define VSCP_ERROR_CONNECTION                   40      // Could not connect   
#define VSCP_ERROR_INVALID_HANDLE               41      // The handle is not valid
#define VSCP_ERROR_OPERATION_FAILED             42      // Operation failed for some reason
#define VSCP_ERROR_BUFFER_TO_SMALL              43      // Supplied buffer is to small to fit content
#define VSCP_ERROR_UNKNOWN_ITEM                 44      // Requested item (remote variable) is unknown
#define VSCP_ERROR_ALREADY_DEFINED              45      // The name is already in use.

/*!
    Template for VSCP XML event data
 
    data: datetime,head,obid,datetime,timestamp,class,type,guid,sizedata,data,note
  
<event>    
    <head>3</head>
    <obid>1234</obid>
    <datetime>2017-01-13T10:16:02</datetime>
    <timestamp>50817</timestamp>
    <class>10</class>
    <type>6</type>
    <guid>00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02</guid>
    <sizedata>7</sizedata>
    <data>0x48,0x34,0x35,0x2E,0x34,0x36,0x34</data>
    <note></note>
</event>
 */
#define VSCP_XML_EVENT_TEMPLATE "<event>\n"\
    "<head>%d</head>\n"\
    "<obid>%lu</obid>\n"\
    "<datetime>%s</datetime>\n"\
    "<timestamp>%lu</timestamp>\n"\
    "<class>%d</class>\n"\
    "<type>%d</type>\n"\
    "<guid>%s</guid>\n"\
    "<sizedata>%d</sizedata>\n"\
    "<data>%s</data>\n"\
    "<note>%s</note>\n"\
"</event>"


/*
  
    Template for VSCP JSON event data
    data: datetime,head,obid,datetime,timestamp,class,type,guid,data,note 
  
{    
    "head": 2,
    "obid"; 123,
    "datetime": "2017-01-13T10:16:02",
    "timestamp":50817,
    "class": 10,
    "type": 8,
    "guid": "00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02",
    "data": [1,2,3,4,5,6,7],
    "note": "This is some text"
}
*/
#define VSCP_JSON_EVENT_TEMPLATE "{\n"\
    "\"head\": %d,\n"\
    "\"obid\":  %lu,\n"\
    "\"datetime\": \"%s\",\n"\
    "\"timestamp\": %lu,\n"\
    "\"class\": %d,\n"\
    "\"type\": %d,\n"\
    "\"guid\": \"%s\",\n"\
    "\"data\": [%s],\n"\
    "\"note\": \"%s\"\n"\
"}"

/*!
 
    Template for VSCP HTML event data  
   
    data: datetime,class,type,data-count,data,guid,head,timestamp,obid,note
 
<h2>VSCP Event</h2>
<p>
Time: 2017-01-13T10:16:02 <br>
</p>
<p>
Class: 10 <br>
Type: 6 <br>
</p>
<p>
Data count: 6<br>
Data: 1,2,3,4,5,6,7<br>
</p>
<p>
From GUID: 00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02<br>
</p>
<p>
Head: 6 <br>
DateTime: 2013-11-02T12:34:22Z
Timestamp: 1234 <br>
obid: 1234 <br>
note: This is a note <br>
</p>
 
*/


#define VSCP_HTML_EVENT_TEMPLATE "<h2>VSCP Event</h2> "\
    "<p>"\
    "Class: %d <br>"\
    "Type: %d <br>"\
    "</p>"\
    "<p>"\
    "Data count: %d<br>"\
    "Data: %s<br>"\
    "</p>"\
    "<p>"\
    "From GUID: %s<br>"\
    "</p>"\
    "<p>"\
    "Head: %d <br>"\
    "<p>"\
    "DateTime: %s <br>"\
    "</p>"\
    "Timestamp: %lu <br>"\
    "obid: %lu <br>"\
    "note: %s <br>"\
    "</p>"


#ifdef __cplusplus
}
#endif



#endif // _VSCP_H_

