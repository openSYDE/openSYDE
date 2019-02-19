//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       openSYDE Hex file class

   For details cf. documentation in .h file.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.09.2017  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include "precomp_headers.h" //pre-compiled headers

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OsyHexFile.h"
#include "CSCLString.h"
#include "CXFLECUInformation.h"
#include "TGLUtils.h"

using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_diag_lib;

//-----------------------------------------------------------------------------
/*!
   \brief   get address of signature block from hex file

   Identify address of signature block in file.
   The address of the first found block is returned.

   Possible approach when we have the flash packages: put the address into the package information.
    The signature generator has this information anyway.

   \param[out]     oru32_Address             address of signature block

   \return
   C_NO_ERR     everything OK; block found
   C_NOACT      no block found

   \created     11.09.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OsyHexFile::GetSignatureBlockAddress(uint32 & oru32_Address)
{
   sint32 s32_Return;

   oru32_Address = this->mu32_MinAdr;

   //lint -e{926}
   s32_Return = this->FindPattern(oru32_Address, 10, reinterpret_cast<const uint8 *>(";zwm2KgUZ!"));
   if (s32_Return == -1)
   {
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   extract device ID from hex-file

   Scan through the hex-file and try to find the "application_info" structure.
   Then extract the device ID from it and return it.
   If the application_info structure with the device ID information is in the hex file more than once, AND
    the device ID in at least one of the copies is different, we fail.

   \param[out]    orc_DeviceID                     device ID found in hex-file

   \return
   C_NO_ERR     everything OK (device ID in orc_DeviceID)
   C_NOACT      device-ID not found
   C_CONFIG     ambiguous device-IDs in hex-file

   \created     19.12.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OsyHexFile::ScanDeviceIdFromHexFile(C_SCLString & orc_DeviceID)
{
   sint32 s32_Return;
   sint32 s32_Index;
   C_SCLString c_DeviceID = "";

   SCLDynamicArray<C_XFLECUInformation> c_InfoBlocks;
   s32_Return = this->GetECUInformationBlocks(c_InfoBlocks, 0x0U, false, false, true);
   if ((s32_Return == C_NO_ERR) && (c_InfoBlocks.GetLength() > 0))
   {
      for (s32_Index = 0; s32_Index < c_InfoBlocks.GetLength(); s32_Index++)
      {
         if (c_InfoBlocks[s32_Index].ContainsDeviceID() == true)
         {
            const C_SCLString c_Help = c_InfoBlocks[s32_Index].GetDeviceID();
            if (c_DeviceID == "") //no device-ID seen yet ...
            {
               c_DeviceID = c_Help;
            }
            else
            {
               if (c_DeviceID != c_Help)
               {
                  s32_Return = C_CONFIG; //ambiguous device IDs !
                  break;
               }
            }
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      if (c_DeviceID != "")
      {
         orc_DeviceID = c_DeviceID;
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   extract device ID from hex-file

   Scan through the hex-file and try to find the "application_info" structure.
   Then extract the information from it and return it.
   Multiple instances of the application_info structure are considered an error.

   \param[out]    orc_InfoBlock        application info block found in hex file

   \return
   C_NO_ERR     everything OK (device ID in orc_DeviceID)
   C_NOACT      no application information block detected in hex file
   C_OVERFLOW   multiple application information blocks detected in hex file

   \created     02.01.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OsyHexFile::ScanApplicationInformationBlockFromHexFile(C_XFLECUInformation & orc_InfoBlock)
{
   sint32 s32_Return;

   SCLDynamicArray<C_XFLECUInformation> c_InfoBlocks;
   s32_Return = this->GetECUInformationBlocks(c_InfoBlocks, 0x0U, false, false, true);
   tgl_assert(s32_Return == C_NO_ERR); //no plausible reasons documented

   if (c_InfoBlocks.GetLength() == 0)
   {
      s32_Return = C_NOACT;
   }
   else if (c_InfoBlocks.GetLength() > 1)
   {
      s32_Return = C_OVERFLOW;
   }
   else
   {
      //one block found !
      orc_InfoBlock = c_InfoBlocks[0];
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}
