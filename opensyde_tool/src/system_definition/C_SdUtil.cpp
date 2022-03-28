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

#include "stwerrors.h"
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OgeRabProperties.h"
#include "C_PuiSdUtil.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "C_OSCNodeSquad.h"
#include "C_SdTooltipUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdBueUnoBusProtNodeConnectCommand.h"
#include "C_SdBueUnoBusProtNodeConnectAndCreateCommand.h"
#include "C_SdBueUnoBusProtNodeDisconnectCommand.h"

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

   \param[in]   orc_NodeIndices              Node indices (match with interface indices expected)
   \param[in]   orc_InterfaceIndices         Interface indices (match with node indices expected)
   \param[out]  orc_Names                    Generate node & interface names
   \param[in]   oq_NameWithInterfaceAlways   Flag if the interface name will be added always, or only in case of
                                             multiple interfaces of node used
   \param[in]   opc_DatapoolIndices          Optional pointer to Datapool indices (match with node indices expected)
   \param[out]  opc_DatapoolNames            Optional pointer to found Datapool names

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdUtil::h_GetNames(const std::vector<uint32> & orc_NodeIndices,
                            const std::vector<uint32> & orc_InterfaceIndices, std::vector<QString> & orc_Names,
                            const bool oq_NameWithInterfaceAlways,
                            const std::vector<uint32> * const opc_DatapoolIndices,
                            std::vector<QString> * const opc_DatapoolNames)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_NodeIndices.size() == orc_InterfaceIndices.size())
   {
      QString c_TmpName;
      // set for all nodes with more than one interface
      std::set<uint32> c_SetNodesWithMultipleItf;

      // detect all nodes with more than one used interface if the interface name should not be visible always
      if ((oq_NameWithInterfaceAlways == false) &&
          (orc_NodeIndices.size() > 1))
      {
         for (uint32 u32_NodeCounter = 0U; u32_NodeCounter < static_cast<uint32>(orc_NodeIndices.size() - 1UL);
              ++u32_NodeCounter)
         {
            for (uint32 u32_NextNodeCounter = u32_NodeCounter + 1U;
                 u32_NextNodeCounter < (orc_NodeIndices.size());
                 ++u32_NextNodeCounter)
            {
               // Check for same node index but different interface index
               // The Datapool index is not relevant for this comparison, but can cause multiple
               // entries with same node index and interface index, so iterating through all entries
               if ((orc_NodeIndices[u32_NodeCounter] == orc_NodeIndices[u32_NextNodeCounter]) &&
                   (orc_InterfaceIndices[u32_NodeCounter] != orc_InterfaceIndices[u32_NextNodeCounter]))
               {
                  c_SetNodesWithMultipleItf.insert(orc_NodeIndices[u32_NodeCounter]);
                  // Node index found, check next node
                  break;
               }
            }
         }
      }

      // get all the node names
      for (uint32 u32_NodeCounter = 0U; (u32_NodeCounter < orc_NodeIndices.size()) && (s32_Retval == C_NO_ERR);
           ++u32_NodeCounter)
      {
         if ((oq_NameWithInterfaceAlways == true) ||
             (c_SetNodesWithMultipleItf.find(orc_NodeIndices[u32_NodeCounter]) != c_SetNodesWithMultipleItf.end()))
         {
            s32_Retval = h_GetName(orc_NodeIndices[u32_NodeCounter], orc_InterfaceIndices[u32_NodeCounter], c_TmpName);
         }
         else
         {
            s32_Retval = h_GetName(orc_NodeIndices[u32_NodeCounter], c_TmpName);
         }
         orc_Names.push_back(c_TmpName);

         if (((opc_DatapoolIndices != NULL) && (opc_DatapoolNames != NULL)) &&
             (orc_NodeIndices.size() == opc_DatapoolIndices->size()))
         {
            // Get the matching Datapool name
            const C_OSCNodeDataPool * const pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
               orc_NodeIndices[u32_NodeCounter], (*opc_DatapoolIndices)[u32_NodeCounter]);

            if (pc_Dp != NULL)
            {
               //lint -e{413} //false positive; opc_DatapoolNames is checked to be != NULL a few lines above
               opc_DatapoolNames->push_back(pc_Dp->c_Name.c_str());
            }
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
/*! \brief   Generate node name

   \param[in]      oru32_NodeIndex  Node index
   \param[in,out]  orc_Name         Generate node & interface name

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

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_InterfaceIndex   Interface index
   \param[in,out]  orc_Name               Generate node & interface name

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

   \param[in]  orc_File    File to use as icon

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

   \param[in]  orc_File    SVG file to use as icon
   \param[in]  orc_Size    Icon size

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

   \param[in]  ore_Type    Tx method

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

   \param[in]  ore_Type    Byte order

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

   \param[in]   ou32_NodeIndex         Node index (identifier)
   \param[in]   orc_Indices            Invalid datapool indices
   \param[in]   oq_NvmSizeInvalid      Flag if NVM size invalid
   \param[in]   oq_NvmOverlapDetected  Flag if NVM overlap of at least two Datapools detected
   \param[out]  orc_Heading            Heading suggestion
   \param[out]  orc_Content            Content suggestion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_GetErrorToolTipDataPools(const uint32 ou32_NodeIndex, const std::vector<uint32> & orc_Indices,
                                          const bool oq_NvmSizeInvalid, const bool oq_NvmOverlapDetected,
                                          QString & orc_Heading, QString & orc_Content)
{
   orc_Heading = C_GtGetText::h_GetText("Datapools with invalid content:");
   orc_Content = "";
   if (oq_NvmSizeInvalid == true)
   {
      orc_Content += C_GtGetText::h_GetText("NVM Datapool sizes over node limit.\n");
   }
   if (oq_NvmOverlapDetected == true)
   {
      orc_Content += C_GtGetText::h_GetText("NVM Datapools overlapping detected.\n");
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
         orc_Content += static_cast<QString>("%1\n").arg(pc_Datapool->c_Name.c_str());
      }
   }
   if (mu32_TOOL_TIP_MAXIMUM_ITEMS < orc_Indices.size())
   {
      orc_Content +=
         static_cast<QString>("+%1\n").arg(orc_Indices.size() - static_cast<uintn>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort source vector descending ( Sorting steps are done for the target vector in sync)

   \param[in,out]  orc_Source    Unsorted source indices
   \param[in,out]  orc_Target    Unsorted target indices
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

   \param[in]  orc_Indices    Input vector to evaluate

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

   \param[in,out]  orc_IndicesTmp   Unsorted indices
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

   \param[in]  oru32_NodeIndex   Node index

   \return
   Node id maximum
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SdUtil::h_GetNodeIdMaximum(const uint32 & oru32_NodeIndex)
{
   uint8 u8_Retval = mu8_MAX_NODE_ID_OS;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCNodeProperties & rc_NodeProp = pc_Node->c_Properties;

      if ((rc_NodeProp.e_DiagnosticServer == C_OSCNodeProperties::eDS_OPEN_SYDE) &&
          (rc_NodeProp.e_FlashLoader == C_OSCNodeProperties::eFL_OPEN_SYDE))
      {
         //openSYDE max id
         u8_Retval = mu8_MAX_NODE_ID_OS;
      }
      else
      {
         //STW FL / KEFEX (+ no diag protocol support) max id
         u8_Retval = mu8_MAX_NODE_ID_STW;
      }
   }
   else
   {
      tgl_assert(false);
   }

   return u8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \param[in]  orc_ComboBox      Combo with node interfaces
   \param[in]  oru32_NodeIndex   Corresponding node index
   \param[in]  ore_Type          Corresponding bus type

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
      const QString c_SelectedName = orc_ComboBox.currentText();

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

   \param[in]      orc_Node                  Node
   \param[in]      ore_BusType               Bus type
   \param[in,out]  opc_ComboBox              ComboBox for using the node interfaces
   \param[in]      ors32_SpecialInterface    Special interface to allow and use as default, even if already connected

   \return
   True  At least one radio button is intractable
   False No interaction possible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_InitNodeInterfaceComboBox(const C_OSCNode & orc_Node, const C_OSCSystemBus::E_Type & ore_BusType,
                                           QComboBox * const opc_ComboBox, const sint32 & ors32_SpecialInterface)
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

         if (rc_Interface.GetBusConnected() == true)
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

   if (q_Retval == false)
   {
      opc_ComboBox->addItem(C_GtGetText::h_GetText("No interface available"));
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all used node ids for current bus

   \param[in]  oru32_BusIndex          Bus index
   \param[in]  oru32_SpecialNodeIndex  Special node index to skip
   \param[in]  ors32_SpecialInterface  Special interface to skip

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
                     if ((rc_CurComInterface.GetBusConnected() == true) &&
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

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  os32_SpecialApplicationIndex  Special application to skip

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

   \param[in]  os32_SpecialBusIndex    Special bus to skip

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
/*! \brief   Get the last bytes of all IP Addresses used on the given bus.
             Only STW default IP addresses (starting with 192.168.0.X) are considered.

   \param[in]  oru32_BusIndex            Bus index of bus to look at
   \param[in]  oru32_SpecialNodeIndex    Special node index to skip
   \param[in]  ors32_SpecialInterface    Special interface to skip

   \return
   Vector of last bytes of IP addresses
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SdUtil::h_GetUsedIpAddressesForBusUniqueAndSortedAscending(const uint32 & oru32_BusIndex,
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
                  //Check special handling, skip own interface
                  if (((static_cast<uint8>(ors32_SpecialInterface) != rc_CurComInterface.u8_InterfaceNumber) ||
                       (oru32_SpecialNodeIndex != c_NodeIndexes[u32_ItNode])) ||
                      (pc_Bus->e_Type != rc_CurComInterface.e_InterfaceType))
                  {
                     if ((rc_CurComInterface.GetBusConnected() == true) &&
                         (rc_CurComInterface.u32_BusIndex == oru32_BusIndex))
                     {
                        //only consider addresses which match the STW default in their first 3 bytes
                        if ((rc_CurComInterface.c_Ip.au8_IpAddress[0] ==
                             C_OSCNodeComInterfaceSettings::C_IpAddress::hu8_IP_FIRST_BYTE) &&
                            (rc_CurComInterface.c_Ip.au8_IpAddress[1] ==
                             C_OSCNodeComInterfaceSettings::C_IpAddress::hu8_IP_SECOND_BYTE) &&
                            (rc_CurComInterface.c_Ip.au8_IpAddress[2] ==
                             C_OSCNodeComInterfaceSettings::C_IpAddress::hu8_IP_THIRD_BYTE))
                        {
                           c_Retval.push_back(rc_CurComInterface.c_Ip.au8_IpAddress[3]);
                        }
                     }
                  }
               }
            }
         }
      }
   }
   //0 and 1 are reserved, therefore used. Always!
   c_Retval.push_back(0U);
   c_Retval.push_back(1U);

   return C_Uti::h_UniquifyAndSortAscending(c_Retval);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets all IP Addresses used on the given bus. (Complete addresses, not just last bytes)

   \param[in]  oru32_BusIndex            Bus index of bus to look at
   \param[in]  oru32_SpecialNodeIndex    Special node index to skip
   \param[in]  ors32_SpecialInterface    Special interface to skip

   \return
   Vector of IP addresses (each stored in a separate vector)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::vector<uint8> > C_SdUtil::h_GetAllUsedIpAddressesForBus(const uint32 & oru32_BusIndex,
                                                                         const uint32 & oru32_SpecialNodeIndex,
                                                                         const sint32 & ors32_SpecialInterface)
{
   std::vector<std::vector<uint8> > c_Retval;

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
                  //Check special handling, skip own interface
                  if (((static_cast<uint8>(ors32_SpecialInterface) != rc_CurComInterface.u8_InterfaceNumber) ||
                       (oru32_SpecialNodeIndex != c_NodeIndexes[u32_ItNode])) ||
                      (pc_Bus->e_Type != rc_CurComInterface.e_InterfaceType))
                  {
                     if ((rc_CurComInterface.GetBusConnected() == true) &&
                         (rc_CurComInterface.u32_BusIndex == oru32_BusIndex))
                     {
                        std::vector<uint8> c_Tmp;
                        c_Tmp.push_back(rc_CurComInterface.c_Ip.au8_IpAddress[0]);
                        c_Tmp.push_back(rc_CurComInterface.c_Ip.au8_IpAddress[1]);
                        c_Tmp.push_back(rc_CurComInterface.c_Ip.au8_IpAddress[2]);
                        c_Tmp.push_back(rc_CurComInterface.c_Ip.au8_IpAddress[3]);
                        c_Retval.push_back(c_Tmp);
                     }
                  }
               }
            }
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt message to protocol restrictions

   \param[in,out]  orc_Message            Message to adapt
   \param[in,out]  opc_UiMessage          Optional Ui Message to adapt
   \param[in]      oe_Type                Protocol type
   \param[in,out]  opc_AdaptationInfos    Optional report about adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_AdaptMessageToProtocolType(C_OSCCanMessage & orc_Message, C_PuiSdNodeCanMessage * const opc_UiMessage,
                                            const C_OSCCanProtocol::E_Type oe_Type,
                                            QStringList * const opc_AdaptationInfos)
{
   QStringList c_Info;

   if ((oe_Type == C_OSCCanProtocol::eECES) || (oe_Type == C_OSCCanProtocol::eCAN_OPEN_SAFETY))
   {
      // general changes for safety protocols

      //Tx method always cyclic
      if (orc_Message.e_TxMethod != C_OSCCanMessage::eTX_METHOD_CYCLIC)
      {
         c_Info.push_back(
            static_cast<QString>(C_GtGetText::h_GetText("Message transmission type changed from \"%1\" to \"%2\" due "
                                                        "to ECeS/ECoS protocol restrictions.")).
            arg(C_SdUtil::h_ConvertTxMethodToName(orc_Message.e_TxMethod)).
            arg(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_CYCLIC)));
         orc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;

         // Adapt the necessary cyclic information
         if ((orc_Message.u32_CycleTimeMs == 0U) ||
             (orc_Message.u32_TimeoutMs == 0U))
         {
            orc_Message.u32_CycleTimeMs = C_PuiSdUtil::h_GetDefaultMessageCycleTime();
            orc_Message.u32_TimeoutMs = C_PuiSdUtil::h_GetMessageAutoTimeoutTime(orc_Message.u32_CycleTimeMs);

            if (opc_UiMessage != NULL)
            {
               opc_UiMessage->e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO;
            }
         }
      }

      // specific changes
      switch (oe_Type)
      {
      case C_OSCCanProtocol::eECES:
         //DLC fix 8
         if (orc_Message.u16_Dlc != 8U)
         {
            c_Info.push_back(
               static_cast<QString>(C_GtGetText::h_GetText("Message DLC changed from %1 to 8 due to ECeS/ECoS "
                                                           "protocol restrictions.")).arg(orc_Message.u16_Dlc));
            orc_Message.u16_Dlc = 8U;
         }
         break;
      case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
         //Extended type
         if (orc_Message.q_IsExtended == true)
         {
            c_Info.push_back(C_GtGetText::h_GetText("Message extended flag changed from \"extended\" to \"standard\" "
                                                    "due to ECoS protocol restrictions."));
            orc_Message.q_IsExtended = false;
         }
         //CAN ID
         if ((orc_Message.u32_CanId < mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN) ||
             (orc_Message.u32_CanId > mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX))
         {
            c_Info.push_back(
               static_cast<QString>(C_GtGetText::h_GetText("Message ID changed from 0x%1 to 0x%2 due to ECoS protocol "
                                                           "restrictions. Allowed IDs: 0x%3-0x%2.")).
               arg(orc_Message.u32_CanId, 0, 16).
               arg(mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX, 0, 16).
               arg(mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN, 0, 16));
            orc_Message.u32_CanId = mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX;
         }
         break;
      case C_OSCCanProtocol::eLAYER2:
      default:
         //No restrictions
         break;
      }

      //Adapt signals
      for (std::vector<C_OSCCanSignal>::iterator c_SignalIt = orc_Message.c_Signals.begin();
           c_SignalIt != orc_Message.c_Signals.end(); ++c_SignalIt)
      {
         h_AdaptSignalToProtocolType(*c_SignalIt, oe_Type, &c_Info);
      }
      c_Info.removeDuplicates();
   }

   if (opc_AdaptationInfos != NULL)
   {
      opc_AdaptationInfos->append(c_Info);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt signal to protocol restrictions

   \param[in,out]  orc_Signal             Signal to adapt
   \param[in]      oe_Type                Protocol type
   \param[in,out]  opc_AdaptationInfos    Optional report about adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_AdaptSignalToProtocolType(C_OSCCanSignal & orc_Signal, const C_OSCCanProtocol::E_Type oe_Type,
                                           QStringList * const opc_AdaptationInfos)
{
   QStringList c_Info;

   if ((oe_Type == C_OSCCanProtocol::eECES) || (oe_Type == C_OSCCanProtocol::eCAN_OPEN_SAFETY))
   {
      //Remove multiplex information
      if (orc_Signal.e_MultiplexerType != C_OSCCanSignal::eMUX_DEFAULT)
      {
         c_Info.append(C_GtGetText::h_GetText("Multiplex information removed from signal because this is not supported "
                                              "in ECeS/ECoS protocols."));
         orc_Signal.e_MultiplexerType = C_OSCCanSignal::eMUX_DEFAULT;
         orc_Signal.u16_MultiplexValue = 0U;
      }

      // Keep reserved bytes free
      if (oe_Type == C_OSCCanProtocol::eECES)
      {
         if (orc_Signal.u16_ComBitStart >= mu32_PROTOCOL_ECES_SIGNALCOUNT_MAX)
         {
            c_Info.append(static_cast<QString>(C_GtGetText::h_GetText("Start bit of signal set from %1 to 0 because "
                                                                      "of reserved bytes in ECeS protocol.")).
                          arg(orc_Signal.u16_ComBitStart));
            orc_Signal.u16_ComBitStart = 0;
         }
      }
   }

   if (opc_AdaptationInfos != NULL)
   {
      opc_AdaptationInfos->append(c_Info);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt used ids string handling

   \param[in]  orc_UsedIds    Unused ids (unique and sorted ascending)
   \param[in]  orc_ItemName   Item name
   \param[in]  orc_ItemType   Item type
   \param[in]  oq_SkipItem    Flag to skip item

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

      for (uint32 u32_ItBusId = 0; u32_ItBusId < static_cast<uint32>(orc_UsedIds.size() - 1UL); ++u32_ItBusId)
      {
         c_BusIds += static_cast<QString>("%1,").arg(orc_UsedIds[u32_ItBusId]);
      }
      //Append last one without comma
      c_BusIds += QString::number(orc_UsedIds[orc_UsedIds.size() - 1]);

      if (oq_SkipItem == true)
      {
         c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Already used IDs: %1")).arg(c_BusIds);
      }
      else
      {
         c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Already used IDs on %3 %1: %2")).arg(orc_ItemName).arg(
            c_BusIds).arg(orc_ItemType);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Builds a string which contains all used IP addresses separated with new line.

   \param[in]      orc_UsedIps    Used IP addresses
   \param[in]      orc_ItemName   Item name (e.g. bus name)
   \param[in]      orc_ItemType   Item type (e.g. "bus" or "node")
   \param[in]      oq_SkiptItem   Flag to skip item (default = false)

   \return
   String with already used IP addresses
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_InitUsedIpsString(const std::vector<std::vector<uint8> > & orc_UsedIps,
                                      const QString & orc_ItemName, const QString & orc_ItemType,
                                      const bool oq_SkiptItem)
{
   QString c_Retval;

   if (orc_UsedIps.size() > 0UL)
   {
      QString c_BusIps;
      for (uint16 u16_ItIp = 0; u16_ItIp < (orc_UsedIps.size() - 1UL); ++u16_ItIp)
      {
         c_BusIps += static_cast<QString>("%1,\n").arg(C_SdUtil::h_IpAddressAsString(orc_UsedIps[u16_ItIp]));
      }
      //append last one without comma & new line
      c_BusIps += C_SdUtil::h_IpAddressAsString(orc_UsedIps[orc_UsedIps.size() - 1]);

      if (oq_SkiptItem == true)
      {
         c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Already used IPs: %1")).arg(c_BusIps);
      }
      else
      {
         c_Retval =
            static_cast<QString>(C_GtGetText::h_GetText("Already used IPs on %3 %1:\n%2")).arg(orc_ItemName).arg(
               c_BusIps).arg(orc_ItemType);
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Converts an IP address represented as a byte-vector to a string

   \param[in]       orc_Ip     Vector with 4 bytes of an IP address

   \return
   IP as string with point separators
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_IpAddressAsString(const std::vector<uint8> & orc_Ip)
{
   QString c_Retval;

   tgl_assert(orc_Ip.size() == 4);
   if (orc_Ip.size() == 4)
   {
      for (uint8 u8_It = 0; u8_It < (orc_Ip.size() - 1UL); ++u8_It)
      {
         c_Retval += static_cast<QString>("%1.").arg(static_cast<uint32>(orc_Ip[u8_It]));
      }
      //append last one without point
      c_Retval += QString::number(static_cast<uint32>(orc_Ip[orc_Ip.size() - 1]));
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function generates node id or last byte for IP address depending on oq_GenerateId

   \param[in]  orc_Interfaces                 Node interfaces
   \param[in]  orc_UsedNodeProperties         Already used node properties (id/ip: unique and sorted ascending)
   \param[in]  ors32_SpecialInterface         Special interface to use as default
   \param[in]  oq_GenerateId                  True: generate ID
                                              False: generate last byte of IP

   \return
   Node ID or IP proposal (Always in range but may be invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdUtil::h_GetNextFreeNodeProperty(const std::vector<C_OSCNodeComInterfaceSettings> & orc_Interfaces,
                                           const std::vector<uint32> & orc_UsedNodeProperties,
                                           const sint32 & ors32_SpecialInterface, const bool oq_GenerateId)
{
   uint32 u32_Retval = 0;

   if (ors32_SpecialInterface >= 0)
   {
      for (uint32 u32_ItInterface = 0; u32_ItInterface < orc_Interfaces.size(); ++u32_ItInterface)
      {
         const C_OSCNodeComInterfaceSettings & rc_CurComIf = orc_Interfaces[u32_ItInterface];
         if (rc_CurComIf.u8_InterfaceNumber == static_cast<uint8>(ors32_SpecialInterface))
         {
            if (oq_GenerateId)
            {
               u32_Retval = rc_CurComIf.u8_NodeID;
            }
            else
            {
               u32_Retval = rc_CurComIf.c_Ip.au8_IpAddress[3];
            }
         }
      }
   }
   else
   {
      for (uint32 u32_It = 0; u32_It < orc_UsedNodeProperties.size(); ++u32_It)
      {
         if (u32_Retval == orc_UsedNodeProperties[u32_It])
         {
            ++u32_Retval;
         }
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if node has at least one connection of this bus type

   \param[in]  orc_Node    Node for check
   \param[in]  oe_Type     Bus type

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

   \param[in]  orc_ComInterfaces    Com interfaces
   \param[in]  ore_BusType          Bus type

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
         if (rc_Interface.GetBusConnectedRawValue() == false)
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

   \param[in]  orc_ComInterfaces    Com interfaces
   \param[in]  ore_BusType          Bus type

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
         if (rc_Interface.GetBusConnectedRawValue() == true)
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

   The total number of datapools is bounded above by C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE.

   \param[in]  oru32_NodeIndex      node index
   \param[in]  orq_AlreadyChecked   for pre-checking cases (true: we know that the number exceeds the maximum)
   \param[in]  opc_Parent           parent widget

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_CheckDatapoolNumber(const uint32 & oru32_NodeIndex, const bool & orq_AlreadyChecked,
                                     QWidget * const opc_Parent)
{
   bool q_Return = false;

   if (orq_AlreadyChecked == true)
   {
      q_Return = true;
   }
   else
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);
      if (pc_Node != NULL)
      {
         // is enough space available
         if (pc_Node->c_DataPools.size() < C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE)
         {
            // all OK
            q_Return = true;
         }
      }
   }

   if (q_Return == false)
   {
      C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool add"));
      c_MessageBox.SetDescription(
         static_cast<QString>(C_GtGetText::h_GetText("The allowed maximum number of Datapools is %1.")).
         arg(C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE));
      c_MessageBox.SetCustomMinHeight(180, 180);
      c_MessageBox.Execute();
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error tool tip for node

   \param[in]   oru32_NodeIndex     Node index
   \param[out]  orc_Text            Tool tip text
   \param[out]  orq_ErrorDetected   Flag if any errors detected
   \param[in]   oq_CheckAll         Check all

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdUtil::h_GetErrorToolTipNode(const uint32 & oru32_NodeIndex, QString & orc_Text, bool & orq_ErrorDetected,
                                       const bool oq_CheckAll)
{
   bool q_IsMulti = C_OSCNodeSquad::h_CheckIsMultiDevice(oru32_NodeIndex,
                                                         C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_NodeSquads);

   std::vector<uint32> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
      oru32_NodeIndex);
   sint32 s32_Retval = C_NO_ERR;

   if (!oq_CheckAll)
   {
      //Only check one node
      q_IsMulti = false;
      c_NodeIndices.clear();
      c_NodeIndices.push_back(oru32_NodeIndex);
   }

   //Starting value
   orq_ErrorDetected = false;

   for (uint32 u32_ItNode = 0UL; (u32_ItNode < c_NodeIndices.size()) && (s32_Retval == C_NO_ERR); ++u32_ItNode)
   {
      bool q_NameConflict;
      bool q_NameEmpty;
      bool q_NodeIdInvalid;
      bool q_IpInvalid;

      bool q_DatapoolNvmSizeConflict;
      bool q_DatapoolNvmOverlapConflict;
      const bool q_DataPoolNvmConflict =
         C_PuiSdHandler::h_GetInstance()->CheckNodeNvmDataPoolsSizeConflict(c_NodeIndices[u32_ItNode],
                                                                            &q_DatapoolNvmSizeConflict,
                                                                            &q_DatapoolNvmOverlapConflict);
      bool q_DataPoolsInvalid;
      bool q_ApplicationsInvalid;
      bool q_DomainsInvalid;

      std::vector<uint32> c_InvalidInterfaceIndices;
      std::vector<uint32> c_InvalidDataPoolIndices;
      std::vector<uint32> c_InvalidApplicationIndices;
      std::vector<uint32> c_InvalidDomainIndices;
      s32_Retval = C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().CheckErrorNode(
         c_NodeIndices[u32_ItNode], &q_NameConflict, &q_NameEmpty, &q_NodeIdInvalid, &q_IpInvalid, &q_DataPoolsInvalid,
         &q_ApplicationsInvalid,
         &q_DomainsInvalid, true, &c_InvalidInterfaceIndices, &c_InvalidDataPoolIndices, &c_InvalidApplicationIndices,
         &c_InvalidDomainIndices);
      if (s32_Retval == C_NO_ERR)
      {
         if (((((((q_NameConflict == true) || (q_NodeIdInvalid == true) || (q_IpInvalid == true)) ||
                 (q_DataPoolsInvalid == true)) ||
                (q_ApplicationsInvalid == true)) || (q_DomainsInvalid == true)) || (q_DataPoolNvmConflict == true)) ||
             (q_NameEmpty == true))
         {
            if (q_IsMulti)
            {
               const C_OSCNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndices[u32_ItNode]);
               if (pc_Node != NULL)
               {
                  orc_Text += pc_Node->c_Properties.c_Name.c_str();
               }
               orc_Text += "\n";
            }
            orq_ErrorDetected = true;
            if (((q_NameConflict == true) || (q_NodeIdInvalid == true) || (q_IpInvalid == true)) ||
                (q_NameEmpty == true))
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
                     orc_Text += static_cast<QString>(C_GtGetText::h_GetText("%1 duplicate node IDs detected.\n")).arg(
                        c_InvalidInterfaceIndices.size());
                  }
               }
               if (q_IpInvalid == true)
               {
                  if (c_InvalidInterfaceIndices.size() == 1UL)
                  {
                     orc_Text += C_GtGetText::h_GetText("Duplicate IP address detected.\n");
                  }
                  else
                  {
                     orc_Text +=
                        static_cast<QString>(C_GtGetText::h_GetText("%1 duplicate IP addresses detected.\n")).arg(
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
                     c_NodeIndices[u32_ItNode], c_InvalidApplicationIndices[u32_ItAppl]);
                  if (pc_Appl != NULL)
                  {
                     orc_Text += static_cast<QString>("%1\n").arg(pc_Appl->c_Name.c_str());
                  }
               }
               if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidApplicationIndices.size())
               {
                  orc_Text += static_cast<QString>("+%1\n").arg(
                     c_InvalidApplicationIndices.size() - static_cast<sintn>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
               }
               orc_Text += "\n";
            }
            if ((q_DataPoolsInvalid == true) || (q_DataPoolNvmConflict == true))
            {
               QString c_Heading;
               QString c_Content;
               C_SdUtil::h_GetErrorToolTipDataPools(c_NodeIndices[u32_ItNode], c_InvalidDataPoolIndices,
                                                    q_DatapoolNvmSizeConflict,
                                                    q_DatapoolNvmOverlapConflict, c_Heading, c_Content);
               orc_Text += c_Heading + "\n";
               orc_Text += c_Content;
               orc_Text += "\n";
            }
            if (q_DomainsInvalid == true)
            {
               orc_Text += C_GtGetText::h_GetText("Invalid HALC Domains:\n");
               for (uint32 u32_ItDomains = 0;
                    (u32_ItDomains < c_InvalidDomainIndices.size()) && (u32_ItDomains < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                    ++u32_ItDomains)
               {
                  const C_OSCHalcConfigDomain * const pc_Domain =
                     C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(c_NodeIndices[u32_ItNode],
                                                                                   c_InvalidDomainIndices[u32_ItDomains]);
                  if (pc_Domain != NULL)
                  {
                     orc_Text += static_cast<QString>("%1\n").arg(pc_Domain->c_Name.c_str());
                  }
               }
               if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidDomainIndices.size())
               {
                  orc_Text +=
                     static_cast<QString>("+%1\n").arg(c_InvalidDomainIndices.size() -
                                                       static_cast<sintn>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
               }
               orc_Text += "\n";
            }
         }
      }
      else
      {
         orq_ErrorDetected = true;
         orc_Text = C_GtGetText::h_GetText("Unknown");
      }
   }

   if (orc_Text.isEmpty())
   {
      orc_Text = C_GtGetText::h_GetText("None");
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error tool tip for bus

   \param[in]   oru32_BusIndex   Bus index
   \param[out]  orc_Text         Tool tip text
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
            orc_Text +=
               static_cast<QString>("+%1\n").
               arg(c_InvalidNodesForBitRate.size() - static_cast<sintn>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
         }
      }
      orc_Text += "\n";
   }
   if (c_InvalidProtocols.size() > 0)
   {
      orc_Text += C_GtGetText::h_GetText("COMM protocols with invalid content:\n");
      for (uint32 u32_ItP = 0; u32_ItP < c_InvalidProtocols.size(); ++u32_ItP)
      {
         orc_Text +=
            static_cast<QString>("%1\n").arg(C_PuiSdUtil::h_ConvertProtocolTypeToString(c_InvalidProtocols[u32_ItP]));
      }
      orc_Text += "\n";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for datapool list.

   \param[in]  oru32_NodeIndex      index of node where the list belongs to
   \param[in]  oru32_DatapoolIndex  index of datapool where the list belongs to
   \param[in]  oru32_ListIndex      index of list for which the tooltip is requested

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
   const uint32 u32_DatasetNumber = pc_DpList->c_DataSets.size();

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

   \param[in]  orc_NodeDatapoolListElementId    indexes of element for which the tooltip is requested
   \param[in]  orc_AdditionalInformation        Optional additional information string which will get inserted after comment
                                                (no "\n" at end necessary)

   \return
      content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_GetToolTipContentDpListElement(const C_OSCNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
                                                   const QString & orc_AdditionalInformation)
{
   QString c_ToolTipContent = "";

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
         c_ToolTipContent.append(C_PuiSdHandlerNodeLogic::h_GetElementTypeName(pc_Datapool->e_Type) +
                                 C_GtGetText::h_GetText(" Properties: "));
         c_ToolTipContent.append("\n");

         //value type
         c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Value type: "));
         if (pc_UIElement->q_InterpretAsString == false)
         {
            c_ToolTipContent.append(C_SdNdeDpUtil::h_ConvertContentTypeToString(
                                       pc_DpListElement->c_Value.GetType()));
         }
         else
         {
            c_ToolTipContent.append(C_GtGetText::h_GetText("string"));
         }
         c_ToolTipContent.append("\n");

         // array size
         c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Array size: "));
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
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Auto min/max: "));
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
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Min: "));
            C_SdNdeDpContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_MinValue,
                                                            pc_DpListElement->f64_Factor,
                                                            pc_DpListElement->f64_Offset, c_HelpVector);
            QString c_HelpString = "";
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
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Max: "));
            c_HelpVector.clear();
            C_SdNdeDpContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_MaxValue,
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
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Factor: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->f64_Factor));
            c_ToolTipContent.append("\n");

            //offset
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Offset: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->f64_Offset));
            c_ToolTipContent.append("\n");

            // unit
            c_HelpString = pc_DpListElement->c_Unit.c_str();
            if (c_HelpString.isEmpty() == false)
            {
               c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Unit: "));
               c_ToolTipContent.append(c_HelpString);
               c_ToolTipContent.append("\n");
            }
         }

         // dataset values
         if (pc_DpList->c_DataSets.size() > 0)
         {
            for (uint32 u32_PosDataset = 0; u32_PosDataset < pc_DpList->c_DataSets.size(); u32_PosDataset++)
            {
               c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Dataset - "));
               c_ToolTipContent.append(pc_DpList->c_DataSets[u32_PosDataset].c_Name.c_str());
               c_ToolTipContent.append(": ");
               c_HelpVector.clear();
               C_SdNdeDpContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_DataSetValues[u32_PosDataset],
                                                               pc_DpListElement->f64_Factor,
                                                               pc_DpListElement->f64_Offset, c_HelpVector);
               QString c_HelpString = "";
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
         c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Access: "));
         c_ToolTipContent.append(C_SdNdeDpUtil::h_ConvertElementAccessToString(pc_DpListElement->e_Access));
         c_ToolTipContent.append("\n");

         // case NVM
         if ((pc_Datapool->e_Type == C_OSCNodeDataPool::eNVM) ||
             (pc_Datapool->e_Type == C_OSCNodeDataPool::eHALC_NVM))
         {
            // data size
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Data size: "));
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
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Address: "));
            c_ToolTipContent.append(QString::number(pc_DpListElement->u32_NvMStartAddress -
                                                    pc_Datapool->u32_NvMStartAddress));
            c_ToolTipContent.append(" (");
            c_ToolTipContent.append(QString::number(pc_DpListElement->u32_NvMStartAddress));
            c_ToolTipContent.append(")");
         }
         // case DIAG or HALC (case COMM is handled at the beginning)
         else
         {
            // event call
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Event call: "));
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

   \param[in]  orc_MessageId  Message id belonging to the signal for which the tooltip is requested

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

   \param[in]  orc_MessageId              message id belonging to the signal for which the tooltip is requested
   \param[in]  oru32_SignalIndex          index of signal for which the tooltip is requested
   \param[in]  orc_AdditionalInformation  Optional additional information string which will get inserted after comment
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
      c_AutoMinMaxInfo.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Auto min/max: "));
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

   \param[in,out]  orc_IndicesTmp      Unsorted indices
   \param[in,out]  orc_OSCContentTmp   Unsorted OSC content
   \param[in,out]  orc_UIContentTmp    Unsorted UI content

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
void C_SdUtil::h_SortIndicesDescendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                              std::vector<T> & orc_OSCContentTmp, std::vector<U> & orc_UIContentTmp)
{
   if (h_CheckSortedDescending(orc_IndicesTmp) == false)
   {
      std::vector<stw_types::uint32> c_IndicesTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<T> c_OSCContentTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<U> c_UIContentTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<stw_types::sint32> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      c_OSCContentTmp.reserve(orc_OSCContentTmp.size());
      c_UIContentTmp.reserve(orc_UIContentTmp.size());
      for (stw_types::uint32 u32_ItIndex = c_IndexMap.size(); u32_ItIndex > 0; --u32_ItIndex)
      {
         if (c_IndexMap[static_cast<uintn>(u32_ItIndex - 1U)] >= 0)
         {
            const stw_types::uint32 u32_CurIndex =
               static_cast<stw_types::uint32>(c_IndexMap[static_cast<uintn>(u32_ItIndex - 1U)]);
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

   \param[in,out]  orc_IndicesTmp      Unsorted indices
   \param[in,out]  orc_OSCContentTmp   Unsorted OSC content
   \param[in,out]  orc_UIContentTmp    Unsorted UI content

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
void C_SdUtil::h_SortIndicesAscendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                             std::vector<T> & orc_OSCContentTmp, std::vector<U> & orc_UIContentTmp)
{
   if (C_Uti::h_CheckSortedAscending(orc_IndicesTmp) == false)
   {
      std::vector<stw_types::uint32> c_IndicesTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<T> c_OSCContentTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
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
