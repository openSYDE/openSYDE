//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition utility functions (implementation)

   System definition utility functions

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <set>
#include <limits>

#include <QElapsedTimer>

#include "stwerrors.h"
#include "constants.h"
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OgeRabProperties.h"
#include "C_PuiSdUtil.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "C_SdTooltipUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_SdNdeDataPoolUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdUtil::C_SdUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate node and interface names

   \param[in]  orc_NodeIndices      Node indices (match with interface indices expected)
   \param[in]  orc_InterfaceIndices Interface indices (match with node indices expected)
   \param[out] orc_Names            Generate node & interface names

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdUtil::h_GetNames(const std::vector<uint32> & orc_NodeIndices,
                            const std::vector<uint32> & orc_InterfaceIndices, std::vector<QString> & orc_Names)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_NodeIndices.size() == orc_InterfaceIndices.size())
   {
      QString c_TmpName;
      // set for all nodes with more than one interface
      std::set<uint32> c_SetNodesWithMultipleItf;

      // detect all nodes with more than one used interface
      if (orc_NodeIndices.size() > 1)
      {
         for (uint32 u32_NodeCounter = 0U; u32_NodeCounter < (orc_NodeIndices.size() - 1UL); ++u32_NodeCounter)
         {
            const uint32 u32_NextElement = u32_NodeCounter + 1U;
            if (orc_NodeIndices[u32_NodeCounter] == orc_NodeIndices[u32_NextElement])
            {
               c_SetNodesWithMultipleItf.insert(orc_NodeIndices[u32_NodeCounter]);
            }
         }
      }

      // get all the node names
      for (uint32 u32_NodeCounter = 0U; (u32_NodeCounter < orc_NodeIndices.size()) && (s32_Retval == C_NO_ERR);
           ++u32_NodeCounter)
      {
         if (c_SetNodesWithMultipleItf.find(orc_NodeIndices[u32_NodeCounter]) != c_SetNodesWithMultipleItf.end())
         {
            s32_Retval = h_GetName(orc_NodeIndices[u32_NodeCounter], orc_InterfaceIndices[u32_NodeCounter], c_TmpName);
         }
         else
         {
            s32_Retval = h_GetName(orc_NodeIndices[u32_NodeCounter], c_TmpName);
         }
         orc_Names.push_back(c_TmpName);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate node name

   \param[in]  orc_NodeIndices      Node index
   \param[out] orc_Names            Generate node & interface name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdUtil::h_GetName(const uint32 & oru32_NodeIndex, QString & orc_Name)
{
   sint32 s32_Retval = C_RANGE;

   const C_OSCNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      orc_Name = pc_Node->c_Properties.c_Name.c_str();
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate node and interface name

   \param[in]  orc_NodeIndices      Node index
   \param[in]  orc_InterfaceIndices Interface index
   \param[out] orc_Names            Generate node & interface name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdUtil::h_GetName(const uint32 & oru32_NodeIndex, const uint32 & oru32_InterfaceIndex, QString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      // check which interfaces are connected to the bus
      if (oru32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OSCNodeComInterfaceSettings & rc_ComInterface =
            pc_Node->c_Properties.c_ComInterfaces[oru32_InterfaceIndex];
         // create the shown string with the interface name
         orc_Name = pc_Node->c_Properties.c_Name.c_str();
         orc_Name += " (" +
                     C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN, rc_ComInterface.u8_InterfaceNumber) + ")";
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init icon image for all states

   \param[in] orc_File File to use as icon

   \return
   Static icon
*/
//----------------------------------------------------------------------------------------------------------------------
QIcon C_SdUtil::h_InitStaticIcon(const QString & orc_File)
{
   QIcon c_Retval;

   c_Retval.addFile(orc_File, QSize(), QIcon::Normal, QIcon::On);
   c_Retval.addFile(orc_File, QSize(), QIcon::Normal, QIcon::Off);
   c_Retval.addFile(orc_File, QSize(), QIcon::Disabled, QIcon::On);
   c_Retval.addFile(orc_File, QSize(), QIcon::Disabled, QIcon::Off);
   c_Retval.addFile(orc_File, QSize(), QIcon::Active, QIcon::On);
   c_Retval.addFile(orc_File, QSize(), QIcon::Active, QIcon::Off);
   c_Retval.addFile(orc_File, QSize(), QIcon::Selected, QIcon::On);
   c_Retval.addFile(orc_File, QSize(), QIcon::Selected, QIcon::Off);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init icon SVG image for all states

   \param[in] orc_File SVG file to use as icon
   \param[in] orc_Size Icon size

   \return
   Static icon
*/
//----------------------------------------------------------------------------------------------------------------------
QIcon C_SdUtil::h_InitStaticIconSvg(const QString & orc_File, const QSize & orc_Size)
{
   QIcon c_Retval;

   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Normal, QIcon::On);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Normal, QIcon::Off);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Disabled, QIcon::On);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Disabled, QIcon::Off);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Active, QIcon::On);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Active, QIcon::Off);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Selected, QIcon::On);
   c_Retval.addPixmap(QIcon(orc_File).pixmap(orc_Size), QIcon::Selected, QIcon::Off);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert tx method to name

   \param[in] ore_Type Tx method

   \return
   Name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_ConvertTxMethodToName(const C_OSCCanMessage::E_TxMethodType & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCCanMessage::eTX_METHOD_CYCLIC:
      c_Retval = C_GtGetText::h_GetText("Cyclic");
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_CHANGE:
      c_Retval = C_GtGetText::h_GetText("On Change");
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_EVENT:
      c_Retval = C_GtGetText::h_GetText("On Event");
      break;
   default:
      c_Retval = "";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert byte order to name

   \param[in] ore_Type Byte order

   \return
   Name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_ConvertByteOrderToName(const C_OSCCanSignal::E_ByteOrderType & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCCanSignal::eBYTE_ORDER_INTEL:
      c_Retval = C_GtGetText::h_GetText("Intel");
      break;
   case C_OSCCanSignal::eBYTE_ORDER_MOTOROLA:
      c_Retval = C_GtGetText::h_GetText("Motorola");
      break;
   default:
      c_Retval = "";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error tooltip suggestion

   \param[in]  ou32_NodeIndex    Node index (identifier)
   \param[in]  orc_Indices       Invalid datapool indices
   \param[in]  oq_NvmSizeInvalid Flag if NVM size invalid
   \param[out] orc_Heading       Heading suggestion
   \param[out] orc_Content       Content suggestion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_GetErrorToolTipDataPools(const uint32 ou32_NodeIndex, const std::vector<uint32> & orc_Indices,
                                          const bool oq_NvmSizeInvalid, QString & orc_Heading, QString & orc_Content)
{
   orc_Heading = C_GtGetText::h_GetText("Datapools with invalid content");
   orc_Content = "";
   if (oq_NvmSizeInvalid == true)
   {
      orc_Content += C_GtGetText::h_GetText("NVM Datapool sizes over node limit.\n");
   }
   for (uint32 u32_ItDataPool = 0;
        (u32_ItDataPool < orc_Indices.size()) && (u32_ItDataPool < mu32_TOOL_TIP_MAXIMUM_ITEMS);
        ++u32_ItDataPool)
   {
      const C_OSCNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(ou32_NodeIndex,
                                                                                                    orc_Indices[
                                                                                                       u32_ItDataPool]);
      if (pc_Datapool != NULL)
      {
         orc_Content += QString("%1\n").arg(pc_Datapool->c_Name.c_str());
      }
   }
   if (mu32_TOOL_TIP_MAXIMUM_ITEMS < orc_Indices.size())
   {
      orc_Content += QString("+%1\n").arg(orc_Indices.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort source vector descending ( Sorting steps are done for the target vector in sync)

   \param[in,out] orc_Source Unsorted source indices
   \param[in,out] orc_Target Unsorted target indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_SortSourceDescending(std::vector<uint32> & orc_Source, std::vector<uint32> & orc_Target)
{
   if (h_CheckSortedDescending(orc_Source) == false)
   {
      std::vector<uint32> c_NewSource;
      std::vector<uint32> c_NewTarget;
      const std::vector<sint32> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_Source);
      c_NewSource.reserve(orc_Source.size());
      c_NewTarget.reserve(orc_Target.size());
      //Reposition
      for (uint32 u32_It = c_IndexMap.size(); u32_It > 0; --u32_It)
      {
         const uint32 u32_PreviousIndex = u32_It - 1U;
         if (c_IndexMap[u32_PreviousIndex] >= 0)
         {
            const uint32 u32_CurIndex = static_cast<uint32>(c_IndexMap[u32_PreviousIndex]);
            if ((u32_CurIndex < orc_Source.size()) && (u32_CurIndex < orc_Target.size()))
            {
               c_NewSource.push_back(orc_Source[u32_CurIndex]);
               c_NewTarget.push_back(orc_Target[u32_CurIndex]);
            }
         }
      }
      //Copy
      orc_Source = c_NewSource;
      orc_Target = c_NewTarget;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if input vector is sorted descending

   \param[in] orc_Indices Input vector to evaluate

   \return
   true:  Sorted
   false: Unsorted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_CheckSortedDescending(const std::vector<uint32> & orc_Indices)
{
   bool q_Retval = true;

   if (orc_Indices.size() > 1)
   {
      uint32 u32_PrevVal = orc_Indices[0];

      for (uint32 u32_It = 1; u32_It < orc_Indices.size(); ++u32_It)
      {
         if (u32_PrevVal >= orc_Indices[u32_It])
         {
            u32_PrevVal = orc_Indices[u32_It];
         }
         else
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending

   \param[in,out] orc_IndicesTmp Unsorted indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_SortIndicesAscending(std::vector<uint32> & orc_IndicesTmp)
{
   if (C_Uti::h_CheckSortedAscending(orc_IndicesTmp) == false)
   {
      std::vector<stw_types::uint32> c_IndicesTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<stw_types::sint32> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      for (stw_types::uint32 u32_ItIndex = 0; u32_ItIndex < c_IndexMap.size(); ++u32_ItIndex)
      {
         if (c_IndexMap[u32_ItIndex] >= 0)
         {
            const stw_types::uint32 u32_CurIndex = static_cast<stw_types::uint32>(c_IndexMap[u32_ItIndex]);
            if (u32_CurIndex < orc_IndicesTmp.size())
            {
               c_IndicesTmp.push_back(orc_IndicesTmp[u32_CurIndex]);
            }
         }
      }
      //Step 3: Copy data structures to internal ones
      orc_IndicesTmp = c_IndicesTmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node id maximum

   \param[in] oru32_NodeIndex Node index

   \return
   Node id maximum
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SdUtil::h_GetNodeIdMaximum(const uint32 & oru32_NodeIndex)
{
   uint8 u8_Retval;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);
   const C_OSCDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;

   //openSYDE server and flashloader require max id = 126.
   if ((pc_DevDef->q_FlashloaderOpenSydeCan == true) || (pc_DevDef->q_DiagnosticProtocolOpenSydeCan == true))
   {
      //openSYDE max id
      u8_Retval = mu8_MAX_NODE_ID_OS;
   }
   else
   {
      //STW / KEFEX max id
      u8_Retval = mu8_MAX_NODE_ID_STW;
   }
   return u8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \param[in] opc_ComboBox     ComboBox with node interfaces
   \param[in] oru32_NodeIndex  Corresponding node index
   \param[in] ore_Type         Corresponding bus type

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdUtil::h_GetActiveNodeInterface(const QComboBox & orc_ComboBox, const uint32 & oru32_NodeIndex,
                                          const C_OSCSystemBus::E_Type & ore_Type)
{
   uint32 u32_Retval = 0;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      uint32 u32_InterfaceCounter;
      QString c_SelectedName = orc_ComboBox.currentText();

      for (u32_InterfaceCounter = 0; u32_InterfaceCounter <  pc_Node->c_Properties.c_ComInterfaces.size();
           ++u32_InterfaceCounter)
      {
         const C_OSCNodeComInterfaceSettings & rc_ComInterface =
            pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter];

         if (rc_ComInterface.e_InterfaceType == ore_Type)
         {
            const QString c_Name = C_PuiSdUtil::h_GetInterfaceName(ore_Type, rc_ComInterface.u8_InterfaceNumber);

            if (c_SelectedName == c_Name)
            {
               u32_Retval = rc_ComInterface.u8_InterfaceNumber;
            }
         }
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the combo box for one interface

   \param[in]     orc_Interfaces             Node interfaces
   \param[in]     ore_BusType                Bus type
   \param[in,out] opc_ComboBox               ComboBox for using the node interfaces
   \param[in]     ors32_SpecialInterface     Special interface to allow and use as default, even if already connected
   \param[out]    orq_ComProtocolL2          Flag if Layer 2 COM datapool exist
   \param[out]    orq_ComProtocolECeS        Flag if ECeS COM datapool exist
   \param[out]    orq_ComProtocolECoS        Flag if ECoS COM datapool exist

   \return
   True  At least one radio button is intractable
   False No interaction possible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_InitNodeInterfaceComboBox(const C_OSCNode & orc_Node, const C_OSCSystemBus::E_Type & ore_BusType,
                                           QComboBox * const opc_ComboBox, const sint32 & ors32_SpecialInterface,
                                           bool & orq_ComProtocolL2, bool & orq_ComProtocolECeS,
                                           bool & orq_ComProtocolECoS)
{
   bool q_Retval = false;
   bool q_Chosen = false;
   uint32 u32_Counter;

   opc_ComboBox->clear();

   // Fill the combo box
   for (u32_Counter = 0U; u32_Counter < orc_Node.c_Properties.c_ComInterfaces.size(); ++u32_Counter)
   {
      const C_OSCNodeComInterfaceSettings & rc_Interface = orc_Node.c_Properties.c_ComInterfaces[u32_Counter];

      if (rc_Interface.e_InterfaceType == ore_BusType)
      {
         const QString c_Name = C_PuiSdUtil::h_GetInterfaceName(ore_BusType, rc_Interface.u8_InterfaceNumber);

         if (rc_Interface.q_IsBusConnected == true)
         {
            if (ors32_SpecialInterface >= 0)
            {
               if (static_cast<uint8>(ors32_SpecialInterface) == rc_Interface.u8_InterfaceNumber)
               {
                  // The already used interface for the bus
                  opc_ComboBox->addItem(c_Name);
                  opc_ComboBox->setCurrentIndex(opc_ComboBox->findText(c_Name));

                  q_Retval = true;
               }
            }
         }
         else
         {
            q_Retval = true;

            opc_ComboBox->addItem(c_Name);
            if (ors32_SpecialInterface < 0)
            {
               if (q_Chosen == false)
               {
                  q_Chosen = true;
                  opc_ComboBox->setCurrentIndex(opc_ComboBox->findText(c_Name));
               }
            }
         }
      }
   }

   // get the existence of COM datapools
   orq_ComProtocolL2 = false;
   orq_ComProtocolECoS = false;
   orq_ComProtocolECeS = false;

   for (u32_Counter = 0U; u32_Counter < orc_Node.c_ComProtocols.size(); ++u32_Counter)
   {
      switch (orc_Node.c_ComProtocols[u32_Counter].e_Type)
      {
      case C_OSCCanProtocol::eLAYER2:
         orq_ComProtocolL2 = true;
         break;
      case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
         orq_ComProtocolECoS = true;
         break;
      case C_OSCCanProtocol::eECES:
         orq_ComProtocolECeS = true;
         break;
      }
   }

   if (q_Retval == false)
   {
      opc_ComboBox->addItem(C_GtGetText::h_GetText("No interface available"));
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all used node ids for current bus

   \param[in] oru32_BusIndex         Bus index
   \param[in] oru32_SpecialNodeIndex Special node index to skip
   \param[in] ors32_SpecialInterface Special interface to skip

   \return
   Vector of used node ids (unique and sorted ascending)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(const uint32 & oru32_BusIndex,
                                                                             const uint32 & oru32_SpecialNodeIndex,
                                                                             const sint32 & ors32_SpecialInterface)
{
   std::vector<uint32> c_Retval;
   const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(oru32_BusIndex);
   if (pc_Bus != NULL)
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<uint32> c_InterfaceIndexes;

      C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(oru32_BusIndex,
                                                                                         c_NodeIndexes,
                                                                                         c_InterfaceIndexes);
      c_Retval.reserve(c_NodeIndexes.size());
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            const C_OSCNode * const pc_CurNode =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndexes[u32_ItNode]);
            if (pc_CurNode != NULL)
            {
               if (c_InterfaceIndexes[u32_ItNode] < pc_CurNode->c_Properties.c_ComInterfaces.size())
               {
                  const C_OSCNodeComInterfaceSettings & rc_CurComInterface =
                     pc_CurNode->c_Properties.c_ComInterfaces[c_InterfaceIndexes[u32_ItNode]];
                  //Check special handling
                  if (((static_cast<uint8>(ors32_SpecialInterface) != rc_CurComInterface.u8_InterfaceNumber) ||
                       (oru32_SpecialNodeIndex != c_NodeIndexes[u32_ItNode])) ||
                      (pc_Bus->e_Type != rc_CurComInterface.e_InterfaceType))
                  {
                     if ((rc_CurComInterface.q_IsBusConnected == true) &&
                         (rc_CurComInterface.u32_BusIndex == oru32_BusIndex))
                     {
                        c_Retval.push_back(rc_CurComInterface.u8_NodeID);
                     }
                  }
               }
            }
         }
      }
   }
   return C_Uti::h_UniquifyAndSortAscending(c_Retval);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all used process ids for current node

   \param[in] ou32_NodeIndex               Node index
   \param[in] os32_SpecialApplicationIndex Special application to skip

   \return
   Vector of used process ids (unique and sorted ascending)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SdUtil::h_GetUsedProcessIdsForApplicationUniqueAndSortedAscending(const uint32 ou32_NodeIndex,
                                                                                        const sint32 os32_SpecialApplicationIndex)
{
   std::vector<uint32> c_Retval;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   if (pc_Node != NULL)
   {
      for (uint32 u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
      {
         if ((os32_SpecialApplicationIndex < 0) || (u32_ItApp != static_cast<uint32>(os32_SpecialApplicationIndex)))
         {
            const C_OSCNodeApplication & rc_CurApplication = pc_Node->c_Applications[u32_ItApp];
            if (rc_CurApplication.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               c_Retval.push_back(static_cast<uint32>(rc_CurApplication.u8_ProcessId));
            }
         }
      }
   }
   return C_Uti::h_UniquifyAndSortAscending(c_Retval);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all used bus ids

   \param[in] os32_SpecialBusIndex Special bus to skip

   \return
   Vector of bus process ids (unique and sorted ascending)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SdUtil::h_GetUsedBusIdsUniqueAndSortedAscending(const sint32 os32_SpecialBusIndex)
{
   std::vector<uint32> c_Retval;
   for (uint32 u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_ItBus)
   {
      if ((os32_SpecialBusIndex < 0) || (u32_ItBus != static_cast<uint32>(os32_SpecialBusIndex)))
      {
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_ItBus);
         if (pc_Bus != NULL)
         {
            c_Retval.push_back(static_cast<uint32>(pc_Bus->u8_BusID));
         }
      }
   }
   return C_Uti::h_UniquifyAndSortAscending(c_Retval);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt message to protocol restrictions

   \param[in,out] orc_Message         Message to adapt
   \param[in]     oe_Type             Protocol type
   \param[in,out] opc_AdaptationInfos Optional report about adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_AdaptMessageToProtocolType(C_OSCCanMessage & orc_Message,  const C_OSCCanProtocol::E_Type oe_Type,
                                            QString * const opc_AdaptationInfos)
{
   std::vector<QString> c_Info;
   switch (oe_Type)
   {
   case C_OSCCanProtocol::eECES:
      //CAN ID only in range of 0-7FF
      if (orc_Message.u32_CanId > 0x7FF)
      {
         c_Info.push_back(QString(C_GtGetText::h_GetText(
                                     "Message ID changed from 0x%1 to 0x7FF due to ECeS protocol restrictions.")).arg(
                             orc_Message.u32_CanId, 0, 16));
         orc_Message.u32_CanId = 0x7FF;
      }
      //DLC fix 8
      if (orc_Message.u16_Dlc != 8U)
      {
         c_Info.push_back(QString(C_GtGetText::h_GetText(
                                     "Message DLC changed from %1 to 8 due to ECeS protocol restrictions.")).arg(
                             orc_Message.u16_Dlc));
         orc_Message.u16_Dlc = 8U;
      }
      //TX method always cyclic
      if (orc_Message.e_TxMethod != C_OSCCanMessage::eTX_METHOD_CYCLIC)
      {
         c_Info.push_back(QString(C_GtGetText::h_GetText(
                                     "Message transmission type changed from \"%1\" to \"%2\" due to ECeS protocol restrictions.")).arg(
                             C_SdUtil::h_ConvertTxMethodToName(orc_Message.e_TxMethod)).arg(
                             C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_CYCLIC)));
         orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
      }
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      //Extended type
      if (orc_Message.q_IsExtended == true)
      {
         c_Info.push_back(C_GtGetText::h_GetText(
                             "Message extended flag changed from \"extended\" to \"standard\" due to ECoS protocol restrictions."));
         orc_Message.q_IsExtended = false;
      }
      //CAN ID
      if ((orc_Message.u32_CanId < mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN) ||
          (orc_Message.u32_CanId > mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX))
      {
         c_Info.push_back(QString(C_GtGetText::h_GetText(
                                     "Message ID changed from 0x%1 to 0x%2 due to ECoS protocol restrictions. Allowed IDs: 0x%3-0x%2.")).arg(
                             orc_Message.u32_CanId, 0, 16).arg(
                             mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX, 0, 16).arg(
                             mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN, 0, 16));
         orc_Message.u32_CanId = 0x7FF;
      }
      //TX method always cyclic
      if (orc_Message.e_TxMethod != C_OSCCanMessage::eTX_METHOD_CYCLIC)
      {
         c_Info.push_back(QString(C_GtGetText::h_GetText(
                                     "Message transmission type changed from \"%1\" to \"%2\" due to ECoS protocol restrictions.")).arg(
                             C_SdUtil::h_ConvertTxMethodToName(orc_Message.e_TxMethod)).arg(
                             C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_CYCLIC)));
         orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
      }
      break;
   case C_OSCCanProtocol::eLAYER2:
      //No restrictions
      break;
   }
   if (opc_AdaptationInfos != NULL)
   {
      for (uint32 u32_ItInfo = 0; u32_ItInfo < c_Info.size(); ++u32_ItInfo)
      {
         //Add line break if necessary
         if (u32_ItInfo > 0UL)
         {
            *opc_AdaptationInfos += "\n";
         }
         *opc_AdaptationInfos += c_Info[u32_ItInfo];
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt used node ids label handling

   \param[in]     orc_UsedNodeIds Unused node ids (unique and sorted ascending)
   \param[in]     orc_BusName     Bus name
   \param[in,out] opc_Label       Label to adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_InitUsedNodeIdsLabel(const std::vector<uint32> & orc_UsedNodeIds, const QString & orc_BusName,
                                      QLabel * const opc_Label)
{
   if (opc_Label != NULL)
   {
      if (orc_UsedNodeIds.size() == 0)
      {
         opc_Label->setVisible(false);
      }
      else
      {
         const QString c_BusIdsComplete = C_SdUtil::h_InitUsedIdsString(orc_UsedNodeIds, orc_BusName,
                                                                        C_GtGetText::h_GetText("bus"));
         opc_Label->setText(c_BusIdsComplete);
         opc_Label->setVisible(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt used ids string handling

   \param[in] orc_UsedIds  Unused ids (unique and sorted ascending)
   \param[in] orc_ItemName Item name
   \param[in] orc_ItemType Item type

   \return
   String for already used IDs
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_InitUsedIdsString(const std::vector<uint32> & orc_UsedIds, const QString & orc_ItemName,
                                      const QString & orc_ItemType, const bool oq_SkipItem)
{
   QString c_Retval;

   if (orc_UsedIds.size() > 0UL)
   {
      QString c_BusIds;

      for (uint32 u32_ItBusId = 0; u32_ItBusId < (orc_UsedIds.size() - 1UL); ++u32_ItBusId)
      {
         c_BusIds += QString("%1,").arg(orc_UsedIds[u32_ItBusId]);
      }
      //Append last one without comma
      c_BusIds += QString::number(orc_UsedIds[orc_UsedIds.size() - 1]);

      if (oq_SkipItem == true)
      {
         c_Retval = QString(C_GtGetText::h_GetText("Already used IDs: %1")).arg(c_BusIds);
      }
      else
      {
         c_Retval = QString(C_GtGetText::h_GetText("Already used IDs on %3 %1: %2")).arg(orc_ItemName).arg(
            c_BusIds).arg(orc_ItemType);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get next free node id

   \param[in] orc_Interfaces         Node interfaces
   \param[in] orc_UsedNodeIds        Unused node ids (unique and sorted ascending)
   \param[in] ors32_SpecialInterface Special interface to use as default

   \return
   Node ID proposal (Always in range but may be invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdUtil::h_GetNextFreeNodeId(const std::vector<C_OSCNodeComInterfaceSettings> & orc_Interfaces,
                                     const std::vector<uint32> & orc_UsedNodeIds, const sint32 & ors32_SpecialInterface)
{
   uint32 u32_Retval = 0;

   if (ors32_SpecialInterface >= 0)
   {
      for (uint32 u32_ItInterface = 0; u32_ItInterface < orc_Interfaces.size(); ++u32_ItInterface)
      {
         const C_OSCNodeComInterfaceSettings & rc_CurComIf = orc_Interfaces[u32_ItInterface];
         if (rc_CurComIf.u8_InterfaceNumber == static_cast<uint8>(ors32_SpecialInterface))
         {
            u32_Retval = rc_CurComIf.u8_NodeID;
         }
      }
   }
   else
   {
      for (uint32 u32_ItBusId = 0; u32_ItBusId < orc_UsedNodeIds.size(); ++u32_ItBusId)
      {
         if (u32_Retval == orc_UsedNodeIds[u32_ItBusId])
         {
            ++u32_Retval;
         }
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if node has at least one connection of this bus type

   \param[in] orc_Node  Node for check
   \param[in] oe_Type   Bus type

   \return
   true     Node has connection of this type
   false    Node has not a connection of this type
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_HasConnectionType(const C_OSCNode & orc_Node, const C_OSCSystemBus::E_Type oe_Type)
{
   bool q_Return = false;
   uint32 u32_IntfCounter;

   // Search an interface of the specified type
   for (u32_IntfCounter = 0U; u32_IntfCounter < orc_Node.c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
   {
      if (orc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter].e_InterfaceType == oe_Type)
      {
         q_Return = true;
         break;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any com interfaces for the specified bus type are available and free

   \param[in] orc_ComInterfaces Com interfaces
   \param[in] ore_BusType       Bus type

   \return
   True  At least one selectable interface available
   False No free interface available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_CheckNodeInterfaceAvailable(const std::vector<C_OSCNodeComInterfaceSettings> & orc_ComInterfaces,
                                             const C_OSCSystemBus::E_Type & ore_BusType)
{
   bool q_Retval = false;

   for (uint32 u32_ItInterface = 0; u32_ItInterface < orc_ComInterfaces.size(); ++u32_ItInterface)
   {
      const C_OSCNodeComInterfaceSettings & rc_Interface = orc_ComInterfaces[u32_ItInterface];
      if (rc_Interface.e_InterfaceType == ore_BusType)
      {
         if (rc_Interface.q_IsBusConnected == false)
         {
            q_Retval = true;
            break;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if at least one com interfaces for the specified bus type is connected

   \param[in] orc_ComInterfaces Com interfaces
   \param[in] ore_BusType       Bus type

   \return
   True  At least one selectable interface connected
   False No interface connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_CheckNodeInterfaceConnected(const std::vector<C_OSCNodeComInterfaceSettings> & orc_ComInterfaces,
                                             const C_OSCSystemBus::E_Type & ore_BusType)
{
   bool q_Retval = false;

   for (uint32 u32_ItInterface = 0; u32_ItInterface < orc_ComInterfaces.size(); ++u32_ItInterface)
   {
      const C_OSCNodeComInterfaceSettings & rc_Interface = orc_ComInterfaces[u32_ItInterface];
      if (rc_Interface.e_InterfaceType == ore_BusType)
      {
         if (rc_Interface.q_IsBusConnected == true)
         {
            q_Retval = true;
            break;
         }
      }
   }
   return q_Retval;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check the total number of datapool

   The total number of datapools is bounded above by mu32_NODE_DATA_POOL_MAX.

   \param[in]     oru32_NodeIndex         node index
   \param[in]     ore32_DataPoolType      data pool type
   \param[in]     orq_AlreadyChecked      for pre-checking cases (true: we know that the number exceeds the maximum)
   \param[in]     opc_Parent              parent widget

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_CheckDatapoolNumber(const uint32 & oru32_NodeIndex, const C_OSCNodeDataPool::E_Type & ore_DataPoolType,
                                     const bool & orq_AlreadyChecked, QWidget * const opc_Parent)
{
   bool q_Return = false;

   if (orq_AlreadyChecked == true)
   {
      C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool add"));
      c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("The allowed maximum number of Datapools is %1.")).
                                  arg(mu32_NODE_DATA_POOL_MAX));
      c_MessageBox.Execute();
   }
   else
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);
      if (pc_Node != NULL)
      {
         // is enough space available
         if (pc_Node->c_DataPools.size() >= mu32_NODE_DATA_POOL_MAX)
         {
            C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool add"));
            c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText(
                                                   "The allowed maximum number of Datapools is %1.")).
                                        arg(mu32_NODE_DATA_POOL_MAX));
            c_MessageBox.Execute();
         }
         else if ((pc_Node->c_ComProtocols.size() >= mu32_NODE_DATA_POOL_COM_MAX) &&
                  (ore_DataPoolType == C_OSCNodeDataPool::eCOM))
         {
            C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("COMM Datapool add"));
            c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText(
                                                   "The allowed maximum number of COMM Datapools is %1.")).
                                        arg(mu32_NODE_DATA_POOL_COM_MAX));
            c_MessageBox.SetDetails(C_GtGetText::h_GetText("There is maximum one Datapool per CAN protocol allowed."));
            c_MessageBox.Execute();
         }
         else
         {
            // all OK
            q_Return = true;
         }
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error tool tip for node

   \param[in]  oru32_NodeIndex   Node index
   \param[out] orc_Text          Tool tip text
   \param[out] orq_ErrorDetected Flag if any errors detected

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdUtil::h_GetErrorToolTipNode(const uint32 & oru32_NodeIndex, QString & orc_Text, bool & orq_ErrorDetected)
{
   bool q_NameConflict;
   bool q_NameEmpty;
   bool q_NodeIdInvalid;
   QElapsedTimer c_Timer;

   if (mq_TIMING_OUTPUT)
   {
      c_Timer.start();
   }
   bool q_DataPoolsSizeConflict =
      C_PuiSdHandler::h_GetInstance()->CheckNodeNvmDataPoolsSizeConflict(oru32_NodeIndex);
   bool q_DataPoolsInvalid;
   bool q_ApplicationsInvalid;

   std::vector<uint32> c_InvalidInterfaceIndices;
   std::vector<uint32> c_InvalidDataPoolIndices;
   std::vector<uint32> c_InvalidApplicationIndices;
   sint32 s32_Retval = C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().CheckErrorNode(
      oru32_NodeIndex, &q_NameConflict, &q_NameEmpty, &q_NodeIdInvalid, &q_DataPoolsInvalid, &q_ApplicationsInvalid,
      true, &c_InvalidInterfaceIndices, &c_InvalidDataPoolIndices, &c_InvalidApplicationIndices);
   if (s32_Retval == C_NO_ERR)
   {
      if ((((((q_NameConflict == true) || (q_NodeIdInvalid == true)) || (q_DataPoolsInvalid == true)) ||
            (q_ApplicationsInvalid == true)) || (q_DataPoolsSizeConflict == true)) || (q_NameEmpty == true))
      {
         orq_ErrorDetected = true;
         if (((q_NameConflict == true) || (q_NodeIdInvalid == true)) || (q_NameEmpty == true))
         {
            orc_Text += C_GtGetText::h_GetText("Invalid properties:\n");
            if (q_NameEmpty == true)
            {
               orc_Text += C_GtGetText::h_GetText("Node name is empty or contains invalid characters.\n");
            }
            if (q_NameConflict == true)
            {
               orc_Text += C_GtGetText::h_GetText("Duplicate node name detected.\n");
            }
            if (q_NodeIdInvalid == true)
            {
               if (c_InvalidInterfaceIndices.size() == 1UL)
               {
                  orc_Text += C_GtGetText::h_GetText("Duplicate node ID detected.\n");
               }
               else
               {
                  orc_Text += QString(C_GtGetText::h_GetText("%1 duplicate node IDs detected.\n")).arg(
                     c_InvalidInterfaceIndices.size());
               }
            }
            orc_Text += "\n";
         }
         if (q_ApplicationsInvalid == true)
         {
            orc_Text += C_GtGetText::h_GetText("Invalid Data Blocks:\n");
            for (uint32 u32_ItAppl = 0;
                 (u32_ItAppl < c_InvalidApplicationIndices.size()) &&
                 (u32_ItAppl < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                 ++u32_ItAppl)
            {
               const C_OSCNodeApplication * const pc_Appl = C_PuiSdHandler::h_GetInstance()->GetApplication(
                  oru32_NodeIndex, c_InvalidApplicationIndices[u32_ItAppl]);
               if (pc_Appl != NULL)
               {
                  orc_Text += QString("%1\n").arg(pc_Appl->c_Name.c_str());
               }
            }
            if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidApplicationIndices.size())
            {
               orc_Text += QString("+%1\n").arg(
                  c_InvalidApplicationIndices.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
            }
            orc_Text += "\n";
         }
         if ((q_DataPoolsInvalid == true) || (q_DataPoolsSizeConflict == true))
         {
            QString c_Heading;
            QString c_Content;
            C_SdUtil::h_GetErrorToolTipDataPools(oru32_NodeIndex, c_InvalidDataPoolIndices, q_DataPoolsSizeConflict,
                                                 c_Heading, c_Content);
            orc_Text += c_Heading + "\n";
            orc_Text += c_Content;
            orc_Text += "\n";
         }
      }
      else
      {
         orq_ErrorDetected = false;
         orc_Text = C_GtGetText::h_GetText("None");
      }
   }
   else
   {
      orq_ErrorDetected = true;
      orc_Text = C_GtGetText::h_GetText("Unknown");
   }
   if (mq_TIMING_OUTPUT)
   {
      std::cout << "Node tooltip error check " << c_Timer.elapsed() << " ms" << &std::endl;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error tool tip for bus

   \param[in]  oru32_BusIndex Bus index
   \param[out] orc_Text       Tool tip text

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_GetErrorToolTipBus(const uint32 & oru32_BusIndex, QString & orc_Text)
{
   bool q_NameConflict;
   bool q_NameEmpty;
   bool q_IdInvalid;

   std::vector<QString> c_InvalidNodesForBitRate;
   std::vector<stw_opensyde_core::C_OSCCanProtocol::E_Type> c_InvalidProtocols;
   C_PuiSdHandler::h_GetInstance()->CheckBusConflictDetailed(oru32_BusIndex, &q_NameConflict, &q_NameEmpty,
                                                             &q_IdInvalid, &c_InvalidNodesForBitRate,
                                                             &c_InvalidProtocols);
   if ((((q_NameConflict == true) || (q_NameEmpty == true)) || (q_IdInvalid == true)) ||
       (c_InvalidNodesForBitRate.size() > 0UL))
   {
      orc_Text += C_GtGetText::h_GetText("Invalid properties:\n");
      if (q_NameConflict == true)
      {
         orc_Text += C_GtGetText::h_GetText("Duplicate bus name detected\n");
      }
      if (q_NameEmpty == true)
      {
         orc_Text += C_GtGetText::h_GetText("Bus name is empty or contains invalid characters.\n");
      }
      if (q_IdInvalid == true)
      {
         orc_Text += C_GtGetText::h_GetText("Duplicate bus ID detected\n");
      }
      if (c_InvalidNodesForBitRate.size() > 0UL)
      {
         orc_Text += C_GtGetText::h_GetText("Selected bitrate not supported by following connected nodes:\n");
         for (uint32 u32_ItNode = 0UL; (u32_ItNode < c_InvalidNodesForBitRate.size()) &&
              (u32_ItNode < mu32_TOOL_TIP_MAXIMUM_ITEMS); ++u32_ItNode)
         {
            orc_Text += "- " + c_InvalidNodesForBitRate[u32_ItNode] + "\n";
         }
         if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidNodesForBitRate.size())
         {
            orc_Text += QString("+%1\n").arg(c_InvalidNodesForBitRate.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
         }
      }
      orc_Text += "\n";
   }
   if (c_InvalidProtocols.size() > 0)
   {
      orc_Text += C_GtGetText::h_GetText("COMM protocols with invalid content:\n");
      for (uint32 u32_ItP = 0; u32_ItP < c_InvalidProtocols.size(); ++u32_ItP)
      {
         orc_Text += QString("%1\n").arg(C_PuiSdUtil::h_ConvertProtocolTypeToString(c_InvalidProtocols[u32_ItP]));
      }
      orc_Text += "\n";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for datapool list.

   \param[in]     orc_NodeIndex        index of node where the list belongs to
   \param[in]     oru32_DatapoolIndex  index of datapool where the list belongs to
   \param[in]     oru32_ListIndex      index of list for which the tooltip is requested

   \return
      content as string
*/
//----------------------------------------------------------------------------------------------------------------------

QString C_SdUtil::h_GetToolTipContentDpList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DatapoolIndex,
                                            const uint32 & oru32_ListIndex)
{
   const C_OSCNodeDataPoolList * const pc_DpList =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(oru32_NodeIndex, oru32_DatapoolIndex, oru32_ListIndex);

   QString c_ToolTipContent;
   QString c_HelpString;

   // comment
   c_HelpString = pc_DpList->c_Comment.c_str();
   if (c_HelpString != "")
   {
      c_ToolTipContent.append(c_HelpString);
      c_ToolTipContent.append("\n \n");
   }

   // datasets
   uint32 u32_DatasetNumber = pc_DpList->c_DataSets.size();

   if (u32_DatasetNumber == 0)
   {
      c_ToolTipContent.append(C_GtGetText::h_GetText("No Datasets \n"));
   }
   else if (u32_DatasetNumber == 1)
   {
      c_ToolTipContent.append(C_GtGetText::h_GetText("Dataset: "));
      c_ToolTipContent.append(pc_DpList->c_DataSets[0].c_Name.c_str());
      c_ToolTipContent.append("\n");
   }
   else
   {
      c_ToolTipContent.append(C_GtGetText::h_GetText("Datasets: \n"));
      for (uint32 u32_Pos = 0; u32_Pos < u32_DatasetNumber; u32_Pos++)
      {
         const C_OSCNodeDataPoolDataSet & rc_Dataset = pc_DpList->c_DataSets[u32_Pos];
         c_ToolTipContent.append("  - ");
         c_ToolTipContent.append(rc_Dataset.c_Name.c_str());
         c_ToolTipContent.append("\n");
      }
   }

   // parameters
   c_ToolTipContent.append(C_GtGetText::h_GetText("Parameters: "));
   c_ToolTipContent.append(QString::number(pc_DpList->c_Elements.size()));

   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for datapool list element.

   \param[in]     orc_NodeDatapoolListElementId    indexes of element for which the tooltip is requested
   \param[in]     orc_AdditionalInformation        Optional additional information string which will get inserted after comment
                                                    (no "\n" at end necessary)

   \return
      content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_GetToolTipContentDpListElement(const C_OSCNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
                                                   const QString & orc_AdditionalInformation)
{
   QString c_ToolTipContent = "";
   QString c_HelpString = "";

   std::vector<QString> c_HelpVector;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      orc_NodeDatapoolListElementId.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      orc_NodeDatapoolListElementId.u32_NodeIndex, orc_NodeDatapoolListElementId.u32_DataPoolIndex);
   const C_OSCNodeDataPoolList * const pc_DpList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      orc_NodeDatapoolListElementId.u32_NodeIndex, orc_NodeDatapoolListElementId.u32_DataPoolIndex,
      orc_NodeDatapoolListElementId.u32_ListIndex);
   const C_OSCNodeDataPoolListElement * const pc_DpListElement =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
         orc_NodeDatapoolListElementId.u32_NodeIndex, orc_NodeDatapoolListElementId.u32_DataPoolIndex,
         orc_NodeDatapoolListElementId.u32_ListIndex, orc_NodeDatapoolListElementId.u32_ElementIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
      C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(orc_NodeDatapoolListElementId.u32_NodeIndex,
                                                                orc_NodeDatapoolListElementId.u32_DataPoolIndex,
                                                                orc_NodeDatapoolListElementId.u32_ListIndex,
                                                                orc_NodeDatapoolListElementId.u32_ElementIndex);

   if ((pc_Node != NULL) && (pc_Datapool != NULL) && (pc_DpList != NULL) && (pc_DpListElement != NULL) &&
       (pc_UIElement != NULL))
   {
      if (pc_Datapool->e_Type == C_OSCNodeDataPool::eCOM)
      {
         // get message id and signal index and use special tool tip content generation
         C_OSCCanMessageIdentificationIndices c_MessageId;
         uint32 u32_SignalIndex;
         C_PuiSdHandler::h_GetInstance()->ConvertElementIndexToSignalIndex(orc_NodeDatapoolListElementId, c_MessageId,
                                                                           u32_SignalIndex);
         c_ToolTipContent = h_GetToolTipContentSignal(c_MessageId, u32_SignalIndex, orc_AdditionalInformation);
      }
      else
      {
         // comment
         if (pc_DpListElement->c_Comment.IsEmpty() == false)
         {
            c_ToolTipContent = pc_DpListElement->c_Comment.c_str();
            c_ToolTipContent.append("\n\n");
         }

         //additional info
         if (orc_AdditionalInformation.isEmpty() == false)
         {
            c_ToolTipContent.append(orc_AdditionalInformation);
            c_ToolTipContent.append("\n\n");
         }

         //Properties:
         if (pc_Datapool->e_Type == C_OSCNodeDataPool::eDIAG)
         {
            c_ToolTipContent.append(C_GtGetText::h_GetText("Variable Properties: "));
         }
         else //eNVM
         {
            c_ToolTipContent.append(C_GtGetText::h_GetText("Parameter Properties: "));
         }
         c_ToolTipContent.append("\n");

         //value type
         c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Value type: "));
         if (pc_UIElement->q_InterpretAsString == false)
         {
            c_ToolTipContent.append(C_SdNdeDataPoolUtil::h_ConvertContentTypeToString(
                                       pc_DpListElement->c_Value.GetType()));
         }
         else
         {
            c_ToolTipContent.append(C_GtGetText::h_GetText("string"));
         }
         c_ToolTipContent.append("\n");

         // array size
         c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Array size: "));
         if (pc_DpListElement->GetArray() == false)
         {
            c_ToolTipContent.append("-");
         }
         else
         {
            c_ToolTipContent.append(QString::number(pc_DpListElement->GetArraySize()));
         }
         c_ToolTipContent.append("\n");

         // do not show min, max, factor, offset, unit for string types
         if (pc_UIElement->q_InterpretAsString == false)
         {
            // auto min max
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Auto min/max: "));
            if (pc_UIElement->q_AutoMinMaxActive == true)
            {
               c_ToolTipContent.append(C_GtGetText::h_GetText("Enabled"));
            }
            else
            {
               c_ToolTipContent.append(C_GtGetText::h_GetText("Disabled"));
            }
            c_ToolTipContent.append("\n");

            // min
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Min: "));
            C_SdNdeDataPoolContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_MinValue,
                                                                  pc_DpListElement->f64_Factor,
                                                                  pc_DpListElement->f64_Offset, c_HelpVector);
            c_HelpString = "";
            for (uint32 u32_Pos = 0; u32_Pos < c_HelpVector.size(); u32_Pos++)
            {
               c_HelpString.append(c_HelpVector[u32_Pos]);
               c_HelpString.append(";");
               // auto min max or no array -> only one number
               if ((pc_DpListElement->GetArray() == false) || (pc_UIElement->q_AutoMinMaxActive == true))
               {
                  break;
               }
            }
            c_HelpString.chop(1); // remove last ";"
            c_ToolTipContent.append(c_HelpString);
            c_ToolTipContent.append("\n");

            // max
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Max: "));
            c_HelpVector.clear();
            C_SdNdeDataPoolContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_MaxValue,
                                                                  pc_DpListElement->f64_Factor,
                                                                  pc_DpListElement->f64_Offset, c_HelpVector);
            c_HelpString = "";
            for (uint32 u32_Pos = 0; u32_Pos < c_HelpVector.size(); u32_Pos++)
            {
               c_HelpString.append(c_HelpVector[u32_Pos]);
               c_HelpString.append(";");
               // auto min max or no array -> only one number
               if ((pc_DpListElement->GetArray() == false) || (pc_UIElement->q_AutoMinMaxActive == true))
               {
                  break;
               }
            }
            c_HelpString.chop(1); // remove last ";"
            c_ToolTipContent.append(c_HelpString);
            c_ToolTipContent.append("\n");

            // factor
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Factor: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->f64_Factor));
            c_ToolTipContent.append("\n");

            //offset
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Offset: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->f64_Offset));
            c_ToolTipContent.append("\n");

            // unit
            c_HelpString = pc_DpListElement->c_Unit.c_str();
            if (c_HelpString.isEmpty() == false)
            {
               c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Unit: "));
               c_ToolTipContent.append(c_HelpString);
               c_ToolTipContent.append("\n");
            }
         }

         // dataset values
         if (pc_DpList->c_DataSets.size() > 0)
         {
            for (uint32 u32_PosDataset = 0; u32_PosDataset < pc_DpList->c_DataSets.size(); u32_PosDataset++)
            {
               c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Dataset - "));
               c_ToolTipContent.append(pc_DpList->c_DataSets[u32_PosDataset].c_Name.c_str());
               c_ToolTipContent.append(": ");
               c_HelpVector.clear();
               C_SdNdeDataPoolContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_DataSetValues[u32_PosDataset],
                                                                     pc_DpListElement->f64_Factor,
                                                                     pc_DpListElement->f64_Offset, c_HelpVector);
               c_HelpString = "";
               for (uint32 u32_PosArray = 0; u32_PosArray < c_HelpVector.size(); u32_PosArray++)
               {
                  c_HelpString.append(c_HelpVector[u32_PosArray]);
                  c_HelpString.append(";");
               }
               c_HelpString.chop(1); // remove last ";"
               c_ToolTipContent.append(c_HelpString);
               c_ToolTipContent.append("\n");
            }
         }

         // access
         c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Access: "));
         c_ToolTipContent.append(C_SdNdeDataPoolUtil::h_ConvertElementAccessToString(pc_DpListElement->e_Access));
         c_ToolTipContent.append("\n");

         // case NVM
         if (pc_Datapool->e_Type == C_OSCNodeDataPool::eNVM)
         {
            // data size
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Data size: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->GetSizeByte()));
            if (pc_DpListElement->GetSizeByte() == 1)
            {
               c_ToolTipContent.append(C_GtGetText::h_GetText(" Byte"));
            }
            else
            {
               c_ToolTipContent.append(C_GtGetText::h_GetText(" Bytes"));
            }
            c_ToolTipContent.append("\n");

            // address
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Address: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->u32_NvMStartAddress -
                                                    pc_Datapool->u32_NvMStartAddress));
            c_ToolTipContent.append(" (");
            c_ToolTipContent.append(QString::number(pc_DpListElement->u32_NvMStartAddress));
            c_ToolTipContent.append(")");
         }
         // case DIAG (case COMM is handled at the beginning)
         else
         {
            // event call
            c_ToolTipContent.append(QString("   ") + C_GtGetText::h_GetText("Event call: "));
            if (pc_DpListElement->q_DiagEventCall == true)
            {
               c_ToolTipContent.append(C_GtGetText::h_GetText("Enabled"));
            }
            else
            {
               c_ToolTipContent.append(C_GtGetText::h_GetText("Disabled"));
            }
         }
      }
   }

   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for message.

   \param[in] orc_MessageId Message id belonging to the signal for which the tooltip is requested

   \return
   Content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_GetToolTipContentMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   QString c_ToolTipContent = "";
   const C_OSCCanMessage * const pc_Message =
      C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

   if (pc_Message != NULL)
   {
      c_ToolTipContent = C_SdTooltipUtil::h_GetToolTipContentMessage(*pc_Message);
   }
   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for signal.

   \param[in]     orc_MessageId                message id belonging to the signal for which the tooltip is requested
   \param[in]     ou32_SignalIndex             for which the tooltip is requested
   \param[in]     orc_AdditionalInformation    Optional additional information string which will get inserted after comment
                                               (no "\n" at end necessary)

   \return
      content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_GetToolTipContentSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                            const uint32 & oru32_SignalIndex, const QString & orc_AdditionalInformation)
{
   QString c_ToolTipContent = "";

   const C_OSCCanSignal * const pc_Signal =
      C_PuiSdHandler::h_GetInstance()->GetCanSignal(orc_MessageId, oru32_SignalIndex);
   const C_OSCCanMessage * const pc_Message =
      C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * const pc_DpListElement =
      C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(orc_MessageId, oru32_SignalIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_DpListElementUi =
      C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(orc_MessageId, oru32_SignalIndex);

   if ((pc_Message != NULL) &&
       (pc_Signal != NULL) &&
       (pc_DpListElement != NULL))
   {
      QString c_AutoMinMaxInfo;
      c_AutoMinMaxInfo.append(QString("   ") + C_GtGetText::h_GetText("Auto min/max: "));
      if (pc_DpListElementUi->q_AutoMinMaxActive == true)
      {
         c_AutoMinMaxInfo.append(C_GtGetText::h_GetText("Enabled"));
      }
      else
      {
         c_AutoMinMaxInfo.append(C_GtGetText::h_GetText("Disabled"));
      }
      c_ToolTipContent = C_SdTooltipUtil::h_GetToolTipContentSignal(*pc_Signal, *pc_Message, *pc_DpListElement,
                                                                    c_AutoMinMaxInfo,
                                                                    orc_AdditionalInformation);
   }

   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices descending ( Sorting steps are done for the content vectors in sync)

   \param[in,out] orc_IndicesTmp    Unsorted indices
   \param[in,out] orc_OSCContentTmp Unsorted OSC content
   \param[in,out] orc_UIContentTmp  Unsorted UI content
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
void C_SdUtil::h_SortIndicesDescendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                              std::vector<T> & orc_OSCContentTmp, std::vector<U> & orc_UIContentTmp)
{
   if (h_CheckSortedDescending(orc_IndicesTmp) == false)
   {
      std::vector<stw_types::uint32> c_IndicesTmp;
      std::vector<T> c_OSCContentTmp;
      std::vector<U> c_UIContentTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<stw_types::sint32> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      c_OSCContentTmp.reserve(orc_OSCContentTmp.size());
      c_UIContentTmp.reserve(orc_UIContentTmp.size());
      for (stw_types::uint32 u32_ItIndex = c_IndexMap.size(); u32_ItIndex > 0; --u32_ItIndex)
      {
         if (c_IndexMap[u32_ItIndex - 1] >= 0)
         {
            const stw_types::uint32 u32_CurIndex = static_cast<stw_types::uint32>(c_IndexMap[u32_ItIndex - 1]);
            if (((u32_CurIndex < orc_IndicesTmp.size()) &&
                 (u32_CurIndex < orc_OSCContentTmp.size())) &&
                (u32_CurIndex < orc_UIContentTmp.size()))
            {
               c_IndicesTmp.push_back(orc_IndicesTmp[u32_CurIndex]);
               c_OSCContentTmp.push_back(orc_OSCContentTmp[u32_CurIndex]);
               c_UIContentTmp.push_back(orc_UIContentTmp[u32_CurIndex]);
            }
         }
      }
      //Step 3: Copy data structures to internal ones
      orc_IndicesTmp = c_IndicesTmp;
      orc_OSCContentTmp = c_OSCContentTmp;
      orc_UIContentTmp = c_UIContentTmp;
   }
}

//Explicit declaration of every type usage is necessary for templates to allow split of declaration and implementation
//lint -esym(754,stw_opensyde_gui_logic::C_SdUtil::h_SortIndicesDescendingAndSync*)
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCCanMessageIdentificationIndices, uint32>(
   std::vector<stw_types::uint32> & orc_IndicesTmp,
   std::vector<C_OSCCanMessageIdentificationIndices> & orc_OSCContentTmp, std::vector<uint32> & orc_UIContentTmp);
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCNodeDataPoolList, C_PuiSdNodeDataPoolList>(
   std::vector<stw_types::uint32> & orc_IndicesTmp, std::vector<C_OSCNodeDataPoolList> & orc_OSCContentTmp,
   std::vector<C_PuiSdNodeDataPoolList> & orc_UIContentTmp);
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCNodeDataPoolDataSet, std::vector<C_OSCNodeDataPoolContent> >(
   std::vector<stw_types::uint32> & orc_IndicesTmp, std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCContentTmp,
   std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_UIContentTmp);
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
   std::vector<stw_types::uint32> & orc_IndicesTmp, std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContentTmp,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContentTmp);

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending ( Sorting steps are done for the content vectors in sync)

   \param[in,out] orc_IndicesTmp    Unsorted indices
   \param[in,out] orc_OSCContentTmp Unsorted OSC content
   \param[in,out] orc_UIContentTmp  Unsorted UI content
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
void C_SdUtil::h_SortIndicesAscendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                             std::vector<T> & orc_OSCContentTmp, std::vector<U> & orc_UIContentTmp)
{
   if (C_Uti::h_CheckSortedAscending(orc_IndicesTmp) == false)
   {
      std::vector<stw_types::uint32> c_IndicesTmp;
      std::vector<T> c_OSCContentTmp;
      std::vector<U> c_UIContentTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<stw_types::sint32> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      c_OSCContentTmp.reserve(orc_OSCContentTmp.size());
      c_UIContentTmp.reserve(orc_UIContentTmp.size());
      for (stw_types::uint32 u32_ItIndex = 0; u32_ItIndex < c_IndexMap.size(); ++u32_ItIndex)
      {
         if (c_IndexMap[u32_ItIndex] >= 0)
         {
            const stw_types::uint32 u32_CurIndex = static_cast<stw_types::uint32>(c_IndexMap[u32_ItIndex]);
            if (((u32_CurIndex < orc_IndicesTmp.size()) &&
                 (u32_CurIndex < orc_OSCContentTmp.size())) &&
                (u32_CurIndex < orc_UIContentTmp.size()))
            {
               c_IndicesTmp.push_back(orc_IndicesTmp[u32_CurIndex]);
               c_OSCContentTmp.push_back(orc_OSCContentTmp[u32_CurIndex]);
               c_UIContentTmp.push_back(orc_UIContentTmp[u32_CurIndex]);
            }
         }
      }
      //Step 3: Copy data structures to internal ones
      orc_IndicesTmp = c_IndicesTmp;
      orc_OSCContentTmp = c_OSCContentTmp;
      orc_UIContentTmp = c_UIContentTmp;
   }
}

//Explicit declaration of every type usage is necessary for templates to allow split of declaration and implementation
//lint -esym(754,stw_opensyde_gui_logic::C_SdUtil::h_SortIndicesAscendingAndSync*)
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OSCNodeDataPoolList, C_PuiSdNodeDataPoolList>(
   std::vector<stw_types::uint32> & orc_IndicesTmp, std::vector<C_OSCNodeDataPoolList> & orc_OSCContentTmp,
   std::vector<C_PuiSdNodeDataPoolList> & orc_UIContentTmp);
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OSCNodeDataPoolDataSet, std::vector<C_OSCNodeDataPoolContent> >(
   std::vector<stw_types::uint32> & orc_IndicesTmp, std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCContentTmp,
   std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_UIContentTmp);
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OSCNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
   std::vector<stw_types::uint32> & orc_IndicesTmp, std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContentTmp,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContentTmp);
