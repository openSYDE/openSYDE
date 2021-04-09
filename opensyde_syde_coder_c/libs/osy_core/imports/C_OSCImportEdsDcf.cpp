//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of file type .eds and .dcf (implementation)

   Utility class to handle imports of file type .eds and .dcf

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include "TGLFile.h"
#include "stwerrors.h"
#include "CSCLIniFile.h"
#include "C_OSCImportEdsDcf.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Import EDS or DCF file

   Some assumptions:
   * Data types are not parsed but calculated automatically instead
   * Cyclic transmission types are only partially supported because the cycle time can not be imported
   * Signal parsing errors are not reported by return value (only by string)

   \param[in]  orc_FilePath         File to import, file type is based on file ending
   \param[in]  ou8_NodeId           Node ID
   \param[out] orc_OSCRxMessageData Imported core Rx message data
   \param[out] orc_OSCRxSignalData  Imported core Rx signal data
   \param[out] orc_OSCTxMessageData Imported core Tx message data
   \param[out] orc_OSCTxSignalData  Imported core Tx signal data
   \param[out] orc_ImportMessagesPerMessage   Import result messages
   \param[out] orc_ParsingError     Optional parsing error message

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_NOACT  Node Id not allowed
   C_CONFIG Parsing error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::h_Import(const C_SCLString & orc_FilePath, const uint8 ou8_NodeId,
                                   std::vector<C_OSCCanMessage> & orc_OSCRxMessageData,
                                   std::vector<C_OSCNodeDataPoolListElement> & orc_OSCRxSignalData,
                                   std::vector<C_OSCCanMessage> & orc_OSCTxMessageData,
                                   std::vector<C_OSCNodeDataPoolListElement> & orc_OSCTxSignalData,
                                   std::vector<std::vector<C_SCLString> > & orc_ImportMessagesPerMessage,
                                   C_SCLString & orc_ParsingError)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear data
   orc_OSCRxMessageData.clear();
   orc_OSCRxSignalData.clear();
   orc_OSCTxMessageData.clear();
   orc_OSCTxSignalData.clear();
   orc_ImportMessagesPerMessage.clear();
   orc_ParsingError = "";

   if (TGL_FileExists(orc_FilePath) == true)
   {
      bool q_Eds = true;
      const C_SCLString c_Extension = TGL_ExtractFileExtension(orc_FilePath).LowerCase();
      if (c_Extension == ".eds")
      {
         q_Eds = true;
         if ((ou8_NodeId == 0U) || (ou8_NodeId > 127U))
         {
            s32_Retval = C_NOACT;
         }
      }
      else if (c_Extension == ".dcf")
      {
         q_Eds = false;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      if (s32_Retval == C_NO_ERR)
      {
         C_OSCCanOpenObjectDictionary c_Dictionary;
         if (c_Dictionary.LoadFromFile(orc_FilePath) == C_NO_ERR)
         {
            std::vector<uint32> c_Dummies;
            mh_LoadDummies(orc_FilePath, c_Dummies);
            s32_Retval = mh_ParseMessages(0x1400, ou8_NodeId, c_Dictionary.c_Objects, c_Dummies, orc_OSCRxMessageData,
                                          orc_OSCRxSignalData,
                                          q_Eds, orc_ImportMessagesPerMessage, false);
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = mh_ParseMessages(0x1800, ou8_NodeId, c_Dictionary.c_Objects, c_Dummies,
                                             orc_OSCTxMessageData,
                                             orc_OSCTxSignalData,
                                             q_Eds, orc_ImportMessagesPerMessage, true);
            }
         }
         else
         {
            s32_Retval = C_RANGE;
            orc_ParsingError = c_Dictionary.GetLastErrorText();
            osc_write_log_warning("Import ECS/DCF", "Could not parse file: " + c_Dictionary.GetLastErrorText());
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCImportEdsDcf::C_OSCImportEdsDcf(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CO object by object ID

   \param[in] orc_Objects All CO objects
   \param[in] ou32_Id     Object ID

   \return
   NULL Object not found
   Else Valid object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCCanOpenObject * C_OSCImportEdsDcf::mh_GetCOObject(
   const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects, const uint32 ou32_Id, const sint32 os32_SubIndex)
{
   const C_OSCCanOpenObject * pc_Retval = NULL;

   for (sint32 s32_ItObject = 0; s32_ItObject < orc_COObjects.GetLength(); ++s32_ItObject)
   {
      const C_OSCCanOpenObject & rc_CurObject = orc_COObjects[s32_ItObject];
      if ((rc_CurObject.u16_Index == ou32_Id) &&
          ((os32_SubIndex < 0) || (rc_CurObject.u8_SubIndex == static_cast<uint8>(os32_SubIndex))))
      {
         pc_Retval = &rc_CurObject;
         break;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load core messages data

   \param[in]  ou32_StartingId      Starting ID for message section
   \param[in]  ou8_NodeId           Node ID
   \param[in]  orc_COObjects        All CO objects of the current file
   \param[out] orc_OSCRxMessageData Imported core message data
   \param[out] orc_OSCRxSignalData  Imported core signal data
   \param[in]  oq_IsEds             Flag if current file is an EDS file
   \param[out] orc_ImportMessages   Import result messages
   \param[in]  oq_IsTx              Flag if message is a Tx message

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_ParseMessages(const uint32 ou32_StartingId, const uint8 ou8_NodeId,
                                           const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                           const std::vector<uint32> & orc_Dummies,
                                           std::vector<C_OSCCanMessage> & orc_OSCMessageData,
                                           std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                           const bool oq_IsEds,
                                           std::vector<std::vector<C_SCLString> > & orc_ImportMessages,
                                           const bool oq_IsTx)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_Found = true;

   for (uint32 u32_ItMessage = 0; ((q_Found == true) && (u32_ItMessage < 0x200UL)) && (s32_Retval == C_NO_ERR);
        ++u32_ItMessage)
   {
      //Main section
      const C_OSCCanOpenObject * const pc_COMessageMainObject =
         mh_GetCOObject(orc_COObjects, ou32_StartingId + u32_ItMessage, -1);
      if (pc_COMessageMainObject != NULL)
      {
         //COB-ID section
         //--------------
         const C_OSCCanOpenObject * const pc_COMessageCOBIDObject =
            mh_GetCOObject(orc_COObjects, ou32_StartingId + u32_ItMessage, 1);
         if (pc_COMessageCOBIDObject != NULL)
         {
            uint32 u32_CobId;
            if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageCOBIDObject, oq_IsEds), ou8_NodeId,
                                   u32_CobId) == C_NO_ERR)
            {
               //Check if message valid
               if ((u32_CobId & 0x80000000UL) == 0UL)
               {
                  std::vector<C_SCLString> c_CurMessages;
                  const C_OSCCanOpenObject * const pc_COMessageTTObject =
                     mh_GetCOObject(orc_COObjects, ou32_StartingId + u32_ItMessage, 2);
                  C_OSCCanMessage c_Message;
                  //Name
                  c_Message.c_Name = pc_COMessageMainObject->c_Name.c_str();
                  //Extended ID
                  if ((u32_CobId & 0x20000000UL) > 0UL)
                  {
                     c_Message.q_IsExtended = true;
                  }
                  else
                  {
                     c_Message.q_IsExtended = false;
                  }
                  //CAN ID
                  c_Message.u32_CanId = u32_CobId & 0x1FFFFFFFUL;
                  //Transmission type section
                  //-------------------------
                  if (pc_COMessageTTObject != NULL)
                  {
                     uint32 u32_TransmissionType;
                     if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageTTObject, oq_IsEds), ou8_NodeId,
                                            u32_TransmissionType) == C_NO_ERR)
                     {
                        if ((u32_TransmissionType >= 1UL) && (u32_TransmissionType <= 0xF0UL))
                        {
                           c_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
                           mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "Transmission type",
                                             "the message type \"synchronous\" was converted to \"cyclic\".\n"
                                             "Cycle time set default to " +
                                             C_SCLString::IntToStr(c_Message.u32_CycleTimeMs) + "ms.",
                                             2L, false, &c_CurMessages);
                        }
                        else if ((u32_TransmissionType == 0xFCUL) || (u32_TransmissionType == 0xFDUL))
                        {
                           mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "Transmission type",
                                             "not supported", 2L, true);
                           s32_Retval = C_CONFIG;
                        }
                        else
                        {
                           c_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
                        }
                     }
                     else
                     {
                        mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "Transmission type",
                                          "empty or not a number, default set to: \"on event\"", 2L, false,
                                          &c_CurMessages);
                        c_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
                     }
                  }
                  else
                  {
                     mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "Transmission type",
                                       "does not exist, default set to: \"on event\"", 2L, false, &c_CurMessages);
                     c_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
                  }
                  //Optional
                  //Event-timer section
                  //-------------------
                  if (oq_IsTx == false)
                  {
                     bool q_UseDefault = false;
                     C_SCLString c_Reason;

                     // Event-timer only relevant for Rx because it equals the timeout time
                     const C_OSCCanOpenObject * const pc_COMessageETObject =
                        mh_GetCOObject(orc_COObjects, ou32_StartingId + u32_ItMessage, 5);
                     if (pc_COMessageETObject != NULL)
                     {
                        uint32 u32_EventTimer;
                        if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageETObject, oq_IsEds),
                                               ou8_NodeId,
                                               u32_EventTimer) == C_NO_ERR)
                        {
                           if ((c_Message.e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_EVENT) ||
                               (u32_EventTimer > 0U))
                           {
                              c_Message.u32_TimeoutMs = u32_EventTimer;
                           }
                           else
                           {
                              // In case of cyclic or change, 0ms is not supported.
                              // In case of on event, timeout is disabled
                              q_UseDefault = true;
                              c_Reason = "was 0ms";
                           }
                        }
                        else
                        {
                           q_UseDefault = true;
                           c_Reason = "empty or not a number";
                        }
                     }
                     else
                     {
                        q_UseDefault = true;
                        c_Reason = "does not exist";
                     }

                     if (q_UseDefault == true)
                     {
                        // In case of cyclic or change, set to default.
                        // In case of on event, timeout is disabled
                        if (c_Message.e_TxMethod != C_OSCCanMessage::eTX_METHOD_ON_EVENT)
                        {
                           mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "Event-timer",
                                             c_Reason + ", default set to: " +
                                             C_SCLString::IntToStr(
                                                c_Message.u32_TimeoutMs) + "ms", 5L, false, &c_CurMessages);
                        }
                        else
                        {
                           mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "Event-timer",
                                             c_Reason + ", default set to: disabled (0)", 5L, false, &c_CurMessages);
                           c_Message.u32_TimeoutMs = 0U;
                        }
                     }
                  }

                  if (s32_Retval == C_NO_ERR)
                  {
                     //Signals
                     s32_Retval = mh_ParseSignals(ou32_StartingId + u32_ItMessage, ou8_NodeId,
                                                  orc_COObjects, orc_Dummies, c_Message, orc_OSCSignalData,
                                                  oq_IsEds);
                     if (s32_Retval != C_NO_ERR)
                     {
                        //Ignore signal parsing errors, at least message was valid
                        s32_Retval = C_NO_ERR;
                        mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "",
                                          "Attention: Signals are missing.",
                                          -1L, false, &c_CurMessages);
                     }
                     //Should be in sync
                     orc_OSCMessageData.push_back(c_Message);
                     orc_ImportMessages.push_back(c_CurMessages);
                  }
               }
               else
               {
                  std::stringstream c_Stream;
                  const uint32 u32_Id = u32_CobId & 0x1FFFFFFFUL;
                  c_Stream << "Skipped message as marked as inactive (highest bit set in COB_ID) ";
                  c_Stream << &std::hex << u32_Id;
                  mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                                    c_Stream.str().c_str(), 1L, true);
               }
            }
            else
            {
               mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                                 "empty or not a number", 1L, true);
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                              "does not exist", 1L, true);
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         q_Found = false;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load core signal data

   \param[in]  ou32_COMessageId     ID for CO message section
   \param[in]  ou8_NodeId           Node ID
   \param[in]  orc_COObjects        All CO objects of the current file
   \param[out] orc_OSCRxMessageData Imported core message data
   \param[out] orc_OSCRxSignalData  Imported core signal data
   \param[in]  oq_IsEds             Flag if current file is an EDS file

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_ParseSignals(const uint32 ou32_COMessageId, const uint8 ou8_NodeId,
                                          const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                          const std::vector<uint32> & orc_Dummies, C_OSCCanMessage & orc_OSCMessageData,
                                          std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                          const bool oq_IsEds)
{
   sint32 s32_Retval = C_NO_ERR;
   //PDO mapping parameter
   //---------------------
   const C_OSCCanOpenObject * const pc_COMessageMappingObject =
      mh_GetCOObject(orc_COObjects, ou32_COMessageId + 0x200, -1);

   if ((pc_COMessageMappingObject != NULL) && (pc_COMessageMappingObject->u8_NumSubs > 0U))
   {
      //Skip first section because this is just the number of sub segments
      const uint32 u32_MappingCount = static_cast<uint32>(pc_COMessageMappingObject->u8_NumSubs) - 1UL;
      if (u32_MappingCount <= 0x40U)
      {
         uint32 u32_StartBitCounter = 0U;
         for (uint32 u32_ItSignal = 0U; u32_ItSignal < u32_MappingCount; ++u32_ItSignal)
         {
            //Signal pointer section
            //----------------------
            //Skip first section because this is just the number of sub segments
            const C_OSCCanOpenObject * const pc_COMessageMappingSubObject =
               mh_GetCOObject(orc_COObjects, ou32_COMessageId + 0x200UL, static_cast<sint32>(u32_ItSignal + 1UL));

            if (pc_COMessageMappingSubObject != NULL)
            {
               uint32 u32_MappingSubIndexValue;
               if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageMappingSubObject, oq_IsEds), ou8_NodeId,
                                      u32_MappingSubIndexValue) == C_NO_ERR)
               {
                  bool q_Dummy = false;
                  const uint32 u32_CORefId = (u32_MappingSubIndexValue & 0xFFFF0000UL) >> 16UL;
                  //Search if this signal is a valid dummy signal
                  for (uint32 u32_ItDummy = 0; u32_ItDummy < orc_Dummies.size(); ++u32_ItDummy)
                  {
                     if (u32_CORefId == orc_Dummies[u32_ItDummy])
                     {
                        q_Dummy = true;
                        //Move start bit to expected position
                        u32_StartBitCounter += (u32_MappingSubIndexValue & 0xFFU);
                        break;
                     }
                  }
                  //If not dummy signal check referenced CO object
                  if (q_Dummy == false)
                  {
                     const uint32 u32_CORefIdSub = (u32_MappingSubIndexValue & 0xFF00UL) >> 8UL;
                     const C_OSCCanOpenObject * const pc_COSignalObject =
                        mh_GetCOObject(orc_COObjects, u32_CORefId, u32_CORefIdSub);
                     C_OSCCanSignal c_CurSignal;
                     C_OSCNodeDataPoolListElement c_CurDataPoolSignal;
                     //Always intel
                     c_CurSignal.e_ComByteOrder = C_OSCCanSignal::eBYTE_ORDER_INTEL;
                     //Get length
                     c_CurSignal.u16_ComBitLength = static_cast<uint16>(u32_MappingSubIndexValue & 0xFFUL);
                     //Handle start bit
                     c_CurSignal.u16_ComBitStart = static_cast<uint16>(u32_StartBitCounter);
                     u32_StartBitCounter += c_CurSignal.u16_ComBitLength;
                     if (pc_COSignalObject != NULL)
                     {
                        //Name
                        if (pc_COSignalObject->c_Denotation != "")
                        {
                           c_CurDataPoolSignal.c_Name = pc_COSignalObject->c_Denotation;
                        }
                        else
                        {
                           c_CurDataPoolSignal.c_Name = pc_COSignalObject->c_Name;
                        }
                        //Signal should never be an array
                        c_CurDataPoolSignal.SetArray(false);
                        //Data type (automatic calculation based on set type)
                        switch (pc_COSignalObject->u8_DataType)
                        {
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER8:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER16:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER32:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER64:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER24:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER40:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER48:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER56:
                           if (c_CurSignal.u16_ComBitLength <= 8U)
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT8);
                           }
                           else if ((c_CurSignal.u16_ComBitLength > 8U) && (c_CurSignal.u16_ComBitLength <= 16U))
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT16);
                           }
                           else if ((c_CurSignal.u16_ComBitLength > 16U) && (c_CurSignal.u16_ComBitLength <= 32U))
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT32);
                           }
                           else
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT64);
                           }
                           break;
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_BOOLEAN:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED8:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED16:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED32:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED64:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED24:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED40:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED48:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED56:
                           if (c_CurSignal.u16_ComBitLength <= 8U)
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT8);
                           }
                           else if ((c_CurSignal.u16_ComBitLength > 8U) && (c_CurSignal.u16_ComBitLength <= 16U))
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT16);
                           }
                           else if ((c_CurSignal.u16_ComBitLength > 16U) && (c_CurSignal.u16_ComBitLength <= 32U))
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT32);
                           }
                           else
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT64);
                           }
                           break;
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_REAL32:
                        case C_OSCCanOpenObject::hu8_DATA_TYPE_REAL64:
                           if (c_CurSignal.u16_ComBitLength <= 32U)
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eFLOAT32);
                           }
                           else
                           {
                              c_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
                           }
                           break;
                        default:
                           mh_AddUserMessage(u32_CORefId, "", "data type not supported",
                                             static_cast<sint32>(u32_CORefIdSub), true);
                           s32_Retval = C_CONFIG;
                           break;
                        }
                        //Min & max
                        if (mh_CalcMinMax(c_CurDataPoolSignal, c_CurSignal.u16_ComBitLength) != C_NO_ERR)
                        {
                           mh_AddUserMessage(u32_CORefId, "",
                                             "signal bit length does not fit the selected data type",
                                             static_cast<sint32>(u32_CORefIdSub), true);
                           s32_Retval = C_CONFIG;
                        }
                        //Access
                        //Use default == current
                     }
                     else
                     {
                        mh_AddUserMessage(u32_CORefId, "", "does not exist",
                                          static_cast<sint32>(u32_CORefIdSub), true);
                        s32_Retval = C_CONFIG;
                     }
                     if (s32_Retval == C_NO_ERR)
                     {
                        //Handle index
                        c_CurSignal.u32_ComDataElementIndex = orc_OSCSignalData.size();
                        //Add
                        orc_OSCMessageData.c_Signals.push_back(c_CurSignal);
                        orc_OSCSignalData.push_back(c_CurDataPoolSignal);
                     }
                  }
               }
               else
               {
                  mh_AddUserMessage(ou32_COMessageId + 0x200, "",
                                    "empty or not a number", static_cast<sint32>(u32_ItSignal + 1UL), true);
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               mh_AddUserMessage(ou32_COMessageId + 0x200, "", "does not exist",
                                 static_cast<sint32>(u32_ItSignal + 1UL), true);
               s32_Retval = C_CONFIG;
            }
         }
      }
      else
      {
         mh_AddUserMessage(ou32_COMessageId + 0x200, "", "has unexpected value", -1L, true);
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_COMessageId + 0x200, "", "does not exist", -1L, true);
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value of CO Object

   \param[in]  orc_COObject CO object
   \param[in]  oq_IsEds     Flag if current file is an EDS file

   \return
   CO object value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCImportEdsDcf::mh_GetCOObjectValue(const C_OSCCanOpenObject & orc_COObject, const bool oq_IsEds)
{
   C_SCLString c_Retval;

   if (oq_IsEds == true)
   {
      c_Retval = orc_COObject.c_DefaultValue;
   }
   else
   {
      c_Retval = orc_COObject.c_ParameterValue;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to integer value

   \param[in]  orc_COValue CO object value
   \param[in]  ou8_NodeId  Node ID
   \param[out] oru32_Value Parsed value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_GetIntegerValue(const C_SCLString & orc_COValue, const uint8 ou8_NodeId,
                                             uint32 & oru32_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   oru32_Value = 0UL;

   if (orc_COValue.Length() > 0)
   {
      C_SCLString c_LowerCaseNoWhiteSpaceNumberM;
      //Remove whitespace and $
      for (uint32 u32_ItChar = 0; u32_ItChar < orc_COValue.Length(); ++u32_ItChar)
      {
         const charn cn_Character = orc_COValue[static_cast<sintn>(u32_ItChar + 1UL)];
         if ((cn_Character == ' ') || (cn_Character == '$'))
         {
            //Skip
         }
         else
         {
            c_LowerCaseNoWhiteSpaceNumberM += orc_COValue[static_cast<sintn>(u32_ItChar + 1UL)];
         }
      }
      //Lower case
      c_LowerCaseNoWhiteSpaceNumberM = c_LowerCaseNoWhiteSpaceNumberM.LowerCase();
      if (c_LowerCaseNoWhiteSpaceNumberM.Length() > 0)
      {
         SCLDynamicArray<C_SCLString> c_Tokens;
         c_LowerCaseNoWhiteSpaceNumberM.Tokenize("+", c_Tokens);
         if (c_Tokens.GetLength() > 0L)
         {
            for (uint32 u32_ItToken = 0;
                 (u32_ItToken < static_cast<uint32>(c_Tokens.GetLength())) && (s32_Retval == C_NO_ERR);
                 ++u32_ItToken)
            {
               const C_SCLString & rc_CurToken = c_Tokens[u32_ItToken];
               if (rc_CurToken.Pos("nodeid") == 0)
               {
                  uint32 u32_CurNumber;
                  //Number
                  if (mh_GetIntegerValueSimple(rc_CurToken, u32_CurNumber) == C_NO_ERR)
                  {
                     oru32_Value += u32_CurNumber;
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  //Node ID
                  oru32_Value += static_cast<uint32>(ou8_NodeId);
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to integer value

   \param[in]  orc_COValue CO object value
   \param[out] oru32_Value Parsed value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_GetIntegerValueSimple(const C_SCLString & orc_COValue, uint32 & oru32_Value)
{
   sint32 s32_Retval;

   oru32_Value = 0U;

   if (orc_COValue.Length() > 0)
   {
      charn * pcn_Ptr = NULL;

      oru32_Value = strtoul(orc_COValue.c_str(), &pcn_Ptr, 0);
      if (pcn_Ptr != NULL)
      {
         if (*pcn_Ptr == '\0')
         {
            s32_Retval = C_NO_ERR;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add user notification  for import section

   \param[in]     ou32_COObjectId     CO object ID
   \param[in]     orc_COSectionName   CO object sub section name
   \param[in]     orc_ErrorMessage    Error message
   \param[in]     os32_COSubSectionId Optional CO object sub section ID
   \param[in]     oq_IsError          Optional CO object sub section ID
   \param[in,out] opc_ImportMessages  Import result messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_AddUserMessage(const uint32 ou32_COObjectId, const C_SCLString & orc_COSectionName,
                                          const C_SCLString & orc_ErrorMessage, const sint32 os32_COSubSectionId,
                                          const bool oq_IsError, std::vector<C_SCLString> * const opc_ImportMessages)
{
   C_SCLString c_Message;

   std::stringstream c_Stream;

   //Object ID in file
   c_Message += "Object ";
   //Use hex notation because that's probably used in the file
   c_Stream << &std::hex << ou32_COObjectId;
   c_Message += "0x";
   c_Message += c_Stream.str().c_str();
   if (os32_COSubSectionId >= 0L)
   {
      c_Message += "sub";
      c_Message += C_SCLString::IntToStr(os32_COSubSectionId);
   }
   //Optional section name by spec
   if (orc_COSectionName != "")
   {
      c_Message += " (\"";
      c_Message += orc_COSectionName;
      c_Message += "\")";
   }
   //Our error message
   c_Message += " ";
   c_Message += orc_ErrorMessage;

   //If error report to file otherwise store as warning
   if (oq_IsError == true)
   {
      osc_write_log_warning("Import EDS/DCF", c_Message.c_str());
   }
   else
   {
      if (opc_ImportMessages != NULL)
      {
         opc_ImportMessages->push_back(c_Message);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate min max based on current type and number of bits

   \param[in,out] orc_Element     Data pool element
   \param[in]     ou16_NumberBits Number of bits

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_CONFIG Operation failure: Element invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_CalcMinMax(C_OSCNodeDataPoolListElement & orc_Element, const uint16 ou16_NumberBits)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;

   //Check type is realistic for number of bits
   switch (orc_Element.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      if (ou16_NumberBits > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      if (ou16_NumberBits > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      if (ou16_NumberBits > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      if (ou16_NumberBits > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      if (ou16_NumberBits > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      if (ou16_NumberBits > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      if (ou16_NumberBits > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      if (ou16_NumberBits > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      if (ou16_NumberBits > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      if (ou16_NumberBits > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   default:
      break;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_Element.GetArray() == false)
      {
         //Unsigned
         if (q_IsUnsigned == true)
         {
            uint64 u64_Max = 0U;

            for (uint16 u16_ItBit = 0U; u16_ItBit < ou16_NumberBits; ++u16_ItBit)
            {
               u64_Max += (static_cast<uint64>(1U) << u16_ItBit);
            }

            switch (orc_Element.GetType()) //lint !e788 not all enum constants used; this is for unsigned only
            {
            case C_OSCNodeDataPoolContent::eUINT8:
               orc_Element.c_MinValue.SetValueU8(0);
               orc_Element.c_MaxValue.SetValueU8(static_cast<uint8>(u64_Max));
               break;
            case C_OSCNodeDataPoolContent::eUINT16:
               orc_Element.c_MinValue.SetValueU16(0);
               orc_Element.c_MaxValue.SetValueU16(static_cast<uint16>(u64_Max));
               break;
            case C_OSCNodeDataPoolContent::eUINT32:
               orc_Element.c_MinValue.SetValueU32(0);
               orc_Element.c_MaxValue.SetValueU32(static_cast<uint32>(u64_Max));
               break;
            case C_OSCNodeDataPoolContent::eUINT64:
               orc_Element.c_MinValue.SetValueU64(0ULL);
               orc_Element.c_MaxValue.SetValueU64(u64_Max);
               break;
            default:
               s32_Retval = C_CONFIG;
               break;
            }
         }
         //Signed
         if (q_IsSigned == true)
         {
            uint64 u64_Max = 0;

            //We need exactly one more than half of the unsigned maximum
            if (ou16_NumberBits > 0U)
            {
               u64_Max += (static_cast<uint64>(1U) << (ou16_NumberBits - 1U));
            }

            switch (orc_Element.GetType()) //lint !e788 not all enum constants used; this is for signed only
            {
            case C_OSCNodeDataPoolContent::eSINT8:
               orc_Element.c_MinValue.SetValueS8(-static_cast<sint8>(u64_Max));
               orc_Element.c_MaxValue.SetValueS8(static_cast<sint8>(u64_Max - 1U));
               break;
            case C_OSCNodeDataPoolContent::eSINT16:
               orc_Element.c_MinValue.SetValueS16(-static_cast<sint16>(u64_Max));
               orc_Element.c_MaxValue.SetValueS16(static_cast<sint16>(u64_Max - 1U));
               break;
            case C_OSCNodeDataPoolContent::eSINT32:
               orc_Element.c_MinValue.SetValueS32(-static_cast<sint32>(u64_Max));
               orc_Element.c_MaxValue.SetValueS32(static_cast<sint32>(u64_Max - 1UL));
               break;
            case C_OSCNodeDataPoolContent::eSINT64:
               orc_Element.c_MinValue.SetValueS64(-static_cast<sint64>(u64_Max));
               orc_Element.c_MaxValue.SetValueS64(static_cast<sint64>(u64_Max - 1ULL));
               break;
            default:
               s32_Retval = C_CONFIG;
               break;
            }
         }
         //Float
         if (q_IsFloat == true)
         {
            switch (orc_Element.GetType()) //lint !e788 not all enum constants used; this is for float only
            {
            case C_OSCNodeDataPoolContent::eFLOAT32:
               orc_Element.c_MinValue.SetValueF32(-std::numeric_limits<float32>::max());
               orc_Element.c_MaxValue.SetValueF32(std::numeric_limits<float32>::max());
               break;
            case C_OSCNodeDataPoolContent::eFLOAT64:
               orc_Element.c_MinValue.SetValueF64(-std::numeric_limits<float64>::max());
               orc_Element.c_MaxValue.SetValueF64(std::numeric_limits<float64>::max());
               break;
            default:
               s32_Retval = C_CONFIG;
               break;
            }
         }
      }

      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dummy data types

   \param[in]  orc_FilePath Ini file path
   \param[out] orc_Dummies  Found and valid dummy data types
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_LoadDummies(const C_SCLString & orc_FilePath, std::vector<uint32> & orc_Dummies)
{
   try
   {
      C_SCLStringList c_StringList;
      C_SCLIniFile c_Ini(orc_FilePath);
      c_Ini.ReadSection("DummyUsage", &c_StringList);
      if (c_StringList.GetCount() > 0UL)
      {
         //1B is last supported data type
         for (uint32 u32_ItPossibleDummy = 1U; u32_ItPossibleDummy <= 0x1B; ++u32_ItPossibleDummy)
         {
            std::stringstream c_Stream;
            C_SCLString c_CurString;
            c_Stream << std::setw(4) << std::setfill('0') << &std::hex << u32_ItPossibleDummy;
            c_CurString = static_cast<C_SCLString>("Dummy") + c_Stream.str().c_str();
            //Check if entry exists
            if (c_StringList.IndexOf(c_CurString) >= 0L)
            {
               //Check if entry specifies dummy can be used
               if (c_Ini.ReadBool("DummyUsage", c_CurString, false) == true)
               {
                  orc_Dummies.push_back(u32_ItPossibleDummy);
               }
            }
         }
      }
   }
   catch (...)
   {
      //No errors are relevant, because section is optional
   }
}
