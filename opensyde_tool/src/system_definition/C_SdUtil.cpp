//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition utility functions (implementation)

   System definition utility functions

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <set>
#include <limits>

#include "stwerrors.hpp"
#include "C_SdUtil.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeRabProperties.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"
#include "C_OscNodeSquad.hpp"
#include "C_SdTooltipUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdBueUnoBusProtNodeConnectCommand.hpp"
#include "C_SdBueUnoBusProtNodeConnectAndCreateCommand.hpp"
#include "C_SdBueUnoBusProtNodeDisconnectCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
int32_t C_SdUtil::h_GetNames(const std::vector<uint32_t> & orc_NodeIndices,
                             const std::vector<uint32_t> & orc_InterfaceIndices, std::vector<QString> & orc_Names,
                             const bool oq_NameWithInterfaceAlways,
                             const std::vector<uint32_t> * const opc_DatapoolIndices,
                             std::vector<QString> * const opc_DatapoolNames)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_NodeIndices.size() == orc_InterfaceIndices.size())
   {
      QString c_TmpName;
      // set for all nodes with more than one interface
      std::set<uint32_t> c_SetNodesWithMultipleItf;

      // detect all nodes with more than one used interface if the interface name should not be visible always
      if ((oq_NameWithInterfaceAlways == false) &&
          (orc_NodeIndices.size() > 1))
      {
         for (uint32_t u32_NodeCounter = 0U; u32_NodeCounter < static_cast<uint32_t>(orc_NodeIndices.size() - 1UL);
              ++u32_NodeCounter)
         {
            for (uint32_t u32_NextNodeCounter = u32_NodeCounter + 1U;
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
      for (uint32_t u32_NodeCounter = 0U; (u32_NodeCounter < orc_NodeIndices.size()) && (s32_Retval == C_NO_ERR);
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
            const C_OscNodeDataPool * const pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
               orc_NodeIndices[u32_NodeCounter], (*opc_DatapoolIndices)[u32_NodeCounter]);

            if (pc_Dp != NULL)
            {
               //lint -e{413} //false positive; opc_DatapoolNames is checked to be != NULL a few lines above
               opc_DatapoolNames->emplace_back(pc_Dp->c_Name.c_str());
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
int32_t C_SdUtil::h_GetName(const uint32_t & oru32_NodeIndex, QString & orc_Name)
{
   int32_t s32_Retval = C_RANGE;

   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(oru32_NodeIndex);

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
int32_t C_SdUtil::h_GetName(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_InterfaceIndex, QString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      // check which interfaces are connected to the bus
      if (oru32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OscNodeComInterfaceSettings & rc_ComInterface =
            pc_Node->c_Properties.c_ComInterfaces[oru32_InterfaceIndex];
         // create the shown string with the interface name
         orc_Name = pc_Node->c_Properties.c_Name.c_str();
         orc_Name += " (" +
                     C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN, rc_ComInterface.u8_InterfaceNumber) + ")";
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
QString C_SdUtil::h_ConvertTxMethodToName(const C_OscCanMessage::E_TxMethodType & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OscCanMessage::eTX_METHOD_CYCLIC:
      c_Retval = C_GtGetText::h_GetText("Cyclic");
      break;
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0:
      c_Retval = C_GtGetText::h_GetText("Type 0 - synchronous transmission after next SYNC and change");
      break;
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240:
      c_Retval = C_GtGetText::h_GetText("Type 1 to 240 - synchronous transmission after 1st to 240th SYNC");
      break;
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254:
      c_Retval = C_GtGetText::h_GetText("Type 254 - asynchronous manufacturer specific");
      break;
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255:
      c_Retval = C_GtGetText::h_GetText("Type 255 - asynchronous device specific");
      break;
   case C_OscCanMessage::eTX_METHOD_ON_CHANGE:
      c_Retval = C_GtGetText::h_GetText("On Change");
      break;
   case C_OscCanMessage::eTX_METHOD_ON_EVENT:
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
QString C_SdUtil::h_ConvertByteOrderToName(const C_OscCanSignal::E_ByteOrderType & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OscCanSignal::eBYTE_ORDER_INTEL:
      c_Retval = C_GtGetText::h_GetText("Intel");
      break;
   case C_OscCanSignal::eBYTE_ORDER_MOTOROLA:
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
void C_SdUtil::h_GetErrorToolTipDataPools(const uint32_t ou32_NodeIndex, const std::vector<uint32_t> & orc_Indices,
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
   for (uint32_t u32_ItDataPool = 0;
        (u32_ItDataPool < orc_Indices.size()) && (u32_ItDataPool < mu32_TOOL_TIP_MAXIMUM_ITEMS);
        ++u32_ItDataPool)
   {
      const C_OscNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(ou32_NodeIndex,
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
         static_cast<QString>("+%1\n").arg(orc_Indices.size() - static_cast<uint32_t>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort source vector descending ( Sorting steps are done for the target vector in sync)

   \param[in,out]  orc_Source    Unsorted source indices
   \param[in,out]  orc_Target    Unsorted target indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_SortSourceDescending(std::vector<uint32_t> & orc_Source, std::vector<uint32_t> & orc_Target)
{
   if (h_CheckSortedDescending(orc_Source) == false)
   {
      std::vector<uint32_t> c_NewSource;
      std::vector<uint32_t> c_NewTarget;
      const std::vector<int32_t> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_Source);
      c_NewSource.reserve(orc_Source.size());
      c_NewTarget.reserve(orc_Target.size());
      //Reposition
      for (uint32_t u32_It = c_IndexMap.size(); u32_It > 0; --u32_It)
      {
         const uint32_t u32_PreviousIndex = u32_It - 1U;
         if (c_IndexMap[u32_PreviousIndex] >= 0)
         {
            const uint32_t u32_CurIndex = static_cast<uint32_t>(c_IndexMap[u32_PreviousIndex]);
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
bool C_SdUtil::h_CheckSortedDescending(const std::vector<uint32_t> & orc_Indices)
{
   bool q_Retval = true;

   if (orc_Indices.size() > 1)
   {
      uint32_t u32_PrevVal = orc_Indices[0];

      for (uint32_t u32_It = 1; u32_It < orc_Indices.size(); ++u32_It)
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
void C_SdUtil::h_SortIndicesAscending(std::vector<uint32_t> & orc_IndicesTmp)
{
   if (C_Uti::h_CheckSortedAscending(orc_IndicesTmp) == false)
   {
      std::vector<uint32_t> c_IndicesTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<int32_t> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      for (uint32_t u32_ItIndex = 0; u32_ItIndex < c_IndexMap.size(); ++u32_ItIndex)
      {
         if (c_IndexMap[u32_ItIndex] >= 0)
         {
            const uint32_t u32_CurIndex = static_cast<uint32_t>(c_IndexMap[u32_ItIndex]);
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
uint8_t C_SdUtil::h_GetNodeIdMaximum(const uint32_t & oru32_NodeIndex)
{
   uint8_t u8_Retval = mu8_MAX_NODE_ID_OS;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscNodeProperties & rc_NodeProp = pc_Node->c_Properties;

      if ((rc_NodeProp.e_DiagnosticServer == C_OscNodeProperties::eDS_OPEN_SYDE) &&
          (rc_NodeProp.e_FlashLoader == C_OscNodeProperties::eFL_OPEN_SYDE))
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
uint32_t C_SdUtil::h_GetActiveNodeInterface(const QComboBox & orc_ComboBox, const uint32_t & oru32_NodeIndex,
                                            const C_OscSystemBus::E_Type & ore_Type)
{
   uint32_t u32_Retval = 0;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      uint32_t u32_InterfaceCounter;
      const QString c_SelectedName = orc_ComboBox.currentText();

      for (u32_InterfaceCounter = 0; u32_InterfaceCounter <  pc_Node->c_Properties.c_ComInterfaces.size();
           ++u32_InterfaceCounter)
      {
         const C_OscNodeComInterfaceSettings & rc_ComInterface =
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
bool C_SdUtil::h_InitNodeInterfaceComboBox(const C_OscNode & orc_Node, const C_OscSystemBus::E_Type & ore_BusType,
                                           QComboBox * const opc_ComboBox, const int32_t & ors32_SpecialInterface)
{
   bool q_Retval = false;
   bool q_Chosen = false;
   uint32_t u32_Counter;

   opc_ComboBox->clear();

   // Fill the combo box
   for (u32_Counter = 0U; u32_Counter < orc_Node.c_Properties.c_ComInterfaces.size(); ++u32_Counter)
   {
      const C_OscNodeComInterfaceSettings & rc_Interface = orc_Node.c_Properties.c_ComInterfaces[u32_Counter];

      if (rc_Interface.e_InterfaceType == ore_BusType)
      {
         const QString c_Name = C_PuiSdUtil::h_GetInterfaceName(ore_BusType, rc_Interface.u8_InterfaceNumber);

         if (rc_Interface.GetBusConnected() == true)
         {
            if (ors32_SpecialInterface >= 0)
            {
               if (static_cast<uint8_t>(ors32_SpecialInterface) == rc_Interface.u8_InterfaceNumber)
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
std::vector<uint32_t> C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(const uint32_t & oru32_BusIndex,
                                                                               const uint32_t & oru32_SpecialNodeIndex,
                                                                               const int32_t & ors32_SpecialInterface)
{
   std::vector<uint32_t> c_Retval;
   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(oru32_BusIndex);
   if (pc_Bus != NULL)
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;

      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(oru32_BusIndex,
                                                                                         c_NodeIndexes,
                                                                                         c_InterfaceIndexes);
      c_Retval.reserve(c_NodeIndexes.size());
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            const C_OscNode * const pc_CurNode =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes[u32_ItNode]);
            if (pc_CurNode != NULL)
            {
               if (c_InterfaceIndexes[u32_ItNode] < pc_CurNode->c_Properties.c_ComInterfaces.size())
               {
                  const C_OscNodeComInterfaceSettings & rc_CurComInterface =
                     pc_CurNode->c_Properties.c_ComInterfaces[c_InterfaceIndexes[u32_ItNode]];
                  //Check special handling
                  if (((static_cast<uint8_t>(ors32_SpecialInterface) != rc_CurComInterface.u8_InterfaceNumber) ||
                       (oru32_SpecialNodeIndex != c_NodeIndexes[u32_ItNode])) ||
                      (pc_Bus->e_Type != rc_CurComInterface.e_InterfaceType))
                  {
                     if ((rc_CurComInterface.GetBusConnected() == true) &&
                         (rc_CurComInterface.u32_BusIndex == oru32_BusIndex))
                     {
                        c_Retval.push_back(rc_CurComInterface.u8_NodeId);
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
std::vector<uint32_t> C_SdUtil::h_GetUsedProcessIdsForApplicationUniqueAndSortedAscending(const uint32_t ou32_NodeIndex,
                                                                                          const int32_t os32_SpecialApplicationIndex)
{
   std::vector<uint32_t> c_Retval;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
      {
         if ((os32_SpecialApplicationIndex < 0) || (u32_ItApp != static_cast<uint32_t>(os32_SpecialApplicationIndex)))
         {
            const C_OscNodeApplication & rc_CurApplication = pc_Node->c_Applications[u32_ItApp];
            if (rc_CurApplication.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               c_Retval.push_back(static_cast<uint32_t>(rc_CurApplication.u8_ProcessId));
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
std::vector<uint32_t> C_SdUtil::h_GetUsedBusIdsUniqueAndSortedAscending(const int32_t os32_SpecialBusIndex)
{
   std::vector<uint32_t> c_Retval;
   for (uint32_t u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(); ++u32_ItBus)
   {
      if ((os32_SpecialBusIndex < 0) || (u32_ItBus != static_cast<uint32_t>(os32_SpecialBusIndex)))
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(u32_ItBus);
         if (pc_Bus != NULL)
         {
            if (pc_Bus->q_UseableForRouting == true)
            {
               c_Retval.push_back(static_cast<uint32_t>(pc_Bus->u8_BusId));
            }
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
std::vector<uint32_t> C_SdUtil::h_GetUsedIpAddressesForBusUniqueAndSortedAscending(const uint32_t & oru32_BusIndex,
                                                                                   const uint32_t & oru32_SpecialNodeIndex,
                                                                                   const int32_t & ors32_SpecialInterface)
{
   std::vector<uint32_t> c_Retval;

   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(oru32_BusIndex);

   if (pc_Bus != NULL)
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;

      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(oru32_BusIndex,
                                                                                         c_NodeIndexes,
                                                                                         c_InterfaceIndexes);
      c_Retval.reserve(c_NodeIndexes.size());
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            const C_OscNode * const pc_CurNode =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes[u32_ItNode]);
            if (pc_CurNode != NULL)
            {
               if (c_InterfaceIndexes[u32_ItNode] < pc_CurNode->c_Properties.c_ComInterfaces.size())
               {
                  const C_OscNodeComInterfaceSettings & rc_CurComInterface =
                     pc_CurNode->c_Properties.c_ComInterfaces[c_InterfaceIndexes[u32_ItNode]];
                  //Check special handling, skip own interface
                  if (((static_cast<uint8_t>(ors32_SpecialInterface) != rc_CurComInterface.u8_InterfaceNumber) ||
                       (oru32_SpecialNodeIndex != c_NodeIndexes[u32_ItNode])) ||
                      (pc_Bus->e_Type != rc_CurComInterface.e_InterfaceType))
                  {
                     if ((rc_CurComInterface.GetBusConnected() == true) &&
                         (rc_CurComInterface.u32_BusIndex == oru32_BusIndex))
                     {
                        //only consider addresses which match the STW default in their first 3 bytes
                        if ((rc_CurComInterface.c_Ip.au8_IpAddress[0] ==
                             C_OscNodeComInterfaceSettings::C_IpAddress::hu8_IP_FIRST_BYTE) &&
                            (rc_CurComInterface.c_Ip.au8_IpAddress[1] ==
                             C_OscNodeComInterfaceSettings::C_IpAddress::hu8_IP_SECOND_BYTE) &&
                            (rc_CurComInterface.c_Ip.au8_IpAddress[2] ==
                             C_OscNodeComInterfaceSettings::C_IpAddress::hu8_IP_THIRD_BYTE))
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

   \param[in]  oru32_BusIndex          Bus index of bus to look at
   \param[in]  oru32_SpecialNodeIndex  Special node index to skip
   \param[in]  ors32_SpecialInterface  Special interface to skip

   \return
   Vector of IP addresses (each stored in a separate vector)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::vector<uint8_t> > C_SdUtil::h_GetAllUsedIpAddressesForBus(const uint32_t & oru32_BusIndex,
                                                                           const uint32_t & oru32_SpecialNodeIndex,
                                                                           const int32_t & ors32_SpecialInterface)
{
   std::vector<std::vector<uint8_t> > c_Retval;

   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(oru32_BusIndex);

   if (pc_Bus != NULL)
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;

      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(oru32_BusIndex,
                                                                                         c_NodeIndexes,
                                                                                         c_InterfaceIndexes);
      c_Retval.reserve(c_NodeIndexes.size());
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            const C_OscNode * const pc_CurNode =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes[u32_ItNode]);
            if (pc_CurNode != NULL)
            {
               if (c_InterfaceIndexes[u32_ItNode] < pc_CurNode->c_Properties.c_ComInterfaces.size())
               {
                  const C_OscNodeComInterfaceSettings & rc_CurComInterface =
                     pc_CurNode->c_Properties.c_ComInterfaces[c_InterfaceIndexes[u32_ItNode]];
                  //Check special handling, skip own interface
                  if (((static_cast<uint8_t>(ors32_SpecialInterface) != rc_CurComInterface.u8_InterfaceNumber) ||
                       (oru32_SpecialNodeIndex != c_NodeIndexes[u32_ItNode])) ||
                      (pc_Bus->e_Type != rc_CurComInterface.e_InterfaceType))
                  {
                     if ((rc_CurComInterface.GetBusConnected() == true) &&
                         (rc_CurComInterface.u32_BusIndex == oru32_BusIndex))
                     {
                        std::vector<uint8_t> c_Tmp;
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

   \param[in,out]  orc_Message               Message to adapt
   \param[in,out]  opc_UiMessage             Optional Ui Message to adapt
   \param[in,out]  orc_SignalListElements    Datapool list elements for signals
   \param[in]      oe_Type                   Protocol type
   \param[in,out]  opc_AdaptationInfos       Optional report about adaptations
   \param[in]      oq_IncludeSignalUpdate    Flag to control signal update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_AdaptMessageToProtocolType(C_OscCanMessage & orc_Message, C_PuiSdNodeCanMessage * const opc_UiMessage,
                                            std::vector<C_OscNodeDataPoolListElement> & orc_SignalListElements,
                                            const C_OscCanProtocol::E_Type oe_Type,
                                            QStringList * const opc_AdaptationInfos, const bool oq_IncludeSignalUpdate)
{
   QStringList c_Info;

   if ((oe_Type == C_OscCanProtocol::eECES) ||
       (oe_Type == C_OscCanProtocol::eCAN_OPEN_SAFETY))
   {
      // general changes for safety protocols

      //Tx method always cyclic
      if (orc_Message.e_TxMethod != C_OscCanMessage::eTX_METHOD_CYCLIC)
      {
         c_Info.push_back(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "Message transmission type changed from \"%1\" to \"%2\" due "
                                    "to ECeS/ECoS protocol restrictions.")).
            arg(C_SdUtil::h_ConvertTxMethodToName(orc_Message.e_TxMethod)).
            arg(C_SdUtil::h_ConvertTxMethodToName(C_OscCanMessage::eTX_METHOD_CYCLIC)));

         orc_Message.e_TxMethod = C_OscCanMessage::eTX_METHOD_CYCLIC;

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
   }

   if ((oe_Type == C_OscCanProtocol::eECES) ||
       (oe_Type == C_OscCanProtocol::eCAN_OPEN_SAFETY) ||
       (oe_Type == C_OscCanProtocol::eJ1939))
   {
      // specific changes
      switch (oe_Type)
      {
      case C_OscCanProtocol::eECES:
         //DLC fix 8
         if (orc_Message.u16_Dlc != 8U)
         {
            c_Info.push_back(
               static_cast<QString>(C_GtGetText::h_GetText("Message DLC changed from %1 to 8 due to ECeS/ECoS "
                                                           "protocol restrictions.")).arg(orc_Message.u16_Dlc));
            orc_Message.u16_Dlc = 8U;
         }
         break;
      case C_OscCanProtocol::eCAN_OPEN_SAFETY:
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
      case C_OscCanProtocol::eJ1939:
         //DLC fix 8
         if (orc_Message.u16_Dlc != 8U)
         {
            c_Info.push_back(
               static_cast<QString>(C_GtGetText::h_GetText("Message DLC changed from %1 to 8 due to J1939 "
                                                           "protocol restrictions.")).arg(orc_Message.u16_Dlc));
            orc_Message.u16_Dlc = 8U;
         }
         //Extended type
         if (orc_Message.q_IsExtended == false)
         {
            c_Info.push_back(C_GtGetText::h_GetText("Message extended flag changed from \"standard\" to \"extended\" "
                                                    "due to J1939 protocol restrictions."));
            orc_Message.q_IsExtended = true;
         }
         break;
      case C_OscCanProtocol::eCAN_OPEN:
      case C_OscCanProtocol::eLAYER2:
      default:
         //No restrictions
         break;
      }

      if (oq_IncludeSignalUpdate)
      {
         //Adapt signals
         for (std::vector<C_OscCanSignal>::iterator c_SignalIt = orc_Message.c_Signals.begin();
              c_SignalIt != orc_Message.c_Signals.end(); ++c_SignalIt)
         {
            tgl_assert(c_SignalIt->u32_ComDataElementIndex < orc_SignalListElements.size());
            if (c_SignalIt->u32_ComDataElementIndex < orc_SignalListElements.size())
            {
               h_AdaptSignalToProtocolType(*c_SignalIt,
                                           orc_SignalListElements[c_SignalIt->u32_ComDataElementIndex],
                                           oe_Type, &c_Info);
            }
         }
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
   \param[in,out]  orc_SignalListElement  Signal list element to adapt
   \param[in]      oe_Type                Protocol type
   \param[in,out]  opc_AdaptationInfos    Optional report about adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdUtil::h_AdaptSignalToProtocolType(C_OscCanSignal & orc_Signal,
                                           C_OscNodeDataPoolListElement & orc_SignalListElement,
                                           const C_OscCanProtocol::E_Type oe_Type,
                                           QStringList * const opc_AdaptationInfos)
{
   QStringList c_Info;

   if ((oe_Type == C_OscCanProtocol::eECES) || (oe_Type == C_OscCanProtocol::eCAN_OPEN_SAFETY))
   {
      //Remove multiplex information
      if (orc_Signal.e_MultiplexerType != C_OscCanSignal::eMUX_DEFAULT)
      {
         c_Info.append(C_GtGetText::h_GetText("Multiplex information removed from signal because this is not supported "
                                              "in ECeS/ECoS protocols."));
         orc_Signal.e_MultiplexerType = C_OscCanSignal::eMUX_DEFAULT;
         orc_Signal.u16_MultiplexValue = 0U;
      }

      // Keep reserved bytes free
      if (oe_Type == C_OscCanProtocol::eECES)
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

   if (oe_Type == C_OscCanProtocol::eJ1939)
   {
      // J1939 supports only Intel byte order
      orc_Signal.e_ComByteOrder = C_OscCanSignal::eBYTE_ORDER_INTEL;

      // Only unsigned allowed
      // But keep the size of the element
      switch (orc_SignalListElement.GetType())
      {
      case C_OscNodeDataPoolContent::eSINT8:
         orc_SignalListElement.SetType(C_OscNodeDataPoolContent::eUINT8);
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orc_SignalListElement.SetType(C_OscNodeDataPoolContent::eUINT16);
         break;
      case C_OscNodeDataPoolContent::eSINT32:
      case C_OscNodeDataPoolContent::eFLOAT32:
         orc_SignalListElement.SetType(C_OscNodeDataPoolContent::eUINT32);
         break;
      case C_OscNodeDataPoolContent::eSINT64:
      case C_OscNodeDataPoolContent::eFLOAT64:
         orc_SignalListElement.SetType(C_OscNodeDataPoolContent::eUINT64);
         break;
      case C_OscNodeDataPoolContent::eUINT8:  // Nothing to do for unsigned
      case C_OscNodeDataPoolContent::eUINT16: // Nothing to do for unsigned
      case C_OscNodeDataPoolContent::eUINT32: // Nothing to do for unsigned
      case C_OscNodeDataPoolContent::eUINT64: // Nothing to do for unsigned
      default:
         break;
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
QString C_SdUtil::h_InitUsedIdsString(const std::vector<uint32_t> & orc_UsedIds, const QString & orc_ItemName,
                                      const QString & orc_ItemType, const bool oq_SkipItem)
{
   QString c_Retval;

   if (orc_UsedIds.size() > 0UL)
   {
      QString c_BusIds;

      for (uint32_t u32_ItBusId = 0; u32_ItBusId < static_cast<uint32_t>(orc_UsedIds.size() - 1UL); ++u32_ItBusId)
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

   \param[in]  orc_UsedIps    Used IP addresses
   \param[in]  orc_ItemName   Item name (e.g. bus name)
   \param[in]  orc_ItemType   Item type (e.g. "bus" or "node")
   \param[in]  oq_SkiptItem   Flag to skip item (default = false)

   \return
   String with already used IP addresses
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_InitUsedIpsString(const std::vector<std::vector<uint8_t> > & orc_UsedIps,
                                      const QString & orc_ItemName, const QString & orc_ItemType,
                                      const bool oq_SkiptItem)
{
   QString c_Retval;

   if (orc_UsedIps.size() > 0UL)
   {
      QString c_BusIps;
      for (uint16_t u16_ItIp = 0; u16_ItIp < (orc_UsedIps.size() - 1UL); ++u16_ItIp)
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

   \param[in]  orc_Ip   Vector with 4 bytes of an IP address

   \return
   IP as string with point separators
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_IpAddressAsString(const std::vector<uint8_t> & orc_Ip)
{
   QString c_Retval;

   tgl_assert(orc_Ip.size() == 4);
   if (orc_Ip.size() == 4)
   {
      for (uint8_t u8_It = 0; u8_It < (orc_Ip.size() - 1UL); ++u8_It)
      {
         c_Retval += static_cast<QString>("%1.").arg(static_cast<uint32_t>(orc_Ip[u8_It]));
      }
      //append last one without point
      c_Retval += QString::number(static_cast<uint32_t>(orc_Ip[orc_Ip.size() - 1]));
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function generates node id or last byte for IP address depending on oq_GenerateId

   \param[in]  orc_Interfaces          Node interfaces
   \param[in]  orc_UsedNodeProperties  Already used node properties (id/ip: unique and sorted ascending)
   \param[in]  ors32_SpecialInterface  Special interface to use as default
   \param[in]  oq_GenerateId           True: generate ID
                                       False: generate last byte of IP

   \return
   Node ID or IP proposal (Always in range but may be invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdUtil::h_GetNextFreeNodeProperty(const std::vector<C_OscNodeComInterfaceSettings> & orc_Interfaces,
                                             const std::vector<uint32_t> & orc_UsedNodeProperties,
                                             const int32_t & ors32_SpecialInterface, const bool oq_GenerateId)
{
   uint32_t u32_Retval = 0;

   if (ors32_SpecialInterface >= 0)
   {
      for (uint32_t u32_ItInterface = 0; u32_ItInterface < orc_Interfaces.size(); ++u32_ItInterface)
      {
         const C_OscNodeComInterfaceSettings & rc_CurComIf = orc_Interfaces[u32_ItInterface];
         if (rc_CurComIf.u8_InterfaceNumber == static_cast<uint8_t>(ors32_SpecialInterface))
         {
            if (oq_GenerateId)
            {
               u32_Retval = rc_CurComIf.u8_NodeId;
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
      for (uint32_t u32_It = 0; u32_It < orc_UsedNodeProperties.size(); ++u32_It)
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
bool C_SdUtil::h_HasConnectionType(const C_OscNode & orc_Node, const C_OscSystemBus::E_Type oe_Type)
{
   bool q_Return = false;
   uint32_t u32_IntfCounter;

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
bool C_SdUtil::h_CheckNodeInterfaceAvailable(const std::vector<C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
                                             const C_OscSystemBus::E_Type & ore_BusType)
{
   bool q_Retval = false;

   for (uint32_t u32_ItInterface = 0; u32_ItInterface < orc_ComInterfaces.size(); ++u32_ItInterface)
   {
      const C_OscNodeComInterfaceSettings & rc_Interface = orc_ComInterfaces[u32_ItInterface];
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
bool C_SdUtil::h_CheckNodeInterfaceConnected(const std::vector<C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
                                             const C_OscSystemBus::E_Type & ore_BusType)
{
   bool q_Retval = false;

   for (uint32_t u32_ItInterface = 0; u32_ItInterface < orc_ComInterfaces.size(); ++u32_ItInterface)
   {
      const C_OscNodeComInterfaceSettings & rc_Interface = orc_ComInterfaces[u32_ItInterface];
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

   The total number of datapools is bounded above by C_OscNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE.

   \param[in]  oru32_NodeIndex      node index
   \param[in]  orq_AlreadyChecked   for pre-checking cases (true: we know that the number exceeds the maximum)
   \param[in]  opc_Parent           parent widget

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdUtil::h_CheckDatapoolNumber(const uint32_t & oru32_NodeIndex, const bool & orq_AlreadyChecked,
                                     QWidget * const opc_Parent)
{
   bool q_Return = false;

   if (orq_AlreadyChecked == true)
   {
      q_Return = true;
   }
   else
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(oru32_NodeIndex);
      if (pc_Node != NULL)
      {
         // is enough space available
         if (pc_Node->c_DataPools.size() < C_OscNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE)
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
         arg(C_OscNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE));
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
int32_t C_SdUtil::h_GetErrorToolTipNode(const uint32_t & oru32_NodeIndex, QString & orc_Text, bool & orq_ErrorDetected,
                                        const bool oq_CheckAll)
{
   bool q_IsMulti = C_OscNodeSquad::h_CheckIsMultiDevice(oru32_NodeIndex,
                                                         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_NodeSquads);

   std::vector<uint32_t> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
      oru32_NodeIndex);
   int32_t s32_Retval = C_NO_ERR;

   if (!oq_CheckAll)
   {
      //Only check one node
      q_IsMulti = false;
      c_NodeIndices.clear();
      c_NodeIndices.push_back(oru32_NodeIndex);
   }

   //Starting value
   orq_ErrorDetected = false;

   for (uint32_t u32_ItNode = 0UL; (u32_ItNode < c_NodeIndices.size()) && (s32_Retval == C_NO_ERR); ++u32_ItNode)
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
      bool q_CommMinSignalCountInvalid;
      bool q_CommMaxSignalCountInvalid;
      bool q_CoPdoCountInvalid;
      bool q_CoNodeIdInvalid;
      bool q_CoHeartbeatInvalid;

      std::vector<uint32_t> c_InvalidInterfaceIndices;
      std::vector<uint32_t> c_InvalidDataPoolIndices;
      std::vector<uint32_t> c_InvalidApplicationIndices;
      std::vector<uint32_t> c_InvalidDomainIndices;
      std::vector<C_OscCanProtocol::E_Type> c_InvalidProtocolTypes;
      s32_Retval = C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().CheckErrorNode(
         c_NodeIndices[u32_ItNode], &q_NameConflict, &q_NameEmpty, &q_NodeIdInvalid, &q_IpInvalid, &q_DataPoolsInvalid,
         &q_ApplicationsInvalid,
         &q_DomainsInvalid, &q_CommMinSignalCountInvalid, &q_CommMaxSignalCountInvalid, &q_CoPdoCountInvalid,
         &q_CoNodeIdInvalid, &q_CoHeartbeatInvalid,
         true, &c_InvalidInterfaceIndices, &c_InvalidDataPoolIndices, &c_InvalidApplicationIndices,
         &c_InvalidDomainIndices, &c_InvalidProtocolTypes);

      if (s32_Retval == C_NO_ERR)
      {
         if ((q_NameConflict == true) || (q_NodeIdInvalid == true) || (q_IpInvalid == true) ||
             (q_DataPoolsInvalid == true) ||
             (q_ApplicationsInvalid == true) || (q_DomainsInvalid == true) || (q_DataPoolNvmConflict == true) ||
             (q_NameEmpty == true) || (q_CommMinSignalCountInvalid == true) || (q_CommMaxSignalCountInvalid == true) ||
             (q_CoPdoCountInvalid == true) ||
             (q_CoNodeIdInvalid == true) || (q_CoHeartbeatInvalid == true))
         {
            if (q_IsMulti)
            {
               const C_OscNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndices[u32_ItNode]);
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
               for (uint32_t u32_ItAppl = 0;
                    (u32_ItAppl < c_InvalidApplicationIndices.size()) &&
                    (u32_ItAppl < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                    ++u32_ItAppl)
               {
                  const C_OscNodeApplication * const pc_Appl = C_PuiSdHandler::h_GetInstance()->GetApplication(
                     c_NodeIndices[u32_ItNode], c_InvalidApplicationIndices[u32_ItAppl]);
                  if (pc_Appl != NULL)
                  {
                     orc_Text += static_cast<QString>("%1\n").arg(pc_Appl->c_Name.c_str());
                  }
               }
               if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidApplicationIndices.size())
               {
                  orc_Text += static_cast<QString>("+%1\n").arg(
                     c_InvalidApplicationIndices.size() - static_cast<int32_t>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
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
               for (uint32_t u32_ItDomains = 0;
                    (u32_ItDomains < c_InvalidDomainIndices.size()) && (u32_ItDomains < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                    ++u32_ItDomains)
               {
                  const C_OscHalcConfigDomain * const pc_Domain =
                     C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(c_NodeIndices[u32_ItNode],
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
                                                       static_cast<int32_t>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
               }
               orc_Text += "\n";
            }

            if ((q_CommMinSignalCountInvalid == true) || (q_CommMaxSignalCountInvalid == true))
            {
               uint32_t u32_InvalidProtCounter;
               orc_Text += C_GtGetText::h_GetText("Invalid COMM protocol configuration with invalid signal count:\n");
               for (u32_InvalidProtCounter = 0U; u32_InvalidProtCounter < c_InvalidProtocolTypes.size();
                    ++u32_InvalidProtCounter)
               {
                  orc_Text +=
                     C_PuiSdUtil::h_ConvertProtocolTypeToString(c_InvalidProtocolTypes[u32_InvalidProtCounter]);
               }
            }

            if ((q_CoNodeIdInvalid == true) ||
                (q_CoHeartbeatInvalid == true) ||
                (q_CoPdoCountInvalid == true))
            {
               orc_Text += C_GtGetText::h_GetText("Invalid CANopen Manager configuration:\n");

               if (q_CoNodeIdInvalid == true)
               {
                  orc_Text += C_GtGetText::h_GetText("Duplicate or invalid CANopen Node ID detected.\n");
               }

               if (q_CoHeartbeatInvalid == true)
               {
                  orc_Text += C_GtGetText::h_GetText("Invalid CANopen Device Heartbeat consumer time detected.\n");
               }

               if (q_CoPdoCountInvalid == true)
               {
                  orc_Text += C_GtGetText::h_GetText("Too many PDOs in a CANopen Manager configuration detected.\n");
               }
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
void C_SdUtil::h_GetErrorToolTipBus(const uint32_t & oru32_BusIndex, QString & orc_Text)
{
   bool q_NameConflict;
   bool q_NameEmpty;
   bool q_IdInvalid;

   std::vector<QString> c_InvalidNodesForBitRate;
   std::vector<stw::opensyde_core::C_OscCanProtocol::E_Type> c_InvalidProtocols;
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
         for (uint32_t u32_ItNode = 0UL; (u32_ItNode < c_InvalidNodesForBitRate.size()) &&
              (u32_ItNode < mu32_TOOL_TIP_MAXIMUM_ITEMS); ++u32_ItNode)
         {
            orc_Text += "- " + c_InvalidNodesForBitRate[u32_ItNode] + "\n";
         }
         if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidNodesForBitRate.size())
         {
            orc_Text +=
               static_cast<QString>("+%1\n").
               arg(c_InvalidNodesForBitRate.size() - static_cast<int32_t>(mu32_TOOL_TIP_MAXIMUM_ITEMS));
         }
      }
      orc_Text += "\n";
   }
   if (c_InvalidProtocols.size() > 0)
   {
      orc_Text += C_GtGetText::h_GetText("COMM protocols with invalid content:\n");
      for (uint32_t u32_ItProtocol = 0; u32_ItProtocol < c_InvalidProtocols.size(); ++u32_ItProtocol)
      {
         orc_Text +=
            static_cast<QString>("%1\n").arg(C_PuiSdUtil::h_ConvertProtocolTypeToString(c_InvalidProtocols[
                                                                                           u32_ItProtocol]));
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

QString C_SdUtil::h_GetToolTipContentDpList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DatapoolIndex,
                                            const uint32_t & oru32_ListIndex)
{
   const C_OscNodeDataPoolList * const pc_DpList =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(oru32_NodeIndex, oru32_DatapoolIndex, oru32_ListIndex);

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
   const uint32_t u32_DatasetNumber = pc_DpList->c_DataSets.size();

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
      for (uint32_t u32_Pos = 0; u32_Pos < u32_DatasetNumber; u32_Pos++)
      {
         const C_OscNodeDataPoolDataSet & rc_Dataset = pc_DpList->c_DataSets[u32_Pos];
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
QString C_SdUtil::h_GetToolTipContentDpListElement(const C_OscNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
                                                   const QString & orc_AdditionalInformation)
{
   QString c_ToolTipContent = "";

   std::vector<QString> c_HelpVector;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
      orc_NodeDatapoolListElementId.u32_NodeIndex);
   const C_OscNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
      orc_NodeDatapoolListElementId.u32_NodeIndex, orc_NodeDatapoolListElementId.u32_DataPoolIndex);
   const C_OscNodeDataPoolList * const pc_DpList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
      orc_NodeDatapoolListElementId.u32_NodeIndex, orc_NodeDatapoolListElementId.u32_DataPoolIndex,
      orc_NodeDatapoolListElementId.u32_ListIndex);
   const C_OscNodeDataPoolListElement * const pc_DpListElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
         orc_NodeDatapoolListElementId.u32_NodeIndex, orc_NodeDatapoolListElementId.u32_DataPoolIndex,
         orc_NodeDatapoolListElementId.u32_ListIndex, orc_NodeDatapoolListElementId.u32_ElementIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
      C_PuiSdHandler::h_GetInstance()->GetUiDataPoolListElement(orc_NodeDatapoolListElementId.u32_NodeIndex,
                                                                orc_NodeDatapoolListElementId.u32_DataPoolIndex,
                                                                orc_NodeDatapoolListElementId.u32_ListIndex,
                                                                orc_NodeDatapoolListElementId.u32_ElementIndex);

   if ((pc_Node != NULL) && (pc_Datapool != NULL) && (pc_DpList != NULL) && (pc_DpListElement != NULL) &&
       (pc_UiElement != NULL))
   {
      if (pc_Datapool->e_Type == C_OscNodeDataPool::eCOM)
      {
         // get message id and signal index and use special tool tip content generation
         C_OscCanMessageIdentificationIndices c_MessageId;
         uint32_t u32_SignalIndex;
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
         if (pc_UiElement->q_InterpretAsString == false)
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
         if (pc_UiElement->q_InterpretAsString == false)
         {
            // auto min max
            c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Auto min/max: "));
            if (pc_UiElement->q_AutoMinMaxActive == true)
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
            for (uint32_t u32_Pos = 0; u32_Pos < c_HelpVector.size(); u32_Pos++)
            {
               c_HelpString.append(c_HelpVector[u32_Pos]);
               c_HelpString.append(";");
               // auto min max or no array -> only one number
               if ((pc_DpListElement->GetArray() == false) || (pc_UiElement->q_AutoMinMaxActive == true))
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
            for (uint32_t u32_Pos = 0; u32_Pos < c_HelpVector.size(); u32_Pos++)
            {
               c_HelpString.append(c_HelpVector[u32_Pos]);
               c_HelpString.append(";");
               // auto min max or no array -> only one number
               if ((pc_DpListElement->GetArray() == false) || (pc_UiElement->q_AutoMinMaxActive == true))
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
            for (uint32_t u32_PosDataset = 0; u32_PosDataset < pc_DpList->c_DataSets.size(); u32_PosDataset++)
            {
               c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Dataset - "));
               c_ToolTipContent.append(pc_DpList->c_DataSets[u32_PosDataset].c_Name.c_str());
               c_ToolTipContent.append(": ");
               c_HelpVector.clear();
               C_SdNdeDpContentUtil::h_GetValuesAsScaledString(pc_DpListElement->c_DataSetValues[u32_PosDataset],
                                                               pc_DpListElement->f64_Factor,
                                                               pc_DpListElement->f64_Offset, c_HelpVector);
               QString c_HelpString = "";
               for (uint32_t u32_PosArray = 0; u32_PosArray < c_HelpVector.size(); u32_PosArray++)
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
         if ((pc_Datapool->e_Type == C_OscNodeDataPool::eNVM) ||
             (pc_Datapool->e_Type == C_OscNodeDataPool::eHALC_NVM))
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
            c_ToolTipContent.append(QString::number(pc_DpListElement->u32_NvmStartAddress -
                                                    pc_Datapool->u32_NvmStartAddress));
            c_ToolTipContent.append(" (");
            c_ToolTipContent.append(QString::number(pc_DpListElement->u32_NvmStartAddress));
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
QString C_SdUtil::h_GetToolTipContentMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   QString c_ToolTipContent = "";
   const C_OscCanMessage * const pc_Message =
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
QString C_SdUtil::h_GetToolTipContentSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                            const uint32_t & oru32_SignalIndex,
                                            const QString & orc_AdditionalInformation)
{
   QString c_ToolTipContent = "";

   const C_OscCanSignal * const pc_Signal =
      C_PuiSdHandler::h_GetInstance()->GetCanSignal(orc_MessageId, oru32_SignalIndex);
   const C_OscCanMessage * const pc_Message =
      C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
   const stw::opensyde_core::C_OscNodeDataPoolListElement * const pc_DpListElement =
      C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(orc_MessageId, oru32_SignalIndex);
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
                                                                    orc_MessageId.e_ComProtocol, c_AutoMinMaxInfo,
                                                                    orc_AdditionalInformation);
   }

   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices descending ( Sorting steps are done for the content vectors in sync)

   \param[in,out]  orc_IndicesTmp      Unsorted indices
   \param[in,out]  orc_OscContentTmp   Unsorted OSC content
   \param[in,out]  orc_UiContentTmp    Unsorted UI content

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
void C_SdUtil::h_SortIndicesDescendingAndSync(std::vector<uint32_t> & orc_IndicesTmp,
                                              std::vector<T> & orc_OscContentTmp, std::vector<U> & orc_UiContentTmp)
{
   if (h_CheckSortedDescending(orc_IndicesTmp) == false)
   {
      std::vector<uint32_t> c_IndicesTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<T> c_OscContentTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<U> c_UiContentTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<int32_t> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      c_OscContentTmp.reserve(orc_OscContentTmp.size());
      c_UiContentTmp.reserve(orc_UiContentTmp.size());
      for (uint32_t u32_ItIndex = c_IndexMap.size(); u32_ItIndex > 0; --u32_ItIndex)
      {
         if (c_IndexMap[static_cast<uint32_t>(u32_ItIndex - 1U)] >= 0)
         {
            const uint32_t u32_CurIndex =
               static_cast<uint32_t>(c_IndexMap[static_cast<uint32_t>(u32_ItIndex - 1U)]);
            if (((u32_CurIndex < orc_IndicesTmp.size()) &&
                 (u32_CurIndex < orc_OscContentTmp.size())) &&
                (u32_CurIndex < orc_UiContentTmp.size()))
            {
               c_IndicesTmp.push_back(orc_IndicesTmp[u32_CurIndex]);
               c_OscContentTmp.push_back(orc_OscContentTmp[u32_CurIndex]);
               c_UiContentTmp.push_back(orc_UiContentTmp[u32_CurIndex]);
            }
         }
      }
      //Step 3: Copy data structures to internal ones
      orc_IndicesTmp = c_IndicesTmp;
      orc_OscContentTmp = c_OscContentTmp;
      orc_UiContentTmp = c_UiContentTmp;
   }
}

//Explicit declaration of every type usage is necessary for templates to allow split of declaration and implementation
//lint -esym(754,stw::opensyde_gui_logic::C_SdUtil::h_SortIndicesDescendingAndSync*)
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OscCanMessageIdentificationIndices, uint32_t>(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscCanMessageIdentificationIndices> & orc_OscContentTmp,
   std::vector<uint32_t> & orc_UiContentTmp);
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OscNodeDataPoolList, C_PuiSdNodeDataPoolList>(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscNodeDataPoolList> & orc_OscContentTmp,
   std::vector<C_PuiSdNodeDataPoolList> & orc_UiContentTmp);
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OscNodeDataPoolDataSet, std::vector<C_OscNodeDataPoolContent> >(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscNodeDataPoolDataSet> & orc_OscContentTmp,
   std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_UiContentTmp);
template
void C_SdUtil::h_SortIndicesDescendingAndSync<C_OscNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscNodeDataPoolListElement> & orc_OscContentTmp,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContentTmp);

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending ( Sorting steps are done for the content vectors in sync)

   \param[in,out]  orc_IndicesTmp      Unsorted indices
   \param[in,out]  orc_OscContentTmp   Unsorted OSC content
   \param[in,out]  orc_UiContentTmp    Unsorted UI content

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
void C_SdUtil::h_SortIndicesAscendingAndSync(std::vector<uint32_t> & orc_IndicesTmp, std::vector<T> & orc_OscContentTmp,
                                             std::vector<U> & orc_UiContentTmp)
{
   if (C_Uti::h_CheckSortedAscending(orc_IndicesTmp) == false)
   {
      std::vector<uint32_t> c_IndicesTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<T> c_OscContentTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<U> c_UiContentTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<int32_t> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      c_OscContentTmp.reserve(orc_OscContentTmp.size());
      c_UiContentTmp.reserve(orc_UiContentTmp.size());
      for (uint32_t u32_ItIndex = 0; u32_ItIndex < c_IndexMap.size(); ++u32_ItIndex)
      {
         if (c_IndexMap[u32_ItIndex] >= 0)
         {
            const uint32_t u32_CurIndex = static_cast<uint32_t>(c_IndexMap[u32_ItIndex]);
            if (((u32_CurIndex < orc_IndicesTmp.size()) &&
                 (u32_CurIndex < orc_OscContentTmp.size())) &&
                (u32_CurIndex < orc_UiContentTmp.size()))
            {
               c_IndicesTmp.push_back(orc_IndicesTmp[u32_CurIndex]);
               c_OscContentTmp.push_back(orc_OscContentTmp[u32_CurIndex]);
               c_UiContentTmp.push_back(orc_UiContentTmp[u32_CurIndex]);
            }
         }
      }
      //Step 3: Copy data structures to internal ones
      orc_IndicesTmp = c_IndicesTmp;
      orc_OscContentTmp = c_OscContentTmp;
      orc_UiContentTmp = c_UiContentTmp;
   }
}

//Explicit declaration of every type usage is necessary for templates to allow split of declaration and implementation
//lint -esym(754,stw::opensyde_gui_logic::C_SdUtil::h_SortIndicesAscendingAndSync*)
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OscCanMessageIdentificationIndices, uint32_t>(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscCanMessageIdentificationIndices> & orc_OscContentTmp,
   std::vector<uint32_t> & orc_UiContentTmp);
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OscNodeDataPoolList, C_PuiSdNodeDataPoolList>(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscNodeDataPoolList> & orc_OscContentTmp,
   std::vector<C_PuiSdNodeDataPoolList> & orc_UiContentTmp);
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OscNodeDataPoolDataSet, std::vector<C_OscNodeDataPoolContent> >(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscNodeDataPoolDataSet> & orc_OscContentTmp,
   std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_UiContentTmp);
template
void C_SdUtil::h_SortIndicesAscendingAndSync<C_OscNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
   std::vector<uint32_t> & orc_IndicesTmp, std::vector<C_OscNodeDataPoolListElement> & orc_OscContentTmp,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContentTmp);

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string of details from eds file

  \param[in]  oc_CanOpenObjDictionary  CANOpen object dictionary

   \return
   content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_GetEdsFileDetails(const C_OscCanOpenObjectDictionary oc_CanOpenObjDictionary)
{
   QString c_String;
   QString c_TrueOrFalse;
   const C_OscCanOpenEdsInfoBlock c_InfoBlock = oc_CanOpenObjDictionary.c_InfoBlock;
   C_OscCanOpenEdsFileInfoBlock c_FileInfoBlock = c_InfoBlock.c_FileInfo;
   C_OscCanOpenEdsDeviceInfoBlock c_DeviceInfoBlock = c_InfoBlock.c_DeviceInfo;

   c_String += "[FileInfo]";
   c_String += "\nFileName=";
   c_String += c_FileInfoBlock.c_FileName.AsStdString()->c_str();
   c_String += "\nFileVersion=";
   c_String += QString::number(c_FileInfoBlock.u8_FileVersion);
   c_String += "\nFileRevision=";
   c_String += QString::number(c_FileInfoBlock.u8_FileRevision);
   if (c_FileInfoBlock.c_EdsVersion != "")
   {
      c_String += "\nEDSVersion=";
      c_String += c_FileInfoBlock.c_EdsVersion.AsStdString()->c_str();
   }
   c_String += "\nDescription=";
   c_String += c_FileInfoBlock.c_Description.AsStdString()->c_str();
   c_String += "\nCreationTime=";
   c_String += c_FileInfoBlock.c_CreationTime.AsStdString()->c_str();
   c_String += "\nCreationDate=";
   c_String += c_FileInfoBlock.c_CreationDate.AsStdString()->c_str();
   c_String += "\nCreatedBy=";
   c_String += c_FileInfoBlock.c_CreatedBy.AsStdString()->c_str();
   c_String += "\nModificationDate=";
   c_String += c_FileInfoBlock.c_ModificationDate.AsStdString()->c_str();
   c_String += "\nModifiedBy=";
   c_String += c_FileInfoBlock.c_ModifiedBy.AsStdString()->c_str();

   c_String += "\n\n[DeviceInfo]";
   c_String += "\nVendorName=";
   c_String += c_DeviceInfoBlock.c_VendorName.AsStdString()->c_str();
   c_String += "\nVendorNumber=";
   c_String += c_DeviceInfoBlock.c_VendorNumber.AsStdString()->c_str();
   c_String += "\nProductName=";
   c_String += c_DeviceInfoBlock.c_ProductName.AsStdString()->c_str();
   c_String += "\nProductNumber=";
   c_String += c_DeviceInfoBlock.c_ProductNumber.AsStdString()->c_str();
   if (c_DeviceInfoBlock.c_RevisionNumber != "")
   {
      c_String += "\nRevisionNumber=";
      c_String += c_DeviceInfoBlock.c_RevisionNumber.AsStdString()->c_str();
   }
   c_String += "\nOrderCode=";
   c_String += c_DeviceInfoBlock.c_OrderCode.AsStdString()->c_str();
   c_String += "\nBaudRate_10=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate10 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_20=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate20 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_50=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate50 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_125=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate125 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_250=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate250 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_500=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate500 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_800=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate800 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nBaudRate_1000=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_BaudRate1000 ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nSimpleBootUpMaster=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_SimpleBootUpMaster ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nSimpleBootUpSlave=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_SimpleBootUpSlave ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nGranularity=";
   c_String += QString::number(c_DeviceInfoBlock.u8_Granularity);
   c_String += "\nDynamicChannelsSupported=";
   c_String += c_DeviceInfoBlock.c_DynamicChannelsSupported.AsStdString()->c_str();
   c_String += "\nGroupMessaging=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_GroupMessaging ? "1" : "0";
   c_String += c_TrueOrFalse;
   c_String += "\nNrOfRxPDO=";
   c_String += QString::number(c_DeviceInfoBlock.u16_NrOfRxPdo);
   c_String += "\nNrOfTxPDO=";
   c_String += QString::number(c_DeviceInfoBlock.u16_NrOfTxPdo);
   c_String += "\nSupported=";
   c_TrueOrFalse = c_DeviceInfoBlock.q_LssSupported ? "1" : "0";
   c_String += c_TrueOrFalse + "\n";

   return c_String;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Counting messages and signals of a specific node for a specific protocol

   \param[in]       ou32_NodeIndex           Node index for counting the messages.
                                             In case of CANopen it must be always the index of the CANopen Manager
   \param[in]       ou32_InterfaceIndex      Used interface index
   \param[in]       oe_Protocol              Current CAN protocol
   \param[in]       opc_CoDeviceInterfaceId  Optional: In case of protocol CANopen if the count of messages of a
                                             specific device is needed. ou32_NodeIndex must be the CANopen Manager
                                             which the device is assigned
   \param[out]      oru32_RxMessageCount     Counted Rx messages of the specific node
   \param[out]      oru32_TxMessageCount     Counted Tx messages of the specific node
   \param[out]      opu32_SignalCount        Optional: Signal count of all Rx and Tx messages


   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdUtil::h_GetMessageCountOfNode(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                          const C_OscCanProtocol::E_Type oe_Protocol,
                                          const C_OscCanInterfaceId * const opc_CoDeviceInterfaceId,
                                          uint32_t & oru32_RxMessageCount, uint32_t & oru32_TxMessageCount,
                                          uint32_t * const opu32_SignalCount)
{
   int32_t s32_Return = C_RANGE;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   oru32_TxMessageCount = 0U;
   oru32_RxMessageCount = 0U;
   if (opu32_SignalCount != NULL)
   {
      *opu32_SignalCount = 0U;
   }

   if ((pc_Node != NULL) &&
       (ou32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
   {
      const uint32_t u32_ListIndex = ou32_InterfaceIndex * 2U;
      uint32_t u32_DatapoolCounter;

      s32_Return = C_NO_ERR;

      for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < pc_Node->c_DataPools.size(); ++u32_DatapoolCounter)
      {
         const C_OscNodeDataPool * const pc_Datapool = &pc_Node->c_DataPools[u32_DatapoolCounter];

         if ((pc_Datapool != NULL) &&
             (pc_Datapool->e_Type == C_OscNodeDataPool::eCOM))
         {
            const C_OscCanProtocol * const pc_Protocol = pc_Node->GetRelatedCanProtocolConst(u32_DatapoolCounter);
            if ((pc_Protocol != NULL) &&
                (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()) &&
                (pc_Protocol->e_Type == oe_Protocol))
            {
               const C_OscNodeDataPoolList * const pc_DataPoolList1 =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
                     ou32_NodeIndex,
                     u32_DatapoolCounter,
                     u32_ListIndex);
               const C_OscNodeDataPoolList * const pc_DataPoolList2 =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
                     ou32_NodeIndex,
                     u32_DatapoolCounter,
                     u32_ListIndex + 1);

               if ((pc_DataPoolList1 != NULL) && (pc_DataPoolList2 != NULL))
               {
                  const C_OscCanMessageContainer & rc_MessageContainer =
                     pc_Protocol->c_ComMessages[ou32_InterfaceIndex];

                  if (oe_Protocol != C_OscCanProtocol::eCAN_OPEN)
                  {
                     const uint32_t u32_SignalSize1 = pc_DataPoolList1->c_Elements.size();
                     const uint32_t u32_SignalSize2 = pc_DataPoolList2->c_Elements.size();

                     oru32_RxMessageCount += static_cast<uint32_t>(rc_MessageContainer.c_RxMessages.size());
                     oru32_TxMessageCount += static_cast<uint32_t>(rc_MessageContainer.c_TxMessages.size());
                     if (opu32_SignalCount != NULL)
                     {
                        *opu32_SignalCount += u32_SignalSize1 + u32_SignalSize2;
                     }
                  }
                  else
                  {
                     // In case of CANopen the active flag of each message must be checked
                     uint32_t u32_MessageCounter;
                     // RX messages
                     for (u32_MessageCounter = 0U; u32_MessageCounter < rc_MessageContainer.c_RxMessages.size();
                          ++u32_MessageCounter)
                     {
                        const C_OscCanMessage & rc_Msg = rc_MessageContainer.c_RxMessages[u32_MessageCounter];
                        // The message must be active and in case of the device, the device must be the matching
                        // communication partner for this PDO
                        if ((rc_Msg.q_CanOpenManagerMessageActive == true) &&
                            ((opc_CoDeviceInterfaceId == NULL) || // No CANopen device, or it must match
                             (rc_Msg.c_CanOpenManagerOwnerNodeIndex == *opc_CoDeviceInterfaceId)))
                        {
                           if (opc_CoDeviceInterfaceId == NULL)
                           {
                              ++oru32_RxMessageCount;
                           }
                           else
                           {
                              // In case of a device, the direction is reversed
                              ++oru32_TxMessageCount;
                           }

                           if (opu32_SignalCount != NULL)
                           {
                              *opu32_SignalCount += rc_Msg.c_Signals.size();
                           }
                        }
                     }

                     // TX messages
                     for (u32_MessageCounter = 0U; u32_MessageCounter < rc_MessageContainer.c_TxMessages.size();
                          ++u32_MessageCounter)
                     {
                        const C_OscCanMessage & rc_Msg = rc_MessageContainer.c_TxMessages[u32_MessageCounter];
                        // The message must be active and in case of the device, the device must be the matching
                        // communication partner for this PDO
                        if ((rc_Msg.q_CanOpenManagerMessageActive == true) &&
                            ((opc_CoDeviceInterfaceId == NULL) || // No CANopen device, or it must match
                             (rc_Msg.c_CanOpenManagerOwnerNodeIndex == *opc_CoDeviceInterfaceId)))
                        {
                           if (opc_CoDeviceInterfaceId == NULL)
                           {
                              ++oru32_TxMessageCount;
                           }
                           else
                           {
                              // In case of a device, the direction is reversed
                              ++oru32_RxMessageCount;
                           }

                           if (opu32_SignalCount != NULL)
                           {
                              *opu32_SignalCount += rc_Msg.c_Signals.size();
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen signal object index

   \param[in]  ou32_ObjectIndex     Object index
   \param[in]  ou32_ObjectSubIndex  Object sub index

   \return
   CANopen signal object index
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdUtil::h_GetCanOpenSignalObjectIndex(const uint32_t ou32_ObjectIndex, const uint32_t ou32_ObjectSubIndex)
{
   const QString c_ObjectIndexString = QString::number(ou32_ObjectIndex, 16).toUpper();
   const QString c_ObjectSubIndexString = QString::number(ou32_ObjectSubIndex, 16).toUpper();
   const QString c_Retval = static_cast<QString>("%1sub%2").arg(c_ObjectIndexString).arg(c_ObjectSubIndexString);

   return c_Retval;
}
