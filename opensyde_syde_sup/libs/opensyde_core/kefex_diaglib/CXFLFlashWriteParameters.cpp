//-----------------------------------------------------------------------------
/*!
   \internal
   \file        CXFLFlashWriteParams.cpp
   \brief       Software flashing parameters class

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.01.2010  STW/A.Stangl (refactored to this module from CXFLDownload.h/.c)
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------ */
#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <string.h>
#include "DiagLib_config.h"  //diaglib configuration

#include "stwtypes.h"
#include "stwerrors.h"
#include "CXFLFlashWriteParameters.h"
#include "CXFLActions.h"

#include "CSCLString.h"
#include "CSCLIniFile.h"

//---------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_diag_lib;

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------- */

/* -- Implementation ------------------------------------------------------ */
C_XFLFlashWriteParameters::C_XFLFlashWriteParameters(void) :
   u16_Version(0U),
   e_EraseMode(eXFL_ERASE_MODE_AUTOMATIC),
   e_FlashFinishedAction(eXFL_FLASH_FINISHED_ACTION_ASK_USER),
   u16_InterFrameDelayUs(0U),
   u8_HexRecordLength(0U),
   q_XtdID(false),
   u32_SendID(XFL_DEFAULT_SEND_ID),
   u32_ReceiveID(XFL_DEFAULT_SEND_ID + 1),
   q_DivertStream(false),
   u8_DivertTargetIndex(0U),
   q_WriteCRCsIfSupported(true),
   q_VerboseMode(true),
   q_XFLExchange(false),
   u8_IgnoreInvalidHexfileError(0U),
   e_DevTypeCheck(eXFL_DEV_TYPE_CHECK_NONE),
   e_DevTypeCheckGetIDFailedReaction(eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_IGNORE),
   e_DevTypeCheckMatchIDFailedReaction(eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_IGNORE)
{
   c_HexFile = "";
   c_UserDefinedSectors = "";
   au8_DivertUserID[0] = 0U;
   au8_DivertUserID[1] = 0U;
}

//-----------------------------------------------------------------------------

C_XFLFlashWriteParameters::~C_XFLFlashWriteParameters(void)
{
   //nothing special to do here, just provide it so inheriting classes may reimplement the destructor
}

//-----------------------------------------------------------------------------
/*!
   \brief   load flashing parameters from an INI file

   Note: will not load all parameters !
   If the specified section does not exists default values will be used !
   The following table lists all loaded parameters. The other member variables of the structure will not be touched.

   \verbatim
   Parameter                            Directive                   Format in INI file               Default
   =====================================================================================================================
   c_WakeupConfig.u8_FLASHIntervalMs    FLASHINTERVAL               number (in milliseconds)         10
   c_WakeupConfig.u32_StartTimeMs       STARTTIME                   number (in seconds)              3
   c_WakeupConfig.c_CompanyID           COMPANYID                   string                           "XX"
   c_WakeupConfig.q_SendResetRQ         SENDRESETRQ                 0 / 1                            0
   c_WakeupConfig.t_ResetMsg.u8_XTD     RESETMSGXTD                 0 / 1                            0
   c_WakeupConfig.t_ResetMsg.u32_ID     RESETMSGID                  number                           0
   c_WakeupConfig.t_ResetMsg.u8_DLC     RESETMSGDLC                 number (0..8)                    0
   c_WakeupConfig.t_ResetMsg.au8_Data[n]RESETMSGDB[n] (n = 1..8)    number                           0
   c_WakeupConfig.u8_LocalID            LOCALID                     number                           0
   ac_WakeupConfig.u8_SNR               SNR                         string ("yyoooooopprr")          000000000000
   u8_HexRecordLength                   HEXRECORDLENGTH             number                           0
   e_DevTypeCheck                       DEV_ID_CHECK                number (index of the enum)       eXFL_DEV_TYPE_CHECK_SCAN_HEX_FILE
   e_DevTypeCheckGetIDFailedReaction    DEV_ID_CHECK_GET_ID_FAIL    number (index of the enum)       eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_ASK
   e_DevTypeCheckMatchIDFailedReaction  DEV_ID_CHECK_MATCH_ID_FAIL  number (index of the enum)       eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_ASK
   u32_SendID                           SENDID                      number                           0x51
   u32_ReceiveID                        -                           will be set to (u32_SendID + 1)  0x52
   q_XtdID                              XTDID                       0 / 1                            0
   e_EraseMode                          PROGTYPE                    number (index of the enum)       0
   c_UserDefinedSectors                 SECTORS                     string (e.g. "1,2,3,4,5,6")      "1,2,3,4,5,6"
   c_HexFile                            FILENAME                    string                           ""
   \endverbatim

   \param[in]     orc_IniFile    opened INI file
   \param[in]     orc_Section    section in .ini file

   \return
   C_NO_ERR     data loaded

   \created     28.07.2008  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_XFLFlashWriteParameters::LoadFromIni(C_SCLIniFile & orc_IniFile, const C_SCLString & orc_Section)
{
   sint32 i;
   sint32 s32_Return;
   C_SCLString c_Temp2;
   sint32 s32_Value;

   c_WakeupConfig.u8_FLASHIntervalMs   = orc_IniFile.ReadUint8(orc_Section, "FLASHINTERVAL", 10U);
   u8_HexRecordLength                  = orc_IniFile.ReadUint8(orc_Section, "HEXRECORDLENGTH", 0U);
   s32_Value                           = orc_IniFile.ReadInteger(orc_Section, "DEV_ID_CHECK",
                                                                static_cast<sint32>(eXFL_DEV_TYPE_CHECK_SCAN_HEX_FILE));
   e_DevTypeCheck                      = static_cast<E_XFLDevTypeCheck>(s32_Value);
   s32_Value                           = orc_IniFile.ReadInteger(orc_Section, "DEV_ID_CHECK_GET_ID_FAIL",
                                                      static_cast<sint32>(eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_ASK));
   e_DevTypeCheckGetIDFailedReaction   = static_cast<E_XFLDevTypeCheckGetIDFailReaction>(s32_Value);
   s32_Value                           = orc_IniFile.ReadInteger(orc_Section, "DEV_ID_CHECK_MATCH_ID_FAIL",
                                                    static_cast<sint32>(eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_ASK));
   e_DevTypeCheckMatchIDFailedReaction = static_cast<E_XFLDevTypeCheckMatchIDFailReaction>(s32_Value);
   c_WakeupConfig.u32_StartTimeMs      = static_cast<uint32>(orc_IniFile.ReadInteger(orc_Section, "STARTTIME", 3)) *
                                          1000U;
   u32_SendID                          = orc_IniFile.ReadInteger(orc_Section, "SendID",    XFL_DEFAULT_SEND_ID);
   u32_ReceiveID                       = u32_SendID + 1U;
   q_XtdID                             = orc_IniFile.ReadBool   (orc_Section, "XTDID",     false);
   c_WakeupConfig.c_CompanyID          = orc_IniFile.ReadString (orc_Section, "CompanyID", "XX");
   c_WakeupConfig.u8_LocalID           = orc_IniFile.ReadUint8  (orc_Section, "LocalID",   0U);
   c_Temp2                             = orc_IniFile.ReadString (orc_Section, "SNR", "000000000000");
   s32_Return = C_XFLActions::SNRStringToBytes(c_Temp2, c_WakeupConfig.au8_SNR);
   if ((s32_Return != C_NO_ERR) && (s32_Return != C_WARN)) //accept weird hex SNRs as well
   {
      (void)memset(&c_WakeupConfig.au8_SNR[0], 0, sizeof(c_WakeupConfig.au8_SNR));
   }
   e_EraseMode           = static_cast<E_XFLEraseMode>(orc_IniFile.ReadInteger(orc_Section, "ProgType",  0));
   c_UserDefinedSectors  = orc_IniFile.ReadString (orc_Section, "Sectors",   "1,2,3,4,5,6");
   c_WakeupConfig.q_SendResetRQ         = orc_IniFile.ReadBool   (orc_Section, "SENDRESETRQ", false);
   c_WakeupConfig.t_ResetMsg.u8_XTD     = orc_IniFile.ReadUint8  (orc_Section, "RESETMSGXTD", 0U);
   c_WakeupConfig.t_ResetMsg.u32_ID     = orc_IniFile.ReadInteger(orc_Section, "RESETMSGID",  0);
   c_WakeupConfig.t_ResetMsg.u8_DLC     = orc_IniFile.ReadUint8  (orc_Section, "RESETMSGDLC", 0U);
   c_WakeupConfig.t_ResetMsg.u8_RTR     = 0U; //make sure to have a clearly defined value
   for (i = 0; i < 8; i++)
   {
      c_WakeupConfig.t_ResetMsg.au8_Data[i] = orc_IniFile.ReadUint8(orc_Section, "RESETMSGDB" +
                                                                    C_SCLString::IntToStr(i), 0U);
   }
   c_HexFile = orc_IniFile.ReadString(orc_Section, "Filename", "");
}

//-----------------------------------------------------------------------------
/*!
   \brief   save flashing parameters to an INI file

   Note: will not save all parameters !
   The following table lists all saved parameters. The other member variables of the structure will not be stored.

   \verbatim
   Parameter                            Directive                   Format in INI file
   ==================================================================================================
   c_WakeupConfig.u32_StartTimeMs       STARTTIME                   number (in seconds)
   c_WakeupConfig.au8_SNR               SNR                         string ("yyoooooopprr")
   c_WakeupConfig.q_SendResetRQ         SENDRESETRQ                 0 / 1
   c_WakeupConfig.t_ResetMsg.u8_XTD     RESETMSGXTD                 0 / 1
   c_WakeupConfig.t_ResetMsg.u32_ID     RESETMSGID                  number
   c_WakeupConfig.t_ResetMsg.u8_DLC     RESETMSGDLC                 number (0..8)
   c_WakeupConfig.t_ResetMsg.au8_Data[n]RESETMSGDB[n] (n = 1..8)    number
   c_WakeupConfig.u8_LocalID            LOCALID                     number
   u32_SendID                           SENDID                      number
   q_XtdID                              XTDID                       0 / 1
   c_CompanyID                          COMPANYID                   string
   e_EraseMode                          PROGTYPE                    number (index of the enum)
   c_UserDefinedSectors                 SECTORS                     string (e.g. "1,2,3,4,5,6")
   c_HexFile                            FILENAME                    string
   e_DevTypeCheck                       DEV_ID_CHECK                number (index of the enum)
   e_DevTypeCheckGetIDFailedReaction    DEV_ID_CHECK_GET_ID_FAIL    number (index of the enum)
   e_DevTypeCheckMatchIDFailedReaction  DEV_ID_CHECK_MATCH_ID_FAIL  number (index of the enum)

   \endverbatim

   \param[in]     orc_IniFile    opened INI file
   \param[in]     orc_Section    section in .ini file

   \return
   C_NO_ERR     data stored   \n
   else         could not write values

   \created     28.07.2008  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_XFLFlashWriteParameters::SaveToIni(C_SCLIniFile & orc_IniFile, const C_SCLString & orc_Section) const
{
   sint32 i;

   try
   {
      orc_IniFile.WriteInteger(orc_Section, "STARTTIME", static_cast<sintn>(c_WakeupConfig.u32_StartTimeMs / 1000U));
      orc_IniFile.WriteInteger(orc_Section, "SENDID",    u32_SendID);
      orc_IniFile.WriteBool   (orc_Section, "XTDID",     q_XtdID);

      orc_IniFile.WriteString (orc_Section, "COMPANYID", c_WakeupConfig.c_CompanyID);
      orc_IniFile.WriteInteger(orc_Section, "LOCALID",   c_WakeupConfig.u8_LocalID);
      orc_IniFile.WriteString (orc_Section, "FILENAME",  c_HexFile);
      orc_IniFile.WriteInteger(orc_Section, "PROGTYPE",  static_cast<sint32>(e_EraseMode));
      orc_IniFile.WriteString (orc_Section, "SECTORS",   c_UserDefinedSectors);
      orc_IniFile.WriteString (orc_Section, "SNR",       C_XFLActions::SNRBytesToString(c_WakeupConfig.au8_SNR, false));

      orc_IniFile.WriteBool   (orc_Section, "SENDRESETRQ", c_WakeupConfig.q_SendResetRQ);
      orc_IniFile.WriteInteger(orc_Section, "RESETMSGXTD", c_WakeupConfig.t_ResetMsg.u8_XTD);
      orc_IniFile.WriteInteger(orc_Section, "RESETMSGID",  static_cast<sint32>(c_WakeupConfig.t_ResetMsg.u32_ID));
      orc_IniFile.WriteInteger(orc_Section, "RESETMSGDLC", c_WakeupConfig.t_ResetMsg.u8_DLC);
      for (i = 0; i < 8; i++)
      {
         orc_IniFile.WriteInteger(orc_Section, "RESETMSGDB" + C_SCLString::IntToStr(i),
                                  c_WakeupConfig.t_ResetMsg.au8_Data[i]);
      }

      orc_IniFile.WriteInteger(orc_Section, "DEV_ID_CHECK", static_cast<sint32>(e_DevTypeCheck));
      orc_IniFile.WriteInteger(orc_Section, "DEV_ID_CHECK_GET_ID_FAIL",
                               static_cast<sint32>(e_DevTypeCheckGetIDFailedReaction));
      orc_IniFile.WriteInteger(orc_Section, "DEV_ID_CHECK_MATCH_ID_FAIL",
                               static_cast<sint32>(e_DevTypeCheckMatchIDFailedReaction));
   }
   catch (...)
   {
      return C_NOACT;
   }
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------

