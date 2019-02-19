//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility functions for system definition (implementation)

   Utility functions for system definition

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_scl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Check name not used in existing items

   \param[in] orc_ExistingStrings Existing item names
   \param[in] orc_ProposedName    Proposal for item name
   \param[in] opu32_SkipIndex     Optional parameter to skip one index
                                  (Use-case: skip current item to avoid conflict with itself)

   \return
   True  Name available
   False Name already in use

   \created     06.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to convert bus type to string (uppercase only)

   \param[in] ore_Type Bus type

   \return
   String for bus type

   \created     03.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to convert protocol type to string

   \param[in] ore_Type   Protocol type

   \return
   String for protocol type

   \created     09.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to convert protocol type to string

   \param[in] oe_Type Protocol type

   \return
   String for protocol type datapool name

   \created     12.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to convert datapool type to string

   \param[in] ore_Type   Datapool type

   \return
   String for protocol type

   \created     18.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
   case C_OSCNodeDataPool::eDIAG: // default case
   default:
      c_Name = "DIAG";
      break;
   }

   return c_Name;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get/create interface name

   \param[in] oe_Type             Bus type
   \param[in] ou8_InterfaceNumber Interface index

   \return
   Interface name

   \created     10.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
   c_Retval = QString("%1%2").arg(c_Type).arg(static_cast<sintn>(ou8_InterfaceNumber) + 1);
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert com datapool element index to message ID

   \param[in]  orc_ElementID     Datapool element ID
   \param[out] orc_MessageID     Message ID
   \param[out] oru32_SignalIndex Signal index

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid

   \created     28.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdUtil::h_ConvertIndex(const C_OSCNodeDataPoolListElementId & orc_ElementID,
                                   C_OSCCanMessageIdentificationIndices & orc_MessageID, uint32 & oru32_SignalIndex)
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_ElementID.u32_NodeIndex);

   //Node
   orc_MessageID.u32_NodeIndex = orc_ElementID.u32_NodeIndex;
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
                                                                            orc_MessageID.u32_InterfaceIndex);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Convert Signal index to datapool index

   \param[in]  orc_MessageID    Message ID
   \param[in]  ou32_SignalIndex Signal index in message
   \param[out] orc_ElementID    According element ID if any

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     08.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdUtil::h_ConvertFromSignalIndex(const C_OSCCanMessageIdentificationIndices & orc_MessageID,
                                             const uint32 ou32_SignalIndex,
                                             C_OSCNodeDataPoolListElementId & orc_ElementID)
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCCanProtocol * const pc_Protocol = C_PuiSdHandler::h_GetInstance()->GetCanProtocol(
      orc_MessageID.u32_NodeIndex, orc_MessageID.e_ComProtocol);
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPool(
      orc_MessageID.u32_NodeIndex, orc_MessageID.e_ComProtocol);

   //Node
   orc_ElementID.u32_NodeIndex = orc_MessageID.u32_NodeIndex;
   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageID);
      //Datapool
      orc_ElementID.u32_DataPoolIndex = pc_Protocol->u32_DataPoolIndex;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdUtil::C_PuiSdUtil(void)
{
}
