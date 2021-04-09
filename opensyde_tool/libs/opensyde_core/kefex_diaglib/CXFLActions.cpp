//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common flashloader actions

   Encapsulates some higher level action that are used by more complex procedures (e.g. downloadclass).

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "DiagLib_config.h" //diaglib configuration

#include <string.h>
#include "stwtypes.h"
#include "stwerrors.h"
#include "CXFLActions.h"
#include "CXFLProtocol.h"
#include "DLLocalize.h"
#include "stwcompid.h"
#include "CSCLString.h"
#include "TGLUtils.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_diag_lib;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_can;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
//constants for ::ReadServerInformation
//some things stay hard-coded for compatibility with older non-self-describing flashloader implementations:
static const sint16 KNOWN_DEVICE_UNKNOWN = -1;
static const sint16 KNOWN_DEVICE_ESX     =  0; //Y ESX2
static const sint16 KNOWN_DEVICE_ESXLT   =  1; //Y ESX-Lt.
static const sint16 KNOWN_DEVICE_EHC     =  2; //CN EHC
static const sint16 KNOWN_DEVICE_MAX     =  3;

static const charn maacn_KNOWN_DEVICE_NAMES[KNOWN_DEVICE_MAX][10] =
{
   "ESX2", //DEVICE_ESX
   "ESL",  //DEVICE_ESXLT
   "EHC"   //DEVICE_EHC
};

//----------------------------------------------------------------------------------------------------------------------

static uint16 mu16_WinflashInternalKey = 0x0000U;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

void XFLSetInternalKey(const uint16 ou16_Key)
{
   mu16_WinflashInternalKey = ou16_Key;
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLWakeupParameters::C_XFLWakeupParameters(void) :
   e_WakeupMode(eXFL_WAKEUP_MODE_LID),
   q_SendResetRQ(false),
   q_SendFLASHRequired(true),
   //defines how long the "FLASH" command should be sent
   u32_StartTimeMs(3000U),
   //interval between individual "FLASH" commands in ms
   u8_FLASHIntervalMs(10U),
   u8_LocalID(0U),
   c_CompanyID("")
{
   t_ResetMsg.u32_ID = 0U;
   t_ResetMsg.u8_XTD = 0U;
   t_ResetMsg.u8_DLC = 0U;
   t_ResetMsg.u8_RTR = 0U;
   (void)memset(&au8_SNR[0], 0, sizeof(au8_SNR));
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLFoundNode::C_XFLFoundNode(void) :
   u8_NodeID(0U),
   c_SNR(""),
   c_DeviceID(""),
   q_Selected(false)
{
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLDivertParameters::C_XFLDivertParameters(void) :
   //0 = CAN (cf. flashloader protocol specification)
   u8_DeviceIndex(0U),
   u8_SelectedPosition(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLDivertParameters & C_XFLDivertParameters::operator =(const C_XFLDivertParameters & orc_Source)
{
   if (this != &orc_Source)
   {
      this->u8_DeviceIndex       = orc_Source.u8_DeviceIndex;
      this->c_PositionNames      = orc_Source.c_PositionNames;
      this->c_Parameters         = orc_Source.c_Parameters;
      this->u8_SelectedPosition  = orc_Source.u8_SelectedPosition;
   }
   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load divert stream parameter definition and values from INI file

   Will read the following parameters:

   \verbatim
   Parameter                            Directive                   Format in INI file               Default
   =====================================================================================================================
   u8_DeviceIndex                       DIVERT_DEVICE_INDEX         number                           0
   u8_SelectedPosition                  DIVERT_SELECTED_POSITION    number                           0
   c_PositionNames.Length               DIVERT_NUM_POSITIONS        number                           0
   c_PositionNames[i]                   DIVERT_POSITION_[i]         string                           ""
   c_Parameters.Length                  DIVERT_NUM_PARAMETERS       number                           0
   c_Parameters[i].u8_ParameterIndex    DIVERT_PARAMETER_[i]_INDEX  number                           0
   c_Parameters[i].c_ParameterName      DIVERT_PARAMETER_[i]_NAME   string                           ""
   c_Parameters[i].u16_ParameterValue   DIVERT_PARAMETER_[i]_VALUE  number                           0
   \endverbatim

   \param[in]   orc_IniFile        opened INI file
   \param[in]   orc_Section        section in INI file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLDivertParameters::LoadFromINI(C_SCLIniFile & orc_IniFile, const C_SCLString & orc_Section)
{
   u8_DeviceIndex      = orc_IniFile.ReadUint8(orc_Section, "DIVERT_DEVICE_INDEX", 0U);
   u8_SelectedPosition = orc_IniFile.ReadUint8(orc_Section, "DIVERT_SELECTED_POSITION", 0U);

   c_PositionNames.SetLength(orc_IniFile.ReadInteger(orc_Section, "DIVERT_NUM_POSITIONS", 0));
   for (sint32 s32_NameIndex = 0; s32_NameIndex < c_PositionNames.GetLength(); s32_NameIndex++)
   {
      c_PositionNames[s32_NameIndex] =
         orc_IniFile.ReadString(orc_Section, "DIVERT_POSITION_" + C_SCLString::IntToStr(s32_NameIndex), "");
   }
   c_Parameters.SetLength(orc_IniFile.ReadInteger(orc_Section, "DIVERT_NUM_PARAMETERS", 0));
   for (sint32 s32_ParameterIndex = 0; s32_ParameterIndex < c_Parameters.GetLength(); s32_ParameterIndex++)
   {
      c_Parameters[s32_ParameterIndex].u8_ParameterIndex  =
         orc_IniFile.ReadUint8(orc_Section, "DIVERT_PARAMETER_" + C_SCLString::IntToStr(s32_ParameterIndex) + "_INDEX",
                               0U);
      c_Parameters[s32_ParameterIndex].c_ParameterName    =
         orc_IniFile.ReadString(orc_Section, "DIVERT_PARAMETER_" + C_SCLString::IntToStr(s32_ParameterIndex) + "_NAME",
                                "");
      c_Parameters[s32_ParameterIndex].u16_ParameterValue =
         orc_IniFile.ReadUint16(orc_Section, "DIVERT_PARAMETER_" + C_SCLString::IntToStr(s32_ParameterIndex) + "_VALUE",
                                0U);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save divert stream parameter definitions and values to INI file

   Will write the following parameters:

   \verbatim
   Parameter                            Directive                   Format in INI file
   ==================================================================================================
   u8_DeviceIndex                       DIVERT_DEVICE_INDEX         number
   u8_SelectedPosition                  DIVERT_SELECTED_POSITION    number
   c_PositionNames.Length               DIVERT_NUM_POSITIONS        number
   c_PositionNames[i]                   DIVERT_POSITION_[i]         string
   c_Parameters.Length                  DIVERT_NUM_PARAMETERS       number
   c_Parameters[i].u8_ParameterIndex    DIVERT_PARAMETER_[i]_INDEX  number
   c_Parameters[i].c_ParameterName      DIVERT_PARAMETER_[i]_NAME   string
   c_Parameters[i].u16_ParameterValue   DIVERT_PARAMETER_[i]_VALUE  number
   \endverbatim

   \param[in]   orc_IniFile        opened INI file
   \param[in]   orc_Section        section in INI file

   \return
   C_NO_ERR   written  \n
   else       error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLDivertParameters::SaveToINI(C_SCLIniFile & orc_IniFile, const C_SCLString & orc_Section)
{
   try
   {
      orc_IniFile.WriteInteger(orc_Section, "DIVERT_DEVICE_INDEX", u8_DeviceIndex);
      orc_IniFile.WriteInteger(orc_Section, "DIVERT_SELECTED_POSITION", u8_SelectedPosition);

      orc_IniFile.WriteInteger(orc_Section, "DIVERT_NUM_POSITIONS", c_PositionNames.GetLength());
      for (sint32 s32_NameIndex = 0; s32_NameIndex < c_PositionNames.GetLength(); s32_NameIndex++)
      {
         orc_IniFile.WriteString(orc_Section, "DIVERT_POSITION_" + C_SCLString::IntToStr(
                                    s32_NameIndex), c_PositionNames[s32_NameIndex]);
      }

      orc_IniFile.WriteInteger(orc_Section, "DIVERT_NUM_PARAMETERS", c_Parameters.GetLength());
      for (sint32 s32_ParameterIndex = 0; s32_ParameterIndex < c_Parameters.GetLength(); s32_ParameterIndex++)
      {
         orc_IniFile.WriteInteger(orc_Section, "DIVERT_PARAMETER_" + C_SCLString::IntToStr(s32_ParameterIndex) +
                                  "_INDEX", c_Parameters[s32_ParameterIndex].u8_ParameterIndex);
         orc_IniFile.WriteString(orc_Section, "DIVERT_PARAMETER_" + C_SCLString::IntToStr(s32_ParameterIndex) +
                                 "_NAME", c_Parameters[s32_ParameterIndex].c_ParameterName);
         orc_IniFile.WriteInteger(orc_Section, "DIVERT_PARAMETER_" + C_SCLString::IntToStr(s32_ParameterIndex) +
                                  "_VALUE", c_Parameters[s32_ParameterIndex].u16_ParameterValue);
      }
   }
   catch (...)
   {
      return C_RD_WR;
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set parameters for diverting from/to CAN bus

   Parameters for CAN are defined in flashloader protocol specification

   \param[in]   ou8_NumPositions        number of possible target busses
*/
//----------------------------------------------------------------------------------------------------------------------
C_XFLDivertParametersCAN::C_XFLDivertParametersCAN(const uint8 ou8_NumPositions) :
   C_XFLDivertParameters()
{
   sint32 s32_Loop;
   C_SCLString c_NamePrefix;

   c_PositionNames.SetLength(ou8_NumPositions);
   for (s32_Loop = 0U; s32_Loop < ou8_NumPositions; s32_Loop++)
   {
      c_PositionNames[s32_Loop] = "CAN_BUS_" + C_SCLString::IntToStr(s32_Loop + 1);
   }
   c_Parameters.SetLength(16);

   for (s32_Loop = 0; s32_Loop < 2; s32_Loop++)
   {
      c_NamePrefix = (s32_Loop == 0) ? "CLIENT" : "TARGET";

      c_Parameters[s32_Loop * 8].u8_ParameterIndex  = static_cast<uint8>(s32_Loop * 32);
      c_Parameters[s32_Loop * 8].c_ParameterName    = c_NamePrefix + "_ID_TYPE";
      c_Parameters[s32_Loop * 8].u16_ParameterValue = 1U;

      c_Parameters[(s32_Loop * 8) + 1].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 1);
      c_Parameters[(s32_Loop * 8) + 1].c_ParameterName    = c_NamePrefix + "_BUS_BITRATE";
      c_Parameters[(s32_Loop * 8) + 1].u16_ParameterValue = 125U;

      c_Parameters[(s32_Loop * 8) + 2].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 2);
      c_Parameters[(s32_Loop * 8) + 2].c_ParameterName    = c_NamePrefix + "_MASK_STD";
      c_Parameters[(s32_Loop * 8) + 2].u16_ParameterValue = 0x51U;

      c_Parameters[(s32_Loop * 8) + 3].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 3);
      c_Parameters[(s32_Loop * 8) + 3].c_ParameterName    = c_NamePrefix + "_LW_MASK_EXT";
      c_Parameters[(s32_Loop * 8) + 3].u16_ParameterValue = 0U;

      c_Parameters[(s32_Loop * 8) + 4].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 4);
      c_Parameters[(s32_Loop * 8) + 4].c_ParameterName    = c_NamePrefix + "_HW_MASK_EXT";
      c_Parameters[(s32_Loop * 8) + 4].u16_ParameterValue = 0U;

      c_Parameters[(s32_Loop * 8) + 5].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 5);
      c_Parameters[(s32_Loop * 8) + 5].c_ParameterName    = c_NamePrefix + "_ID_STD";
      c_Parameters[(s32_Loop * 8) + 5].u16_ParameterValue = 0x51U;

      c_Parameters[(s32_Loop * 8) + 6].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 6);
      c_Parameters[(s32_Loop * 8) + 6].c_ParameterName    = c_NamePrefix + "_LW_ID_EXT";
      c_Parameters[(s32_Loop * 8) + 6].u16_ParameterValue = 0U;

      c_Parameters[(s32_Loop * 8) + 7].u8_ParameterIndex  = static_cast<uint8>((s32_Loop * 32) + 7);
      c_Parameters[(s32_Loop * 8) + 7].c_ParameterName    = c_NamePrefix + "_HW_ID_EXT";
      c_Parameters[(s32_Loop * 8) + 7].u16_ParameterValue = 0U;

      if (s32_Loop == 1)
      {
         //target side: we need to receive on ID + 1
         c_Parameters[(s32_Loop * 8) + 2].u16_ParameterValue++;
         c_Parameters[(s32_Loop * 8) + 5].u16_ParameterValue++;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueClientIDType(const bool oq_StdIDs, const bool oq_ExtIDs)
{
   c_Parameters[mu8_PARAM_INDEX_CLIENT_ID_TYPE].u16_ParameterValue =  static_cast<uint16>(oq_StdIDs ? 1U : 0U);
   c_Parameters[mu8_PARAM_INDEX_CLIENT_ID_TYPE].u16_ParameterValue += static_cast<uint16>(oq_ExtIDs ? 2U : 0U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueClientBitrate(const uint16 ou16_Bitrate_kBitS)
{
   c_Parameters[mu8_PARAM_INDEX_CLIENT_BUS_BITRATE].u16_ParameterValue = ou16_Bitrate_kBitS;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueClientMaskStandard(const uint16 ou16_Mask)
{
   c_Parameters[mu8_PARAM_INDEX_CLIENT_MASK_STD].u16_ParameterValue = ou16_Mask;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueClientMaskExtended(const uint32 ou32_Mask)
{
   c_Parameters[mu8_PARAM_INDEX_CLIENT_LW_MASK_EXT].u16_ParameterValue = static_cast<uint16>(ou32_Mask);
   c_Parameters[mu8_PARAM_INDEX_CLIENT_HW_MASK_EXT].u16_ParameterValue = static_cast<uint16>(ou32_Mask >> 16U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueClientIDStandard(const uint16 ou16_ID)
{
   c_Parameters[mu8_PARAM_INDEX_CLIENT_ID_STD].u16_ParameterValue = ou16_ID;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueClientIDExtended(const uint32 ou32_ID)
{
   c_Parameters[mu8_PARAM_INDEX_CLIENT_LW_ID_EXT].u16_ParameterValue = static_cast<uint16>(ou32_ID);
   c_Parameters[mu8_PARAM_INDEX_CLIENT_HW_ID_EXT].u16_ParameterValue = static_cast<uint16>(ou32_ID >> 16U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueTargetIDType(const bool oq_StdIDs, const bool oq_ExtIDs)
{
   c_Parameters[mu8_PARAM_INDEX_TARGET_ID_TYPE].u16_ParameterValue =  (oq_StdIDs ? 1U : 0U);
   c_Parameters[mu8_PARAM_INDEX_TARGET_ID_TYPE].u16_ParameterValue += (oq_ExtIDs ? 2U : 0U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueTargetBitrate(const uint16 ou16_Bitrate_kBitS)
{
   c_Parameters[mu8_PARAM_INDEX_TARGET_BUS_BITRATE].u16_ParameterValue = ou16_Bitrate_kBitS;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueTargetMaskStandard(const uint16 ou16_Mask)
{
   c_Parameters[mu8_PARAM_INDEX_TARGET_MASK_STD].u16_ParameterValue = ou16_Mask;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueTargetMaskExtended(const uint32 ou32_Mask)
{
   c_Parameters[mu8_PARAM_INDEX_TARGET_LW_MASK_EXT].u16_ParameterValue = static_cast<uint16>(ou32_Mask);
   c_Parameters[mu8_PARAM_INDEX_TARGET_HW_MASK_EXT].u16_ParameterValue = static_cast<uint16>(ou32_Mask >> 16U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueTargetIDStandard(const uint16 ou16_ID)
{
   c_Parameters[mu8_PARAM_INDEX_TARGET_ID_STD].u16_ParameterValue = ou16_ID;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLDivertParametersCAN::SetValueTargetIDExtended(const uint32 ou32_ID)
{
   c_Parameters[mu8_PARAM_INDEX_TARGET_LW_ID_EXT].u16_ParameterValue = static_cast<uint16>(ou32_ID);
   c_Parameters[mu8_PARAM_INDEX_TARGET_HW_ID_EXT].u16_ParameterValue = static_cast<uint16>(ou32_ID >> 16U);
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLActions::C_XFLActions(void) :
   C_XFLProtocol()
{
   //nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------

bool C_XFLActions::m_AllowSTWCID(const C_XFLCompanyID & orc_ConfiguredCompanyID,
                                 const C_XFLCompanyID & orc_RespondedCompanyID)
{
   C_SCLString c_Text;

   //STW internal version ?
   if (mu16_WinflashInternalKey == 0x3472U)
   {
      if ((orc_ConfiguredCompanyID.u8_NumBytes == 2U) && (orc_ConfiguredCompanyID.au8_Data[0] == 0x59U) &&
          (orc_ConfiguredCompanyID.au8_Data[1] == 0x2AU))
      {
         C_XFLActions::CompIDStructToString(orc_RespondedCompanyID, c_Text);
         TRG_ReportStatus(TGL_LoadStr(STR_FM_CORRECT_CID) + c_Text, gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
         return true; //allow flashing with STW-Company ID
      }
   }
   return false;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLActions::RequestNodeReset(const T_STWCAN_Msg_TX * const opt_ResetMessage, const bool oq_SingleNode)
{
   sint32 s32_Return = C_NO_ERR;

   if (opt_ResetMessage != NULL)
   {
      if (oq_SingleNode == true)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_RESETTING), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_RESETTING_S), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      }
      s32_Return = SendSingleMessage(*opt_ResetMessage);
   }
   else
   {
      if (oq_SingleNode == true)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_TURN_ON), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_TURN_ON_S), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
//bring server node into the wakeup state using the "wakeup_local_id" service
sint32 C_XFLActions::m_WakeupLocalID(const C_XFLCompanyID & orc_CompanyID, const bool oq_MultiResponsesOK)
{
   sint32 s32_Return;
   uint8 u8_NumFound;
   C_XFLCompanyID c_CompIDOut;

   s32_Return = WakeupLocalId(orc_CompanyID, &u8_NumFound, &c_CompIDOut);
   switch (s32_Return)
   {
   case C_NO_ERR:
      if ((oq_MultiResponsesOK == false) && (u8_NumFound > 1U)) //more than one node active means trouble ...
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_MULTI_ID), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         s32_Return = -1;
      }
      break;
   case C_COM:
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_LOCALID), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      s32_Return = -1;
      break;
   case C_DEFAULT:
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_USER_ABORT), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      s32_Return = -1;
      break;
   case C_WARN:
      if ((oq_MultiResponsesOK == false) && (u8_NumFound > 1U)) //more than one node active means trouble ...
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_MULTI_ID), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         s32_Return = -1;
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_COMPANYID), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         if (m_AllowSTWCID(orc_CompanyID, c_CompIDOut) == true)
         {
            s32_Return = C_NO_ERR;
         }
         else
         {
            s32_Return = -1;
         }
      }
      break;
   default:
      s32_Return = -1;
      break;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
//bring server node into the wakeup state using the "wakeup_with_snr" service
sint32 C_XFLActions::m_WakeupSNR(const C_XFLCompanyID & orc_CompanyID, const uint8 (&orau8_SNR)[6],
                                 uint8 & oru8_LocalID)
{
   sint32 s32_Return;
   C_XFLCompanyID c_CompIDOut;

   s32_Return = WakeupSerialNumber(orau8_SNR, orc_CompanyID, oru8_LocalID, &c_CompIDOut);
   switch (s32_Return)
   {
   case C_NO_ERR:
      TRG_ReportStatus(TGL_LoadStr(STR_FM_WAKEUP_SNR_OK), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      break;
   case C_COM:
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_SNR), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      s32_Return = -1;
      break;
   case C_WARN:
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_COMPANYID), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      if (m_AllowSTWCID(orc_CompanyID, c_CompIDOut) == true)
      {
         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = -1;
      }
      break;
   default:
      s32_Return = -1;
      break;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Bring server node into wakeup state.

   This variation is to be used if:
   - the SNR and local ID of the server node are known
   - there might be multiple nodes with the same local ID and there might be multiple nodes with the same serial number

   Sequence:
   - Perform a node_wakeup with local ID. All nodes with this local ID will enter SNOOZE state.
   - Perform node_compid. All nodes with the local ID and the correct company ID will enter ACTIVE state.
   - Perform a get_snr. All ACTIVE nodes will respond with their serial number. If there is only one response we are
      finished: exactly one node is in ACTIVE state.
   - If there are multiple responses there are multiple nodes with the same SNR. So we perform a node_wakeup with
      serial number. Only that node will go into SNOOZE state. Then do a node_compid to bring it into active state.

   \param[in]   orc_CompanyID      company ID of server node
   \param[in]   orau8_SNR          serial number of server node
   \param[in]   ou8_LocalID        local ID of server node

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::m_WakeupLocalIDAndSNR(const C_XFLCompanyID & orc_CompanyID, const uint8 (&orau8_SNR)[6])
{
   const uint8 u8_MAX_NUM_ECUS_PER_LOCAL_ID = 100U;

   sint32 s32_Return;
   uint8 u8_NumFound;
   uint8 u8_Dummy;

   //first wakeup with local-ID:
   s32_Return = m_WakeupLocalID(orc_CompanyID, true);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   //wakeup OK -> get SNR(s)
   s32_Return = GetSNRExt(NULL, u8_MAX_NUM_ECUS_PER_LOCAL_ID, u8_NumFound);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_RD_SNR_CDN_WAKE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return s32_Return;
   }
   //SNR(s) was/were read.
   if (u8_NumFound > 1U)
   {
      //we need to do a wakeup with SNR:
      //Some versions of the C167 flashloader (e.g. ESX2) do not respond to "wakeup-snr"
      // if the nodes are already in the ACTIVE state.
      //-> send them to sleep first:
      NodeSleep();

      //-> do a wakeup with SNR
      //we know we have enough space -> cast
      s32_Return = m_WakeupSNR(orc_CompanyID, orau8_SNR, u8_Dummy);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   function for generic divert stream mechanism

   Assumptions:
   - server node is already in wakeup state
   - communication parameters are correctly set up in C_XFLProtocol (CfgSet* functions)

   Sequence:
   If activating:
   - send all parameters
   - activate divert stream
   If deactivating:
   - deactivate divert stream

   \param[in]   oq_On              false -> deactivate divert stream; true -> activate divert stream
   \param[in]   orc_DivertParams   parameters to send before turning divert mode on (not used with oq_On == false)
   \param[in]   oq_UseHopHandle    true -> use hop handle in protocol service
                                   (cf. STW flashloader specification for details)
   \param[in]   ou8_HopHandle      Handle depicting hop number (cf. STW flashloader specification for details)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server                 \n
   C_RANGE            ou8_HopHandle is != 0 with protocol versions < V3.02r0
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::DivertStreamOnOff(const bool oq_On, const C_XFLDivertParameters & orc_DivertParams,
                                       const bool oq_UseHopHandle, const uint8 ou8_HopHandle)
{
   sint32 s32_Return;
   uint8 au8_Params[2];

   if (oq_On == false)
   {
      //turn divert stream off
      if (oq_UseHopHandle == true)
      {
         s32_Return = this->DivertStream(orc_DivertParams.u8_DeviceIndex, orc_DivertParams.u8_SelectedPosition, 0U,
                                         ou8_HopHandle);
      }
      else
      {
         s32_Return = this->DivertStream(orc_DivertParams.u8_DeviceIndex, orc_DivertParams.u8_SelectedPosition, 0U);
      }
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DIVERT_STREAM), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FM_DIVERT_STREAM_OK), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }
   else
   {
      //turn divert stream on
      //first send all parameters:
      for (sint32 s32_Parameter = 0; s32_Parameter < orc_DivertParams.c_Parameters.GetLength(); s32_Parameter++)
      {
         TRG_ReportStatus(TGL_LoadStr(
                             STR_FM_TXT_SET_PARA_PT1) + orc_DivertParams.c_Parameters[s32_Parameter].c_ParameterName +
                          TGL_LoadStr(STR_FM_TXT_SET_PARA_PT2) +
                          C_SCLString::IntToStr(orc_DivertParams.c_Parameters[s32_Parameter].u16_ParameterValue),
                          gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
         au8_Params[0] = static_cast<uint8>(orc_DivertParams.c_Parameters[s32_Parameter].u16_ParameterValue);
         au8_Params[1] = static_cast<uint8>(orc_DivertParams.c_Parameters[s32_Parameter].u16_ParameterValue >> 8U);

         s32_Return = SetDivertParameter(orc_DivertParams.u8_DeviceIndex, orc_DivertParams.u8_SelectedPosition,
                                         orc_DivertParams.c_Parameters[s32_Parameter].u8_ParameterIndex, au8_Params);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_SET_DIVERT_STREAM_PARA), gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return s32_Return;
         }
      }

      if (oq_UseHopHandle == true)
      {
         s32_Return = this->DivertStream(orc_DivertParams.u8_DeviceIndex, orc_DivertParams.u8_SelectedPosition, 1U,
                                         ou8_HopHandle);
      }
      else
      {
         s32_Return = this->DivertStream(orc_DivertParams.u8_DeviceIndex, orc_DivertParams.u8_SelectedPosition, 1U);
      }
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DIVERT_STREAM), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FM_DIVERT_STREAM_OK), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
//function for ESX2 BBB specific divert stream mechanism
sint32 C_XFLActions::DivertStreamOnOffBBB(const bool oq_OnOff, const uint8 ou8_TargetPosition,
                                          const uint8 (&orau8_UserID)[2])
{
   sint32 s32_Return;

   if (oq_OnOff == false)
   {
      //Divert Stream abschalten
      s32_Return = DivertStream(XFL_DIVERT_TARGET_BABY_B, ou8_TargetPosition, 0U);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DIVERT_STREAM), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FM_DIVERT_STREAM_OK), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }
   else
   {
      //divert stream on
      TRG_ReportStatus(TGL_LoadStr(STR_FM_WAKEUP_BBB), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      s32_Return = BBWakeup(ou8_TargetPosition, orau8_UserID);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_WAKEUP_BBB), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FM_WAKEUP_BBB_OK), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      TRG_ReportStatus(TGL_LoadStr(STR_FM_DIVERT_STREAM_BBB), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      s32_Return = DivertStream(XFL_DIVERT_TARGET_BABY_B, ou8_TargetPosition, 1U);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DIVERT_STREAM), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FM_DIVERT_STREAM_OK), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read flash information

   read all flash memory information from server

   \param[out]   orc_Information    structure containing all data from the server
   \param[out]   orc_ErrorText      readable error message

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::ReadFlashInformation(C_XFLFlashInformation & orc_Information, C_SCLString & orc_ErrorText)
{
   sint32 s32_Return;
   uint8 u8_Index;
   uint8 u8_RegionIndex;
   uint8 u8_NumICs;
   uint8 u8_NumRegions;
   C_XFLFlashICInformation * pt_IC;

   s32_Return = GetFlashInformationNumberOfICs(u8_NumICs);
   if (s32_Return != C_NO_ERR)
   {
      orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_NUM_FLASH_IC);
      return s32_Return;
   }
   orc_Information.c_ICs.SetLength(u8_NumICs);

   s32_Return = GetFlashInformationProtectedSectors(orc_Information.c_ProtectedSectors);
   if (s32_Return != C_NO_ERR)
   {
      orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_PSEC_INFO);
      return s32_Return;
   }

   //read out information about all flash ICs
   for (u8_Index = 0U; u8_Index < u8_NumICs; u8_Index++)
   {
      pt_IC = &orc_Information.c_ICs[u8_Index];

      s32_Return = GetFlashInformationTotalMemorySize(u8_Index, pt_IC->u32_TotalSize);
      if (s32_Return != C_NO_ERR)
      {
         orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_MEM_SIZE_IC) + C_SCLString::IntToStr(u8_Index) + "!";
         return s32_Return;
      }

      s32_Return = GetFlashInformationOffsetSector0(u8_Index, pt_IC->u32_Sector0Offset);
      if (s32_Return != C_NO_ERR)
      {
         orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_SEC_OFFSET_IC) + C_SCLString::IntToStr(u8_Index) + "!";
         return s32_Return;
      }

      s32_Return = GetFlashInformationEraseTime(u8_Index, pt_IC->u32_SectorEraseTime);
      if (s32_Return != C_NO_ERR)
      {
         orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_MAX_ERASE) + C_SCLString::IntToStr(u8_Index) + "!";
         return s32_Return;
      }

      s32_Return = GetFlashInformationWriteTime(u8_Index, pt_IC->u32_ProgrammingTime);
      if (s32_Return != C_NO_ERR)
      {
         orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_MAX_PRG_TIME) + C_SCLString::IntToStr(u8_Index) + "!";
         return s32_Return;
      }

      s32_Return = GetFlashInformationNumberRegions(u8_Index, u8_NumRegions);
      if (s32_Return != C_NO_ERR)
      {
         orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_NUM_REGIONS) + C_SCLString::IntToStr(u8_Index) + "!";
         return s32_Return;
      }

      orc_Information.c_ICs[u8_Index].c_Regions.SetLength(u8_NumRegions);
      for (u8_RegionIndex = 0U; u8_RegionIndex < static_cast<uint8>(pt_IC->c_Regions.GetLength()); u8_RegionIndex++)
      {
         s32_Return = GetFlashInformationRegionInformation(u8_Index, u8_RegionIndex,
                                                           pt_IC->c_Regions[u8_RegionIndex].u32_BlockSize,
                                                           pt_IC->c_Regions[u8_RegionIndex].u16_NumBlocks);
         if (s32_Return != C_NO_ERR)
         {
            orc_ErrorText = TGL_LoadStr(STR_FM_ERR_RD_REGION_INFO_IC) + C_SCLString::IntToStr(u8_Index) + "!";
            return s32_Return;
         }
      }
   }

   //aliasing information
   s32_Return = GetFlashInformationAliases(orc_Information.c_Aliases);
   if (s32_Return != C_NO_ERR)
   {
      //spec: "The server shall respond with "OUT_OF_RANGE" error if they are not supported."
      orc_Information.c_Aliases.SetLength(0);
      //no error ...
   }

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the total number of sectors in this IC

   \return
   total number of sectors in this IC
*/
//----------------------------------------------------------------------------------------------------------------------
uint16 C_XFLFlashICInformation::GetNumberOfSectors(void) const
{
   sint32 s32_RangeIndex;
   uint16 u16_NumSectors = 0U;

   for (s32_RangeIndex = 0; s32_RangeIndex < this->c_Regions.GetLength(); s32_RangeIndex++)
   {
      u16_NumSectors += this->c_Regions[s32_RangeIndex].u16_NumBlocks;
   }
   return u16_NumSectors;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   convert flash mapping table to a linear array with all the individual sectors

   The flash information structure contains all information in a compact format.
   This is good for storage but not for searching through a list of all sectors.
   So this functions can be used to extract the sector mapping information from the
    structure and put it in a table.

   \param[out]  orc_Sectors           array of sectors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLFlashInformation::ConvertToFlashSectorTable(C_XFLFlashSectors & orc_Sectors) const
{
   sint32 s32_ICIndex;
   sint32 s32_RangeIndex;
   uint32 u32_SectorIndex;
   uint32 u32_TotalIndex;
   uint32 u32_CurrentOffset;
   sint32 s32_ProtectedIndex;
   const C_XFLFlashICInformation * pt_IC;

   uint16 u16_NumBlocks;

   orc_Sectors.SetLength(0);

   //first go through list to set at_Sectors.Length (faster than dynamically incrementing it all the time)
   for (s32_ICIndex = 0; s32_ICIndex < this->c_ICs.GetLength(); s32_ICIndex++)
   {
      orc_Sectors.IncLength(this->c_ICs[s32_ICIndex].GetNumberOfSectors());
   }

   u32_TotalIndex = 0;
   for (s32_ICIndex = 0; s32_ICIndex < this->c_ICs.GetLength(); s32_ICIndex++)
   {
      pt_IC = &this->c_ICs[s32_ICIndex];

      u32_CurrentOffset = pt_IC->u32_Sector0Offset;
      for (s32_RangeIndex = 0; s32_RangeIndex < pt_IC->c_Regions.GetLength(); s32_RangeIndex++)
      {
         u16_NumBlocks = pt_IC->c_Regions[s32_RangeIndex].u16_NumBlocks;
         for (u32_SectorIndex = 0U; u32_SectorIndex < static_cast<uint32>(u16_NumBlocks); u32_SectorIndex++)
         {
            orc_Sectors[u32_TotalIndex].u32_LowestAddress  = u32_CurrentOffset;
            orc_Sectors[u32_TotalIndex].u32_HighestAddress = u32_CurrentOffset +
                                                             (pt_IC->c_Regions[s32_RangeIndex].u32_BlockSize - 1);
            orc_Sectors[u32_TotalIndex].u8_ICIndex = static_cast<uint8>(s32_ICIndex);

            orc_Sectors[u32_TotalIndex].q_IsProtected = false;
            for (s32_ProtectedIndex = 0; s32_ProtectedIndex < this->c_ProtectedSectors.GetLength();
                 s32_ProtectedIndex++)
            {
               if ((this->c_ProtectedSectors[s32_ProtectedIndex].u16_SectorNumber ==
                    static_cast<uint16>(u32_TotalIndex)) &&
                   (this->c_ProtectedSectors[s32_ProtectedIndex].u8_ICIndex == static_cast<uint8>(s32_ICIndex)))
               {
                  orc_Sectors[u32_TotalIndex].q_IsProtected = true;
                  break;
               }
            }
            u32_CurrentOffset += pt_IC->c_Regions[s32_RangeIndex].u32_BlockSize;
            u32_TotalIndex++;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns true if specified address lies within sector range

   \param[in]   ou32_Address    address to check

   \return
   true:      (ou32_Address >= u32_LowestAddress) && (ou32_Address <= u32_HighestAddress)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_XFLFlashSector::IsAddressWithinSector(const uint32 ou32_Address) const
{
   return ((ou32_Address >= u32_LowestAddress) && (ou32_Address <= u32_HighestAddress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the sector index the specified address resides in

   \param[in]   ou32_Address    address to check
   \param[out]  oru16_Sector    sector the address resides in

   \return
   C_NO_ERR    no problems; oru16_Sector contains sector index
   else        could not find address within any sector
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashSectors::GetSectorOccupiedByAddress(const uint32 ou32_Address, uint16 & oru16_Sector) const
{
   sint32 s32_Return = C_RANGE;
   sint32 s32_Index;

   for (s32_Index = 0; s32_Index < this->GetLength(); s32_Index++)
   {
      if (this->operator [](s32_Index).IsAddressWithinSector(ou32_Address) == true)
      {
         oru16_Sector = static_cast<uint16>(s32_Index);
         s32_Return = C_NO_ERR;
         break;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the erase time for a specified linear sector

   \param[in]   ou16_Sector         Linear sector index (0 = first sector in IC 0)

   \return
   Erase time of specified sector in [ms] (0xFFFFFFFFU if sector is not valid)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_XFLFlashInformation::GetEraseTimeByLinearSectorNumber(const uint16 ou16_Sector) const
{
   uint16 u16_NumSectors = 0U;
   uint32 u32_EraseTime = 0xFFFFFFFFU;
   sint32 s32_ICIndex;

   for (s32_ICIndex = 0; s32_ICIndex < this->c_ICs.GetLength(); s32_ICIndex++)
   {
      u16_NumSectors += this->c_ICs[s32_ICIndex].GetNumberOfSectors();
      if (ou16_Sector < u16_NumSectors)
      {
         u32_EraseTime = this->c_ICs[s32_ICIndex].u32_SectorEraseTime;
         break;
      }
   }
   return u32_EraseTime;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the maximum write time for up to 255 bytes of data

   Returns time taken for slowest IC

   \return
   Write to for up to 255 bytes for slowest IC in [ms] (0xFFFFFFFF if no ICs are defined)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_XFLFlashInformation::GetWriteTimeOfSlowestIC(void) const
{
   uint32 u32_WriteTime;
   sint32 s32_ICIndex;

   if (this->c_ICs.GetLength() == 0)
   {
      u32_WriteTime = 0xFFFFFFFFU;
   }
   else
   {
      u32_WriteTime = 0U;
      for (s32_ICIndex = 0; s32_ICIndex < this->c_ICs.GetLength(); s32_ICIndex++)
      {
         if (this->c_ICs[s32_ICIndex].u32_ProgrammingTime > u32_WriteTime)
         {
            u32_WriteTime = this->c_ICs[s32_ICIndex].u32_ProgrammingTime;
         }
      }
   }
   return u32_WriteTime;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert company-ID string to byte-array to send to server

   cf. details in STWCOMPID::CID_string_to_bytes

   \param[in]     orc_String      Company-ID as string
   \param[out]    orc_Data        encoded company-ID

   \return
   C_NO_ERR   converted company-id      \n
   C_CONFIG   invalid company-id
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::CompIDStringToStruct(const C_SCLString & orc_String, C_XFLCompanyID & orc_Data)
{
   //just to make sure we don't get any crap:
   (void)memset(&orc_Data.au8_Data[0], 0, sizeof(orc_Data.au8_Data));
   return stw_company_id::CID_string_to_bytes(orc_String.c_str(), &orc_Data.au8_Data[0], &orc_Data.u8_NumBytes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert encoded company-ID structure to a displayable string

   cf. details in stw_company_id::CID_bytes_to_string

   \param[in]     orc_Data        company ID structure
   \param[out]    orc_String      string with displayable ID

   \return
   C_NO_ERR   converted company-id        \n
   C_CONFIG   invalid length
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::CompIDStructToString(const C_XFLCompanyID & orc_Data, C_SCLString & orc_String)
{
   sint32 s32_Return;
   charn acn_Help[6];

   s32_Return = stw_company_id::CID_bytes_to_string(&orc_Data.au8_Data[0], orc_Data.u8_NumBytes, &acn_Help[0]);
   if (s32_Return == C_NO_ERR)
   {
      orc_String = acn_Help;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert flash protocol driver error code to displayable error text

   Converts the return value of a CXFLProtocol funciton and an server-side error code
    to a displayable string.
   This functions is not in CXFLProtocol to keep it as target-independent as possible.

   \param[in]     os32_ReturnValue   return value of CXFKProtocol function
   \param[in]     ou8_ErrorCode      error code from server

   \return
   Displayable error text
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_XFLActions::XFLProtocolErrorToText(const sint32 os32_ReturnValue, const uint8 ou8_ErrorCode)
{
   C_SCLString c_Help;

   switch (os32_ReturnValue)
   {
   case C_NO_ERR: //no problems
      c_Help = "";
      break;
   case C_COM: //no response
      c_Help = TGL_LoadStr(STR_XFL_ERR_NO_RESPONSE);
      break;
   case C_RANGE: //invalid parameter
      c_Help = TGL_LoadStr(STR_XFL_ERR_INVALID_PARAMETER);
      break;
   case C_NOACT: //error response
      if ((ou8_ErrorCode >= XFL_ERR_APP_DEF_ERR_MIN) && (ou8_ErrorCode <= XFL_ERR_APP_DEF_ERR_MAX))
      {
         c_Help.PrintFormatted("%s %02x", TGL_LoadStr(STR_FDL_ERR_ERASE_NO_CODE).c_str(), ou8_ErrorCode);
      }
      else
      {
         switch (ou8_ErrorCode)
         {
         case XFL_ERR_CHECKSUM_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_TRANSMISSION_ERR);
            break;
         case XFL_ERR_FLASH_PROG_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_DURING_FLASH_PROG);
            break;
         case XFL_ERR_FLASH_ERASE_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_DURING_DEL);
            break;
         case XFL_ERR_CAN_BITTIME_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_BITRATE);
            break;
         case XFL_ERR_OUT_OF_RANGE_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_INVALID_PARA);
            break;
         case XFL_ERR_EE_VERIFY_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_EEP_VERIFY);
            break;
         case XFL_ERR_EE_READ_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_EEP_READ);
            break;
         case XFL_ERR_EE_WRITE_ERR:
            c_Help = TGL_LoadStr(STR_XFL_ERR_EEPROM_WRITE);
            break;
         case XFL_ERR_DIVERT_STREAM_COM_ERR:
            c_Help = TGL_LoadStr(STR_FM_ERR_DIVERT_STREAM_TRG_GW);
            break;
         case XFL_ERR_WRONG_REC_TYPE:
            c_Help = TGL_LoadStr(STR_FM_ERR_HEX_CMD_UNK);
            break;
         case XFL_ERR_UNDEFINED_ERROR: //no break;
         default:
            c_Help.PrintFormatted("%s Code: 0x%02x", TGL_LoadStr(STR_XFL_ERR_UNDEFINED_CODE).c_str(), ou8_ErrorCode);
            break;
         }
      }
      break;
   default:
      c_Help = TGL_LoadStr(STR_XFL_ERR_UNDEFINED_ERR);
      break;
   }
   return c_Help;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLActions::SNRBytesToString(const uint8 (&orau8_Data)[6], const bool oq_IncludeDots)
{
   C_SCLString c_Text;

   if (oq_IncludeDots == false)
   {
      c_Text.PrintFormatted("%02X%02X%02X%02X%02X%02X", orau8_Data[0], orau8_Data[1], orau8_Data[2], orau8_Data[3],
                            orau8_Data[4], orau8_Data[5]);
   }
   else
   {
      c_Text.PrintFormatted("%02X.%02X%02X%02X.%02X%02X", orau8_Data[0], orau8_Data[1], orau8_Data[2], orau8_Data[3],
                            orau8_Data[4], orau8_Data[5]);
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLActions::SNRStringToBytes(const C_SCLString & orc_Text, uint8 (&orau8_Data)[6])
{
   uint8 u8_Index;
   C_SCLString c_SNR;
   C_SCLString c_Help;
   sint32 s32_Return = C_NO_ERR;

   c_SNR = orc_Text;
   switch (orc_Text.Length())
   {
   case 12:
      break;
   case 14:
      (void)c_SNR.Delete(3U, 1U);
      (void)c_SNR.Delete(9U, 1U);
      break;
   default:
      return C_CONFIG;
   }

   try //is the whole thing decimal ?
   {
      (void)c_SNR.ToInt64();
   }
   catch (...)
   {
      s32_Return = C_WARN; //hex or a total mess ...
   }

   try
   {
      for (u8_Index = 0U; u8_Index < 6U; u8_Index++)
      {
         c_Help = static_cast<C_SCLString>("0x") + c_SNR[(static_cast<sintn>(u8_Index) * 2U) + 1U];
         orau8_Data[u8_Index] = static_cast<uint8>(static_cast<uint8>(c_Help.ToInt()) << 4U);
         c_Help = static_cast<C_SCLString>("0x") + c_SNR[(static_cast<sintn>(u8_Index) * 2U) + 2U];
         orau8_Data[u8_Index] += static_cast<uint8>(c_Help.ToInt());
      }
   }
   catch (...)
   {
      return C_CONFIG; //definitely a total mess ...
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search Nodes in network

   \param[in]     orc_CompanyID       company-id to use to wake up nodes (2, 3, 5 characters)
   \param[in]     ou32_StartTime      time to send "FLASH" message in milli seconds
   \param[in]     ou8_FlashInterval   gap between individual "FLASH" messages in milli seconds
   \param[out]    orc_FoundNodes      list with found nodes

   \return
   C_NO_ERR        no problems           \n
   C_NOACT         no nodes found        \n
   C_DEFAULT       aborted by user       \n
   C_OVERFLOW      too many nodes found  \n
   C_RANGE         invalid parameter(s)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::SearchNodes(const C_SCLString & orc_CompanyID, const uint32 ou32_StartTime,
                                 const uint8 ou8_FlashInterval, SCLDynamicArray<C_XFLFoundNode> & orc_FoundNodes)
{
   //maximum allowed: 100 ECU with same ID, really should be enough
   const uint8 u8_MAX_NUM_ECUS_PER_LOCAL_ID = 100U;

   sint32 s32_Return;
   uint8 au8_Found[XFL_NUM_DIFFERENT_LOCAL_IDS];
   C_XFLCompanyID c_CompIDIn;
   uint8 u8_NumFound;
   uint8 u8_NumSNRsFound;
   uint16 u16_ProtocolVersion;
   sint32 s32_j;
   uint8 aau8_SNRs[u8_MAX_NUM_ECUS_PER_LOCAL_ID][6];
   uint8 u8_Dummy;

   sint32 s32_IndexInTable;
   C_SCLString c_SNR;

   s32_Return = C_XFLActions::CompIDStringToStruct(orc_CompanyID, c_CompIDIn);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_COMP_ID_LENGTH), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_RANGE;
   }

   s32_Return = SearchId(au8_Found, &u8_NumFound, ou32_StartTime, ou8_FlashInterval);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }

   orc_FoundNodes.SetLength(u8_NumFound);

   TRG_ReportStatus(TGL_LoadStr(STR_FM_TXT_NUM_NODES) + C_SCLString::IntToStr(u8_NumFound),
                    gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

   if (u8_NumFound > 0U)
   {
      for (sint32 s32_Node = 0; s32_Node < orc_FoundNodes.GetLength(); s32_Node++)
      {
         orc_FoundNodes[s32_Node].c_SNR = "?\?\?";
         orc_FoundNodes[s32_Node].c_DeviceID = "?\?\?";
      }

      s32_IndexInTable = 0;
      for (sint32 s32_IdIndex = 0; s32_IdIndex < XFL_NUM_DIFFERENT_LOCAL_IDS; s32_IdIndex++)
      {
         if ((static_cast<uint16>(au8_Found[s32_IdIndex])) > u8_MAX_NUM_ECUS_PER_LOCAL_ID)
         {
            return C_OVERFLOW;
         }

         if (au8_Found[s32_IdIndex] > 0U)
         {
            for (s32_j = s32_IndexInTable; s32_j < (s32_IndexInTable + au8_Found[s32_IdIndex]); s32_j++)
            {
               orc_FoundNodes[s32_j].u8_NodeID = static_cast<uint8>(s32_IdIndex);
            }

            //read out SNR(s)
            CfgSetLocalId(static_cast<uint8>(s32_IdIndex));
            s32_Return = WakeupLocalId(c_CompIDIn, &u8_NumFound, NULL);
            if (s32_Return == C_WARN)
            {
               //allow C_WARN (company-ID error). we want to get all possible nodes here ...
               s32_Return = C_NO_ERR;
            }

            // < : if we have more responses now only use a max. of abFound[i]
            if ((s32_Return != C_NO_ERR) || (u8_NumFound < au8_Found[s32_IdIndex]))
            {
               TRG_ReportStatus(TGL_LoadStr(STR_FM_TXT_NODE_WAKE_PT1) + C_SCLString::IntToStr(s32_IdIndex) +
                                TGL_LoadStr(STR_FM_TXT_NODE_WAKE_PT2),
                                gu8_DL_REPORT_STATUS_TYPE_WARNING);
            }
            else
            {
               //wakeup OK -> get SNR(s)
               s32_Return = GetSNRExt(&aau8_SNRs[0][0], u8_MAX_NUM_ECUS_PER_LOCAL_ID, u8_NumSNRsFound);
               if (s32_Return != C_NO_ERR)
               {
                  TRG_ReportStatus(TGL_LoadStr(STR_FM_TXT_NODE_SNR_PT1) + C_SCLString::IntToStr(s32_IdIndex) +
                                   TGL_LoadStr(STR_FM_TXT_NODE_SNR_PT2),
                                   gu8_DL_REPORT_STATUS_TYPE_WARNING);
               }
               else
               {
                  //SNR(s) was/were read.
                  //We might have more responses here than we got originally:
                  //- maybe the previous wakeup only worked for part of the nodes
                  //  e.g. if only some of the nodes match the used company-ID.
                  //- if more than one node is present with the same local-id get_snr will most probably yield
                  //  multiple responses as well, due to the different SNR in the data byte area
                  //  (wakeup and send-comp-id can result in identical responses from different nodes, possibly
                  //   resulting in a number of the responses not actually making it to the CAN bus)
                  if (u8_NumSNRsFound > au8_Found[s32_IdIndex])
                  {
                     //more responses here than we had before:
                     //-> expand list
                     C_XFLFoundNode c_Node;
                     c_Node.u8_NodeID = static_cast<uint8>(s32_IdIndex);
                     c_Node.c_SNR = "?\?\?";
                     c_Node.c_DeviceID = "?\?\?";

                     for (s32_j = au8_Found[s32_IdIndex]; s32_j < u8_NumSNRsFound; s32_j++)
                     {
                        orc_FoundNodes.Insert(static_cast<uint16>(s32_IndexInTable), c_Node);
                        TRG_ReportStatus(TGL_LoadStr(STR_FM_TXT_SURP_NODE_AMBIG_ID) + C_SCLString::IntToStr(s32_IdIndex),
                                         gu8_DL_REPORT_STATUS_TYPE_WARNING);
                     }
                     au8_Found[s32_IdIndex] = u8_NumSNRsFound; //to be added to s32_IndexInTable later !
                  }

                  for (s32_j = s32_IndexInTable; s32_j < (s32_IndexInTable + u8_NumSNRsFound); s32_j++)
                  {
                     //convert bcd to serial number in STW format
                     //we know we have enough space -> cast
                     c_SNR = SNRBytesToString(aau8_SNRs[(s32_j - s32_IndexInTable) * 6], true);
                     orc_FoundNodes[s32_j].c_SNR = c_SNR;
                  }

                  //get device IDs
                  //we have to consider a number of scenarios here:
                  //- If there is more than one node in wakeup now we have to do a wakeup with SNR
                  //   to be sure to address a unique one.
                  //- We have to read the protocol-version in order to know the syntax of the get_device_id
                  //   service
                  for (s32_j = 0; s32_j < u8_NumSNRsFound; s32_j++)
                  {
                     if (au8_Found[s32_IdIndex] > 1)
                     {
                        //Some versions of the C167 flashloader (e.g. ESX2) do not respond to "wakeup-snr"
                        // if the nodes are already in the ACTIVE state.
                        //-> send them to sleep first:
                        NodeSleep();

                        //-> do a wakeup with SNR
                        //we know we have enough space -> cast
                        s32_Return = WakeupSerialNumber(aau8_SNRs[s32_j * 6], c_CompIDIn, u8_Dummy);
                     }
                     else
                     {
                        s32_Return = C_NO_ERR;
                     }

                     if (s32_Return == C_WARN)
                     {
                        //allow C_WARN as we want to display information for all nodes
                        s32_Return = C_NO_ERR;
                     }

                     if (s32_Return == C_NO_ERR)
                     {
                        //wakeup OK, or not required
                        //get flashloader protocol version:
                        s32_Return = GetImplementationInformationProtocolVersion(u16_ProtocolVersion);
                        if (s32_Return != C_NO_ERR)
                        {
                           //no response or error response: we have to assume we have a version <=
                           u16_ProtocolVersion = 0x0000U;
                        }

                        //get device ID
                        s32_Return = GetDeviceID(u16_ProtocolVersion >= 0x3000U,
                                                 orc_FoundNodes[s32_IndexInTable + s32_j].c_DeviceID);
                        if (s32_Return != C_NO_ERR)
                        {
                           TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DEV_ID_TYPE),
                                            gu8_DL_REPORT_STATUS_TYPE_WARNING);
                        }

                        //Lay node back to sleep (would not really be required in most cases; just to make sure to
                        // leave the network in a clearly defined state; there is at least one buggy legacy flashloader
                        // implementation (ESX1) where a subsequent "wakeup" would not work if the node is already
                        // awake)
                        NodeSleep();
                     }
                     else
                     {
                        TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_WAKE_SNR_PT1) + C_SCLString::IntToStr(s32_IdIndex) +
                                         TGL_LoadStr(STR_FM_ERR_WAKE_SNR_PT2) +
                                         TGL_LoadStr(STR_FM_ERR_WAKE_SNR_PT3), gu8_DL_REPORT_STATUS_TYPE_WARNING);
                     }
                  }
               }
            }
            s32_IndexInTable += au8_Found[s32_IdIndex]; //skip all the ones originally found ...
         }
      }
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read finger print information if available

   Read available finger print information from server

   \param[in,out]   orc_Information      target configuration and possibly read data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLActions::m_ReadFingerPrintInformation(C_XFLInformationFromServer & orc_Information)
{
   sint32 s32_Return;

   if (orc_Information.c_AvailableFeatures.q_FingerPrint == true)
   {
      s32_Return = GetFingerPrintSupportedIndexes(orc_Information.c_FingerPrintData.c_SupportedIndexes);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RE_LOI),
                          gu8_DL_REPORT_STATUS_TYPE_WARNING);
      }
      else
      {
         orc_Information.c_FingerPrintData.q_SupportedIndexesValid = true;
      }
      if (orc_Information.c_FingerPrintData.c_SupportedIndexes.q_ProgrammingDate == true)
      {
         s32_Return = GetFingerPrintProgrammingDate(orc_Information.c_FingerPrintData.u8_ProgrammingDateYear,
                                                    orc_Information.c_FingerPrintData.u8_ProgrammingDateMonth,
                                                    orc_Information.c_FingerPrintData.u8_ProgrammingDateDay);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RE_PROG_DATE),
                             gu8_DL_REPORT_STATUS_TYPE_WARNING);
         }
         else
         {
            orc_Information.c_FingerPrintData.q_ProgrammingDateValid = true;
         }
      }

      if (orc_Information.c_FingerPrintData.c_SupportedIndexes.q_ProgrammingTime == true)
      {
         s32_Return = GetFingerPrintProgrammingTime(orc_Information.c_FingerPrintData.u8_ProgrammingTimeHour,
                                                    orc_Information.c_FingerPrintData.u8_ProgrammingTimeMinute,
                                                    orc_Information.c_FingerPrintData.u8_ProgrammingTimeSecond);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RE_PROG_TIME),
                             gu8_DL_REPORT_STATUS_TYPE_WARNING);
         }
         else
         {
            orc_Information.c_FingerPrintData.q_ProgrammingTimeValid = true;
         }
      }

      if (orc_Information.c_FingerPrintData.c_SupportedIndexes.q_UsernamePart1 == true)
      {
         s32_Return = GetFingerPrintUserName(orc_Information.c_FingerPrintData.c_UserName);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RE_USER_NAME),
                             gu8_DL_REPORT_STATUS_TYPE_WARNING);
         }
         else
         {
            orc_Information.c_FingerPrintData.q_UserNameValid = true;
         }
      }

      if (orc_Information.c_FingerPrintData.c_SupportedIndexes.q_ApplicationCheckSum == true)
      {
         s32_Return = GetFingerPrintChecksum(orc_Information.c_FingerPrintData.u32_Checksum);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RE_CHECKSUM),
                             gu8_DL_REPORT_STATUS_TYPE_WARNING);
         }
         else
         {
            orc_Information.c_FingerPrintData.q_ChecksumValid = true;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read device info information if available

   Read available finger print information from server

   \param[in,out]   orc_Information      target configuration and possibly read data
                                          (u16_ProtocolVersion must already have been set !)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLActions::m_ReadDeviceInfo(C_XFLInformationFromServer & orc_Information)
{
   sint32 s32_Return;
   uint8 u8_Block;

   if (orc_Information.c_AvailableFeatures.q_DeviceInfo == true)
   {
      s32_Return = GetDeviceInfoAddresses(orc_Information.c_DeviceInfoAddresses, orc_Information.u16_ProtocolVersion);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RE_DEV_INFO_ADDR),
                          gu8_DL_REPORT_STATUS_TYPE_WARNING);
         return;
      }
      orc_Information.q_DeviceInfoAddressesValid = true;
   }

   orc_Information.c_DeviceInfoBlocks.SetLength(orc_Information.c_DeviceInfoAddresses.GetLength());
   orc_Information.c_DeviceInfoBlocksValid.SetLength(orc_Information.c_DeviceInfoAddresses.GetLength());

   for (u8_Block = 0U; u8_Block < orc_Information.c_DeviceInfoBlocks.GetLength(); u8_Block++)
   {
      //read data block from this address
      s32_Return = GetDeviceInfoBlock(orc_Information.c_DeviceInfoAddresses[u8_Block],
                                      orc_Information.c_DeviceInfoBlocks[u8_Block]);
      if (s32_Return == C_NO_ERR)
      {
         orc_Information.c_DeviceInfoBlocksValid[u8_Block] = 1U;
      }
      else
      {
         orc_Information.c_DeviceInfoBlocksValid[u8_Block] = 0U;
         //This is not considered a hard error: probably the application just does not have the project information
         // at the correct spot. Or it did not set the address properly.
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLFeaturesAvailable::C_XFLFeaturesAvailable(void)
{
   this->Clear();
};

//----------------------------------------------------------------------------------------------------------------------

void C_XFLFeaturesAvailable::Clear(void)
{
   q_ProtocolVersionGE3 = false;
   q_SMMHandling = false;
   q_EraseCount = false;
   q_SectorBasedCRCs = false;
   q_BlockBasedCRCsEEPROM = false;
   q_BlockBasedCRCsFlash = false;
   q_ListAvailableServices = false;
   q_FlashInformation = false;
   q_FingerPrint = false;
   q_DeviceInfo = false;
}

//----------------------------------------------------------------------------------------------------------------------
//determine available features from list if services available on server
void C_XFLFeaturesAvailable::DetermineFeatures(const C_XFLImplementedServices & orc_Services,
                                               const uint16 ou16_ProtocolVersion)
{
   this->q_ProtocolVersionGE3 = (ou16_ProtocolVersion >= 0x3000U) ? true : false;

   this->q_SMMHandling     = orc_Services.q_GetTimeOutFactor && orc_Services.q_SetTimeoutFactor;
   this->q_EraseCount      = orc_Services.q_GetDownloadCount;
   this->q_SectorBasedCRCs = orc_Services.q_GetSectorChecksum &&
                             orc_Services.q_GetSectorModeCompare &&
                             orc_Services.q_SetSectorChecksum &&
                             orc_Services.q_SetSectorModeCompare;
   this->q_BlockBasedCRCsEEPROM = orc_Services.q_GetBlockStartAddress &&
                                  orc_Services.q_GetBlockEndAddress &&
                                  orc_Services.q_GetBlockChecksum &&
                                  orc_Services.q_SetBlockStartAddress &&
                                  orc_Services.q_SetBlockEndAddress &&
                                  orc_Services.q_SetBlockChecksum &&
                                  orc_Services.q_GetBlockCompareMode &&
                                  orc_Services.q_SetBlockCompareMode;
   this->q_BlockBasedCRCsFlash = orc_Services.q_GetBlockStartAddress &&
                                 orc_Services.q_GetBlockEndAddress &&
                                 orc_Services.q_GetBlockChecksum &&
                                 (!orc_Services.q_SetBlockStartAddress) &&
                                 (!orc_Services.q_SetBlockEndAddress) &&
                                 (!orc_Services.q_SetBlockChecksum) &&
                                 orc_Services.q_GetBlockCompareMode &&
                                 (!orc_Services.q_SetBlockCompareMode); //must not be available
   this->q_ListAvailableServices = orc_Services.q_GetImplementationInformation;
   this->q_FlashInformation = orc_Services.q_GetFlashInformation;
   this->q_FingerPrint      = orc_Services.q_SetFingerPrint && orc_Services.q_GetFingerPrint;
   this->q_DeviceInfo       = orc_Services.q_GetDeviceInfoAddress && orc_Services.q_ReadFlash;
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLFingerPrintInformationFromServer::C_XFLFingerPrintInformationFromServer(void) :
   q_SupportedIndexesValid(false),
   u8_ProgrammingDateYear(0U),
   u8_ProgrammingDateMonth(0U),
   u8_ProgrammingDateDay(0U),
   q_ProgrammingDateValid(false),
   u8_ProgrammingTimeHour(0U),
   u8_ProgrammingTimeMinute(0U),
   u8_ProgrammingTimeSecond(0U),
   q_ProgrammingTimeValid(false),
   q_UserNameValid(false),
   u32_Checksum(0U),
   q_ChecksumValid(false)
{
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLInformationFromServer::C_XFLInformationFromServer(void) :
   u16_ProtocolVersion(0U),
   q_ProtocolVersionValid(false),
   u8_LocalID(0U),
   q_LocalIDValid(false),
   q_SerialNumberValid(false),
   u16_SectorCount(0U),
   q_SectorCountValid(false),
   u32_ControlID(0U),
   q_ControlIDValid(false),
   q_DeviceIDValid(false),
   q_FlashloaderVersionValid(false),
   u32_EraseCount(0U),
   q_EraseCountValid(false),
   q_ImplementationInformationAvailableServicesValid(false),
   u8_ImplementationInformationMaxHexRecordLength(0U),
   u8_ImplementationInformationHexRecordGranularity(0U),
   q_ImplementationInformationHexFileInformationValid(false),
   q_DeviceInfoAddressesValid(false),
   q_FlashMappingInformationValid(false),
   u8_SMMEraseTimeoutFactor(0U),
   q_SMMEraseTimeoutFactorValid(false),
   u8_SMMWriteTimeoutFactor(0U),
   q_SMMWriteTimeoutFactorValid(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read all available information from server

   Assumption: server node is already in wakeup state.
   On errors on individual read requests TRG_ReportStatus will be executed and the process will be
    continued with the next piece of information.

   Will read everything it can get, except:
   - sector / area checksums
   - flash content dump
   - EEPROM content dump
   - anything related to ESX2 divert stream

   \param[out]    orc_Info    all information that could be read

   \return
   C_NO_ERR     executed / check callback texts and valid flags in structure for details \n
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::ReadServerInformation(C_XFLInformationFromServer & orc_Info)
{
   sint32 s32_Return;
   sint32 s32_Loop;
   uint8 u8_Size;
   uint8 au8_Version[5];
   uint8 au8_SNR[6];
   uint32 u32_FlashloaderVersion;
   uint16 u16_Ident;
   sint16 s16_Device = KNOWN_DEVICE_UNKNOWN;
   C_SCLString c_ErrorText;
   C_XFLImplementedServices * pt_Services;
   C_XFLFeaturesAvailable  * pt_Features;

   //shortcuts:
   pt_Services = &orc_Info.c_ImplementationInformationAvailableServices;
   pt_Features = &orc_Info.c_AvailableFeatures;

   //read protocol version
   s32_Return = GetImplementationInformationProtocolVersion(orc_Info.u16_ProtocolVersion);
   if (s32_Return == C_NO_ERR)
   {
      orc_Info.q_ProtocolVersionValid = true;
   }
   else
   {
      //no response or error response: we have to assume we have a version < 3.00r0
      orc_Info.u16_ProtocolVersion = 0x0000U;
      //do not report: this can be expected with older services and is not a hard error !
   }

   //read device ID
   //we need to do this before trying to detect availabe features as there are some checks depending on hard-coded
   // device IDs (sigh)
   s32_Return = GetDeviceID((orc_Info.u16_ProtocolVersion >= 0x3000U) ? true : false, orc_Info.c_DeviceID);
   if (s32_Return == C_NO_ERR)
   {
      orc_Info.q_DeviceIDValid = true;

      //is it one of the devices for which we have a special hard-coded handling ?
      for (s32_Loop = 0; s32_Loop < KNOWN_DEVICE_MAX; s32_Loop++)
      {
         if (orc_Info.c_DeviceID == static_cast<C_SCLString>(&maacn_KNOWN_DEVICE_NAMES[s32_Loop][0]))
         {
            s16_Device = static_cast<sint16>(s32_Loop);
         }
      }
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(STR_FW_ERR_READ_DEV_ID),
                       gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }

   //read flashloader implementation version number
   //default: assume no special stuff is available:
   pt_Features->Clear();

   s32_Return = GetVersionNumber(au8_Version, u8_Size);
   if (s32_Return == C_NO_ERR)
   {
      switch (u8_Size)
      {
      case 1U: //"old" implementation
         //BCD coded
         orc_Info.c_FlashloaderVersion = C_SCLString::IntToStr(static_cast<uint8>(au8_Version[0] >> 4U)) + "." +
                                         C_SCLString::IntToStr(au8_Version[0] & 0x0FU);

         orc_Info.q_FlashloaderVersionValid = true;
         break;
      case 5U:
         u16_Ident = static_cast<uint16>(((static_cast<uint16>(au8_Version[3])) << 8U) + au8_Version[4]);
         orc_Info.c_FlashloaderVersion.PrintFormatted("%c.%c%cr%d", au8_Version[0], au8_Version[1], au8_Version[2],
                                                      u16_Ident);
         u32_FlashloaderVersion = ((au8_Version[0] - 48U) * 100U) +
                                  ((au8_Version[1] - 48U) * 10U) +
                                  (au8_Version[2] - 48U);

         if (orc_Info.u16_ProtocolVersion >= 0x3000U) //read_implementation_info should be available !
         {
            s32_Return = GetImplementationInformationServices(*pt_Services);
            if (s32_Return != C_NO_ERR)
            {
               TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_IMPL_SERVICES),
                                gu8_DL_REPORT_STATUS_TYPE_WARNING);
            }
            else
            {
               pt_Features->DetermineFeatures(*pt_Services, orc_Info.u16_ProtocolVersion);
            }
         }
         else
         {
            //hard coded decisions on what we expect to be implemented
            // (depressing, but we need a way to stay compatible to server's that do not provide information about
            //  themselves):
            if (u32_FlashloaderVersion >= 202U)
            {
               pt_Features->q_SectorBasedCRCs = true;
            }
            if (((s16_Device == KNOWN_DEVICE_ESX) && (u32_FlashloaderVersion >= 203U)) ||
                (s16_Device == KNOWN_DEVICE_EHC))
            {
               pt_Features->q_SMMHandling = true;
            }
            if ((u32_FlashloaderVersion >= 203U) &&
                ((s16_Device == KNOWN_DEVICE_ESX) || (s16_Device == KNOWN_DEVICE_ESXLT) ||
                 (s16_Device == KNOWN_DEVICE_EHC)))
            {
               pt_Features->q_EraseCount = true;
            }
         }
         orc_Info.q_FlashloaderVersionValid = true;
         break;
      default:
         break; //no valid size
      }
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(STR_FW_ERR_READ_XFL_VER),
                       gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }

   //read number of flash sectors
   s32_Return = GetSectorCount(orc_Info.u16_SectorCount);
   if (s32_Return == C_NO_ERR)
   {
      orc_Info.q_SectorCountValid = true;
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(STR_FW_ERR_READ_SEC_COUNT),
                       gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }

   //read control ID
   s32_Return = GetControlID(orc_Info.u32_ControlID);
   if (s32_Return == C_NO_ERR)
   {
      orc_Info.q_ControlIDValid = true;
   }
   else
   {
      //no warning: control ID service is optional !
      //however also a lot of ECUs with protocol < V3.x have it implemented so we cannot simply check the availability
      // bits
   }

   //read ESX2 SMM information:
   if (pt_Features->q_SMMHandling == true)
   {
      //read and display SMM settings
      s32_Return = GetTimeoutFactor(0U, orc_Info.u8_SMMEraseTimeoutFactor);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(STR_FW_ERR_READ_ERASE_TO),
                          gu8_DL_REPORT_STATUS_TYPE_WARNING);
      }
      else
      {
         orc_Info.q_SMMEraseTimeoutFactorValid = true;
      }

      s32_Return = GetTimeoutFactor(1, orc_Info.u8_SMMWriteTimeoutFactor);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(STR_FW_ERR_READ_WRITE_TO),
                          gu8_DL_REPORT_STATUS_TYPE_WARNING);
      }
      else
      {
         orc_Info.q_SMMWriteTimeoutFactorValid = true;
      }
   }

   //read "erase count"
   if (pt_Features->q_EraseCount == true)
   {
      s32_Return = GetEraseCount(orc_Info.u32_EraseCount);
      if (s32_Return == C_NO_ERR)
      {
         orc_Info.q_EraseCountValid = true;
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(STR_FW_ERR_READ_DL_COUNT),
                          gu8_DL_REPORT_STATUS_TYPE_WARNING);
      }
   }

   //read finger print information
   m_ReadFingerPrintInformation(orc_Info);

   //read device_info information
   m_ReadDeviceInfo(orc_Info);

   //read hex record support
   if (pt_Features->q_ListAvailableServices == true) //goes with this service ...
   {
      s32_Return = GetImplementationInformationHexRecords(orc_Info.u8_ImplementationInformationMaxHexRecordLength,
                                                          orc_Info.u8_ImplementationInformationHexRecordGranularity);
      if (s32_Return == C_NO_ERR)
      {
         orc_Info.q_ImplementationInformationHexFileInformationValid = true;
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RD_HEX_REC_SUP_INF),
                          gu8_DL_REPORT_STATUS_TYPE_WARNING);
      }
   }

   //read flash information
   if (pt_Features->q_FlashInformation == true)
   {
      s32_Return = this->ReadFlashInformation(orc_Info.c_FlashMappingInformation, c_ErrorText);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + c_ErrorText, gu8_DL_REPORT_STATUS_TYPE_WARNING);
      }
      else
      {
         orc_Info.q_FlashMappingInformationValid = true;
      }
   }

   //read local ID (mandatory function for all implementations)
   s32_Return = GetLocalID(orc_Info.u8_LocalID);
   if (s32_Return == C_NO_ERR)
   {
      orc_Info.q_LocalIDValid = true;
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(
                          STR_FW_ERR_READ_LID), gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }

   //read SNR (mandatory function for all implementations)
   s32_Return = GetSNR(au8_SNR);
   if (s32_Return == C_NO_ERR)
   {
      orc_Info.q_SerialNumberValid = true;
      orc_Info.c_SerialNumber = C_XFLActions::SNRBytesToString(au8_SNR, true);
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + ": " + TGL_LoadStr(
                          STR_FW_ERR_READ_SNR), gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read block based flash checksum data from server

   Assumption: server node is already in wakeup state.
   On errors on individual read requests TRG_ReportStatus will be executed with detailed error
   information. The process will be continued if possible.

   \param[out]    orc_ChecksumInformation    read information

   \return
   C_NO_ERR     executed / check TRG_ReportStatus texts for warnings   \n
   C_RANGE      invalid function parameter                             \n
   C_COM        no response from server                                \n
   C_NOACT      error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::ReadServerBlockChecksumInformation(C_XFLChecksumAreas & orc_ChecksumInformation)
{
   sint32 s32_Return;
   sint32 i;
   bool q_CheckStartup;
   bool q_CheckAfterFlashing;
   uint32 u32_CheckSumEEPROM;
   uint32 u32_CheckSumCalc;

   orc_ChecksumInformation.c_Areas.SetLength(0);
   orc_ChecksumInformation.q_IsBlockBased = true;

   s32_Return = GetBlockAddressesAll(orc_ChecksumInformation.c_BlockConfig);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_ERROR) + ": " + TGL_LoadStr(STR_FW_ERR_READ_CRC),
                       gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return s32_Return;
   }

   orc_ChecksumInformation.c_Areas.SetLength(orc_ChecksumInformation.c_BlockConfig.GetLength());

   tgl_assert(orc_ChecksumInformation.c_BlockConfig.GetLength() < 0xFF);

   for (i = 0; i < static_cast<sint32>(orc_ChecksumInformation.c_BlockConfig.GetLength()); i++)
   {
      q_CheckStartup = false;
      q_CheckAfterFlashing = false;
      u32_CheckSumEEPROM = 0U;
      u32_CheckSumCalc = 0U;

      //only read checksums for configured blocks:
      if ((orc_ChecksumInformation.c_BlockConfig[i].q_BlockDefinitionValid == true) &&
          (orc_ChecksumInformation.c_BlockConfig[i].u32_EndAddress != XFL_CHK_BLOCK_END_INVALID))
      {
         s32_Return = GetBlockChecksum(static_cast<uint8>(i), 0U, u32_CheckSumEEPROM);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_ERROR) + ": " + TGL_LoadStr(STR_FW_ERR_READ_CRC),
                             gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return s32_Return;
         }

         s32_Return = GetBlockChecksum(static_cast<uint8>(i), 1U, u32_CheckSumCalc);
         if (s32_Return != C_NO_ERR)
         {
            //just produce a warning: maybe the configured range is invalid ...
            TRG_ReportStatus(TGL_LoadStr(STR_WARNING) + TGL_LoadStr(STR_FM_ERR_RD_CALC_CHKSM_BLK) +
                             C_SCLString::IntToStr(static_cast<sintn>(i)), gu8_DL_REPORT_STATUS_TYPE_WARNING);
            u32_CheckSumCalc = 0U;
         }

         //read block check on / off
         s32_Return = GetBlockCompareMode(static_cast<uint8>(i), q_CheckStartup, q_CheckAfterFlashing);
         if (s32_Return != C_NO_ERR)
         {
            //do not show an error message: original server implementation did not have this service !
            //if a block existed it was automatically active to be checked at startup
            q_CheckStartup = true;
            q_CheckAfterFlashing = false;
         }
      }

      orc_ChecksumInformation.c_Areas[i].q_CheckAtStartup     = q_CheckStartup;
      orc_ChecksumInformation.c_Areas[i].q_CheckAfterFlashing = q_CheckAfterFlashing;
      orc_ChecksumInformation.c_Areas[i].u32_ChecksumEEP  = u32_CheckSumEEPROM;
      orc_ChecksumInformation.c_Areas[i].u32_ChecksumCalc = u32_CheckSumCalc;
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read sector based flash checksum data from server

   Assumption: server node is already in wakeup state.
   On errors on individual read requests TRG_ReportStatus will be executed with detailed error
   information. The process will be continued if possible.

   \param[in]     ou16_SectorCount           number of sectors (only used with oq_BlockBasedChecksums == false)
   \param[out]    orc_ChecksumInformation    read information

   \return
   C_NO_ERR     executed / check TRG_ReportStatus texts for warnings   \n
   C_RANGE      invalid function parameter                             \n
   C_COM        no response from server                                \n
   C_NOACT      error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::ReadServerSectorChecksumInformation(const uint16 ou16_SectorCount,
                                                         C_XFLChecksumAreas & orc_ChecksumInformation)
{
   sint32 s32_Return;
   uint16 u16_CRC;
   uint16 u16_CRCEE;

   orc_ChecksumInformation.c_Areas.SetLength(0);
   orc_ChecksumInformation.q_IsBlockBased = false;

   //sector based checksums
   uint8 u8_ModeCompare;
   if (ou16_SectorCount == 0U)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_ERROR) + ": " + TGL_LoadStr(STR_FW_ERR_SEC_COUNT_IVALID),
                       gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_RANGE;
   }

   orc_ChecksumInformation.c_Areas.SetLength(ou16_SectorCount);

   for (uint16 u16_Sector = 0; u16_Sector < ou16_SectorCount; u16_Sector++)
   {
      s32_Return = GetSecCRC(static_cast<uint16>(u16_Sector), u16_CRC, u16_CRCEE);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_ERROR) + ": " + TGL_LoadStr(STR_FW_ERR_READ_CRC),
                          gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }

      s32_Return = GetModeCompare(u16_Sector, u8_ModeCompare);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_ERROR) + ": " +
                          C_XFLActions::XFLProtocolErrorToText(s32_Return, GetLastXFLError()),
                          gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return s32_Return;
      }

      orc_ChecksumInformation.c_BlockConfig.SetLength(orc_ChecksumInformation.c_Areas.GetLength());
      orc_ChecksumInformation.c_BlockConfig[u16_Sector].u32_StartAddress = 0U; //can not be determined for sector based ...
      orc_ChecksumInformation.c_BlockConfig[u16_Sector].u32_EndAddress = 0U;   // ... checksums :-(
      orc_ChecksumInformation.c_BlockConfig[u16_Sector].q_BlockDefinitionValid = true;
      orc_ChecksumInformation.c_Areas[u16_Sector].u32_ChecksumEEP  = u16_CRCEE;
      orc_ChecksumInformation.c_Areas[u16_Sector].u32_ChecksumCalc = u16_CRC;
      orc_ChecksumInformation.c_Areas[u16_Sector].q_CheckAtStartup = (u8_ModeCompare == 1U);
      orc_ChecksumInformation.c_Areas[u16_Sector].q_CheckAfterFlashing = false; //not supported for sector based CRCs
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform node wakeup

   Configurable wakeup procedure.
   Steps:
   - send reset request message if configured
   - send "FLASH" if configured
   - perform wakeup with SNR or local ID, depending on configuration

   After a wakeup with SNR the protocol driver base class will be configured to use the returned local ID
   subsequently.

   \param[in]   orc_Parameters      configuration of wakeup procedure
   \param[out]  opu8_ActualLocalID  if performing wakeup with SNR: actual local ID of the target node

   \return
   C_NO_ERR           no errors                                  \n
   else               wakeup failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLActions::PerformWakeup(const C_XFLWakeupParameters & orc_Parameters, uint8 * const opu8_ActualLocalID)
{
   sint32 s32_Return;
   uint8 u8_ActualLocalID;
   C_XFLCompanyID c_CompID;

   this->CfgSetLocalId(orc_Parameters.u8_LocalID);

   s32_Return = C_XFLActions::CompIDStringToStruct(orc_Parameters.c_CompanyID, c_CompID);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_COMP_ID_LENGTH), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return -1;
   }

   if (orc_Parameters.q_SendFLASHRequired == true)
   {
      //send reset request if configured:
      s32_Return = RequestNodeReset((orc_Parameters.q_SendResetRQ == true) ? &orc_Parameters.t_ResetMsg : NULL);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_WR_RESET_RQ), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }

      s32_Return = SendFLASH(orc_Parameters.u32_StartTimeMs, orc_Parameters.u8_FLASHIntervalMs);
      if (s32_Return == C_DEFAULT)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_USER_ABORT), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }

   switch (orc_Parameters.e_WakeupMode)
   {
   case eXFL_WAKEUP_MODE_LID:
      s32_Return = m_WakeupLocalID(c_CompID);
      break;
   case eXFL_WAKEUP_MODE_SNR:
      s32_Return = m_WakeupSNR(c_CompID, orc_Parameters.au8_SNR, u8_ActualLocalID);
      if (s32_Return == C_NO_ERR)
      {
         CfgSetLocalId(u8_ActualLocalID);
         if (opu8_ActualLocalID != NULL)
         {
            (*opu8_ActualLocalID) = u8_ActualLocalID;
         }
      }
      break;
   case eXFL_WAKEUP_MODE_LID_THEN_SNR:
      s32_Return = m_WakeupLocalIDAndSNR(c_CompID, orc_Parameters.au8_SNR);
      break;
   case eXFL_WAKEUP_MODE_NO_WAKEUP:
      //already in wakeup -> nothing to do
      s32_Return = C_NO_ERR;
      break;
   default:
      s32_Return = C_CONFIG;
      break;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
