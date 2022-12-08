//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of file type .eds and .dcf (implementation)

   Utility class to handle imports of file type .eds and .dcf

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_SclIniFile.hpp"
#include "C_OscImportEdsDcf.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

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
   \param[out]  orc_OscRxMessageData                        Imported core Rx message data
   \param[out]  orc_OscRxSignalData                         Imported core Rx signal data
   \param[out]  orc_RxSignalDefaultMinMaxValuesUsed         Flag if imported core Rx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_OscTxMessageData                        Imported core Tx message data
   \param[out]  orc_OscTxSignalData                         Imported core Tx signal data
   \param[out]  orc_TxSignalDefaultMinMaxValuesUsed         Flag if imported core Rx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_ImportMessagesPerMessage                Import result messages
   \param[out]  orc_ParsingError                            Optional parsing error message
   \param[in]   oq_RestrictForCanOpenUsage                  Flag to restrict for CANopen usage
   \param[out]  orc_InvalidOscRxMessageData                 Invalid core Rx message data
   \param[out]  orc_InvalidOscRxSignalData                  Invalid core Rx signal data
   \param[out]  orc_InvalidRxSignalDefaultMinMaxValuesUsed  Flag if invalid core Rx signal data uses the default min
                                                            max values or or specific set values
   \param[out]  orc_InvalidOscTxMessageData                 Invalid core Tx message data
   \param[out]  orc_InvalidOscTxSignalData                  Invalid core Tx signal data
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
int32_t C_OscImportEdsDcf::h_Import(const C_SclString & orc_FilePath, const uint8_t ou8_NodeId,
                                    std::vector<C_OscCanMessage> & orc_OscRxMessageData,
                                    std::vector<C_OscNodeDataPoolListElement> & orc_OscRxSignalData,
                                    std::vector<uint8_t> & orc_RxSignalDefaultMinMaxValuesUsed,
                                    std::vector<C_OscCanMessage> & orc_OscTxMessageData,
                                    std::vector<C_OscNodeDataPoolListElement> & orc_OscTxSignalData,
                                    std::vector<uint8_t> & orc_TxSignalDefaultMinMaxValuesUsed,
                                    std::vector<std::vector<C_SclString> > & orc_ImportMessagesPerMessage,
                                    C_SclString & orc_ParsingError, const bool oq_RestrictForCanOpenUsage,
                                    std::vector<C_OscCanMessage> & orc_InvalidOscRxMessageData,
                                    std::vector<C_OscNodeDataPoolListElement> & orc_InvalidOscRxSignalData,
                                    std::vector<uint8_t> & orc_InvalidRxSignalDefaultMinMaxValuesUsed,
                                    std::vector<C_OscCanMessage> & orc_InvalidOscTxMessageData,
                                    std::vector<C_OscNodeDataPoolListElement> & orc_InvalidOscTxSignalData,
                                    std::vector<uint8_t> & orc_InvalidTxSignalDefaultMinMaxValuesUsed,
                                    std::vector<std::vector<C_SclString> > & orc_InvalidImportMessagesPerMessage)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear data
   orc_OscRxMessageData.clear();
   orc_OscRxSignalData.clear();
   orc_RxSignalDefaultMinMaxValuesUsed.clear();
   orc_OscTxMessageData.clear();
   orc_OscTxSignalData.clear();
   orc_TxSignalDefaultMinMaxValuesUsed.clear();
   orc_ImportMessagesPerMessage.clear();
   orc_ParsingError = "";

   if (TglFileExists(orc_FilePath) == true)
   {
      bool q_Eds = true;
      const C_SclString c_Extension = TglExtractFileExtension(orc_FilePath).LowerCase();
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
         C_OscCanOpenObjectDictionary c_Dictionary;
         if (c_Dictionary.LoadFromFile(orc_FilePath) == C_NO_ERR)
         {
            std::vector<uint32_t> c_Dummies;
            uint32_t u32_StartId;
            mh_LoadDummies(orc_FilePath, c_Dummies);
            if (oq_RestrictForCanOpenUsage)
            {
               u32_StartId = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO;
            }
            else
            {
               u32_StartId = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO;
            }
            s32_Retval = mh_ParseMessages(u32_StartId, ou8_NodeId, c_Dictionary.c_Objects, c_Dummies,
                                          orc_OscRxMessageData,
                                          orc_OscRxSignalData,
                                          orc_RxSignalDefaultMinMaxValuesUsed,
                                          q_Eds, orc_ImportMessagesPerMessage, false, oq_RestrictForCanOpenUsage,
                                          orc_InvalidOscRxMessageData, orc_InvalidOscRxSignalData,
                                          orc_InvalidRxSignalDefaultMinMaxValuesUsed,
                                          orc_InvalidImportMessagesPerMessage);
            if (s32_Retval == C_NO_ERR)
            {
               if (oq_RestrictForCanOpenUsage)
               {
                  u32_StartId = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO;
               }
               else
               {
                  u32_StartId = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO;
               }
               s32_Retval = mh_ParseMessages(u32_StartId, ou8_NodeId, c_Dictionary.c_Objects, c_Dummies,
                                             orc_OscTxMessageData,
                                             orc_OscTxSignalData,
                                             orc_TxSignalDefaultMinMaxValuesUsed,
                                             q_Eds, orc_ImportMessagesPerMessage, true, oq_RestrictForCanOpenUsage,
                                             orc_InvalidOscTxMessageData, orc_InvalidOscTxSignalData,
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

   \param[in]      orc_CoObjects                All CANopen objects of the current file
   \param[in]      ou32_CoSignalObjectIndex     CANopen signal object index
   \param[in]      ou32_CoSignalObjectSubIndex  CANopen signal object sub index
   \param[in]      ou32_StartBitCounter         Start bit counter
   \param[in]      oq_RestrictForCanOpenUsage   Restrict for CANopen usage
   \param[in]      oq_IsEds                     Flag if current file is an EDS file
   \param[in,out]  orc_CurSignal                Current signal data
   \param[in,out]  orc_CurDataPoolSignal        Current data pool signal data
   \param[out]     orq_DefaultMinMax            Flag if default values min max values are set or object specific
                                                limits are set

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::h_ParseSignalContent(const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects,
                                                const uint32_t ou32_CoSignalObjectIndex,
                                                const uint32_t ou32_CoSignalObjectSubIndex,
                                                const uint32_t ou32_StartBitCounter,
                                                const bool oq_RestrictForCanOpenUsage, const bool oq_IsEds,
                                                C_OscCanSignal & orc_CurSignal,
                                                C_OscNodeDataPoolListElement & orc_CurDataPoolSignal,
                                                bool & orq_DefaultMinMax)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanOpenObject * pc_CoSignalObject;

   if (ou32_CoSignalObjectSubIndex == 0)
   {
      //Access object directly
      pc_CoSignalObject =
         mh_GetCoObject(orc_CoObjects, ou32_CoSignalObjectIndex, -1);
   }
   else
   {
      //Access sub index
      pc_CoSignalObject =
         mh_GetCoObject(orc_CoObjects, ou32_CoSignalObjectIndex, ou32_CoSignalObjectSubIndex);
   }

   //CANopen attributes
   if (oq_RestrictForCanOpenUsage)
   {
      orc_CurSignal.u16_CanOpenManagerObjectDictionaryIndex = static_cast<uint16_t>(ou32_CoSignalObjectIndex);
      orc_CurSignal.u8_CanOpenManagerObjectDictionarySubIndex = static_cast<uint8_t>(ou32_CoSignalObjectSubIndex);
   }
   //Always intel
   orc_CurSignal.e_ComByteOrder = C_OscCanSignal::eBYTE_ORDER_INTEL;
   //Handle start bit
   orc_CurSignal.u16_ComBitStart = static_cast<uint16_t>(ou32_StartBitCounter);
   if (pc_CoSignalObject != NULL)
   {
      //Name
      orc_CurDataPoolSignal.c_Name = C_OscImportEdsDcf::h_GetObjectName(*pc_CoSignalObject);
      //Signal should never be an array
      orc_CurDataPoolSignal.SetArray(false);
      // Prepare dataset for the default value
      orc_CurDataPoolSignal.c_DataSetValues.resize(1);
      //Get length
      switch (pc_CoSignalObject->u8_DataType)
      {
      case C_OscCanOpenObject::hu8_DATA_TYPE_BOOLEAN:
         orc_CurSignal.u16_ComBitLength = 1U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED8:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER8:
         orc_CurSignal.u16_ComBitLength = 8U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER16:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED16:
         orc_CurSignal.u16_ComBitLength = 16U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER24:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED24:
         orc_CurSignal.u16_ComBitLength = 24U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_REAL32:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER32:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED32:
         orc_CurSignal.u16_ComBitLength = 32U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER40:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED40:
         orc_CurSignal.u16_ComBitLength = 40U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER48:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED48:
         orc_CurSignal.u16_ComBitLength = 48U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER56:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED56:
         orc_CurSignal.u16_ComBitLength = 56U;
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER64:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED64:
      case C_OscCanOpenObject::hu8_DATA_TYPE_REAL64:
         orc_CurSignal.u16_ComBitLength = 64U;
         break;
      default:
         //Reported next
         break;
      }
      //Data type (automatic calculation based on set type)
      switch (pc_CoSignalObject->u8_DataType)
      {
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER8:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER16:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER32:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER64:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER24:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER40:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER48:
      case C_OscCanOpenObject::hu8_DATA_TYPE_INTEGER56:
         if (orc_CurSignal.u16_ComBitLength <= 8U)
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eSINT8);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 8U) && (orc_CurSignal.u16_ComBitLength <= 16U))
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eSINT16);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 16U) && (orc_CurSignal.u16_ComBitLength <= 32U))
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eSINT32);
         }
         else
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eSINT64);
         }
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_BOOLEAN:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED8:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED16:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED32:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED64:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED24:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED40:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED48:
      case C_OscCanOpenObject::hu8_DATA_TYPE_UNSIGNED56:
         if (orc_CurSignal.u16_ComBitLength <= 8U)
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eUINT8);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 8U) && (orc_CurSignal.u16_ComBitLength <= 16U))
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eUINT16);
         }
         else if ((orc_CurSignal.u16_ComBitLength > 16U) && (orc_CurSignal.u16_ComBitLength <= 32U))
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eUINT32);
         }
         else
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eUINT64);
         }
         break;
      case C_OscCanOpenObject::hu8_DATA_TYPE_REAL32:
      case C_OscCanOpenObject::hu8_DATA_TYPE_REAL64:
         if (orc_CurSignal.u16_ComBitLength <= 32U)
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eFLOAT32);
         }
         else
         {
            orc_CurDataPoolSignal.SetType(C_OscNodeDataPoolContent::eFLOAT64);
         }
         break;
      default:
         mh_AddUserMessage(ou32_CoSignalObjectIndex, "", "data type not supported",
                           static_cast<int32_t>(ou32_CoSignalObjectSubIndex), true);
         s32_Retval = C_CONFIG;
         break;
      }

      //Min & max & init value
      if (mh_CalcMinMaxInit(pc_CoSignalObject, orc_CurDataPoolSignal, orc_CurSignal.u16_ComBitLength, oq_IsEds,
                            orq_DefaultMinMax) != C_NO_ERR)
      {
         mh_AddUserMessage(ou32_CoSignalObjectIndex, "",
                           "signal bit length does not fit the selected data type",
                           static_cast<int32_t>(ou32_CoSignalObjectSubIndex), true);
         s32_Retval = C_CONFIG;
      }

      //Access
      //Use default == current
   }
   else
   {
      mh_AddUserMessage(ou32_CoSignalObjectIndex, "", "does not exist",
                        static_cast<int32_t>(ou32_CoSignalObjectSubIndex), true);
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get object name

   \param[in]  orc_CoObject   CANopen object

   \return
   Object name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscImportEdsDcf::h_GetObjectName(const C_OscCanOpenObject & orc_CoObject)
{
   C_SclString c_Retval;

   if (orc_CoObject.c_Denotation != "")
   {
      c_Retval = orc_CoObject.c_Denotation;
   }
   else
   {
      c_Retval = orc_CoObject.c_Name;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value of CO Object

   \param[in]  orc_CoObject   CO object
   \param[in]  oq_IsEds       Flag if current file is an EDS file

   \return
   CO object value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscImportEdsDcf::h_GetCoObjectValue(const C_OscCanOpenObject & orc_CoObject, const bool oq_IsEds)
{
   C_SclString c_Retval;

   if (oq_IsEds == true)
   {
      c_Retval = orc_CoObject.c_DefaultValue;
   }
   else
   {
      if (orc_CoObject.c_ParameterValue.IsEmpty())
      {
         c_Retval = orc_CoObject.c_DefaultValue;
      }
      else
      {
         c_Retval = orc_CoObject.c_ParameterValue;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscImportEdsDcf::C_OscImportEdsDcf(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CO object by object ID

   \param[in]  orc_CoObjects  All CO objects
   \param[in]  ou32_Id        Object ID
   \param[in]  os32_SubIndex  Sub index

   \return
   NULL Object not found
   Else Valid object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenObject * C_OscImportEdsDcf::mh_GetCoObject(
   const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects, const uint32_t ou32_Id,
   const int32_t os32_SubIndex)
{
   const C_OscCanOpenObject * pc_Retval = NULL;

   for (int32_t s32_ItObject = 0; s32_ItObject < orc_CoObjects.GetLength(); ++s32_ItObject)
   {
      const C_OscCanOpenObject & rc_CurObject = orc_CoObjects[s32_ItObject];
      if ((rc_CurObject.u16_Index == ou32_Id) &&
          ((os32_SubIndex < 0) ||
           ((rc_CurObject.u8_NumSubs == 255) && (rc_CurObject.u8_SubIndex == static_cast<uint8_t>(os32_SubIndex)))))
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
   \param[in]      orc_CoObjects                               All CO objects of the current file
   \param[in]      orc_Dummies                                 Found and valid dummy data types
   \param[in,out]  orc_OscMessageData                          Imported core message data
   \param[in,out]  orc_OscSignalData                           Imported core signal data
   \param[in,out]  orc_SignalDefaultMinMaxValuesUsed           Flag if imported core signal data uses the default min max
                                                               values or specific set values
   \param[in]      oq_IsEds                                    Flag if current file is an EDS file
   \param[in,out]  orc_ImportMessages                          Import result messages
   \param[in]      oq_IsTx                                     Flag if message is a Tx message
   \param[in]      oq_RestrictForCanOpenUsage                  Restrict for can open usage
   \param[in,out]  orc_InvalidOscMessageData                   Invalid core message data
   \param[in,out]  orc_InvalidOscSignalData                    Invalid core signal data
   \param[in,out]  orc_InvalidSignalDefaultMinMaxValuesUsed    Flag if invalid core signal data uses the default min max
                                                               values or specific set values
   \param[in,out]  orc_InvalidImportMessages                   Import result messages per invalid message

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_ParseMessages(const uint32_t ou32_StartingId, const uint8_t ou8_NodeId,
                                            const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects,
                                            const std::vector<uint32_t> & orc_Dummies,
                                            std::vector<C_OscCanMessage> & orc_OscMessageData,
                                            std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalData,
                                            std::vector<uint8_t> & orc_SignalDefaultMinMaxValuesUsed,
                                            const bool oq_IsEds,
                                            std::vector<std::vector<C_SclString> > & orc_ImportMessages,
                                            const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                            std::vector<C_OscCanMessage> & orc_InvalidOscMessageData,
                                            std::vector<C_OscNodeDataPoolListElement> & orc_InvalidOscSignalData,
                                            std::vector<uint8_t> & orc_InvalidSignalDefaultMinMaxValuesUsed,
                                            std::vector<std::vector<stw::scl::C_SclString> > & orc_InvalidImportMessages)
{
   int32_t s32_Retval = C_NO_ERR;

   for (int32_t s32_ItObject = 0; (s32_ItObject < orc_CoObjects.GetLength()) && (s32_Retval == C_NO_ERR);
        ++s32_ItObject)
   {
      //Main section
      const C_OscCanOpenObject & rc_CoMessageMainObject = orc_CoObjects[s32_ItObject];
      //Check if relevant object
      if ((rc_CoMessageMainObject.u8_NumSubs != 255) &&
          ((rc_CoMessageMainObject.u16_Index >= ou32_StartingId) &&
           (rc_CoMessageMainObject.u16_Index < (ou32_StartingId + 0x200))))
      {
         const uint32_t u32_ItMessage = rc_CoMessageMainObject.u16_Index - ou32_StartingId;
         //COB-ID section
         //--------------
         const C_OscCanOpenObject * const pc_CoMessageCobIdObject =
            mh_GetCoObject(orc_CoObjects, ou32_StartingId + u32_ItMessage,
                           C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID);
         if (pc_CoMessageCobIdObject != NULL)
         {
            bool q_CobIdIncludesNodeId;
            uint32_t u32_CobId;
            if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageCobIdObject, oq_IsEds), ou8_NodeId,
                                   u32_CobId, &q_CobIdIncludesNodeId) == C_NO_ERR)
            {
               //Check if message active
               if ((u32_CobId & 0x80000000UL) == 0UL)
               {
                  s32_Retval = C_OscImportEdsDcf::mh_ParseMessageContent(ou32_StartingId, ou8_NodeId, orc_CoObjects,
                                                                         orc_Dummies,
                                                                         orc_OscMessageData, orc_OscSignalData,
                                                                         orc_SignalDefaultMinMaxValuesUsed,
                                                                         oq_IsEds,
                                                                         orc_ImportMessages, oq_IsTx,
                                                                         oq_RestrictForCanOpenUsage,
                                                                         rc_CoMessageMainObject, u32_ItMessage,
                                                                         u32_CobId, orc_InvalidOscMessageData,
                                                                         orc_InvalidOscSignalData,
                                                                         orc_InvalidSignalDefaultMinMaxValuesUsed,
                                                                         orc_InvalidImportMessages,
                                                                         q_CobIdIncludesNodeId);
               }
               else
               {
                  if (oq_RestrictForCanOpenUsage)
                  {
                     s32_Retval = C_OscImportEdsDcf::mh_ParseMessageContent(ou32_StartingId, ou8_NodeId, orc_CoObjects,
                                                                            orc_Dummies,
                                                                            orc_OscMessageData, orc_OscSignalData,
                                                                            orc_SignalDefaultMinMaxValuesUsed,
                                                                            oq_IsEds,
                                                                            orc_ImportMessages, oq_IsTx,
                                                                            oq_RestrictForCanOpenUsage,
                                                                            rc_CoMessageMainObject, u32_ItMessage,
                                                                            u32_CobId, orc_InvalidOscMessageData,
                                                                            orc_InvalidOscSignalData,
                                                                            orc_InvalidSignalDefaultMinMaxValuesUsed,
                                                                            orc_InvalidImportMessages,
                                                                            q_CobIdIncludesNodeId);
                  }
                  else
                  {
                     std::stringstream c_Stream;
                     const uint32_t u32_Id = u32_CobId & 0x1FFFFFFFUL;
                     c_Stream << "Skipped message as marked as inactive (highest bit set in COB_ID) ";
                     c_Stream << &std::hex << u32_Id;
                     mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                                       c_Stream.str().c_str(), C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID,
                                       true);
                  }
               }
            }
            else
            {
               mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                                 "empty or not a number",
                                 C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID, true);
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            mh_AddUserMessage(ou32_StartingId + u32_ItMessage, "COB-ID used by PDO",
                              "does not exist", C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID,
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
   \param[in]      orc_CoObjects                               All CO objects of the current file
   \param[in]      orc_Dummies                                 Found and valid dummy data types
   \param[in,out]  orc_OscMessageData                          Imported core message data
   \param[in,out]  orc_OscSignalData                           Imported core signal data
   \param[in,out]  orc_SignalDefaultMinMaxValuesUsed           Flag if imported core signal data uses the default min max
                                                               values or specific set values
   \param[in]      oq_IsEds                                    Flag if current file is an EDS file
   \param[in,out]  orc_ImportMessages                          Import result messages
   \param[in]      oq_IsTx                                     Flag if message is a Tx message
   \param[in]      oq_RestrictForCanOpenUsage                  Restrict for can open usage
   \param[in]      orc_CoMessageMainObject                     CANOpen message main object
   \param[in]      ou32_ItMessage                              Current message index
   \param[in]      ou32_CobId                                  Message COB-ID
   \param[in,out]  orc_InvalidOscMessageData                   Invalid core message data
   \param[in,out]  orc_InvalidOscSignalData                    Invalid core signal data
   \param[in,out]  orc_InvalidSignalDefaultMinMaxValuesUsed    Flag if invalid core signal data uses the default min max
                                                               values or specific set values
   \param[in,out]  orc_InvalidImportMessages                   Import result messages per invalid message
   \param[in]      oq_CobIdIncludesNodeId                      Flag if message COB-ID includes node id

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_ParseMessageContent(const uint32_t ou32_StartingId, const uint8_t ou8_NodeId,
                                                  const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects,
                                                  const std::vector<uint32_t> & orc_Dummies,
                                                  std::vector<C_OscCanMessage> & orc_OscMessageData,
                                                  std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalData,
                                                  std::vector<uint8_t> & orc_SignalDefaultMinMaxValuesUsed,
                                                  const bool oq_IsEds,
                                                  std::vector<std::vector<C_SclString> > & orc_ImportMessages,
                                                  const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                                  const C_OscCanOpenObject & orc_CoMessageMainObject,
                                                  const uint32_t ou32_ItMessage, const uint32_t ou32_CobId,
                                                  std::vector<C_OscCanMessage> & orc_InvalidOscMessageData,
                                                  std::vector<C_OscNodeDataPoolListElement> & orc_InvalidOscSignalData,
                                                  std::vector<uint8_t> & orc_InvalidSignalDefaultMinMaxValuesUsed,
                                                  std::vector<std::vector<stw::scl::C_SclString> > & orc_InvalidImportMessages,
                                                  const bool oq_CobIdIncludesNodeId)
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_AddToSkippedMessages = false;

   std::vector<C_SclString> c_CurMessages;
   C_OscCanMessage c_Message;
   //Name
   if (oq_IsEds)
   {
      c_Message.c_Name = orc_CoMessageMainObject.c_Name.c_str();
   }
   else
   {
      c_Message.c_Name = C_OscImportEdsDcf::h_GetObjectName(orc_CoMessageMainObject);
   }
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
      c_Message.q_CanOpenManagerCobIdIncludesNodeId = oq_CobIdIncludesNodeId;
      // u32_CanId has the resulting CAN ID with already added node id
      if (oq_CobIdIncludesNodeId)
      {
         c_Message.u32_CanOpenManagerCobIdOffset = c_Message.u32_CanId - ou8_NodeId;
      }
      else
      {
         c_Message.u32_CanOpenManagerCobIdOffset = c_Message.u32_CanId;
      }
      c_Message.u16_CanOpenManagerPdoIndex = static_cast<uint16_t>(ou32_ItMessage);
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
      C_OscImportEdsDcf::mh_LoadMessageTransmissionTypeCanOpen(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                               orc_CoObjects, oq_IsEds, q_AddToSkippedMessages,
                                                               c_CurMessages, c_Message);
   }
   else
   {
      s32_Retval = C_OscImportEdsDcf::mh_LoadMessageTransmissionType(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                                     orc_CoObjects, oq_IsEds, c_CurMessages, c_Message);
   }
   //Optional
   //Event-timer section
   //-------------------
   if (oq_RestrictForCanOpenUsage)
   {
      C_OscImportEdsDcf::mh_LoadEventTimerSectionCanOpen(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                         orc_CoObjects, oq_IsEds, oq_IsTx, c_Message);
   }
   else
   {
      C_OscImportEdsDcf::mh_LoadEventTimerSection(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                  orc_CoObjects, oq_IsEds, oq_IsTx, c_CurMessages, c_Message);
   }

   //Inhibit-time section
   //-------------------
   if (oq_RestrictForCanOpenUsage)
   {
      C_OscImportEdsDcf::mh_LoadInhibitTimeSectionCanOpen(ou32_StartingId, ou32_ItMessage, ou8_NodeId,
                                                          orc_CoObjects, oq_IsEds, c_CurMessages, c_Message);
   }

   if (s32_Retval == C_NO_ERR)
   {
      //Signals

      if (q_AddToSkippedMessages)
      {
         s32_Retval = mh_ParseSignals(ou32_StartingId + ou32_ItMessage, ou8_NodeId,
                                      orc_CoObjects, orc_Dummies, c_Message, orc_InvalidOscSignalData,
                                      orc_InvalidSignalDefaultMinMaxValuesUsed,
                                      oq_IsEds, oq_RestrictForCanOpenUsage, c_CurMessages);
      }
      else
      {
         s32_Retval = mh_ParseSignals(ou32_StartingId + ou32_ItMessage, ou8_NodeId,
                                      orc_CoObjects, orc_Dummies, c_Message, orc_OscSignalData,
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
         orc_InvalidOscMessageData.push_back(c_Message);
         orc_InvalidImportMessages.push_back(c_CurMessages);
      }
      else
      {
         //Should be in sync
         orc_OscMessageData.push_back(c_Message);
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
   \param[in]      orc_CoObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in,out]  orc_CurMessages  Current import result messages
   \param[in,out]  orc_Message      Message

   \return
   STW error codes

   \retval   C_NO_ERR   Detailed description
   \retval   C_CONFIG   Detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_LoadMessageTransmissionType(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                          const uint8_t ou8_NodeId,
                                                          const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, std::vector<C_SclString> & orc_CurMessages,
                                                          C_OscCanMessage & orc_Message)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanOpenObject * const pc_CoMessageTransTypeObject =
      mh_GetCoObject(orc_CoObjects, ou32_StartingId + ou32_ItMessage,
                     C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE);

   if (pc_CoMessageTransTypeObject != NULL)
   {
      uint32_t u32_TransmissionType;
      if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageTransTypeObject, oq_IsEds), ou8_NodeId,
                             u32_TransmissionType) == C_NO_ERR)
      {
         if ((u32_TransmissionType >= 1UL) && (u32_TransmissionType <= 0xF0UL))
         {
            orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CYCLIC;
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                              "the message type \"synchronous\" was converted to \"cyclic\".\n"
                              "Cycle time set default to " +
                              C_SclString::IntToStr(
                                 orc_Message.u32_CycleTimeMs) + "ms.",
                              C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                              &orc_CurMessages);
         }
         else if ((u32_TransmissionType == 0xFCUL) || (u32_TransmissionType == 0xFDUL))
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                              "not supported",
                              C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, true);
            s32_Retval = C_CONFIG;
         }
         else
         {
            orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_ON_EVENT;
         }
      }
      else
      {
         mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                           "empty or not a number, default set to: \"on event\"",
                           C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                           &orc_CurMessages);
         orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_ON_EVENT;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                        "does not exist, default set to: \"on event\"", 2L, false, &orc_CurMessages);
      orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_ON_EVENT;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load message transmission type in CANopen use-case

   \param[in]      ou32_StartingId           Starting ID for message section
   \param[in]      ou32_ItMessage            Current message index
   \param[in]      ou8_NodeId                Node ID
   \param[in]      orc_CoObjects             All CO objects of the current file
   \param[in]      oq_IsEds                  Flag if current file is an EDS file
   \param[out]     orq_AddToSkippedMessages  Add to skipped messages
   \param[in,out]  orc_CurMessages           Current import result messages
   \param[in,out]  orc_Message               Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportEdsDcf::mh_LoadMessageTransmissionTypeCanOpen(const uint32_t ou32_StartingId,
                                                              const uint32_t ou32_ItMessage, const uint8_t ou8_NodeId,
                                                              const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, bool & orq_AddToSkippedMessages, std::vector<C_SclString> & orc_CurMessages,
                                                              C_OscCanMessage & orc_Message)
{
   const C_OscCanOpenObject * const pc_CoMessageTransTypeObject =
      mh_GetCoObject(orc_CoObjects, ou32_StartingId + ou32_ItMessage,
                     C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE);

   if (pc_CoMessageTransTypeObject != NULL)
   {
      uint32_t u32_TransmissionType;
      if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageTransTypeObject, oq_IsEds), ou8_NodeId,
                             u32_TransmissionType) == C_NO_ERR)
      {
         if (u32_TransmissionType == 254UL)
         {
            orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
         }
         else if (u32_TransmissionType == 255UL)
         {
            orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255;
         }
         else if (u32_TransmissionType == 0UL)
         {
            orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0;
         }
         else if (u32_TransmissionType <= 240UL)
         {
            orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240;
            orc_Message.u8_CanOpenTxMethodAdditionalInfo = static_cast<uint8_t>(u32_TransmissionType);
         }
         else
         {
            if (pc_CoMessageTransTypeObject->IsWriteable())
            {
               orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
               mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                                 "the message type \"CANopen type " +
                                 C_SclString::IntToStr(
                                    u32_TransmissionType) + "\" was converted to \"async, manufacturer specific (254)\".",
                                 C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                                 &orc_CurMessages);
            }
            else
            {
               orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
               mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                                 "the message type \"CANopen type " +
                                 C_SclString::IntToStr(
                                    u32_TransmissionType) + "\" was found which is not supported"
                                 " and cannot be changed because it is read only.",
                                 C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                                 &orc_CurMessages);
               orq_AddToSkippedMessages = true;
            }
         }
      }
      else
      {
         mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                           "empty or not a number, default set to: \"async, manufacturer specific (254)\"",
                           C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                           &orc_CurMessages);
         orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Transmission type",
                        "does not exist, default set to: \"async, manufacturer specific (254)\"",
                        C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE, false,
                        &orc_CurMessages);
      orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
      orq_AddToSkippedMessages = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load event timer section

   \param[in]      ou32_StartingId  Starting ID for message section
   \param[in]      ou32_ItMessage   Current message index
   \param[in]      ou8_NodeId       Node ID
   \param[in]      orc_CoObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in]      oq_IsTx          Flag if message is a Tx message
   \param[in,out]  orc_CurMessages  Current import result messages
   \param[in,out]  orc_Message      Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportEdsDcf::mh_LoadEventTimerSection(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                 const uint8_t ou8_NodeId,
                                                 const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects,
                                                 const bool oq_IsEds, const bool oq_IsTx,
                                                 std::vector<C_SclString> & orc_CurMessages,
                                                 C_OscCanMessage & orc_Message)
{
   if (oq_IsTx == false)
   {
      bool q_UseDefault = false;
      C_SclString c_Reason;

      // Event-timer only relevant for Rx because it equals the timeout time
      const C_OscCanOpenObject * const pc_CoMessageEventTimerObject =
         mh_GetCoObject(orc_CoObjects, ou32_StartingId + ou32_ItMessage,
                        C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER);
      if (pc_CoMessageEventTimerObject != NULL)
      {
         uint32_t u32_EventTimer;
         if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageEventTimerObject, oq_IsEds),
                                ou8_NodeId,
                                u32_EventTimer) == C_NO_ERR)
         {
            if ((orc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_EVENT) ||
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
         if (orc_Message.e_TxMethod != C_OscCanMessage::eTX_METHOD_ON_EVENT)
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Event-timer",
                              c_Reason + ", default set to: " +
                              C_SclString::IntToStr(
                                 orc_Message.u32_TimeoutMs) + "ms", C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER, false,
                              &orc_CurMessages);
         }
         else
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Event-timer",
                              c_Reason + ", default set to: disabled (0)",
                              C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER, false,
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
   \param[in]      orc_CoObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in]      oq_IsTx          Flag if message is a Tx message
   \param[in,out]  orc_Message      Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportEdsDcf::mh_LoadEventTimerSectionCanOpen(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                        const uint8_t ou8_NodeId,
                                                        const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, const bool oq_IsTx,
                                                        C_OscCanMessage & orc_Message)
{
   const C_OscCanOpenObject * const pc_CoMessageEventTimerObject =
      mh_GetCoObject(orc_CoObjects, ou32_StartingId + ou32_ItMessage,
                     C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER);

   if (pc_CoMessageEventTimerObject != NULL)
   {
      uint32_t u32_EventTimer;
      if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageEventTimerObject, oq_IsEds),
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
   \param[in]      orc_CoObjects    All CO objects of the current file
   \param[in]      oq_IsEds         Flag if current file is an EDS file
   \param[in,out]  orc_CurMessages  Current import result messages
   \param[in,out]  orc_Message      Message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportEdsDcf::mh_LoadInhibitTimeSectionCanOpen(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                         const uint8_t ou8_NodeId,
                                                         const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, std::vector<C_SclString> & orc_CurMessages,
                                                         C_OscCanMessage & orc_Message)
{
   const C_OscCanOpenObject * const pc_CoMessageInhibitTimeObjec =
      mh_GetCoObject(orc_CoObjects, ou32_StartingId + ou32_ItMessage,
                     C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME);

   if (pc_CoMessageInhibitTimeObjec != NULL)
   {
      uint32_t u32_InhibitTime;
      if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageInhibitTimeObjec, oq_IsEds),
                             ou8_NodeId,
                             u32_InhibitTime) == C_NO_ERR)
      {
         orc_Message.u16_DelayTimeMs =
            static_cast<uint16_t>(std::ceil(static_cast<float32_t>(u32_InhibitTime) / 10.0F));
         if ((u32_InhibitTime % 10) != 0)
         {
            mh_AddUserMessage(ou32_StartingId + ou32_ItMessage, "Inhibit-time",
                              "Inhibit time cannot \"" + stw::scl::C_SclString::IntToStr(
                                 u32_InhibitTime * 100U) + "\" ns cannot be used, and has been rounded to \"" +
                              stw::scl::C_SclString::IntToStr(orc_Message.u16_DelayTimeMs) + "\" ms",
                              C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME, false,
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

   \param[in]      ou32_CoMessageId                   ID for CO message section
   \param[in]      ou8_NodeId                         Node ID
   \param[in]      orc_CoObjects                      All CO objects of the current file
   \param[in]      orc_Dummies                        Found and valid dummy data types
   \param[in,out]  orc_OscMessageData                 Imported core message data
   \param[in,out]  orc_OscSignalData                  Imported core signal data
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
int32_t C_OscImportEdsDcf::mh_ParseSignals(const uint32_t ou32_CoMessageId, const uint8_t ou8_NodeId,
                                           const stw::scl::C_SclDynamicArray<C_OscCanOpenObject> & orc_CoObjects,
                                           const std::vector<uint32_t> & orc_Dummies,
                                           C_OscCanMessage & orc_OscMessageData,
                                           std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalData,
                                           std::vector<uint8_t> & orc_SignalDefaultMinMaxValuesUsed,
                                           const bool oq_IsEds, const bool oq_RestrictForCanOpenUsage,
                                           std::vector<C_SclString> & orc_ImportMessages)
{
   int32_t s32_Retval = C_NO_ERR;
   //PDO mapping parameter
   //---------------------
   const C_OscCanOpenObject * const pc_CoMessageMappingObject =
      mh_GetCoObject(orc_CoObjects, ou32_CoMessageId + 0x200, 0);

   if (oq_RestrictForCanOpenUsage == true)
   {
      // In case of CANopen the DLC is adapted automatically
      orc_OscMessageData.u16_Dlc = 0U;
   }

   if (pc_CoMessageMappingObject != NULL)
   {
      //Skip first section because this is just the number of sub segments
      uint32_t u32_MappingCount;
      if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageMappingObject, oq_IsEds), ou8_NodeId,
                             u32_MappingCount) == C_NO_ERR)
      {
         if (u32_MappingCount <= 0x40U)
         {
            uint32_t u32_StartBitCounter = 0U;
            for (uint32_t u32_ItSignal = 0U; (u32_ItSignal < u32_MappingCount) && (s32_Retval == C_NO_ERR);
                 ++u32_ItSignal)
            {
               //Signal pointer section
               //----------------------
               //Skip first section because this is just the number of sub segments
               const C_OscCanOpenObject * const pc_CoMessageMappingSubObject =
                  mh_GetCoObject(orc_CoObjects, ou32_CoMessageId + 0x200UL, static_cast<int32_t>(u32_ItSignal + 1UL));

               if (pc_CoMessageMappingSubObject != NULL)
               {
                  uint32_t u32_MappingSubIndexValue;
                  if (mh_GetIntegerValue(h_GetCoObjectValue(*pc_CoMessageMappingSubObject, oq_IsEds), ou8_NodeId,
                                         u32_MappingSubIndexValue) == C_NO_ERR)
                  {
                     bool q_Dummy = false;
                     const uint32_t u32_CoRefId = (u32_MappingSubIndexValue & 0xFFFF0000UL) >> 16UL;
                     //Search if this signal is a valid dummy signal
                     for (uint32_t u32_ItDummy = 0; u32_ItDummy < orc_Dummies.size(); ++u32_ItDummy)
                     {
                        if (u32_CoRefId == orc_Dummies[u32_ItDummy])
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
                        const uint32_t u32_CoRefIdSub = (u32_MappingSubIndexValue & 0xFF00UL) >> 8UL;
                        const uint16_t u16_ExpectedLength = static_cast<uint16_t>(u32_MappingSubIndexValue & 0xFFUL);
                        C_OscCanSignal c_CurSignal;
                        C_OscNodeDataPoolListElement c_CurDataPoolSignal;
                        bool q_DefaultMinMax = true;

                        s32_Retval = C_OscImportEdsDcf::h_ParseSignalContent(orc_CoObjects, u32_CoRefId, u32_CoRefIdSub,
                                                                             u32_StartBitCounter,
                                                                             oq_RestrictForCanOpenUsage, oq_IsEds,
                                                                             c_CurSignal, c_CurDataPoolSignal,
                                                                             q_DefaultMinMax);
                        if (u16_ExpectedLength != c_CurSignal.u16_ComBitLength)
                        {
                           mh_AddUserMessage(u32_CoRefId, "",
                                             "mapping signal bit length " +
                                             stw::scl::C_SclString::IntToStr(
                                                u16_ExpectedLength) + " did not match to bit length from data type " +
                                             stw::scl::C_SclString::IntToStr(
                                                c_CurSignal.u16_ComBitLength) + ". Using bit length " +
                                             stw::scl::C_SclString::IntToStr(c_CurSignal.u16_ComBitLength),
                                             static_cast<int32_t>(u32_CoRefIdSub), false, &orc_ImportMessages);
                        }
                        u32_StartBitCounter += c_CurSignal.u16_ComBitLength;
                        if (s32_Retval == C_NO_ERR)
                        {
                           //Handle index
                           c_CurSignal.u32_ComDataElementIndex = orc_OscSignalData.size();
                           //Add
                           orc_OscMessageData.c_Signals.push_back(c_CurSignal);
                           orc_OscSignalData.push_back(c_CurDataPoolSignal);
                           orc_SignalDefaultMinMaxValuesUsed.push_back(static_cast<uint8_t>(q_DefaultMinMax));

                           if (oq_RestrictForCanOpenUsage == true)
                           {
                              // In case of CANopen the DLC is adapted automatically. Adapt to the added signals
                              const uint16_t u16_LastBit = c_CurSignal.u16_ComBitStart +
                                                           c_CurSignal.u16_ComBitLength;
                              uint16_t u16_NeededBytes = u16_LastBit / 8U;

                              // Check for not byte aligned signals
                              if ((u16_LastBit % 8U) != 0U)
                              {
                                 ++u16_NeededBytes;
                              }

                              if (u16_NeededBytes > orc_OscMessageData.u16_Dlc)
                              {
                                 orc_OscMessageData.u16_Dlc = u16_NeededBytes;

                                 if (orc_OscMessageData.u16_Dlc > 8U)
                                 {
                                    // 8 is maximum
                                    orc_OscMessageData.u16_Dlc = 8U;
                                 }
                              }
                           }
                        }
                     }
                  }
                  else
                  {
                     mh_AddUserMessage(ou32_CoMessageId + 0x200, "",
                                       "empty or not a number", static_cast<int32_t>(u32_ItSignal + 1UL), true);
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  mh_AddUserMessage(ou32_CoMessageId + 0x200, "", "does not exist",
                                    static_cast<int32_t>(u32_ItSignal + 1UL), true);
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            mh_AddUserMessage(ou32_CoMessageId + 0x200, "", "has unexpected value", -1L, true);
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         mh_AddUserMessage(ou32_CoMessageId + 0x200, "",
                           "empty or not a number", 0, true);
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      mh_AddUserMessage(ou32_CoMessageId + 0x200, "", "does not exist", 0, true);
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to integer value

   \param[in]      orc_CoValue         CO object value
   \param[in]      ou8_NodeId          Node ID
   \param[out]     oru32_Value         Parsed value
   \param[in,out]  opq_IncludesNodeId  Info flag if value includes node id

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_GetIntegerValue(const C_SclString & orc_CoValue, const uint8_t ou8_NodeId,
                                              uint32_t & oru32_Value, bool * const opq_IncludesNodeId)
{
   int32_t s32_Retval = C_NO_ERR;

   oru32_Value = 0UL;

   if (opq_IncludesNodeId != NULL)
   {
      *opq_IncludesNodeId = false;
   }

   if (orc_CoValue.Length() > 0)
   {
      C_SclString c_LowerCaseNoWhiteSpaceNumber;
      //Remove whitespace and $
      for (uint32_t u32_ItChar = 0; u32_ItChar < orc_CoValue.Length(); ++u32_ItChar)
      {
         const char_t cn_Character = orc_CoValue[u32_ItChar + 1U];
         if ((cn_Character == ' ') || (cn_Character == '$'))
         {
            //Skip
         }
         else
         {
            c_LowerCaseNoWhiteSpaceNumber += orc_CoValue[u32_ItChar + 1U];
         }
      }
      //Lower case
      c_LowerCaseNoWhiteSpaceNumber = c_LowerCaseNoWhiteSpaceNumber.LowerCase();
      if (c_LowerCaseNoWhiteSpaceNumber.Length() > 0)
      {
         C_SclDynamicArray<C_SclString> c_Tokens;
         c_LowerCaseNoWhiteSpaceNumber.Tokenize("+", c_Tokens);
         if (c_Tokens.GetLength() > 0L)
         {
            for (uint32_t u32_ItToken = 0;
                 (u32_ItToken < static_cast<uint32_t>(c_Tokens.GetLength())) && (s32_Retval == C_NO_ERR);
                 ++u32_ItToken)
            {
               const C_SclString & rc_CurToken = c_Tokens[u32_ItToken];
               if (rc_CurToken.Pos("nodeid") == 0)
               {
                  uint32_t u32_CurNumber;
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
                  oru32_Value += static_cast<uint32_t>(ou8_NodeId);

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

   \param[in]   orc_CoValue   CO object value
   \param[out]  ors64_Value   Parsed value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_Get64IntegerValue(const C_SclString & orc_CoValue, int64_t & ors64_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   ors64_Value = 0UL;

   if (orc_CoValue.Length() > 0)
   {
      C_SclString c_ReducedString;
      //Remove whitespace and $
      for (uint32_t u32_ItChar = 0; u32_ItChar < orc_CoValue.Length(); ++u32_ItChar)
      {
         const char_t cn_Character = orc_CoValue[u32_ItChar + 1U];
         if ((cn_Character == ' ') || (cn_Character == '$'))
         {
            //Skip
         }
         else
         {
            c_ReducedString += orc_CoValue[u32_ItChar + 1U];
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

   \param[in]   orc_CoValue   CO object value
   \param[out]  oru32_Value   Parsed value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_GetIntegerValueSimple(const C_SclString & orc_CoValue, uint32_t & oru32_Value)
{
   int32_t s32_Retval;

   oru32_Value = 0U;

   if (orc_CoValue.Length() > 0)
   {
      char_t * pcn_Ptr = NULL;

      oru32_Value = strtoul(orc_CoValue.c_str(), &pcn_Ptr, 0);
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

   \param[in]      ou32_CoObjectId        CO object ID
   \param[in]      orc_CoSectionName      CO object sub section name
   \param[in]      orc_ErrorMessage       Error message
   \param[in]      os32_CoSubSectionId    Optional CO object sub section ID
   \param[in]      oq_IsError             Optional CO object sub section ID
   \param[in,out]  opc_ImportMessages     Import result messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportEdsDcf::mh_AddUserMessage(const uint32_t ou32_CoObjectId, const C_SclString & orc_CoSectionName,
                                          const C_SclString & orc_ErrorMessage, const int32_t os32_CoSubSectionId,
                                          const bool oq_IsError, std::vector<C_SclString> * const opc_ImportMessages)
{
   C_SclString c_Message;

   //Object ID in file
   c_Message += "Object ";
   //Add object id to message
   //Use hex notation because that's probably used in the file
   c_Message += C_OscImportEdsDcf::mh_GetNumberAsHex(ou32_CoObjectId);

   if (os32_CoSubSectionId >= 0L)
   {
      c_Message += "sub";
      //Use hex notation because that's probably used in the file
      c_Message += C_OscImportEdsDcf::mh_GetNumberAsHex(os32_CoSubSectionId);
   }
   //Optional section name by spec
   if (orc_CoSectionName != "")
   {
      c_Message += " (\"";
      c_Message += orc_CoSectionName;
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
C_SclString C_OscImportEdsDcf::mh_GetNumberAsHex(const uint32_t ou32_Number)
{
   const C_SclString c_HexObjectIdUpperCase = C_SclString::IntToHex(ou32_Number, 1).UpperCase();

   return c_HexObjectIdUpperCase;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate min max based on current type and number of bits

   \param[in]      opc_CoSignalObject  Current CANopen signal object
   \param[in,out]  orc_Element         Data pool element
   \param[in]      ou16_NumberBits     Number of bits
   \param[in]      oq_IsEds            Flag if current file is an EDS file
   \param[out]     orq_DefaultMinMax   Flag if default values min max values are set or object specific limits are set

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_CONFIG Operation failure: Element invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportEdsDcf::mh_CalcMinMaxInit(const C_OscCanOpenObject * const opc_CoSignalObject,
                                             C_OscNodeDataPoolListElement & orc_Element, const uint16_t ou16_NumberBits,
                                             const bool oq_IsEds, bool & orq_DefaultMinMax)
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;

   orq_DefaultMinMax = true;

   //Check type is realistic for number of bits
   switch (orc_Element.GetType())
   {
   case C_OscNodeDataPoolContent::eUINT8:
      if (ou16_NumberBits > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if (ou16_NumberBits > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if (ou16_NumberBits > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      if (ou16_NumberBits > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      if (ou16_NumberBits > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      if (ou16_NumberBits > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      if (ou16_NumberBits > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      if (ou16_NumberBits > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      if (ou16_NumberBits > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
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
      uint64_t u64_LowLimit = 0U;
      int64_t s64_LowLimit = 0U;
      uint64_t u64_HighLimit = 0U;
      int64_t s64_HighLimit = 0U;
      uint64_t u64_DefaultValue = 0U;
      int64_t s64_DefaultValue = 0U;
      bool q_LowLimitSet = false;
      bool q_HighLimitSet = false;

      if (q_IsFloat == false)
      {
         bool q_DefaultValueSet = false;

         // Check for specific limits and check against the generic limits
         if (mh_Get64IntegerValue(opc_CoSignalObject->c_LowLimit, s64_LowLimit) == C_NO_ERR)
         {
            q_LowLimitSet = true;
         }
         if (mh_Get64IntegerValue(opc_CoSignalObject->c_HighLimit, s64_HighLimit) == C_NO_ERR)
         {
            q_HighLimitSet = true;
         }
         if (mh_Get64IntegerValue(h_GetCoObjectValue(*opc_CoSignalObject, oq_IsEds), s64_DefaultValue) == C_NO_ERR)
         {
            q_DefaultValueSet = true;
         }

         if (q_IsSigned == false)
         {
            if (q_LowLimitSet == true)
            {
               u64_LowLimit = static_cast<uint64_t>(s64_LowLimit);
            }
            if (q_HighLimitSet == true)
            {
               u64_HighLimit = static_cast<uint64_t>(s64_HighLimit);
            }
            if (q_DefaultValueSet == true)
            {
               u64_DefaultValue = static_cast<uint64_t>(s64_DefaultValue);
            }
         }
      }

      if (orc_Element.GetArray() == false)
      {
         //Unsigned
         if (q_IsUnsigned == true)
         {
            uint64_t u64_Max = 0U;
            uint64_t u64_Min = 0U;

            for (uint16_t u16_ItBit = 0U; u16_ItBit < ou16_NumberBits; ++u16_ItBit)
            {
               u64_Max += (static_cast<uint64_t>(1U) << u16_ItBit);
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
            case C_OscNodeDataPoolContent::eUINT8:
               orc_Element.c_MinValue.SetValueU8(static_cast<uint8_t>(u64_Min));
               orc_Element.c_MaxValue.SetValueU8(static_cast<uint8_t>(u64_Max));
               orc_Element.c_DataSetValues[0].SetValueU8(static_cast<uint8_t>(u64_DefaultValue));
               break;
            case C_OscNodeDataPoolContent::eUINT16:
               orc_Element.c_MinValue.SetValueU16(static_cast<uint16_t>(u64_Min));
               orc_Element.c_MaxValue.SetValueU16(static_cast<uint16_t>(u64_Max));
               orc_Element.c_DataSetValues[0].SetValueU16(static_cast<uint16_t>(u64_DefaultValue));
               break;
            case C_OscNodeDataPoolContent::eUINT32:
               orc_Element.c_MinValue.SetValueU32(static_cast<uint32_t>(u64_Min));
               orc_Element.c_MaxValue.SetValueU32(static_cast<uint32_t>(u64_Max));
               orc_Element.c_DataSetValues[0].SetValueU32(static_cast<uint32_t>(u64_DefaultValue));
               break;
            case C_OscNodeDataPoolContent::eUINT64:
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
            uint64_t u64_Max = 0U;
            int64_t s64_Max;
            int64_t s64_Min;

            //We need exactly one more than half of the unsigned maximum
            if (ou16_NumberBits > 0U)
            {
               u64_Max += (static_cast<uint64_t>(1U) << (ou16_NumberBits - 1U));
            }
            // Get the correct signed minimum value
            s64_Min = -static_cast<int64_t>(u64_Max);
            // Adapt the max due to signed
            --u64_Max;
            s64_Max = static_cast<int64_t>(u64_Max);

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
            case C_OscNodeDataPoolContent::eSINT8:
               orc_Element.c_MinValue.SetValueS8(static_cast<int8_t>(s64_Min));
               orc_Element.c_MaxValue.SetValueS8(static_cast<int8_t>(s64_Max));
               orc_Element.c_DataSetValues[0].SetValueS8(static_cast<int8_t>(s64_DefaultValue));
               break;
            case C_OscNodeDataPoolContent::eSINT16:
               orc_Element.c_MinValue.SetValueS16(static_cast<int16_t>(s64_Min));
               orc_Element.c_MaxValue.SetValueS16(static_cast<int16_t>(s64_Max));
               orc_Element.c_DataSetValues[0].SetValueS16(static_cast<int16_t>(s64_DefaultValue));
               break;
            case C_OscNodeDataPoolContent::eSINT32:
               orc_Element.c_MinValue.SetValueS32(static_cast<int32_t>(s64_Min));
               orc_Element.c_MaxValue.SetValueS32(static_cast<int32_t>(s64_Max));
               orc_Element.c_DataSetValues[0].SetValueS32(static_cast<int32_t>(s64_DefaultValue));
               break;
            case C_OscNodeDataPoolContent::eSINT64:
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
            case C_OscNodeDataPoolContent::eFLOAT32:
               orc_Element.c_MinValue.SetValueF32(-std::numeric_limits<float32_t>::max());
               orc_Element.c_MaxValue.SetValueF32(std::numeric_limits<float32_t>::max());
               orc_Element.c_DataSetValues[0].SetValueF32(0.0F);
               break;
            case C_OscNodeDataPoolContent::eFLOAT64:
               orc_Element.c_MinValue.SetValueF64(-std::numeric_limits<float64_t>::max());
               orc_Element.c_MaxValue.SetValueF64(std::numeric_limits<float64_t>::max());
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
void C_OscImportEdsDcf::mh_LoadDummies(const C_SclString & orc_FilePath, std::vector<uint32_t> & orc_Dummies)
{
   try
   {
      C_SclStringList c_StringList;
      C_SclIniFile c_Ini(orc_FilePath);
      c_Ini.ReadSection("DummyUsage", &c_StringList);
      if (c_StringList.GetCount() > 0UL)
      {
         //1B is last supported data type
         for (uint32_t u32_ItPossibleDummy = 1U; u32_ItPossibleDummy <= 0x1B; ++u32_ItPossibleDummy)
         {
            std::stringstream c_Stream;
            C_SclString c_CurString;
            c_Stream << std::setw(4) << std::setfill('0') << &std::hex << u32_ItPossibleDummy;
            c_CurString = static_cast<C_SclString>("Dummy") + c_Stream.str().c_str();
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
