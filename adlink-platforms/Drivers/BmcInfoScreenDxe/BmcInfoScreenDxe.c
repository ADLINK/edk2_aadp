/** @file
  BMC Management setup screen

  Copyright (c) 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Guid/BmcInfoScreenHii.h>
#include <Guid/MdeModuleHii.h>
#include <IndustryStandard/Ipmi.h>
#include <IndustryStandard/IpmiNetFnApp.h>
#include <IndustryStandard/IpmiNetFnTransport.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/IpmiCommandLib.h>
#include <Library/IpmiCommandLibExt.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "BmcInfoScreenDxe.h"
#include <Library/MmcLib.h>

//
// HII Handle for BMC Info Screen package
//
EFI_HII_HANDLE mHiiHandle;

/**
  This function updates info for Main form.

  @param[in] VOID

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval Other             Some error occurs when executing this entry point.

**/
EFI_STATUS
UpdateMainForm (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT16                      StrBuf[MAX_STRING_SIZE];
  UINT8                       Buffer[19];
#if 1
  Status = MmcFirmwareVersion (Buffer, sizeof(Buffer));
#else  
  Buffer[6]=1;
  Buffer[7]=6;
  Status = EFI_SUCCESS;
#endif  
if (!EFI_ERROR (Status)) {
  
    DEBUG ((DEBUG_INFO, "%a : Buffer = ", __FUNCTION__));
    for (int i=0; i<sizeof(Buffer); i++) {
      DEBUG ((DEBUG_INFO, "%x=%2x ", i, Buffer[i]));
    }
    DEBUG ((DEBUG_INFO, "\n"));

    DEBUG ((DEBUG_INFO, "%a : MMC firmware version = %x.%x\n", __FUNCTION__, Buffer[6], Buffer[7]));
    UnicodeSPrint (
      StrBuf,
      sizeof (StrBuf),
      L"%x.%x",
      Buffer[6],
      Buffer[7]
      );
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_BMC_FIRMWARE_REV_VALUE), StrBuf, NULL);
  }
  else {
    DEBUG ((DEBUG_ERROR, "%a : MMC firmware version retrieving error\n", __FUNCTION__));
  }

  return Status;
}

/**
  The user Entry Point for the BMC Screen driver.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval Other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
BmcInfoScreenEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_HANDLE DriverHandle;

  Status = EFI_SUCCESS;
  DriverHandle = NULL;

  //
  // Publish our HII data
  //
  mHiiHandle = HiiAddPackages (
                 &gBmcInfoScreenFormSetGuid,
                 DriverHandle,
                 BmcInfoScreenDxeStrings,
                 VfrBin,
                 NULL
                 );

  ASSERT (mHiiHandle != NULL);

  Status = UpdateMainForm ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a Failed to update BMC Info Screen\n", __FUNCTION__));
  }

  return Status;
}
