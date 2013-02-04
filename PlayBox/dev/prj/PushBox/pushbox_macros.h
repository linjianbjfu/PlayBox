#pragma once
#ifndef __PUSHBOX_MACRO
#define __PUSHBOX_MACRO
#include <NtDDNdis.h>
#include "winioctl.h"
#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ( ( FILE_DEVICE_SCSI << 16 ) + 0x0501 )

#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

#define  IDENTIFY_BUFFER_SIZE  512
#define  SENDIDLENGTH  ( sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE )

#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

/*typedef struct _IDEREGS
{
	BYTE bFeaturesReg;       // Used for specifying SMART "commands".
	BYTE bSectorCountReg;    // IDE sector count register
	BYTE bSectorNumberReg;   // IDE sector number register
	BYTE bCylLowReg;         // IDE low order cylinder value
	BYTE bCylHighReg;        // IDE high order cylinder value
	BYTE bDriveHeadReg;      // IDE drive/head register
	BYTE bCommandReg;        // Actual IDE command.
	BYTE bReserved;          // reserved for future use.  Must be zero.
} IDEREGS, *PIDEREGS, *LPIDEREGS;*/

#define ADS_TIMER_ID 1001
#define SLEEP_TIMER_ID	1002
//Time to display the ads ,by seconds.
//#define ADS_TIME			60
#define CONF_FILE			"\\Conf.ini"
#define CONF_REAL_ADS	"\\ads_url.ini"
//#define ADS_SECTION	"Task_1"
#define ADS_SLEEP_TIME "Sleep"
#define ADS_TYPE			"Type"
#define ADS_SEC_URL	"Url"
#define ADS_SEC_TIME	"ADS_TIME"
#define ADS_HEIGHT		"x"
#define ADS_WIDTH		"y"
#define ADS_PUSH_URL "PUSH_ADDR"
#define ADS_DEFAULT_PAGE "\\Default.html"
#define ADS_PUSH_DEFAULT_URL "http://push.laidiantuan.com/addpush/addpush.php?"
#define ADS_NUM_SEC		"Task_Num"
#define ADS_NUM				"Num"
//Process exit if Action = 0, otherwise do nothing.
#define KILLER_ACTION		"Action"
//Time to get the configurations for the push box. by minutes.
#define FILE_USER_BEHAVIOR	"\\Low\\BHO_Demo.log"
#define PUSH_TIME		16
#define HKEY_PLAYBOX_ROOT HKEY_LOCAL_MACHINE

#endif