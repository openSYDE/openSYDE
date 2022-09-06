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

#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>
#include <cstdlib>
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

   \param[in]   orc_FilePath                                File to import, file type is based on file ending
   \param[in]   ou8_NodeId                                  Node ID
   \param[out]  orc_OSCRxMessageData                        Imported core Rx message data
   \param[out]  orc_OSCRxSignalData                         Imported core Rx signal data
   \param[out]  orc_RxSignalDefaultMinMaxValuesUsed         Flag if imported core Rx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_OSCTxMessageData                        Imported core Tx message data
   \param[out]  orc_OSCTxSignalData                         Imported core Tx signal data
   \param[out]  orc_TxSignalDefaultMinMaxValuesUsed         Flag if imported core Rx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_ImportMessagesPerMessage                Import result messages
   \param[out]  orc_ParsingError                            Optional parsing error message
   \param[in]   oq_RestrictForCanOpenUsage                  Flag to restrict for CANopen usage
   \param[out]  orc_InvalidOSCRxMessageData                 Invalid core Rx message data
   \param[out]  orc_InvalidOSCRxSignalData                  Invalid core Rx signal data
   \param[out]  orc_InvalidRxSignalDefaultMinMaxValuesUsed  Flag if invalid core Rx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_InvalidOSCTxMessageData                 Invalid core Tx message data
   \param[out]  orc_InvalidOSCTxSignalData                  Invalid core Tx signal data
   \param[out]  orc_InvalidTxSignalDefaultMinMaxValuesUsed  Flag if invalid core Tx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_InvalidImportMessagesPerMessage         Import result messages per invalid message

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
                                   std::vector<stw_types::uint8> & orc_RxSignalDefaultMinMaxValuesUsed,
                                   std::vector<C_OSCCanMessage> & orc_OSCTxMessageData,
                                   std::vector<C_OSCNodeDataPoolListElement> & orc_OSCTxSignalData,
                                   std::vector<stw_types::uint8> & orc_TxSignalDefaultMinMaxValuesUsed,
                                   std::vector<std::vector<C_SCLString> > & orc_ImportMessagesPerMessage,
                                   C_SCLString & orc_ParsingError, const bool oq_RestrictForCanOpenUsage,
                                   std::vector<C_OSCCanMessage> & orc_InvalidOSCRxMessageData,
                                   std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCRxSignalData,
                                   std::vector<uint8> & orc_InvalidRxSignalDefaultMinMaxValuesUsed,
                                   std::vector<C_OSCCanMessage> & orc_InvalidOSCTxMessageData,
                                   std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCTxSignalData,
                                   std::vector<uint8> & orc_InvalidTxSignalDefaultMinMaxValuesUsed,
                                   std::vector<std::vector<C_SCLString> > & orc_InvalidImportMessagesPerMessage)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear data
   orc_OSCRxMessageData.clear();
   orc_OSCRxSignalData.clear();
   orc_RxSignalDefaultMinMaxValuesUsed.clear();
   orc_OSCTxMessageData.clear();
   orc_OSCTxSignalData.clear();
   orc_TxSignalDefaultMinMaxValuesUsed.clear();
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
            uint32 u32_StartId;
            mh_LoadDummies(orc_FilePath, c_Dummies);
            if (oq_RestrictForCanOpenUsage)
            {
               u32_StartId = C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO;
            }
            else
            {
               u32_StartId = C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO;
            }
            s32_Retval = mh_ParseMessages(u32_StartId, ou8_NodeId, c_Dictionary.c_Objects, c_Dummies,
                                          orc_OSCRxMessageData,
                                          orc_OSCRxSignalData,
                                          orc_RxSignalDefaultMinMaxValuesUsed,
                                          q_Eds, orc_ImportMessagesPerMessage, false, oq_RestrictForCanOpenUsage,
                                          orc_InvalidOSCRxMessageData, orc_InvalidOSCRxSignalData,
                                          orc_InvalidRxSignalDefaultMinMaxValuesUsed,
                                          orc_InvalidImportMessagesPerMessage);
            if (s32_Retval == C_NO_ERR)
            {
               if (oq_RestrictForCanOpenUsage)
               {
                  u32_StartId = C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO;
               }
               else
               {
                  u32_StartId = C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO;
               }
               s32_Retval = mh_ParseMessages(u32_StartId, ou8_NodeId, c_Dictionary.c_Objects, c_Dummies,
                                             orc_OSCTxMessageData,
                                             orc_OSCTxSignalData,
                                             orc_TxSignalDefaultMinMaxValuesUsed,
                                             q_Eds, orc_ImportMessagesPerMessage, true, oq_RestrictForCanOpenUsage,
                                             orc_InvalidOSCTxMessageData, orc_InvalidOSCTxSignalData,
                                             orc_InvalidTxSignalDefaultMinMaxValuesUsed,
                                             orc_InvalidImportMessagesPerMessage);
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
/*! \brief  Parse signal content

   OSY signal factory for CANopen objects

   \param[in]      orc_COObjects                All CANopen objects of the current file
   \param[in]      ou32_COSignalObjectIndex     CANopen signal object index
   \param[in]      ou32_COSignalObjectSubIndex  CANopen signal object sub index
   \param[in]      ou32_StartBitCounter         Start bit counter
   \param[in]      oq_RestrictForCanOpenUsage   Restrict for CANopen usage
   \param[in,out]  orc_CurSignal                Current signal data
   \param[in,out]  orc_CurDataPoolSignal        Current data pool signal data
   \param[out]     orq_DefaultMinMax            Flag if default values min max values are set or object specific
                                                limits are set

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::h_ParseSignalContent(const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                               const uint32 ou32_COSignalObjectIndex,
                                               const uint32 ou32_COSignalObjectSubIndex,
                                               const uint32 ou32_StartBitCounter, const bool oq_RestrictForCanOpenUsage,
                                               C_OSCCanSignal & orc_CurSignal,
                                               C_OSCNodeDataPoolListElement & orc_CurDataPoolSignal,
                                               bool & orq_DefaultMinMax)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanOpenObject * pc_COSignalObject;

   if (ou32_COSignalObjectSubIndex == 0)
   {
      //Access object directly
      pc_COSignalObject =
         mh_GetCOObject(orc_COObjects, ou32_COSignalObjectIndex, -1);
   }
   else
   {
      //Access sub index
      pc_COSignalObject =
         mh_GetCOObject(orc_COObjects, ou32_COSignalObjectIndex, ou32_COSignalObjectSubIndex);
   }

   //CANopen attributes
   if (oq_RestrictForCanOpenUsage)
   {
      orc_CurSignal.u16_CanOpenManagerObjectDictionaryIndex = static_cast<uint16>(ou32_COSignalObjectIndex);
      orc_CurSignal.u8_CanOpenManagerObjectDictionarySubIndex = static_cast<uint8>(ou32_COSignalObjectSubIndex);
   }
   //Always intel
   orc_CurSignal.e_ComByteOrder = C_OSCCanSignal::eBYTE_ORDER_INTEL;
   //Handle start bit
   orc_CurSignal.u16_ComBitStart = static_cast<uint16>(ou32_StartBitCounter);
   if (pc_COSignalObject != NULL)
   {
      //Name
      orc_CurDataPoolSignal.c_Name = C_OSCImportEdsDcf::h_GetObjectName(*pc_COSignalObject);
      //Signal should never be an array
      orc_CurDataPoolSignal.SetArray(false);
      // Prepare dataset for the default value
      orc_CurDataPoolSignal.c_DataSetValues.resize(1);
      //Get length
      switch (pc_COSignalObject->u8_DataType)
      {
      case C_OSCCanOpenObject::hu8_DATA_TYPE_BOOLEAN:
         orc_CurSignal.u16_ComBitLength = 1U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED8:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER8:
         orc_CurSignal.u16_ComBitLength = 8U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER16:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED16:
         orc_CurSignal.u16_ComBitLength = 16U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER24:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED24:
         orc_CurSignal.u16_ComBitLength = 24U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_REAL32:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER32:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED32:
         orc_CurSignal.u16_ComBitLength = 32U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER40:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED40:
         orc_CurSignal.u16_ComBitLength = 40U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER48:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED48:
         orc_CurSignal.u16_ComBitLength = 48U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER56:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED56:
         orc_CurSignal.u16_ComBitLength = 56U;
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_INTEGER64:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_UNSIGNED64:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_REAL64:
         orc_CurSignal.u16_ComBitLength = 64U;
         break;
      default:
         //Reported next
         break;
      }
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
         if (orc_CurSignal.u16_ComBitLength <= 8U)
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT8);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 8U) && (orc_CurSignal.u16_ComBitLength <= 16U))
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT16);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 16U) && (orc_CurSignal.u16_ComBitLength <= 32U))
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT32);
         }
         else
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eSINT64);
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
         if (orc_CurSignal.u16_ComBitLength <= 8U)
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT8);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 8U) && (orc_CurSignal.u16_ComBitLength <= 16U))
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT16);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 16U) && (orc_CurSignal.u16_ComBitLength <= 32U))
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT32);
         }
         else
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eUINT64);
         }
         break;
      case C_OSCCanOpenObject::hu8_DATA_TYPE_REAL32:
      case C_OSCCanOpenObject::hu8_DATA_TYPE_REAL64:
         if (orc_CurSignal.u16_ComBitLength <= 32U)
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eFLOAT32);
         }
         else
         {
            orc_CurDataPoolSignal.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
         }
         break;
      default:
         mh_AddUserMessage(ou32_COSignalObjectIndex, "", "data type not supported",
                           static_cast<sint32>(ou32_COSignalObjectSubIndex), true);
         s32_Retval = C_CONFIG;
         break;
      }

      //Min & max & init value
      if (mh_CalcMinMaxInit(pc_COSignalObject, orc_CurDataPoolSignal, orc_CurSignal.u16_ComBitLength,
                            orq_DefaultMinMax) != C_NO_ERR)
      {
         mh_AddUserMessage(ou32_COSignalObjectIndex, "",
                           "signal bit length does not fit the selected data type",
                           static_cast<sint32>(ou32_COSignalObjectSubIndex), true);
         s32_Retval = C_CONFIG;
      }

      //Access
      //Use default == current
   }
   else
   {
      mh_AddUserMessage(ou32_COSignalObjectIndex, "", "does not exist",
                        static_cast<sint32>(ou32_COSignalObjectSubIndex), true);
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get object name

   \param[in]  orc_COObject   CANopen object

   \return
   Object name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCImportEdsDcf::h_GetObjectName(const C_OSCCanOpenObject & orc_COObject)
{
   C_SCLString c_Retval;

   if (orc_COObject.c_Denotation != "")
   {
      c_Retval = orc_COObject.c_Denotation;
   }
   else
   {
      c_Retval = orc_COObject.c_Name;
   }
   return c_Retval;
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

   \param[in]  orc_COObjects  All CO objects
   \param[in]  ou32_Id        Object ID
   \param[in]  os32_SubIndex  Sub index

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
          ((os32_SubIndex < 0) ||
           ((rc_CurObject.u8_NumSubs == 255) && (rc_CurObject.u8_SubIndex == static_cast<uint8>(os32_SubIndex)))))
      {
         pc_Retval = &rc_CurObject;
         break;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load core messages data

   \param[in]      ou32_StartingId                             Starting ID for message section
   \param[in]      ou8_NodeId                                  Node ID
   \param[in]      orc_COObjects                               All CO objects of the current file
   \param[in]      orc_Dummies                                 Found and valid dummy data types
   \param[in,out]  orc_OSCMessageData                          Imported core message data
   \param[in,out]  orc_OSCSignalData                           Imported core signal data
   \param[in,out]  orc_SignalDefaultMinMaxValuesUsed           Flag if imported core signal data uses the default min max
                                                               values or specific set values
   \param[in]      oq_IsEds                                    Flag if current file is an EDS file
   \param[in,out]  orc_ImportMessages                          Import result messages
   \param[in]      oq_IsTx                                     Flag if message is a Tx message
   \param[in]      oq_RestrictForCanOpenUsage                  Restrict for can open usage
   \param[in,out]  orc_InvalidOSCMessageData                   Invalid core message data
   \param[in,out]  orc_InvalidOSCSignalData                    Invalid core signal data
   \param[in,out]  orc_InvalidSignalDefaultMinMaxValuesUsed    Flag if invalid core signal data uses the default min max
                                                               values or specific set values
   \param[in,out]  orc_InvalidImportMessages                   Import result messages per invalid message

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
                                           std::vector<uint8> & orc_SignalDefaultMinMaxValuesUsed, const bool oq_IsEds,
                                           std::vector<std::vector<C_SCLString> > & orc_ImportMessages,
                                           const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                           std::vector<C_OSCCanMessage> & orc_InvalidOSCMessageData,
                                           std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCSignalData,
                                           std::vector<uint8> & orc_InvalidSignalDefaultMinMaxValuesUsed,
                                           std::vector<std::vector<stw_scl::C_SCLString> > & orc_InvalidImportMessages)
{
   sint32 s32_Retval = C_NO_ERR;

   for (sint32 s32_ItObject = 0; (s32_ItObject < orc_COObjects.GetLength()) && (s32_Retval == C_NO_ERR); ++s32_ItObject)
   {
      //Main section
      const C_OSCCanOpenObject & rc_COMessageMainObject = orc_COObjects[s32_ItObject];
      //Check if relevant object
      if ((rc_COMessageMainObject.u8_NumSubs != 255) &&
          ((rc_COMessageMainObject.u16_Index >= ou32_StartingId) &&
           (rc_COMessageMainObject.u16_Index < (ou32_StartingId + 0x200))))
      {
         const uint32 u32_ItMessage = rc_COMessageMainObject.u16_Index - ou32_StartingId;
         //COB-ID section
         //--------------
         const C_OSCCanOpenObject * const pc_COMessageCOBIDObject =
            mh_GetCOObject(orc_COObjects, ou32_StartingId + u32_ItMessage,
                           C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID);
         if (pc_COMessageCOBIDObject != NULL)
         {
            bool q_CobIdIncludesNodeId;
            uint32 u32_CobId;
            if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageCOBIDObject, oq_IsEds), ou8_NodeId,
                                   u32_CobId, &q_CobIdIncludesNodeId) == C_NO_ERR)
            {
               //Check if message active
               if ((u32_CobId & 0x80000000UL) == 0UL)
               {
                  s32_Retval = C_OSCImportEdsDcf::mh_ParseMessageContent(ou32_StartingId, ou8_NodeId, orc_COObjects,
                                                                         orc_Dummies,
                                                                         orc_OSCMessageData, orc_OSCSignalData,
                                                                         orc_SignalDefaultMinMaxValuesUsed,
                                                                         oq_IsEds,
                                                                         orc_ImportMessages, oq_IsTx,
                                                                         oq_RestrictForCanOpenUsage,
                                                                         rc_COMessageMainObject, u32_ItMessage,
                                                                         u32_CobId, orc_InvalidOSCMessageData,
                                                                         orc_InvalidOSCSignalData,
                                                                         orc_InvalidSignalDefaultMinMaxValuesUsed,
                                                                         orc_InvalidImportMessages,
                                                                         q_CobIdIncludesNodeId);
               }
               else
               {
                  if (oq_RestrictForCanOpenUsage)
                  {
                     s32_Retval = C_OSCImportEdsDcf::mh_ParseMessageContent(ou32_StartingId, ou8_NodeId, orc_COObjects,
                                                                            orc_Dummies,
                                                                            orc_OSCMessageData, orc_OSCSignalData,
                                                                            orc_SignalDefaultMinMaxValuesUsed,
                                                                            oq_IsEds,
                                                                            orc_ImportMessages, oq_IsTx,
                                                                            oq_RestrictForCanOpenUsage,
                                                                            rc_COMessageMainObject, u32_ItMessage,
                                                                            u32_CobId, orc_InvalidOSCMessageData,
                                                                            orc_InvalidOSCSignalData,
                                                                            orc_InvalidSignalDefaultMinMaxValuesUsed,
                                                                            orc_InvalidImportMessages,
                                                                            q_CobIdIncludesNodeId);
                  }
                  else
                  {
                     std::stringstream c_Stream;
                     const uint32 u32_Id = u32_CobId & 0x1FFFFFFFUL;
                     c_Stream << "Skipped message as marked as inactive (highest bit set in COB_ID) ";
                     c_Stream << &std::hex << u32_Id;
                     mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                                       c_Stream.str().c_str(), C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID,
                                       true);
                  }
               }
            }
            else
            {
               mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                                 "empty or not a number",
                                 C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID, true);
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                              "does not exist", C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID,
                              true);
            s32_Retval = C_CONFIG;
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load core messages data

   \param[in]      ou32_StartingId                             Starting ID for message section
   \param[in]      ou8_NodeId                                  Node ID
   \param[in]      orc_COObjects                               All CO objects of the current file
   \param[in]      orc_Dummies                                 Found and valid dummy data types
   \param[in,out]  orc_OSCMessageData                          Imported core message data
   \param[in,out]  orc_OSCSignalData                           Imported core signal data
   \param[in,out]  orc_SignalDefaultMinMaxValuesUsed           Flag if imported core signal data uses the default min max
                                                               values or specific set values
   \param[in]      oq_IsEds                                    Flag if current file is an EDS file
   \param[in,out]  orc_ImportMessages                          Import result messages
   \param[in]      oq_IsTx                                     Flag if message is a Tx message
   \param[in]      oq_RestrictForCanOpenUsage                  Restrict for can open usage
   \param[in]      orc_COMessageMainObject                     CANOpen message main object
   \param[in]      ou32_ItMessage                              Current message index
   \param[in]      ou32_CobId                                  Message COB-ID
   \param[in,out]  orc_InvalidOSCMessageData                   Invalid core message data
   \param[in,out]  orc_InvalidOSCSignalData                    Invalid core signal data
   \param[in,out]  orc_InvalidSignalDefaultMinMaxValuesUsed    Flag if invalid core signal data uses the default min max
                                                               values or specific set values
   \param[in,out]  orc_InvalidImportMessages                   Import result messages per invalid message
   \param[in]      oq_CobIdIncludesNodeId                      Flag if message COB-ID includes node id

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_ParseMessageContent(const uint32 ou32_StartingId, const uint8 ou8_NodeId,
                                                 const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                 const std::vector<uint32> & orc_Dummies,
                                                 std::vector<C_OSCCanMessage> & orc_OSCMessageData,
                                                 std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                                 std::vector<uint8> & orc_SignalDefaultMinMaxValuesUsed,
                                                 const bool oq_IsEds,
                                                 std::vector<std::vector<C_SCLString> > & orc_ImportMessages,
                                                 const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                                 const C_OSCCanOpenObject & orc_COMessageMainObject,
                                                 const uint32 ou32_ItMessage, const uint32 ou32_CobId,
                                                 std::vector<C_OSCCanMessage> & orc_InvalidOSCMessageData,
                                                 std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCSignalData,
                                                 std::vector<stw_types::uint8> & orc_InvalidSignalDefaultMinMaxValuesUsed, std::vector<std::vector<stw_scl::C_SCLString> > & orc_InvalidImportMessages,
                                                 const bool oq_CobIdIncludesNodeId)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_AddToSkippedMessages = false;

   std::vector<C_SCLString> c_CurMessages;
   C_OSCCanMessage c_Message;
   //Name
   c_Message.c_Name = orc_COMessageMainObject.c_Name.c_str();
   //Extended ID
   if ((ou32_CobId & 0x20000000UL) > 0UL)
   {
      c_Message.q_IsExtended = true;
   }
   else
   {
      c_Message.q_IsExtended = false;
   }
   //CAN ID
   c_Message.u32_CanId = ou32_CobId & 0x1FFFFFFFUL;
   //CANopen attributes
   if (oq_RestrictForCanOpenUsage)
   {
      c_Message.q_CanOpenManagerCobIdIncludesNodeID = oq_CobIdIncludesNodeId;
      // u32_CanId has the resulting CAN ID with already added node id
      if (oq_CobIdIncludesNodeId)
      {
         c_Message.u32_CanOpenManagerCobIdOffset = c_Message.u32_CanId - ou8_NodeId;
      }
      else
      {
         c_Message.u32_CanOpenManagerCobIdOffset = c_Message.u32_CanId;
      }
      c_Message.u16_CanOpenManagerPdoIndex = static_cast<uint16>(ou32_ItMessage);
      if ((ou32_CobId & 0x80000000UL) == 0UL)
      {
         c_Message.q_CanOpenManagerMessageActive = true;
      }
      else
      {
         c_Message.q_CanOpenManagerMessageActive = false;
      }

      //Transmission type section
      //-------------------------
      C_OSCImportEdsDcf::mh_LoadMessageTransmissionTypeCanOpen(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                               orc_COObjects, oq_IsEds, q_AddToSkippedMessages,
                                                               c_CurMessages, c_Message);
   }
   else
   {
      s32_Retval = C_OSCImportEdsDcf::mh_LoadMessageTransmissionType(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                                     orc_COObjects, oq_IsEds, c_CurMessages, c_Message);
   }
   //Optional
   //Event-timer section
   //-------------------
   if (oq_RestrictForCanOpenUsage)
   {
      C_OSCImportEdsDcf::mh_LoadEventTimerSectionCanOpen(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                         orc_COObjects, oq_IsEds, oq_IsTx, c_Message);
   }
   else
   {
      C_OSCImportEdsDcf::mh_LoadEventTimerSection(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                  orc_COObjects, oq_IsEds, oq_IsTx, c_CurMessages, c_Message);
   }

   //Inhibit-time section
   //-------------------
   if (oq_RestrictForCanOpenUsage)
   {
      C_OSCImportEdsDcf::mh_LoadInhibitTimeSectionCanOpen(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                          orc_COObjects, oq_IsEds, c_CurMessages, c_Message);
   }

   if (s32_Retval == C_NO_ERR)
   {
      //Signals

      if (q_AddToSkippedMessages)
      {
         s32_Retval = mh_ParseSignals(ou32_StartingId + ou32_ItMessage, ou8_NodeId,
                                      orc_COObjects, orc_Dummies, c_Message, orc_InvalidOSCSignalData,
                                      orc_InvalidSignalDefaultMinMaxValuesUsed,
                                      oq_IsEds, oq_RestrictForCanOpenUsage, c_CurMessages);
      }
      else
      {
         s32_Retval = mh_ParseSignals(ou32_StartingId + ou32_ItMessage, ou8_NodeId,
                                      orc_COObjects, orc_Dummies, c_Message, orc_OSCSignalData,
                                      orc_SignalDefaultMinMaxValuesUsed,
                                      oq_IsEds, oq_RestrictForCanOpenUsage, c_CurMessages);
      }
      if (s32_Retval != C_NO_ERR)
      {
         //Ignore signal parsing errors, at least message was valid
         s32_Retval = C_NO_ERR;
         mh_AddUserMessage(ou32_StartingId + ou32_ItMessage + 0x200UL, "",
                           "Information: PDO has no mapping.",
                           -1L, false, &c_CurMessages);
      }
      if (q_AddToSkippedMessages)
      {
         //Should be in sync
         orc_InvalidOSCMessageData.push_back(c_Message);
         orc_InvalidImportMessages.push_back(c_CurMessages);
      }
      else
      {
         //Should be in sync
         orc_OSCMessageData.push_back(c_Message);
         orc_ImportMessages.push_back(c_CurMessages);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load message transmission type

   \param[in]      ou32_StartingId  Starting ID for message section
   \param[in]      ou32_ItMessage   Current message index
   \param[in]      ou8_NodeId       Node ID
   \param[in]      orc_COObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in,out]  orc_CurMessages  Current import result messages
   \param[in,out]  orc_Message      Message

   \return
   STW error codes

   \retval   C_NO_ERR   Detailed description
   \retval   C_CONFIG   Detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_LoadMessageTransmissionType(const uint32 ou32_StartingId, const uint32 ou32_ItMessage,
                                                         const uint8 ou8_NodeId,
                                                         const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects, const bool oq_IsEds, std::vector<C_SCLString> & orc_CurMessages,
                                                         C_OSCCanMessage & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanOpenObject * const pc_COMessageTTObject =
      mh_GetCOObject(orc_COObjects, ou32_StartingId + ou32_ItMessage,
                     C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE);

   if (pc_COMessageTTObject != NULL)
   {
      uint32 u32_TransmissionType;
      if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageTTObject, oq_IsEds), ou8_NodeId,
                             u32_TransmissionType) == C_NO_ERR)
      {
         if ((u32_TransmissionType >= 1UL) && (u32_TransmissionType <= 0xF0UL))
         {
            orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                              "the message type \"synchronous\" was converted to \"cyclic\".\n"
                              "Cycle time set default to " +
                              C_SCLString::IntToStr(
                                 orc_Message.u32_CycleTimeMs) + "ms.",
                              C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                              &orc_CurMessages);
         }
         else if ((u32_TransmissionType == 0xFCUL) || (u32_TransmissionType == 0xFDUL))
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                              "not supported",
                              C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, true);
            s32_Retval = C_CONFIG;
         }
         else
         {
            orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
         }
      }
      else
      {
         mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                           "empty or not a number, default set to: \"on event\"",
                           C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                           &orc_CurMessages);
         orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                        "does not exist, default set to: \"on event\"", 2L, false, &orc_CurMessages);
      orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load message transmission type in CANopen use-case

   \param[in]      ou32_StartingId           Starting ID for message section
   \param[in]      ou32_ItMessage            Current message index
   \param[in]      ou8_NodeId                Node ID
   \param[in]      orc_COObjects             All CO objects of the current file
   \param[in]      oq_IsEds                  Flag if current file is an EDS file
   \param[out]     orq_AddToSkippedMessages  Add to skipped messages
   \param[in,out]  orc_CurMessages           Current import result messages
   \param[in,out]  orc_Message               Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_LoadMessageTransmissionTypeCanOpen(const uint32 ou32_StartingId, const uint32 ou32_ItMessage,
                                                              const uint8 ou8_NodeId,
                                                              const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects, const bool oq_IsEds, bool & orq_AddToSkippedMessages, std::vector<C_SCLString> & orc_CurMessages,
                                                              C_OSCCanMessage & orc_Message)
{
   const C_OSCCanOpenObject * const pc_COMessageTTObject =
      mh_GetCOObject(orc_COObjects, ou32_StartingId + ou32_ItMessage,
                     C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE);

   if (pc_COMessageTTObject != NULL)
   {
      uint32 u32_TransmissionType;
      if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageTTObject, oq_IsEds), ou8_NodeId,
                             u32_TransmissionType) == C_NO_ERR)
      {
         if (u32_TransmissionType == 254UL)
         {
            orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
         }
         else if (u32_TransmissionType == 255UL)
         {
            orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255;
         }
         else
         {
            if (pc_COMessageTTObject->IsWriteable())
            {
               orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
               mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                                 "the message type \"CANopen type " +
                                 C_SCLString::IntToStr(
                                    u32_TransmissionType) + "\" was converted to \"async, manufacturer specific (254)\".",
                                 C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                                 &orc_CurMessages);
            }
            else
            {
               orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
               mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                                 "the message type \"CANopen type " +
                                 C_SCLString::IntToStr(
                                    u32_TransmissionType) + "\" was found which is not supported"
                                 " and cannot be changed because it is read only.",
                                 C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                                 &orc_CurMessages);
               orq_AddToSkippedMessages = true;
            }
         }
      }
      else
      {
         mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                           "empty or not a number, default set to: \"async, manufacturer specific (254)\"",
                           C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                           &orc_CurMessages);
         orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                        "does not exist, default set to: \"async, manufacturer specific (254)\"",
                        C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                        &orc_CurMessages);
      orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
      orq_AddToSkippedMessages = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load event timer section

   \param[in]      ou32_StartingId  Starting ID for message section
   \param[in]      ou32_ItMessage   Current message index
   \param[in]      ou8_NodeId       Node ID
   \param[in]      orc_COObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in]      oq_IsTx          Flag if message is a Tx message
   \param[in,out]  orc_CurMessages  Current import result messages
   \param[in,out]  orc_Message      Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_LoadEventTimerSection(const uint32 ou32_StartingId, const uint32 ou32_ItMessage,
                                                 const uint8 ou8_NodeId,
                                                 const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                 const bool oq_IsEds, const bool oq_IsTx,
                                                 std::vector<C_SCLString> & orc_CurMessages,
                                                 C_OSCCanMessage & orc_Message)
{
   if (oq_IsTx == false)
   {
      bool q_UseDefault = false;
      C_SCLString c_Reason;

      // Event-timer only relevant for Rx because it equals the timeout time
      const C_OSCCanOpenObject * const pc_COMessageETObject =
         mh_GetCOObject(orc_COObjects, ou32_StartingId + ou32_ItMessage,
                        C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER);
      if (pc_COMessageETObject != NULL)
      {
         uint32 u32_EventTimer;
         if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageETObject, oq_IsEds),
                                ou8_NodeId,
                                u32_EventTimer) == C_NO_ERR)
         {
            if ((orc_Message.e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_EVENT) ||
                (u32_EventTimer > 0U))
            {
               orc_Message.u32_TimeoutMs = u32_EventTimer;
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
         if (orc_Message.e_TxMethod != C_OSCCanMessage::eTX_METHOD_ON_EVENT)
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Event-timer",
                              c_Reason + ", default set to: " +
                              C_SCLString::IntToStr(
                                 orc_Message.u32_TimeoutMs) + "ms", C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER, false,
                              &orc_CurMessages);
         }
         else
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Event-timer",
                              c_Reason + ", default set to: disabled (0)",
                              C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER, false,
                              &orc_CurMessages);
            orc_Message.u32_TimeoutMs = 0U;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load event timer section in CANopen use-case

   \param[in]      ou32_StartingId  Starting ID for message section
   \param[in]      ou32_ItMessage   Current message index
   \param[in]      ou8_NodeId       Node ID
   \param[in]      orc_COObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in]      oq_IsTx          Flag if message is a Tx message
   \param[in,out]  orc_Message      Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_LoadEventTimerSectionCanOpen(const uint32 ou32_StartingId, const uint32 ou32_ItMessage,
                                                        const uint8 ou8_NodeId,
                                                        const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects, const bool oq_IsEds, const bool oq_IsTx,
                                                        C_OSCCanMessage & orc_Message)
{
   const C_OSCCanOpenObject * const pc_COMessageETObject =
      mh_GetCOObject(orc_COObjects, ou32_StartingId + ou32_ItMessage,
                     C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER);

   if (pc_COMessageETObject != NULL)
   {
      uint32 u32_EventTimer;
      if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageETObject, oq_IsEds),
                             ou8_NodeId,
                             u32_EventTimer) == C_NO_ERR)
      {
         //RPDO device info
         orc_Message.u32_CycleTimeMs = u32_EventTimer;
         //TPDO device info
         if ((u32_EventTimer == 0UL) ||
             (oq_IsTx == true))
         {
            // Special case: The device is the receiver and defines the timeout time with the value of the event time
            // In this case, the timeout time must be set to the original device value
            // Definition: In this scenario the cycle time and timeout will be set to the same value
            orc_Message.u32_TimeoutMs = u32_EventTimer;
         }
         else
         {
            orc_Message.u32_TimeoutMs = (u32_EventTimer * 3UL) + 10UL;
         }
      }
      else
      {
         //Default
         orc_Message.u32_CycleTimeMs = 0UL;
         orc_Message.u32_TimeoutMs = 0UL;
      }
   }
   else
   {
      //Default
      orc_Message.u32_CycleTimeMs = 0UL;
      orc_Message.u32_TimeoutMs = 0UL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load inhibit time section in CANopen use-case

   \param[in]      ou32_StartingId  Starting ID for message section
   \param[in]      ou32_ItMessage   Current message index
   \param[in]      ou8_NodeId       Node ID
   \param[in]      orc_COObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in,out]  orc_CurMessages  Current import result messages
   \param[in,out]  orc_Message      Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_LoadInhibitTimeSectionCanOpen(const uint32 ou32_StartingId, const uint32 ou32_ItMessage,
                                                         const uint8 ou8_NodeId,
                                                         const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects, const bool oq_IsEds, std::vector<C_SCLString> & orc_CurMessages,
                                                         C_OSCCanMessage & orc_Message)
{
   const C_OSCCanOpenObject * const pc_COMessageITObject =
      mh_GetCOObject(orc_COObjects, ou32_StartingId + ou32_ItMessage,
                     C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME);

   if (pc_COMessageITObject != NULL)
   {
      uint32 u32_InhibitTime;
      if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageITObject, oq_IsEds),
                             ou8_NodeId,
                             u32_InhibitTime) == C_NO_ERR)
      {
         orc_Message.u16_DelayTimeMs = static_cast<uint16>(std::ceil(static_cast<float32>(u32_InhibitTime) / 10.0F));
         if ((u32_InhibitTime % 10) != 0)
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Inhibit-time",
                              "Inhibit time cannot \"" + stw_scl::C_SCLString::IntToStr(
                                 u32_InhibitTime * 100UL) + "\" ns cannot be used, and has been rounded to \"" +
                              stw_scl::C_SCLString::IntToStr(orc_Message.u16_DelayTimeMs) + "\" ms",
                              C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME, false,
                              &orc_CurMessages);
         }
      }
      else
      {
         //Default
         orc_Message.u16_DelayTimeMs = 0U;
      }
   }
   else
   {
      //Default
      orc_Message.u16_DelayTimeMs = 0U;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load core signal data

   \param[in]      ou32_COMessageId                   ID for CO message section
   \param[in]      ou8_NodeId                         Node ID
   \param[in]      orc_COObjects                      All CO objects of the current file
   \param[in]      orc_Dummies                        Found and valid dummy data types
   \param[in,out]  orc_OSCMessageData                 Imported core message data
   \param[in,out]  orc_OSCSignalData                  Imported core signal data
   \param[in,out]  orc_SignalDefaultMinMaxValuesUsed  Flag if imported core signal data uses the default min max values or
                                                      or specific set values
   \param[in]      oq_IsEds                           Flag if current file is an EDS file
   \param[in]      oq_RestrictForCanOpenUsage         Restrict for can open usage
   \param[in,out]  orc_ImportMessages                 Import result messages

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_ParseSignals(const uint32 ou32_COMessageId, const uint8 ou8_NodeId,
                                          const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                          const std::vector<uint32> & orc_Dummies, C_OSCCanMessage & orc_OSCMessageData,
                                          std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                          std::vector<uint8> & orc_SignalDefaultMinMaxValuesUsed, const bool oq_IsEds,
                                          const bool oq_RestrictForCanOpenUsage,
                                          std::vector<C_SCLString> & orc_ImportMessages)
{
   sint32 s32_Retval = C_NO_ERR;
   //PDO mapping parameter
   //---------------------
   const C_OSCCanOpenObject * const pc_COMessageMappingObject =
      mh_GetCOObject(orc_COObjects, ou32_COMessageId + 0x200, 0);

   if (oq_RestrictForCanOpenUsage == true)
   {
      // In case of CANopen the DLC is adapted automatically
      orc_OSCMessageData.u16_Dlc = 0U;
   }

   if (pc_COMessageMappingObject != NULL)
   {
      //Skip first section because this is just the number of sub segments
      uint32 u32_MappingCount;
      if (mh_GetIntegerValue(mh_GetCOObjectValue(*pc_COMessageMappingObject, oq_IsEds), ou8_NodeId,
                             u32_MappingCount) == C_NO_ERR)
      {
         if (u32_MappingCount <= 0x40U)
         {
            uint32 u32_StartBitCounter = 0U;
            for (uint32 u32_ItSignal = 0U; (u32_ItSignal < u32_MappingCount) && (s32_Retval == C_NO_ERR);
                 ++u32_ItSignal)
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
                        const uint16 u16_ExpectedLength = static_cast<uint16>(u32_MappingSubIndexValue & 0xFFUL);
                        C_OSCCanSignal c_CurSignal;
                        C_OSCNodeDataPoolListElement c_CurDataPoolSignal;
                        bool q_DefaultMinMax = true;

                        s32_Retval = C_OSCImportEdsDcf::h_ParseSignalContent(orc_COObjects, u32_CORefId, u32_CORefIdSub,
                                                                             u32_StartBitCounter,
                                                                             oq_RestrictForCanOpenUsage,
                                                                             c_CurSignal, c_CurDataPoolSignal,
                                                                             q_DefaultMinMax);
                        if (u16_ExpectedLength != c_CurSignal.u16_ComBitLength)
                        {
                           mh_AddUserMessage(u32_CORefId, "",
                                             "mapping signal bit length " +
                                             stw_scl::C_SCLString::IntToStr(
                                                u16_ExpectedLength) + " did not match to bit length from data type " +
                                             stw_scl::C_SCLString::IntToStr(
                                                c_CurSignal.u16_ComBitLength) + ". Using bit length " +
                                             stw_scl::C_SCLString::IntToStr(c_CurSignal.u16_ComBitLength),
                                             static_cast<sint32>(u32_CORefIdSub), false, &orc_ImportMessages);
                        }
                        u32_StartBitCounter += c_CurSignal.u16_ComBitLength;
                        if (s32_Retval == C_NO_ERR)
                        {
                           //Handle index
                           c_CurSignal.u32_ComDataElementIndex = orc_OSCSignalData.size();
                           //Add
                           orc_OSCMessageData.c_Signals.push_back(c_CurSignal);
                           orc_OSCSignalData.push_back(c_CurDataPoolSignal);
                           orc_SignalDefaultMinMaxValuesUsed.push_back(static_cast<uint8>(q_DefaultMinMax));

                           if (oq_RestrictForCanOpenUsage == true)
                           {
                              // In case of CANopen the DLC is adapted automatically. Adapt to the added signals
                              const uint16 u16_LastBit = c_CurSignal.u16_ComBitStart +
                                                         c_CurSignal.u16_ComBitLength;
                              uint16 u16_NeededBytes = u16_LastBit / 8U;

                              // Check for not byte aligned signals
                              if ((u16_LastBit % 8U) != 0U)
                              {
                                 ++u16_NeededBytes;
                              }

                              if (u16_NeededBytes > orc_OSCMessageData.u16_Dlc)
                              {
                                 orc_OSCMessageData.u16_Dlc = u16_NeededBytes;

                                 if (orc_OSCMessageData.u16_Dlc > 8U)
                                 {
                                    // 8 is maximum
                                    orc_OSCMessageData.u16_Dlc = 8U;
                                 }
                              }
                           }
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
         mh_AddUserMessage(ou32_COMessageId + 0x200, "",
                           "empty or not a number", 0, true);
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_COMessageId + 0x200, "", "does not exist", 0, true);
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value of CO Object

   \param[in]  orc_COObject   CO object
   \param[in]  oq_IsEds       Flag if current file is an EDS file

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

   \param[in]      orc_COValue         CO object value
   \param[in]      ou8_NodeId          Node ID
   \param[out]     oru32_Value         Parsed value
   \param[in,out]  opq_IncludesNodeId  Info flag if value includes node id

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_GetIntegerValue(const C_SCLString & orc_COValue, const uint8 ou8_NodeId,
                                             uint32 & oru32_Value, bool * const opq_IncludesNodeId)
{
   sint32 s32_Retval = C_NO_ERR;

   oru32_Value = 0UL;

   if (opq_IncludesNodeId != NULL)
   {
      *opq_IncludesNodeId = false;
   }

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

                  if (opq_IncludesNodeId != NULL)
                  {
                     *opq_IncludesNodeId = true;
                  }
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
/*! \brief   Convert string to signed 64 bit integer value

   \param[in]   orc_COValue   CO object value
   \param[out]  ors64_Value   Parsed value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_Get64IntegerValue(const C_SCLString & orc_COValue, sint64 & ors64_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   ors64_Value = 0UL;

   if (orc_COValue.Length() > 0)
   {
      C_SCLString c_ReducedString;
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
            c_ReducedString += orc_COValue[static_cast<sintn>(u32_ItChar + 1UL)];
         }
      }

      //Try to convert it
      if (c_ReducedString.Length() > 0)
      {
         try
         {
            ors64_Value = c_ReducedString.ToInt64();
         }
         catch (...)
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

   \param[in]   orc_COValue   CO object value
   \param[out]  oru32_Value   Parsed value

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

   \param[in]      ou32_COObjectId        CO object ID
   \param[in]      orc_COSectionName      CO object sub section name
   \param[in]      orc_ErrorMessage       Error message
   \param[in]      os32_COSubSectionId    Optional CO object sub section ID
   \param[in]      oq_IsError             Optional CO object sub section ID
   \param[in,out]  opc_ImportMessages     Import result messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCImportEdsDcf::mh_AddUserMessage(const uint32 ou32_COObjectId, const C_SCLString & orc_COSectionName,
                                          const C_SCLString & orc_ErrorMessage, const sint32 os32_COSubSectionId,
                                          const bool oq_IsError, std::vector<C_SCLString> * const opc_ImportMessages)
{
   C_SCLString c_Message;

   //Object ID in file
   c_Message += "Object ";
   //Add object id to message
   //Use hex notation because that's probably used in the file
   c_Message += C_OSCImportEdsDcf::mh_GetNumberAsHex(ou32_COObjectId);

   if (os32_COSubSectionId >= 0L)
   {
      c_Message += "sub";
      //Use hex notation because that's probably used in the file
      c_Message += C_OSCImportEdsDcf::mh_GetNumberAsHex(os32_COSubSectionId);
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
/*! \brief  Get number as hex

   \param[in]  ou32_Number    Number

   \return
   Number as hex
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCImportEdsDcf::mh_GetNumberAsHex(const uint32 ou32_Number)
{
   std::stringstream c_Stream;
   C_SCLString c_HexObjectIdUpperCase;
   //Hex
   c_Stream << &std::hex << ou32_Number;
   c_HexObjectIdUpperCase = c_Stream.str().c_str();
   //Use uppercase
   c_HexObjectIdUpperCase = c_HexObjectIdUpperCase.UpperCase();
   return c_HexObjectIdUpperCase;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate min max based on current type and number of bits

   \param[in]      opc_COSignalObject  Current CANopen signal object
   \param[in,out]  orc_Element         Data pool element
   \param[in]      ou16_NumberBits     Number of bits
   \param[out]     orq_DefaultMinMax   Flag if default values min max values are set or object specific limits are set

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_CONFIG Operation failure: Element invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCImportEdsDcf::mh_CalcMinMaxInit(const C_OSCCanOpenObject * const opc_COSignalObject,
                                            C_OSCNodeDataPoolListElement & orc_Element, const uint16 ou16_NumberBits,
                                            bool & orq_DefaultMinMax)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;

   orq_DefaultMinMax = true;

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
      uint64 u64_LowLimit = 0U;
      sint64 s64_LowLimit = 0U;
      uint64 u64_HighLimit = 0U;
      sint64 s64_HighLimit = 0U;
      uint64 u64_DefaultValue = 0U;
      sint64 s64_DefaultValue = 0U;
      bool q_LowLimitSet = false;
      bool q_HighLimitSet = false;

      if (q_IsFloat == false)
      {
         bool q_DefaultValueSet = false;

         // Check for specific limits and check against the generic limits
         if (mh_Get64IntegerValue(opc_COSignalObject->c_LowLimit, s64_LowLimit) == C_NO_ERR)
         {
            q_LowLimitSet = true;
         }
         if (mh_Get64IntegerValue(opc_COSignalObject->c_HighLimit, s64_HighLimit) == C_NO_ERR)
         {
            q_HighLimitSet = true;
         }
         if (mh_Get64IntegerValue(opc_COSignalObject->c_DefaultValue, s64_DefaultValue) == C_NO_ERR)
         {
            q_DefaultValueSet = true;
         }

         if (q_IsSigned == false)
         {
            if (q_LowLimitSet == true)
            {
               u64_LowLimit = static_cast<uint64>(s64_LowLimit);
            }
            if (q_HighLimitSet == true)
            {
               u64_HighLimit = static_cast<uint64>(s64_HighLimit);
            }
            if (q_DefaultValueSet == true)
            {
               u64_DefaultValue = static_cast<uint64>(s64_DefaultValue);
            }
         }
      }

      if (orc_Element.GetArray() == false)
      {
         //Unsigned
         if (q_IsUnsigned == true)
         {
            uint64 u64_Max = 0U;
            uint64 u64_Min = 0U;

            for (uint16 u16_ItBit = 0U; u16_ItBit < ou16_NumberBits; ++u16_ItBit)
            {
               u64_Max += (static_cast<uint64>(1U) << u16_ItBit);
            }

            // Check custom range for a valid limit
            if ((q_HighLimitSet == true) &&
                (u64_HighLimit < u64_Max))
            {
               u64_Max = u64_HighLimit;
               orq_DefaultMinMax = false;
            }
            if ((q_LowLimitSet == true) &&
                (u64_LowLimit < u64_Max))
            {
               u64_Min = u64_LowLimit;
               orq_DefaultMinMax = false;
            }

            // Check range for default value in any case
            // If no specific default value is set, 0 will be used and adapted for the new limits if necessary
            if (u64_DefaultValue > u64_Max)
            {
               u64_DefaultValue = u64_Max;
            }
            else if (u64_DefaultValue < u64_Min)
            {
               u64_DefaultValue = u64_Min;
            }
            else
            {
               // Nothing to do
            }

            switch (orc_Element.GetType()) //lint !e788 not all enum constants used; this is for unsigned only
            {
            case C_OSCNodeDataPoolContent::eUINT8:
               orc_Element.c_MinValue.SetValueU8(static_cast<uint8>(u64_Min));
               orc_Element.c_MaxValue.SetValueU8(static_cast<uint8>(u64_Max));
               orc_Element.c_DataSetValues[0].SetValueU8(static_cast<uint8>(u64_DefaultValue));
               break;
            case C_OSCNodeDataPoolContent::eUINT16:
               orc_Element.c_MinValue.SetValueU16(static_cast<uint16>(u64_Min));
               orc_Element.c_MaxValue.SetValueU16(static_cast<uint16>(u64_Max));
               orc_Element.c_DataSetValues[0].SetValueU16(static_cast<uint16>(u64_DefaultValue));
               break;
            case C_OSCNodeDataPoolContent::eUINT32:
               orc_Element.c_MinValue.SetValueU32(static_cast<uint32>(u64_Min));
               orc_Element.c_MaxValue.SetValueU32(static_cast<uint32>(u64_Max));
               orc_Element.c_DataSetValues[0].SetValueU32(static_cast<uint32>(u64_DefaultValue));
               break;
            case C_OSCNodeDataPoolContent::eUINT64:
               orc_Element.c_MinValue.SetValueU64(u64_Min);
               orc_Element.c_MaxValue.SetValueU64(u64_Max);
               orc_Element.c_DataSetValues[0].SetValueU64(u64_DefaultValue);
               break;
            default:
               s32_Retval = C_CONFIG;
               break;
            }
         }
         //Signed
         if (q_IsSigned == true)
         {
            uint64 u64_Max = 0U;
            sint64 s64_Max;
            sint64 s64_Min;

            //We need exactly one more than half of the unsigned maximum
            if (ou16_NumberBits > 0U)
            {
               u64_Max += (static_cast<uint64>(1U) << (ou16_NumberBits - 1U));
            }
            // Get the correct signed minimum value
            s64_Min = -static_cast<sint64>(u64_Max);
            // Adapt the max due to signed
            --u64_Max;
            s64_Max = static_cast<sint64>(u64_Max);

            // Check custom range for a valid limit
            if ((q_HighLimitSet == true) &&
                (s64_HighLimit < s64_Max))
            {
               s64_Max = s64_HighLimit;
               orq_DefaultMinMax = false;
            }
            if ((q_LowLimitSet == true) &&
                (s64_LowLimit > s64_Min) &&
                (s64_LowLimit < s64_Max))
            {
               s64_Min = s64_LowLimit;
               orq_DefaultMinMax = false;
            }

            // Check range for default value in any case
            // If no specific default value is set, 0 will be used and adapted for the new limits if necessary
            if (s64_DefaultValue > s64_Max)
            {
               s64_DefaultValue = s64_Max;
            }
            else if (s64_DefaultValue < s64_Min)
            {
               s64_DefaultValue = s64_Min;
            }
            else
            {
               // Nothing to do
            }

            switch (orc_Element.GetType()) //lint !e788 not all enum constants used; this is for signed only
            {
            case C_OSCNodeDataPoolContent::eSINT8:
               orc_Element.c_MinValue.SetValueS8(static_cast<sint8>(s64_Min));
               orc_Element.c_MaxValue.SetValueS8(static_cast<sint8>(s64_Max));
               orc_Element.c_DataSetValues[0].SetValueS8(static_cast<sint8>(s64_DefaultValue));
               break;
            case C_OSCNodeDataPoolContent::eSINT16:
               orc_Element.c_MinValue.SetValueS16(static_cast<sint16>(s64_Min));
               orc_Element.c_MaxValue.SetValueS16(static_cast<sint16>(s64_Max));
               orc_Element.c_DataSetValues[0].SetValueS16(static_cast<sint16>(s64_DefaultValue));
               break;
            case C_OSCNodeDataPoolContent::eSINT32:
               orc_Element.c_MinValue.SetValueS32(static_cast<sint32>(s64_Min));
               orc_Element.c_MaxValue.SetValueS32(static_cast<sint32>(s64_Max));
               orc_Element.c_DataSetValues[0].SetValueS32(static_cast<sint32>(s64_DefaultValue));
               break;
            case C_OSCNodeDataPoolContent::eSINT64:
               orc_Element.c_MinValue.SetValueS64(s64_Min);
               orc_Element.c_MaxValue.SetValueS64(s64_Max);
               orc_Element.c_DataSetValues[0].SetValueS64(s64_DefaultValue);
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
               orc_Element.c_DataSetValues[0].SetValueF32(0.0F);
               break;
            case C_OSCNodeDataPoolContent::eFLOAT64:
               orc_Element.c_MinValue.SetValueF64(-std::numeric_limits<float64>::max());
               orc_Element.c_MaxValue.SetValueF64(std::numeric_limits<float64>::max());
               orc_Element.c_DataSetValues[0].SetValueF64(0.0);
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

   \param[in]   orc_FilePath  Ini file path
   \param[out]  orc_Dummies   Found and valid dummy data types
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
