//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system definition (implementation)

   Utility functions for system definition

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OscHalcMagicianUtil.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::scl;

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
                                       const QString & orc_Proposal, const uint32_t * const opu32_SkipIndex)
{
   bool q_Retval = true;

   for (uint32_t u32_ItString = 0; u32_ItString < orc_ExistingStrings.size(); ++u32_ItString)
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
QString C_PuiSdUtil::h_ConvertBusTypeToStringUppercase(const C_OscSystemBus::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case stw::opensyde_core::C_OscSystemBus::eCAN:
      c_Retval = C_GtGetText::h_GetText("CAN");
      break;
   case stw::opensyde_core::C_OscSystemBus::eETHERNET:
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
QString C_PuiSdUtil::h_ConvertProtocolTypeToString(const C_OscCanProtocol::E_Type & ore_Type)
{
   QString c_ProtocolName;

   switch (ore_Type)
   {
   case C_OscCanProtocol::eECES:
      c_ProtocolName = "ECeS";
      break;
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
      c_ProtocolName = "ECoS";
      break;
   case C_OscCanProtocol::eCAN_OPEN:
      c_ProtocolName = "CANopen";
      break;
   case C_OscCanProtocol::eJ1939:
      c_ProtocolName = "J1939";
      break;
   case C_OscCanProtocol::eLAYER2: // default case
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
QString C_PuiSdUtil::h_ConvertProtocolTypeToDatapoolNameString(const C_OscCanProtocol::E_Type oe_Type)
{
   QString c_ProtocolName;

   switch (oe_Type)
   {
   case C_OscCanProtocol::eECES:
      c_ProtocolName = "ECeS";
      break;
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
      c_ProtocolName = "ECoS";
      break;
   case C_OscCanProtocol::eCAN_OPEN:
      c_ProtocolName = "CANopen";
      break;
   case C_OscCanProtocol::eJ1939:
      c_ProtocolName = "J1939";
      break;
   case C_OscCanProtocol::eLAYER2: // default case
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
QString C_PuiSdUtil::h_ConvertDataPoolTypeToString(const C_OscNodeDataPool::E_Type & ore_Type)
{
   QString c_Name;

   switch (ore_Type)
   {
   case C_OscNodeDataPool::eCOM:
      c_Name = "COMM";
      break;
   case C_OscNodeDataPool::eNVM:
      c_Name = "NVM";
      break;
   case C_OscNodeDataPool::eHALC:     // For user is no difference between HALC and HALC_NVM Datapools
   case C_OscNodeDataPool::eHALC_NVM: // For user is no difference between HALC and HALC_NVM Datapools
      c_Name = "HAL";
      break;
   case C_OscNodeDataPool::eDIAG: // default case
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
QString C_PuiSdUtil::h_GetInterfaceName(const C_OscSystemBus::E_Type oe_Type, const uint8_t ou8_InterfaceNumber)
{
   QString c_Retval;
   QString c_Type;

   switch (oe_Type)
   {
   case C_OscSystemBus::eCAN:
      c_Type = C_GtGetText::h_GetText("CAN");
      break;
   case C_OscSystemBus::eETHERNET:
      c_Type = C_GtGetText::h_GetText("ETHERNET");
      break;
   default:
      tgl_assert(false);
      break;
   }

   c_Retval = static_cast<QString>("%1%2").arg(c_Type).arg(static_cast<int32_t>(ou8_InterfaceNumber) + 1);
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
C_OscCanProtocol::E_Type C_PuiSdUtil::h_GetRelatedCanProtocolType(const uint32_t ou32_NodeIndex,
                                                                  const uint32_t ou32_DatapoolIndex)
{
   C_OscCanProtocol::E_Type e_Return = C_OscCanProtocol::eLAYER2;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      const C_OscCanProtocol * const pc_Protocol = pc_Node->GetRelatedCanProtocolConst(ou32_DatapoolIndex);
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

   \param[in]   orc_ElementId       Datapool element ID
   \param[out]  orc_MessageId       Message ID
   \param[out]  oru32_SignalIndex   Signal index

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdUtil::h_ConvertIndex(const C_OscNodeDataPoolListElementId & orc_ElementId,
                                    C_OscCanMessageIdentificationIndices & orc_MessageId, uint32_t & oru32_SignalIndex)
{
   int32_t s32_Retval = C_RANGE;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_ElementId.u32_NodeIndex);

   //Node
   orc_MessageId.u32_NodeIndex = orc_ElementId.u32_NodeIndex;
   orc_MessageId.u32_DatapoolIndex = orc_ElementId.u32_DataPoolIndex;
   if (pc_Node != NULL)
   {
      bool q_Found = false;
      //Protocol
      for (uint32_t u32_ItProtocol = 0UL; u32_ItProtocol < pc_Node->c_ComProtocols.size(); ++u32_ItProtocol)
      {
         const C_OscCanProtocol & rc_CurProtocol = pc_Node->c_ComProtocols[u32_ItProtocol];
         if (rc_CurProtocol.u32_DataPoolIndex == orc_ElementId.u32_DataPoolIndex)
         {
            q_Found = true;
            orc_MessageId.e_ComProtocol = rc_CurProtocol.e_Type;
            orc_MessageId.u32_InterfaceIndex = orc_ElementId.u32_ListIndex / 2;
         }
      }
      if (q_Found == true)
      {
         const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
            orc_ElementId.u32_NodeIndex, orc_ElementId.u32_DataPoolIndex, orc_ElementId.u32_ListIndex);
         const C_OscCanMessageContainer * const pc_Container =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId.u32_NodeIndex,
                                                                            orc_MessageId.e_ComProtocol,
                                                                            orc_MessageId.u32_InterfaceIndex,
                                                                            orc_MessageId.u32_DatapoolIndex);
         if ((pc_Container != NULL) && (pc_List != NULL))
         {
            const std::vector<C_OscCanMessage> * pc_Messages;

            if (C_OscCanProtocol::h_ListIsComTx(*pc_List) == true)
            {
               orc_MessageId.q_MessageIsTx = true;
               pc_Messages = &pc_Container->c_TxMessages;
            }
            else
            {
               orc_MessageId.q_MessageIsTx = false;
               pc_Messages = &pc_Container->c_RxMessages;
            }
            for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < pc_Messages->size(); ++u32_ItMessage)
            {
               const C_OscCanMessage & rc_CurMessage = (*pc_Messages)[u32_ItMessage];
               for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < rc_CurMessage.c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OscCanSignal & rc_Signal = rc_CurMessage.c_Signals[u32_ItSignal];
                  if (rc_Signal.u32_ComDataElementIndex == orc_ElementId.u32_ElementIndex)
                  {
                     orc_MessageId.u32_MessageIndex = u32_ItMessage;
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

   \param[in]   orc_MessageId       Message ID
   \param[in]   ou32_SignalIndex    Signal index in message
   \param[out]  orc_ElementId       According element ID if any

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdUtil::h_ConvertFromSignalIndex(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                              const uint32_t ou32_SignalIndex,
                                              C_OscNodeDataPoolListElementId & orc_ElementId)
{
   int32_t s32_Retval = C_RANGE;
   const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPool(
      orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol, orc_MessageId.u32_DatapoolIndex);

   //Node
   orc_ElementId.u32_NodeIndex = orc_MessageId.u32_NodeIndex;
   if (pc_DataPool != NULL)
   {
      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
      //Datapool
      orc_ElementId.u32_DataPoolIndex = orc_MessageId.u32_DatapoolIndex;
      //List
      if (((C_OscCanProtocol::h_GetComListIndex(*pc_DataPool, orc_MessageId.u32_InterfaceIndex,
                                                orc_MessageId.q_MessageIsTx,
                                                orc_ElementId.u32_ListIndex) == C_NO_ERR) && (pc_Message != NULL)) &&
          (ou32_SignalIndex < pc_Message->c_Signals.size()))
      {
         const C_OscCanSignal & rc_Signal = pc_Message->c_Signals[ou32_SignalIndex];
         //Element
         orc_ElementId.u32_ElementIndex = rc_Signal.u32_ComDataElementIndex;
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
uint32_t C_PuiSdUtil::h_GetDefaultMessageCycleTime(void)
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
uint32_t C_PuiSdUtil::h_GetMessageAutoTimeoutTime(const uint32_t ou32_CycleTime)
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
QString C_PuiSdUtil::h_GetNamespace(const C_OscNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;

   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_Id.u32_NodeIndex);
   const C_OscNodeDataPool * const pc_DataPool =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_Id.u32_NodeIndex,
                                                      orc_Id.u32_DataPoolIndex);
   const C_OscNodeDataPoolList * const pc_List =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(orc_Id.u32_NodeIndex,
                                                          orc_Id.u32_DataPoolIndex,
                                                          orc_Id.u32_ListIndex);
   const C_OscNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id.u32_NodeIndex,
                                                                 orc_Id.u32_DataPoolIndex,
                                                                 orc_Id.u32_ListIndex,
                                                                 orc_Id.u32_ElementIndex);

   if ((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Element != NULL))
   {
      // special handling for signals: instead of list better show message name
      if (pc_DataPool->e_Type == C_OscNodeDataPool::eCOM)
      {
         QString c_ElementName = pc_Element->c_Name.c_str(); // to have an adequate default
         C_OscCanMessageIdentificationIndices c_MessageId;
         uint32_t u32_SignalIndex;

         if (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageId, u32_SignalIndex) == C_NO_ERR)
         {
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
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
      } //lint !e438 //value of u32_SignalIndex not used; we just called the convert function to check whether the
        // parameters are valid
      else
      {
         c_Retval = static_cast<QString>("%1::%2::%3::%4").
                    arg(pc_Node->c_Properties.c_Name.c_str()).
                    arg(pc_DataPool->c_Name.c_str()).
                    arg(pc_List->c_Name.c_str()).
                    arg(pc_Element->c_Name.c_str());
         // HALC extra handling not possible if only C_OscNodeDataPoolListElementId is provided
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
QString C_PuiSdUtil::h_GetSignalNamespace(const C_OscNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;
   C_OscCanMessageIdentificationIndices c_MessageId;
   uint32_t u32_SignalIndex;

   if (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageId, u32_SignalIndex) == C_NO_ERR)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_MessageId.u32_NodeIndex);
      if ((pc_Node != NULL) && (c_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
      {
         const C_OscNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[c_MessageId.u32_InterfaceIndex];
         if (rc_Interface.GetBusConnected() == true)
         {
            const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(rc_Interface.u32_BusIndex);
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id.u32_NodeIndex,
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
   return c_Retval; //lint !e438 //value of u32_SignalIndex not used; we just called the convert function to check
                    // whether the parameters are valid
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC namespace from Id

   \param[in]  orc_Id   Datapool element ID

   \return
   HALC namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetHalcNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval = C_GtGetText::h_GetText("Unknown HAL data element");
   uint32_t u32_DomainIndex;
   bool q_UseChannelIndex;
   uint32_t u32_ChannelIndex;

   C_OscHalcDefDomain::E_VariableSelector e_Selector;
   uint32_t u32_ParameterIndex;
   bool q_UseElementIndex;
   uint32_t u32_ParameterElementIndex;
   bool q_IsUseCaseIndex;
   bool q_IsChanNumIndex;
   bool q_IsSafetyFlagIndex;

   if (C_PuiSdHandler::h_GetInstance()->TranslateToHalcIndex(orc_Id, orc_Id.GetArrayElementIndexOrZero(),
                                                             u32_DomainIndex, q_UseChannelIndex,
                                                             u32_ChannelIndex, e_Selector, u32_ParameterIndex,
                                                             q_UseElementIndex,
                                                             u32_ParameterElementIndex, q_IsUseCaseIndex,
                                                             q_IsChanNumIndex, q_IsSafetyFlagIndex) == C_NO_ERR)
   {
      {
         const C_OscNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_Id.u32_NodeIndex);
         const C_OscNodeDataPool * const pc_DataPool =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_Id.u32_NodeIndex,
                                                            orc_Id.u32_DataPoolIndex);
         const C_OscNodeDataPoolList * const pc_List =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(orc_Id.u32_NodeIndex,
                                                                orc_Id.u32_DataPoolIndex,
                                                                orc_Id.u32_ListIndex);
         const C_OscHalcConfigChannel * const pc_Config =
            C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(orc_Id.u32_NodeIndex,
                                                                            u32_DomainIndex,
                                                                            u32_ChannelIndex,
                                                                            q_UseChannelIndex);
         const C_OscHalcDefDomain * const pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileDataConst(orc_Id.u32_NodeIndex,
                                                                        u32_DomainIndex);

         if (((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Config != NULL)) &&
             (pc_Domain != NULL))
         {
            QString c_ElementName;

            if (q_IsUseCaseIndex)
            {
               c_ElementName = C_OscHalcMagicianUtil::h_GetUseCaseVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsChanNumIndex)
            {
               c_ElementName = C_OscHalcMagicianUtil::h_GetChanNumVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsSafetyFlagIndex)
            {
               c_ElementName = C_OscHalcMagicianUtil::h_GetSafetyFlagVariableName(pc_Domain->c_SingularName).c_str();
            }
            else
            {
               const C_OscHalcDefStruct * const pc_Param =
                  C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileVariableData(orc_Id.u32_NodeIndex,
                                                                                 u32_DomainIndex, e_Selector,
                                                                                 u32_ParameterIndex);
               c_ElementName = pc_Param->c_Display.c_str();
               if (q_UseElementIndex)
               {
                  if (u32_ParameterElementIndex < pc_Param->c_StructElements.size())
                  {
                     const C_OscHalcDefElement & rc_Param = pc_Param->c_StructElements[u32_ParameterElementIndex];
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
/*! \brief  Get sub node name by node index.

   Make sure the node is a sub node and the device definition exists, else this will return an empty string.

   \param[in]  ou32_NodeIndex    Node index

   \return
   sub node name from sub device definition
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetSubNodeDeviceName(const uint32_t ou32_NodeIndex)
{
   QString c_Retval = "";
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      if (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size())
      {
         c_Retval =
            pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].c_SubDeviceName.c_str();
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node base name or name

   If the node belongs to a node squad, this will return the base name. Else it will simply return the node name.

   \param[in]  ou32_NodeIndex    Node index

   \return
   node base name if sub node, else node name (or empty string if index is invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdUtil::h_GetNodeBaseNameOrName(const uint32_t ou32_NodeIndex)
{
   QString c_Return = "";
   uint32_t u32_NodeSquadIndex;

   if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex,
                                                                       u32_NodeSquadIndex) == C_NO_ERR)
   {
      // part of a node squad
      const stw::opensyde_core::C_OscNodeSquad * const pc_NodeSquad =
         C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_NodeSquadIndex);
      if (pc_NodeSquad != NULL)
      {
         c_Return = pc_NodeSquad->c_BaseName.c_str();
      }
   }
   else
   {
      // normal node
      const stw::opensyde_core::C_OscNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         c_Return = pc_Node->c_Properties.c_Name.c_str();
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get index of first node in squad

   For node squads, this returns the index of the first node in the corresponding node squad.
   For normal nodes this just returns the given node index.
   This is useful because the index of the first node in a squad corresponds to the index on topology.

   \param[in]  ou32_NodeIndex    Node index

   \return
   index of first node in squad
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSdUtil::h_GetIndexOfFirstNodeInGroup(const uint32_t ou32_NodeIndex)
{
   uint32_t u32_FirstNodeIndex = ou32_NodeIndex;

   uint32_t u32_NodeSquadIndex;

   if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex,
                                                                       u32_NodeSquadIndex) == C_NO_ERR)
   {
      const stw::opensyde_core::C_OscNodeSquad * const pc_NodeSquad =
         C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_NodeSquadIndex);
      if ((pc_NodeSquad != NULL) && (pc_NodeSquad->c_SubNodeIndexes.size() > 0))
      {
         u32_FirstNodeIndex = pc_NodeSquad->c_SubNodeIndexes[0];
      }
   }

   return u32_FirstNodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check is first in any group or not in any

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  orc_AvailableGroups  Available groups

   \return
   Is first in any group or not in any
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(const uint32_t ou32_NodeIndex,
                                                     const std::vector<C_OscNodeSquad> & orc_AvailableGroups)
{
   bool q_IsFirst = true;

   for (uint32_t u32_ItGroup = 0; u32_ItGroup < orc_AvailableGroups.size(); ++u32_ItGroup)
   {
      const C_OscNodeSquad & rc_Group = orc_AvailableGroups[u32_ItGroup];

      for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < rc_Group.c_SubNodeIndexes.size(); ++u32_ItSubDevice)
      {
         if (ou32_NodeIndex == rc_Group.c_SubNodeIndexes[u32_ItSubDevice])
         {
            if (u32_ItSubDevice != 0UL)
            {
               q_IsFirst = false;
            }
            break;
         }
      }
   }
   return q_IsFirst;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interface data for node

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_ConnectionId    Connection id
   \param[in,out]  orc_Properties      Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdUtil::h_GetInterfaceDataForNode(const uint32_t ou32_NodeIndex,
                                            const C_PuiSdNodeConnectionId & orc_ConnectionId,
                                            std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   const std::vector<uint32_t> c_NodeIndices =
      C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   orc_Properties.clear();
   orc_Properties.reserve(c_NodeIndices.size());
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
   {
      const C_OscNode * const pc_NodeData =
         C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndices[u32_ItNode]);
      if (pc_NodeData != NULL)
      {
         for (uint32_t u32_ItComInterface =
                 0; u32_ItComInterface < pc_NodeData->c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            const C_OscNodeComInterfaceSettings & rc_ComInterface =
               pc_NodeData->c_Properties.c_ComInterfaces[u32_ItComInterface];
            if ((rc_ComInterface.u8_InterfaceNumber == orc_ConnectionId.u8_InterfaceNumber) &&
                (orc_ConnectionId.e_InterfaceType == rc_ComInterface.e_InterfaceType))
            {
               C_PuiSdNodeInterfaceAutomaticProperties c_Property;
               c_Property.c_Ip.reserve(4);
               for (uint32_t u32_It = 0UL; u32_It < 4; ++u32_It)
               {
                  c_Property.c_Ip.push_back(rc_ComInterface.c_Ip.au8_IpAddress[u32_It]);
               }
               c_Property.u8_NodeId = rc_ComInterface.u8_NodeId;
               orc_Properties.push_back(c_Property);
            }
         }
      }
   }
   tgl_assert(orc_Properties.size() == c_NodeIndices.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdUtil::C_PuiSdUtil(void)
{
}
