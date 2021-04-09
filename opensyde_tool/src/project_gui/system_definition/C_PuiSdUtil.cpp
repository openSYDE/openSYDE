//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system definition (implementation)

   Utility functions for system definition

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_OSCHALCMagicianUtil.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check name not used in existing items

   \param[in]  orc_ExistingStrings  Existing item names
   \param[in]  orc_Proposal         Proposal for item name
   \param[in]  opu32_SkipIndex      Optional parameter to skip one index
                                    (Use-case: skip current item to avoid conflict with itself)

   \return
   True  Name available
   False Name already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdUtil::h_CheckNameAvailable(const std::vector<const QString *> & orc_ExistingStrings,
                                       const QString & orc_Proposal, const uint32 * const opu32_SkipIndex)
{
   bool q_Retval = true;

   for (uint32 u32_ItString = 0; u32_ItString < orc_ExistingStrings.size(); ++u32_ItString)
   {
      bool q_Skip = false;
      if (opu32_SkipIndex != NULL)
      {
         if (*opu32_SkipIndex == u32_ItString)
         {
            q_Skip = true;
         }
      }
      if (q_Skip == false)
      {
         const QString * const pc_CurString = orc_ExistingStrings[u32_ItString];
         if (pc_CurString != NULL)
         {
            if (pc_CurString->compare(orc_Proposal) == 0)
            {
               q_Retval = false;
               break;
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to convert bus type to string (uppercase only)

   \param[in]  ore_Type    Bus type

   \return
   String for bus type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_ConvertBusTypeToStringUppercase(const C_OSCSystemBus::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case stw_opensyde_core::C_OSCSystemBus::eCAN:
      c_Retval = C_GtGetText::h_GetText("CAN");
      break;
   case stw_opensyde_core::C_OSCSystemBus::eETHERNET:
      c_Retval = C_GtGetText::h_GetText("ETHERNET");
      break;
   default:
      //Does not exist yet
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to convert protocol type to string

   \param[in]  ore_Type    Protocol type

   \return
   String for protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_ConvertProtocolTypeToString(const C_OSCCanProtocol::E_Type & ore_Type)
{
   QString c_ProtocolName;

   switch (ore_Type)
   {
   case C_OSCCanProtocol::eECES:
      c_ProtocolName = "ECeS";
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      c_ProtocolName = "ECoS";
      break;
   case C_OSCCanProtocol::eLAYER2: // default case
   default:
      c_ProtocolName = "OSI Layer 2";
      break;
   }

   return c_ProtocolName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to convert protocol type to string

   \param[in]  oe_Type  Protocol type

   \return
   String for protocol type datapool name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_ConvertProtocolTypeToDatapoolNameString(const C_OSCCanProtocol::E_Type oe_Type)
{
   QString c_ProtocolName;

   switch (oe_Type)
   {
   case C_OSCCanProtocol::eECES:
      c_ProtocolName = "ECeS";
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      c_ProtocolName = "ECoS";
      break;
   case C_OSCCanProtocol::eLAYER2: // default case
   default:
      // special case layer 2 -> no spaces and no underscore number at the end
      c_ProtocolName = "OSI_Layer2";
      break;
   }

   return c_ProtocolName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to convert datapool type to string

   \param[in]  ore_Type    Datapool type

   \return
   String for protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_ConvertDataPoolTypeToString(const C_OSCNodeDataPool::E_Type & ore_Type)
{
   QString c_Name;

   switch (ore_Type)
   {
   case C_OSCNodeDataPool::eCOM:
      c_Name = "COMM";
      break;
   case C_OSCNodeDataPool::eNVM:
      c_Name = "NVM";
      break;
   case C_OSCNodeDataPool::eHALC:
      c_Name = "HAL";
      break;
   case C_OSCNodeDataPool::eDIAG: // default case
   default:
      c_Name = "DIAG";
      break;
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get/create interface name

   \param[in]  oe_Type              Bus type
   \param[in]  ou8_InterfaceNumber  Interface index

   \return
   Interface name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetInterfaceName(const C_OSCSystemBus::E_Type oe_Type, const uint8 ou8_InterfaceNumber)
{
   QString c_Retval;
   QString c_Type;

   switch (oe_Type)
   {
   case C_OSCSystemBus::eCAN:
      c_Type = C_GtGetText::h_GetText("CAN");
      break;
   case C_OSCSystemBus::eETHERNET:
      c_Type = C_GtGetText::h_GetText("ETHERNET");
      break;
   }
   c_Retval = static_cast<QString>("%1%2").arg(c_Type).arg(static_cast<sintn>(ou8_InterfaceNumber) + 1);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get related COMM protocol of a datapool.

   Indices must belong to a COMM datapool! Asserts else.

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DatapoolIndex   Datapool index

   \return
   protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanProtocol::E_Type C_PuiSdUtil::h_GetRelatedCANProtocolType(const uint32 ou32_NodeIndex,
                                                                  const uint32 ou32_DatapoolIndex)
{
   C_OSCCanProtocol::E_Type e_Return = C_OSCCanProtocol::eLAYER2;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetRelatedCANProtocolConst(ou32_DatapoolIndex);
      tgl_assert(pc_Protocol != NULL);
      if (pc_Protocol != NULL)
      {
         e_Return = pc_Protocol->e_Type;
      }
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert com datapool element index to message ID

   \param[in]   orc_ElementID       Datapool element ID
   \param[out]  orc_MessageID       Message ID
   \param[out]  oru32_SignalIndex   Signal index

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdUtil::h_ConvertIndex(const C_OSCNodeDataPoolListElementId & orc_ElementID,
                                   C_OSCCanMessageIdentificationIndices & orc_MessageID, uint32 & oru32_SignalIndex)
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_ElementID.u32_NodeIndex);

   //Node
   orc_MessageID.u32_NodeIndex = orc_ElementID.u32_NodeIndex;
   orc_MessageID.u32_DatapoolIndex = orc_ElementID.u32_DataPoolIndex;
   if (pc_Node != NULL)
   {
      bool q_Found = false;
      //Protocol
      for (uint32 u32_ItProtocol = 0UL; u32_ItProtocol < pc_Node->c_ComProtocols.size(); ++u32_ItProtocol)
      {
         const C_OSCCanProtocol & rc_CurProtocol = pc_Node->c_ComProtocols[u32_ItProtocol];
         if (rc_CurProtocol.u32_DataPoolIndex == orc_ElementID.u32_DataPoolIndex)
         {
            q_Found = true;
            orc_MessageID.e_ComProtocol = rc_CurProtocol.e_Type;
            orc_MessageID.u32_InterfaceIndex = orc_ElementID.u32_ListIndex / 2;
         }
      }
      if (q_Found == true)
      {
         const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            orc_ElementID.u32_NodeIndex, orc_ElementID.u32_DataPoolIndex, orc_ElementID.u32_ListIndex);
         const C_OSCCanMessageContainer * const pc_Container =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageID.u32_NodeIndex,
                                                                            orc_MessageID.e_ComProtocol,
                                                                            orc_MessageID.u32_InterfaceIndex,
                                                                            orc_MessageID.u32_DatapoolIndex);
         if ((pc_Container != NULL) && (pc_List != NULL))
         {
            const std::vector<C_OSCCanMessage> * pc_Messages;

            if (C_OSCCanProtocol::h_ListIsComTx(*pc_List) == true)
            {
               orc_MessageID.q_MessageIsTx = true;
               pc_Messages = &pc_Container->c_TxMessages;
            }
            else
            {
               orc_MessageID.q_MessageIsTx = false;
               pc_Messages = &pc_Container->c_RxMessages;
            }
            for (uint32 u32_ItMessage = 0UL; u32_ItMessage < pc_Messages->size(); ++u32_ItMessage)
            {
               const C_OSCCanMessage & rc_CurMessage = (*pc_Messages)[u32_ItMessage];
               for (uint32 u32_ItSignal = 0UL; u32_ItSignal < rc_CurMessage.c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OSCCanSignal & rc_Signal = rc_CurMessage.c_Signals[u32_ItSignal];
                  if (rc_Signal.u32_ComDataElementIndex == orc_ElementID.u32_ElementIndex)
                  {
                     orc_MessageID.u32_MessageIndex = u32_ItMessage;
                     oru32_SignalIndex = u32_ItSignal;
                     s32_Retval = C_NO_ERR;
                  }
               }
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert Signal index to datapool index

   \param[in]   orc_MessageID       Message ID
   \param[in]   ou32_SignalIndex    Signal index in message
   \param[out]  orc_ElementID       According element ID if any

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdUtil::h_ConvertFromSignalIndex(const C_OSCCanMessageIdentificationIndices & orc_MessageID,
                                             const uint32 ou32_SignalIndex,
                                             C_OSCNodeDataPoolListElementId & orc_ElementID)
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPool(
      orc_MessageID.u32_NodeIndex, orc_MessageID.e_ComProtocol, orc_MessageID.u32_DatapoolIndex);

   //Node
   orc_ElementID.u32_NodeIndex = orc_MessageID.u32_NodeIndex;
   if (pc_DataPool != NULL)
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageID);
      //Datapool
      orc_ElementID.u32_DataPoolIndex = orc_MessageID.u32_DatapoolIndex;
      //List
      if (((C_OSCCanProtocol::h_GetComListIndex(*pc_DataPool, orc_MessageID.u32_InterfaceIndex,
                                                orc_MessageID.q_MessageIsTx,
                                                orc_ElementID.u32_ListIndex) == C_NO_ERR) && (pc_Message != NULL)) &&
          (ou32_SignalIndex < pc_Message->c_Signals.size()))
      {
         const C_OSCCanSignal & rc_Signal = pc_Message->c_Signals[ou32_SignalIndex];
         //Element
         orc_ElementID.u32_ElementIndex = rc_Signal.u32_ComDataElementIndex;
         //Signal success
         s32_Retval = C_NO_ERR;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the default cycle time of a CAN message

   \return
   Default message cycle time
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdUtil::h_GetDefaultMessageCycleTime(void)
{
   return 100U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the automatic timeout time of a CAN message depending of the cylce time

   The time is (3 * cycle time) + 10

   \param[in]  ou32_CycleTime    Cycle time of CAN message

   \return
   Calculated timeout time
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdUtil::h_GetMessageAutoTimeoutTime(const uint32 ou32_CycleTime)
{
   return (ou32_CycleTime * 3U) + 10U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get namespace for ID

   \param[in]  orc_Id   Datapool element ID

   \return
   Namespace node::datapool::list::element resp. node::datapool::message::signal
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetNamespace(const C_OSCNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;

   const C_OSCNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_Id.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_DataPool =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_Id.u32_NodeIndex,
                                                      orc_Id.u32_DataPoolIndex);
   const C_OSCNodeDataPoolList * const pc_List =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(orc_Id.u32_NodeIndex,
                                                          orc_Id.u32_DataPoolIndex,
                                                          orc_Id.u32_ListIndex);
   const C_OSCNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_Id.u32_NodeIndex,
                                                                 orc_Id.u32_DataPoolIndex,
                                                                 orc_Id.u32_ListIndex,
                                                                 orc_Id.u32_ElementIndex);

   if ((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Element != NULL))
   {
      // special handling for signals: instead of list better show message name
      if (pc_DataPool->e_Type == C_OSCNodeDataPool::eCOM)
      {
         QString c_ElementName = pc_Element->c_Name.c_str(); // to have an adequate default
         C_OSCCanMessageIdentificationIndices c_MessageID;
         uint32 u32_SignalIndex;

         if (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageID, u32_SignalIndex) == C_NO_ERR)
         {
            const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageID);
            if (pc_Message != NULL)
            {
               c_ElementName = static_cast<QString>("%1::%2").arg(pc_Message->c_Name.c_str()).arg(
                  pc_Element->c_Name.c_str());
            }
         }
         c_Retval = static_cast<QString>("%1::%2::%3").
                    arg(pc_Node->c_Properties.c_Name.c_str()).
                    arg(pc_DataPool->c_Name.c_str()).
                    arg(c_ElementName);
      }
      else
      {
         c_Retval = static_cast<QString>("%1::%2::%3::%4").
                    arg(pc_Node->c_Properties.c_Name.c_str()).
                    arg(pc_DataPool->c_Name.c_str()).
                    arg(pc_List->c_Name.c_str()).
                    arg(pc_Element->c_Name.c_str());
         // HALC extra handling not possible if only C_OSCNodeDataPoolListElementId is provided
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get namespace for signal ID

   \param[in]  orc_Id   Signal ID

   \return
   Namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetSignalNamespace(const C_OSCNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;
   C_OSCCanMessageIdentificationIndices c_MessageID;
   uint32 u32_SignalIndex;

   if (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageID, u32_SignalIndex) == C_NO_ERR)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_MessageID.u32_NodeIndex);
      if ((pc_Node != NULL) && (c_MessageID.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
      {
         const C_OSCNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[c_MessageID.u32_InterfaceIndex];
         if (rc_Interface.q_IsBusConnected == true)
         {
            const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(rc_Interface.u32_BusIndex);
            const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageID);
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_Id.u32_NodeIndex,
                                                                          orc_Id.u32_DataPoolIndex,
                                                                          orc_Id.u32_ListIndex,
                                                                          orc_Id.u32_ElementIndex);

            if (((pc_Bus != NULL) && (pc_Message != NULL)) && (pc_Element != NULL))
            {
               c_Retval =
                  static_cast<QString>("%1::%2::%3").arg(pc_Bus->c_Name.c_str()).arg(pc_Message->c_Name.c_str()).arg(
                     pc_Element->c_Name.c_str());
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC namespace from Id

   \param[in]  orc_Id   Datapool element ID

   \return
   HALC namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetHALCNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval = C_GtGetText::h_GetText("Unknown HAL data element");
   uint32 u32_DomainIndex;
   bool q_UseChannelIndex;
   uint32 u32_ChannelIndex;

   C_OSCHalcDefDomain::E_VariableSelector e_Selector;
   uint32 u32_ParameterIndex;
   bool q_UseElementIndex;
   uint32 u32_ParameterElementIndex;
   bool q_IsUseCaseIndex;
   bool q_IsChanNumIndex;

   if (C_PuiSdHandler::h_GetInstance()->TranslateToHALCIndex(orc_Id, orc_Id.GetArrayElementIndexOrZero(),
                                                             u32_DomainIndex, q_UseChannelIndex,
                                                             u32_ChannelIndex, e_Selector, u32_ParameterIndex,
                                                             q_UseElementIndex,
                                                             u32_ParameterElementIndex, q_IsUseCaseIndex,
                                                             q_IsChanNumIndex) == C_NO_ERR)
   {
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_Id.u32_NodeIndex);
         const C_OSCNodeDataPool * const pc_DataPool =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_Id.u32_NodeIndex,
                                                            orc_Id.u32_DataPoolIndex);
         const C_OSCNodeDataPoolList * const pc_List =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(orc_Id.u32_NodeIndex,
                                                                orc_Id.u32_DataPoolIndex,
                                                                orc_Id.u32_ListIndex);
         const C_OSCHalcConfigChannel * const pc_Config =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(orc_Id.u32_NodeIndex,
                                                                            u32_DomainIndex,
                                                                            u32_ChannelIndex,
                                                                            q_UseChannelIndex);
         const C_OSCHalcDefDomain * const pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileDataConst(orc_Id.u32_NodeIndex,
                                                                        u32_DomainIndex);

         if (((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Config != NULL)) &&
             (pc_Domain != NULL))
         {
            QString c_ElementName;

            if (q_IsUseCaseIndex)
            {
               c_ElementName = C_OSCHALCMagicianUtil::h_GetUseCaseVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsChanNumIndex)
            {
               c_ElementName = C_OSCHALCMagicianUtil::h_GetChanNumVariableName(pc_Domain->c_SingularName).c_str();
            }
            else
            {
               const C_OSCHalcDefStruct * const pc_Param =
                  C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileVariableData(orc_Id.u32_NodeIndex,
                                                                                 u32_DomainIndex, e_Selector,
                                                                                 u32_ParameterIndex);
               c_ElementName = pc_Param->c_Display.c_str();
               if (q_UseElementIndex)
               {
                  if (u32_ParameterElementIndex < pc_Param->c_StructElements.size())
                  {
                     const C_OSCHalcDefElement & rc_Param = pc_Param->c_StructElements[u32_ParameterElementIndex];
                     c_ElementName = rc_Param.c_Display.c_str();
                  }
               }
            }
            c_Retval = static_cast<QString>("%1::%2::%3::%4::%5").
                       arg(pc_Node->c_Properties.c_Name.c_str()).
                       arg(pc_DataPool->c_Name.c_str()).
                       arg(pc_Config->c_Name.c_str()).
                       arg(pc_List->c_Name.c_str()).
                       arg(c_ElementName);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdUtil::C_PuiSdUtil(void)
{
}
