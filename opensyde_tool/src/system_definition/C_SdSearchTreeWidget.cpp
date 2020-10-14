//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for searching in the system definition (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <vector>
#include <QKeyEvent>

#include "stwtypes.h"
#include "constants.h"
#include "C_SdSearchTreeWidget.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OgeTreeWidgetToolBarSearchItemDelegate.h"
#include "C_PuiSdUtil.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_TITLE = static_cast<sintn>(Qt::DisplayRole);
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_SUBTITLE = static_cast<sintn>(Qt::UserRole) + 1;
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_NAME = static_cast<sintn>(Qt::UserRole) + 2;
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_INDEX_1 = static_cast<sintn>(Qt::UserRole) + 3;
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_INDEX_2 = static_cast<sintn>(Qt::UserRole) + 4;
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_INDEX_3 = static_cast<sintn>(Qt::UserRole) + 5;
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_INDEX_4 = static_cast<sintn>(Qt::UserRole) + 6;
const sintn C_SdSearchTreeWidget::mhsn_DATAROLE_INDEX_5 = static_cast<sintn>(Qt::UserRole) + 7;

const uint8 C_SdSearchTreeWidget::mhu8_DATAELEMENT_TYPE_PARAMETER = 0U;
const uint8 C_SdSearchTreeWidget::mhu8_DATAELEMENT_TYPE_VARIABLE = 1U;
const uint8 C_SdSearchTreeWidget::mhu8_DATAELEMENT_TYPE_SIGNAL = 2U;
const uint8 C_SdSearchTreeWidget::mhu8_DATAELEMENT_TYPE_HALC = 3U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] opc_Parent  Optional parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdSearchTreeWidget::C_SdSearchTreeWidget(QWidget * const opc_Parent) :
   C_OgeTreeWidgetToolBarSearch(opc_Parent)
{
   QPixmap c_Pixmap;

   this->setItemDelegate(new C_OgeTreeWidgetToolBarSearchItemDelegate(this));

   m_SetupStartingState();

   // prepare the icons
   c_Pixmap.load("://images/system_definition/IconNode.svg");
   this->mc_IconNode.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconBus.svg");
   this->mc_IconBusCan.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconBus.svg");
   this->mc_IconBusEthernet.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconDataPoolSmall.svg");
   this->mc_IconDataPool.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconDatapoolList.svg");
   this->mc_IconList.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconMessage.svg");
   this->mc_IconMessage.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconParameter.svg");
   this->mc_IconParameter.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconSignal.svg");
   this->mc_IconSignal.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconVariable.svg");
   this->mc_IconVariable.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/IconDataBlock.svg");
   this->mc_IconApplication.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/NodeEdit/halc/OtherSmallActive.svg");
   this->mc_IconHalcChannel.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/NodeEdit/halc/OutputSmallActive.svg");
   this->mc_IconHalcOutputChannel.addPixmap(c_Pixmap);

   c_Pixmap.load(":/images/system_definition/NodeEdit/halc/InputSmallActive.svg");
   this->mc_IconHalcInputChannel.addPixmap(c_Pixmap);

   connect(this, &QTreeWidget::itemClicked, this, &C_SdSearchTreeWidget::m_ItemClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdSearchTreeWidget::~C_SdSearchTreeWidget()
{
   //lint -e{1540}  no memory leak because of the parent all elements and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Searching for substring in the names of system definition items

   \param[in]     orc_SearchString   Search substring

   \return
   true: results found
   false: no results found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdSearchTreeWidget::Search(const QString & orc_SearchString)
{
   bool q_ResultsFound = false;

   // clean up the old results
   this->ClearResult();

   if (orc_SearchString != "")
   {
      QString c_CounterResult = " (%1)";

      this->mc_SearchString = orc_SearchString;

      // search in the systemdefinition
      this->m_StartSearch();

      this->expandAll();

      // show the result counter
      this->mpc_TreeItemRootNodes->setText(0, C_GtGetText::h_GetText("  Nodes") +
                                           c_CounterResult.arg(QString::number(this->mu32_NodesFound)));
      this->mpc_TreeItemRootBusses->setText(0, C_GtGetText::h_GetText("  Buses") +
                                            c_CounterResult.arg(QString::number(this->mu32_BussesFound)));
      this->mpc_TreeItemRootDataPools->setText(0, C_GtGetText::h_GetText("  Datapools") +
                                               c_CounterResult.arg(QString::number(this->mu32_DataPoolsFound)));
      this->mpc_TreeItemRootLists->setText(0, C_GtGetText::h_GetText("  Lists") +
                                           c_CounterResult.arg(QString::number(this->mu32_ListsFound)));
      this->mpc_TreeItemRootMessages->setText(0, C_GtGetText::h_GetText("  Messages") +
                                              c_CounterResult.arg(QString::number(this->mu32_MessagesFound)));
      this->mpc_TreeItemRootDataElements->setText(0, C_GtGetText::h_GetText("  Data elements") +
                                                  c_CounterResult.arg(QString::number(this->mu32_DataElementsFound)));
      this->mpc_TreeItemRootApplications->setText(0, C_GtGetText::h_GetText("  Data Blocks") +
                                                  c_CounterResult.arg(QString::number(this->mu32_ApplicationsFound)));
      this->mpc_TreeItemRootHalcChannels->setText(0, C_GtGetText::h_GetText("  Channels") +
                                                  c_CounterResult.arg(QString::number(this->mu32_HalcChannelsFound)));

      //get result found status
      if ((mu32_NodesFound != 0) ||
          (mu32_BussesFound != 0) ||
          (mu32_DataPoolsFound != 0) ||
          (mu32_ListsFound != 0) ||
          (mu32_MessagesFound != 0) ||
          (mu32_DataElementsFound != 0) ||
          (mu32_ApplicationsFound != 0) ||
          (mu32_HalcChannelsFound != 0))
      {
         q_ResultsFound = true;
      }
   }

   return q_ResultsFound;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all search results
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::ClearResult(void)
{
   this->clear();

   m_SetupStartingState();

   this->mu32_NodesFound = 0U;
   this->mu32_BussesFound = 0U;
   this->mu32_DataPoolsFound = 0U;
   this->mu32_ListsFound = 0U;
   this->mu32_MessagesFound = 0U;
   this->mu32_DataElementsFound = 0U;
   this->mu32_ApplicationsFound = 0U;
   this->mu32_HalcChannelsFound = 0U;

   this->mq_ResultsFound = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the first element selected and the sets the focus to the tree widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::SetSearchResultFocus(void)
{
   QTreeWidgetItem * pc_Item;

   if (this->mu32_NodesFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootNodes->child(0);
   }
   else if (this->mu32_BussesFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootBusses->child(0);
   }
   else if (this->mu32_DataPoolsFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootDataPools->child(0);
   }
   else if (this->mu32_ListsFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootLists->child(0);
   }
   else if (this->mu32_MessagesFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootMessages->child(0);
   }
   else if (this->mu32_DataElementsFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootDataElements->child(0);
   }
   else if (this->mu32_ApplicationsFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootApplications->child(0);
   }
   else if (this->mu32_HalcChannelsFound > 0U)
   {
      pc_Item = this->mpc_TreeItemRootHalcChannels->child(0);
   }
   else
   {
      pc_Item = NULL;
   }

   this->setFocus();
   this->setItemSelected(pc_Item, true);
   this->setCurrentItem(pc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Open concrete item or hide this tree widget

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return))
   {
      this->m_ItemClicked();
   }
   else if (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Escape))
   {
      Q_EMIT this->SigHide();
   }
   else
   {
      C_OgeTreeWidgetToolBarSearch::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus in event slot

   Here: Remove the selection

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::focusOutEvent(QFocusEvent * const opc_Event)
{
   QList<QTreeWidgetItem *> c_SelectedItems = this->selectedItems();
   QList<QTreeWidgetItem *>::iterator c_ItItem;

   for (c_ItItem = c_SelectedItems.begin(); c_ItItem != c_SelectedItems.end(); ++c_ItItem)
   {
      (*c_ItItem)->setSelected(false);
   }

   Q_EMIT (this->SigFocusOut());

   C_OgeTreeWidgetToolBarSearch::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_StartSearch(void)
{
   const std::vector<C_OSCNode> & rc_Nodes = C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_Nodes;
   const std::vector<C_OSCSystemBus> & rc_Busses =
      C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_Buses;
   uint32 u32_Counter;

   // search nodes and the content of nodes
   for (u32_Counter = 0U; u32_Counter < rc_Nodes.size(); ++u32_Counter)
   {
      this->m_SearchNodeContent(rc_Nodes[u32_Counter], u32_Counter);
   }

   // search busses and the content of busses
   for (u32_Counter = 0U; u32_Counter < rc_Busses.size(); ++u32_Counter)
   {
      this->m_SearchBusContent(rc_Busses[u32_Counter], u32_Counter);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_SearchNodeContent(const C_OSCNode & orc_Node, const uint32 ou32_NodeIndex)
{
   uint32 u32_Counter;
   QString c_Name;
   QString c_DataBlockName;

   c_Name = orc_Node.c_Properties.c_Name.c_str();

   if (c_Name.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
   {
      this->m_AddNodeResult(c_Name, ou32_NodeIndex, orc_Node.c_DeviceType.c_str());
   }

   // search datapools and the content of the datapools
   for (u32_Counter = 0U; u32_Counter < orc_Node.c_DataPools.size(); ++u32_Counter)
   {
      this->m_SearchDataPoolContent(orc_Node.c_DataPools[u32_Counter], u32_Counter, c_Name, ou32_NodeIndex);
   }

   // search CAN protocol content
   for (u32_Counter = 0U; u32_Counter < orc_Node.c_ComProtocols.size(); ++u32_Counter)
   {
      this->m_SearchCanProtocolContent(orc_Node.c_ComProtocols[u32_Counter], c_Name, ou32_NodeIndex);
   }

   // search data block / application content
   for (u32_Counter = 0U; u32_Counter < orc_Node.c_Applications.size(); ++u32_Counter)
   {
      c_DataBlockName = orc_Node.c_Applications[u32_Counter].c_Name.c_str();

      if (c_DataBlockName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
      {
         this->m_AddApplicationResult(c_DataBlockName, u32_Counter, c_Name, ou32_NodeIndex);
      }
   }

   // search HALC elements
   this->m_SearchHalcConfigurationContent(orc_Node.c_HALCConfig, c_Name, ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_SearchDataPoolContent(const C_OSCNodeDataPool & orc_DataPool,
                                                   const uint32 ou32_DataPoolIndex, const QString & orc_NodeName,
                                                   const uint32 ou32_NodeIndex)
{
   uint32 u32_ListCounter;
   uint32 u32_DataElementCounter;
   QString c_DataPoolName;
   QString c_ListName;
   QString c_DataElementName;

   c_DataPoolName = orc_DataPool.c_Name.c_str();

   if (c_DataPoolName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
   {
      this->m_AddDataPoolResult(c_DataPoolName, ou32_DataPoolIndex, orc_NodeName, ou32_NodeIndex);
   }

   // search datapool content
   for (u32_ListCounter = 0U; u32_ListCounter < orc_DataPool.c_Lists.size(); ++u32_ListCounter)
   {
      c_ListName = orc_DataPool.c_Lists[u32_ListCounter].c_Name.c_str();

      if (c_ListName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
      {
         this->m_AddListResult(c_ListName, u32_ListCounter, c_DataPoolName, ou32_DataPoolIndex, orc_NodeName,
                               ou32_NodeIndex);
      }

      // search list content
      for (u32_DataElementCounter = 0U;
           u32_DataElementCounter < orc_DataPool.c_Lists[u32_ListCounter].c_Elements.size();
           ++u32_DataElementCounter)
      {
         c_DataElementName =
            orc_DataPool.c_Lists[u32_ListCounter].c_Elements[u32_DataElementCounter].c_Name.c_str();

         if (c_DataElementName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
         {
            uint8 u8_Type = mhu8_DATAELEMENT_TYPE_VARIABLE;

            switch (orc_DataPool.e_Type)
            {
            case C_OSCNodeDataPool::eDIAG:
               u8_Type = mhu8_DATAELEMENT_TYPE_VARIABLE;
               break;
            case C_OSCNodeDataPool::eNVM:
               u8_Type = mhu8_DATAELEMENT_TYPE_PARAMETER;
               break;
            case C_OSCNodeDataPool::eCOM:
               u8_Type = mhu8_DATAELEMENT_TYPE_SIGNAL;
               break;
            case C_OSCNodeDataPool::eHALC:
               u8_Type = mhu8_DATAELEMENT_TYPE_HALC;
               break;
            }

            this->m_AddDataElementsResult(c_DataElementName, u32_DataElementCounter, c_ListName, u32_ListCounter,
                                          c_DataPoolName, ou32_DataPoolIndex, orc_NodeName, ou32_NodeIndex, u8_Type);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_SearchCanProtocolContent(const C_OSCCanProtocol & orc_CanProtocol,
                                                      const QString & orc_NodeName, const uint32 ou32_NodeIndex)
{
   uint32 u32_MessageContainerCounter;
   uint32 u32_MessageCounter;
   QString c_MessageName;
   const QString c_ProtocolName = C_PuiSdUtil::h_ConvertProtocolTypeToString(orc_CanProtocol.e_Type);
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      ou32_NodeIndex, orc_CanProtocol.u32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      // Search CAN messages
      for (u32_MessageContainerCounter = 0U;
           u32_MessageContainerCounter < orc_CanProtocol.c_ComMessages.size();
           ++u32_MessageContainerCounter)
      {
         const std::vector<C_OSCCanMessage> & rc_TxMessages =
            orc_CanProtocol.c_ComMessages[u32_MessageContainerCounter].c_TxMessages;
         const std::vector<C_OSCCanMessage> & rc_RxMessages =
            orc_CanProtocol.c_ComMessages[u32_MessageContainerCounter].c_RxMessages;
         const sint32 s32_ListIndexTx =
            C_OSCCanProtocol::h_GetListIndex(*pc_DataPool, u32_MessageContainerCounter, true);
         const sint32 s32_ListIndexRx =
            C_OSCCanProtocol::h_GetListIndex(*pc_DataPool, u32_MessageContainerCounter, false);

         // search the Tx messages
         for (u32_MessageCounter = 0U; u32_MessageCounter < rc_TxMessages.size(); ++u32_MessageCounter)
         {
            c_MessageName = rc_TxMessages[u32_MessageCounter].c_Name.c_str();

            if (c_MessageName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
            {
               this->m_AddMessageResult(c_MessageName, u32_MessageCounter, true, static_cast<uint32>(s32_ListIndexTx),
                                        c_ProtocolName, orc_CanProtocol.u32_DataPoolIndex, orc_NodeName,
                                        ou32_NodeIndex);
            }
         }

         // search the Rx messages
         for (u32_MessageCounter = 0U; u32_MessageCounter < rc_RxMessages.size(); ++u32_MessageCounter)
         {
            c_MessageName = rc_RxMessages[u32_MessageCounter].c_Name.c_str();

            if (c_MessageName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
            {
               this->m_AddMessageResult(c_MessageName, u32_MessageCounter, false, static_cast<uint32>(s32_ListIndexRx),
                                        c_ProtocolName, orc_CanProtocol.u32_DataPoolIndex, orc_NodeName,
                                        ou32_NodeIndex);
            }
         }
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Searching in HALC configuration of node

   \param[in]       orc_HalcConfig     HALC configuration for searching
   \param[in]       orc_NodeName       Name of node
   \param[in]       ou32_NodeIndex     Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_SearchHalcConfigurationContent(const stw_opensyde_core::C_OSCHalcConfig & orc_HalcConfig,
                                                            const QString & orc_NodeName,
                                                            const stw_types::uint32 ou32_NodeIndex)
{
   const uint32 u32_DomainCount = orc_HalcConfig.GetDomainSize();
   uint32 u32_DomainCounter;

   for (u32_DomainCounter = 0U; u32_DomainCounter < u32_DomainCount; ++u32_DomainCounter)
   {
      const C_OSCHalcDefDomain * const pc_HalcDef = orc_HalcConfig.GetDomainDefDataConst(u32_DomainCounter);

      if (pc_HalcDef != NULL)
      {
         QString c_DomainName;
         c_DomainName = pc_HalcDef->c_Name.c_str();

         if (pc_HalcDef->c_Channels.size() == 0)
         {
            // Handling domains without any channels
            if (c_DomainName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
            {
               this->m_AddHalcChannelResult(c_DomainName, u32_DomainCounter, "", 0U, false, orc_NodeName,
                                            ou32_NodeIndex, pc_HalcDef->e_Category);
            }
         }
         else
         {
            // Handling domains with channels
            // Search for domain name is not possible in this case
            const C_OSCHalcConfigDomain * const pc_HalcConf =
               orc_HalcConfig.GetDomainConfigDataConst(u32_DomainCounter);

            if (pc_HalcConf != NULL)
            {
               uint32 u32_ChannelCounter;
               tgl_assert(pc_HalcDef->c_Channels.size() == pc_HalcConf->c_ChannelConfigs.size());

               for (u32_ChannelCounter = 0U; u32_ChannelCounter < pc_HalcDef->c_Channels.size(); ++u32_ChannelCounter)
               {
                  QString c_UserName = pc_HalcConf->c_ChannelConfigs[u32_ChannelCounter].c_Name.c_str();
                  QString c_DefName = pc_HalcDef->c_Channels[u32_ChannelCounter].c_Name.c_str();

                  if ((c_UserName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true) ||
                      (c_DefName.contains(this->mc_SearchString, Qt::CaseInsensitive) == true))
                  {
                     QString c_ShownName = c_UserName + " (" + c_DefName + ")";
                     this->m_AddHalcChannelResult(c_DomainName, u32_DomainCounter, c_ShownName, u32_ChannelCounter,
                                                  true, orc_NodeName, ou32_NodeIndex, pc_HalcDef->e_Category);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_SearchBusContent(const C_OSCSystemBus & orc_Bus, const uint32 ou32_BusIndex)
{
   QString c_Name;

   c_Name = orc_Bus.c_Name.c_str();

   if (c_Name.contains(this->mc_SearchString, Qt::CaseInsensitive) == true)
   {
      if (orc_Bus.e_Type == C_OSCSystemBus::eETHERNET)
      {
         this->m_AddBusResult(c_Name, ou32_BusIndex, true);
      }
      else
      {
         this->m_AddBusResult(c_Name, ou32_BusIndex, false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SdSearchTreeWidget::m_CheckWhereToJumpCan(const QTreeWidgetItem * const opc_Item,
                                                 uint32 & oru32_InterfaceIndex) const
{
   // default case is open node
   bool q_Return = false;
   const sintn sn_NodeIndex = opc_Item->data(0, mhsn_DATAROLE_INDEX_1).toInt();
   const sint32 s32_DataPoolIndex = opc_Item->data(0, mhsn_DATAROLE_INDEX_2).toInt();
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      sn_NodeIndex, s32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      // the datapool must be a COM datapool ...
      if (pc_DataPool->e_Type == C_OSCNodeDataPool::eCOM)
      {
         // the node protocol must be "connected" to a bus
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(sn_NodeIndex);
         const uint32 u32_InterfaceIndex = static_cast<uint32>(opc_Item->data(0, mhsn_DATAROLE_INDEX_3).toInt()) / 2;

         if (pc_Node != NULL)
         {
            uint32 u32_Counter;

            // search the correct protocol
            for (u32_Counter = 0U; u32_Counter < pc_Node->c_ComProtocols.size(); ++u32_Counter)
            {
               const C_OSCCanProtocol & rc_Protocol = pc_Node->c_ComProtocols[u32_Counter];
               if (rc_Protocol.u32_DataPoolIndex == static_cast<uint32>(s32_DataPoolIndex))
               {
                  if (u32_InterfaceIndex < rc_Protocol.c_ComMessages.size())
                  {
                     // flag if the protocol is "connected" to the bus
                     q_Return = rc_Protocol.c_ComMessages[u32_InterfaceIndex].q_IsComProtocolUsedByInterface;

                     // get the bus index
                     if (u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
                     {
                        oru32_InterfaceIndex = pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex].u32_BusIndex;
                     }
                  }

                  break;
               }
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_ItemClicked(void)
{
   QTreeWidgetItem * pc_Item = this->currentItem();

   if (pc_Item != NULL)
   {
      if ((pc_Item == this->mpc_TreeItemRootNodes) ||
          (pc_Item == this->mpc_TreeItemRootBusses) ||
          (pc_Item == this->mpc_TreeItemRootDataPools) ||
          (pc_Item == this->mpc_TreeItemRootLists) ||
          (pc_Item == this->mpc_TreeItemRootDataElements) ||
          (pc_Item == this->mpc_TreeItemRootMessages) ||
          (pc_Item == this->mpc_TreeItemRootApplications) ||
          (pc_Item == this->mpc_TreeItemRootHalcChannels))
      {
         //do nothing on root click
      }
      else
      {
         QString c_Text = pc_Item->data(0, mhsn_DATAROLE_NAME).toString();
         const sintn sn_Type = pc_Item->data(0, mhsn_DATAROLE_INDEX_5).toInt();
         uint32 u32_Index;
         uint32 u32_BusIndex = 0U;
         sint32 s32_Flag;
         bool q_OpenBus = false;
         bool q_OpenHalc = false;

         // special cases: check where to jump, node or bus if a not clear case was clicked
         if ((pc_Item->parent() == this->mpc_TreeItemRootDataElements) && (sn_Type == mhu8_DATAELEMENT_TYPE_SIGNAL))
         {
            q_OpenBus = this->m_CheckWhereToJumpCan(pc_Item, u32_BusIndex);
            // flag for handling CAN signals
            s32_Flag = 1;
         }
         else if (pc_Item->parent() == this->mpc_TreeItemRootMessages)
         {
            q_OpenBus = this->m_CheckWhereToJumpCan(pc_Item, u32_BusIndex);
            // flag for handling CAN messages
            s32_Flag = 2;
         }
         else if (pc_Item->parent() == this->mpc_TreeItemRootApplications)
         {
            // flag for handling applications/data blocks
            s32_Flag = 3;
         }
         else if ((pc_Item->parent() == this->mpc_TreeItemRootHalcChannels) ||
                  ((pc_Item->parent() == this->mpc_TreeItemRootDataElements) &&
                   (sn_Type == mhu8_DATAELEMENT_TYPE_HALC)))
         {
            q_OpenHalc = true;
            // flag for handling HALC view, but no specific channel
            s32_Flag = 4;
         }
         else
         {
            // default handling
            s32_Flag = 0;
         }

         if (q_OpenBus == true)
         {
            // special case: jump to bus
            u32_Index = u32_BusIndex;
         }
         else
         {
            // normal case: decision by category
            u32_Index = static_cast<uint32>(pc_Item->data(0, mhsn_DATAROLE_INDEX_1).toInt());
         }

         if ((pc_Item->parent() == this->mpc_TreeItemRootNodes) ||
             (pc_Item->parent() == this->mpc_TreeItemRootDataPools) ||
             (pc_Item->parent() == this->mpc_TreeItemRootLists) ||
             (pc_Item->parent() == this->mpc_TreeItemRootApplications) ||
             (pc_Item->parent() == this->mpc_TreeItemRootHalcChannels) ||
             ((pc_Item->parent() == this->mpc_TreeItemRootDataElements) && (q_OpenBus == false)) ||
             ((pc_Item->parent() == this->mpc_TreeItemRootMessages) && (q_OpenBus == false)))
         {
            Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT, u32_Index, c_Text, ""));

            if ((pc_Item->parent() == this->mpc_TreeItemRootApplications) ||
                (pc_Item->parent() == this->mpc_TreeItemRootDataPools) ||
                (pc_Item->parent() == this->mpc_TreeItemRootLists) ||
                ((pc_Item->parent() == this->mpc_TreeItemRootDataElements) && (q_OpenHalc == false)) ||
                (pc_Item->parent() == this->mpc_TreeItemRootMessages))
            {
               // Datapool or application index
               sint32 s32_MainIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_2).toInt();
               // to make a difference between only datapool, list or dataelement
               sint32 s32_ListIndex = -1;
               sint32 s32_DataElementIndex = -1;

               if ((pc_Item->parent() == this->mpc_TreeItemRootLists) ||
                   (pc_Item->parent() == this->mpc_TreeItemRootDataElements) ||
                   (pc_Item->parent() == this->mpc_TreeItemRootMessages))
               {
                  s32_ListIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_3).toInt();

                  if ((pc_Item->parent() == this->mpc_TreeItemRootDataElements) ||
                      (pc_Item->parent() == this->mpc_TreeItemRootMessages))
                  {
                     s32_DataElementIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_4).toInt();
                  }
               }

               // open the concrete element
               Q_EMIT (this->SigOpenDetail(s32_MainIndex, s32_ListIndex, s32_DataElementIndex, -1, s32_Flag));
            }
            else if ((pc_Item->parent() == this->mpc_TreeItemRootDataElements) ||
                     (pc_Item->parent() == this->mpc_TreeItemRootHalcChannels))
            {
               // Open the HALC configuration
               sint32 s32_DomainIndex = -1;
               sint32 s32_ChannelIndex = -1;

               // In case of a selection in section data elements, no concrete domain or channel will be selected
               // Only in case of a selection of an element of the section channels
               if (pc_Item->parent() == this->mpc_TreeItemRootHalcChannels)
               {
                  const bool q_UseChannelIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_4).toBool();
                  s32_DomainIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_2).toInt();

                  if (q_UseChannelIndex == true)
                  {
                     s32_ChannelIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_3).toInt();
                  }
               }

               // open the concrete element
               Q_EMIT (this->SigOpenDetail(s32_DomainIndex, s32_ChannelIndex, -1, -1, s32_Flag));
            }
            else
            {
               // Nothing to do
            }
         }
         else if ((pc_Item->parent() == this->mpc_TreeItemRootBusses) ||
                  (pc_Item->parent() == this->mpc_TreeItemRootMessages) ||
                  (pc_Item->parent() == this->mpc_TreeItemRootDataElements))
         {
            if ((pc_Item->parent() == this->mpc_TreeItemRootMessages) ||
                (pc_Item->parent() == this->mpc_TreeItemRootDataElements))
            {
               // Special case: Search result showed as a part of node, but the bus edit will be opened.
               const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_BusIndex);

               if (pc_Bus != NULL)
               {
                  c_Text = pc_Bus->c_Name.c_str();
               }
            }

            Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, u32_Index, c_Text, ""));

            if ((pc_Item->parent() == this->mpc_TreeItemRootMessages) ||
                (pc_Item->parent() == this->mpc_TreeItemRootDataElements))
            {
               const sint32 s32_NodeIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_1).toInt();
               const sint32 s32_DataPoolIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_2).toInt();
               const sint32 s32_ListIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_3).toInt();
               const sint32 s32_DataElementIndex = pc_Item->data(0, mhsn_DATAROLE_INDEX_4).toInt();

               // open the signal or message
               Q_EMIT (this->SigOpenDetail(s32_NodeIndex, s32_DataPoolIndex, s32_ListIndex, s32_DataElementIndex,
                                           s32_Flag));
            }
         }
         else
         {
            // nothing to do
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddNodeResult(const QString & orc_NodeName, const uint32 ou32_NodeIndex,
                                           const QString & orc_DeviceType)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootNodes);
   QString c_ResultString = orc_NodeName;
   const QString c_Subtitle = C_GtGetText::h_GetText("Type: ") + orc_DeviceType;

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconNode);

   ++this->mu32_NodesFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootNodes->addChild(pc_Item);
   mpc_TreeItemRootNodes->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddBusResult(const QString & orc_BusName, const uint32 ou32_BusIndex,
                                          const bool oq_Ethernet)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootBusses);
   QString c_ResultString = orc_BusName;
   const QString c_Subtitle = C_GtGetText::h_GetText("Type: ");

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_BusName);

   if (oq_Ethernet == true)
   {
      pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle + "Ethernet");
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconBusEthernet);
   }
   else
   {
      pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle + "CAN");
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconBusCan);
   }
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_BusIndex));

   ++this->mu32_BussesFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootBusses->addChild(pc_Item);
   mpc_TreeItemRootBusses->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddDataPoolResult(const QString & orc_DataPoolName, const uint32 ou32_DataPoolIndex,
                                               const QString & orc_NodeName, const uint32 ou32_NodeIndex)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootDataPools);
   QString c_ResultString = orc_DataPoolName;
   const QString c_Subtitle = C_GtGetText::h_GetText("Node: ") + orc_NodeName;

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_2, static_cast<sintn>(ou32_DataPoolIndex));
   pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconDataPool);

   ++this->mu32_DataPoolsFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootDataPools->addChild(pc_Item);
   mpc_TreeItemRootDataPools->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddListResult(const QString & orc_ListName, const uint32 ou32_ListIndex,
                                           const QString & orc_DataPoolName, const uint32 ou32_DataPoolIndex,
                                           const QString & orc_NodeName, const uint32 ou32_NodeIndex)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootLists);
   QString c_ResultString = orc_ListName;
   const QString c_Subtitle = orc_NodeName + " - " + orc_DataPoolName;

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_2, static_cast<sintn>(ou32_DataPoolIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_3, static_cast<sintn>(ou32_ListIndex));
   pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconList);

   ++this->mu32_ListsFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootLists->addChild(pc_Item);
   mpc_TreeItemRootLists->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddMessageResult(const QString & orc_MessageName, const uint32 ou32_MessageIndex,
                                              const bool oq_IsTx, const uint32 ou32_ListIndex,
                                              const QString & orc_ProtocolName, const uint32 ou32_DataPoolIndex,
                                              const QString & orc_NodeName, const uint32 ou32_NodeIndex)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootMessages);
   QString c_ResultString = orc_MessageName;
   QString c_Subtitle =
      orc_NodeName + " - " + orc_ProtocolName + " - CAN" + QString::number(ou32_ListIndex) + " - ";

   if (oq_IsTx == true)
   {
      c_Subtitle += "Tx";
   }
   else
   {
      c_Subtitle += "Rx";
   }

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_2, static_cast<sintn>(ou32_DataPoolIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_3, static_cast<sintn>(ou32_ListIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_4, static_cast<sintn>(ou32_MessageIndex));
   pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconMessage);

   ++this->mu32_MessagesFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootMessages->addChild(pc_Item);
   mpc_TreeItemRootMessages->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddDataElementsResult(const QString & orc_DataElementName,
                                                   const uint32 ou32_DataElementIndex, const QString & orc_ListName,
                                                   const uint32 ou32_ListIndex, const QString & orc_DataPoolName,
                                                   const uint32 ou32_DataPoolIndex, const QString & orc_NodeName,
                                                   const uint32 ou32_NodeIndex, const uint8 ou8_Type)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootDataElements);
   QString c_ResultString = orc_DataElementName;
   const QString c_Subtitle = orc_NodeName + " - " + orc_DataPoolName + " - " + orc_ListName;

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_2, static_cast<sintn>(ou32_DataPoolIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_3, static_cast<sintn>(ou32_ListIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_4, static_cast<sintn>(ou32_DataElementIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_5, static_cast<sintn>(ou8_Type));

   if (ou8_Type == mhu8_DATAELEMENT_TYPE_VARIABLE)
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconVariable);
   }
   else if (ou8_Type == mhu8_DATAELEMENT_TYPE_PARAMETER)
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconParameter);
   }
   else if (ou8_Type == mhu8_DATAELEMENT_TYPE_SIGNAL)
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconSignal);
   }
   else
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconVariable);
   }

   ++this->mu32_DataElementsFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootDataElements->addChild(pc_Item);
   mpc_TreeItemRootDataElements->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddApplicationResult(const QString & orc_ApplicationName,
                                                  const uint32 ou32_ApplicationIndex, const QString & orc_NodeName,
                                                  const uint32 ou32_NodeIndex)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootApplications);
   QString c_ResultString = orc_ApplicationName;
   const QString c_Subtitle = C_GtGetText::h_GetText("Node: ") + orc_NodeName;

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_2, static_cast<sintn>(ou32_ApplicationIndex));
   pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconApplication);

   ++this->mu32_ApplicationsFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootApplications->addChild(pc_Item);
   mpc_TreeItemRootApplications->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adding an entry for a found HALC element (domain or channel)

   \param[in]       orc_HalcDomainName      Name of found HALC domain
   \param[in]       ou32_HalcDomainIndex    Index of found HALC domain
   \param[in]       orc_HalcChannelName     Name of found HALC channel
   \param[in]       ou32_HalcChannelIndex   Index of found HALC channel
   \param[in]       oq_UseElementIndex      Flag if the element is a domain or a channel
   \param[in]       orc_NodeName            Name of found node of HALC element
   \param[in]       ou32_NodeIndex          Index of found node of HALC element
   \param[in]       oe_Category             Category of domain (output, input, other)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_AddHalcChannelResult(const QString & orc_HalcDomainName, const uint32 ou32_HalcDomainIndex,
                                                  const QString & orc_HalcChannelName,
                                                  const uint32 ou32_HalcChannelIndex, const bool oq_UseChannelIndex,
                                                  const QString & orc_NodeName, const uint32 ou32_NodeIndex,
                                                  const C_OSCHalcDefDomain::E_Category oe_Category)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(this->mpc_TreeItemRootHalcChannels);
   QString c_ResultString;
   QString c_Subtitle;

   if (oq_UseChannelIndex == true)
   {
      c_ResultString = orc_HalcChannelName;
      c_Subtitle = orc_NodeName + " - " + orc_HalcDomainName;
   }
   else
   {
      c_ResultString = orc_HalcDomainName + " (-)";
      c_Subtitle = C_GtGetText::h_GetText("Node: ") + orc_NodeName;
   }

   this->m_MarkResultString(c_ResultString);

   pc_Item->setData(0, mhsn_DATAROLE_TITLE, c_ResultString);
   pc_Item->setData(0, mhsn_DATAROLE_SUBTITLE, c_Subtitle);
   pc_Item->setData(0, mhsn_DATAROLE_NAME, orc_NodeName);
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_1, static_cast<sintn>(ou32_NodeIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_2, static_cast<sintn>(ou32_HalcDomainIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_3, static_cast<sintn>(ou32_HalcChannelIndex));
   pc_Item->setData(0, mhsn_DATAROLE_INDEX_4, static_cast<sintn>(oq_UseChannelIndex));
   if (oe_Category == C_OSCHalcDefDomain::eCA_INPUT)
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconHalcInputChannel);
   }
   else if (oe_Category == C_OSCHalcDefDomain::eCA_OUTPUT)
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconHalcOutputChannel);
   }
   else
   {
      pc_Item->setData(0, static_cast<sintn>(Qt::DecorationRole), this->mc_IconHalcChannel);
   }

   ++this->mu32_HalcChannelsFound;
   this->mq_ResultsFound = true;

   mpc_TreeItemRootHalcChannels->addChild(pc_Item);
   mpc_TreeItemRootHalcChannels->setHidden(false);
   //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup/restore starting state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdSearchTreeWidget::m_SetupStartingState(void)
{
   this->mpc_TreeItemRootNodes = new QTreeWidgetItem();
   this->mpc_TreeItemRootNodes->setText(0, C_GtGetText::h_GetText("Nodes"));
   this->mpc_TreeItemRootNodes->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootNodes);
   this->mpc_TreeItemRootNodes->setHidden(true);

   this->mpc_TreeItemRootBusses = new QTreeWidgetItem();
   this->mpc_TreeItemRootBusses->setText(0, C_GtGetText::h_GetText("Buses"));
   this->mpc_TreeItemRootBusses->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootBusses);
   this->mpc_TreeItemRootBusses->setHidden(true);

   this->mpc_TreeItemRootDataPools = new QTreeWidgetItem();
   this->mpc_TreeItemRootDataPools->setText(0, C_GtGetText::h_GetText("Datapools"));
   this->mpc_TreeItemRootDataPools->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootDataPools);
   this->mpc_TreeItemRootDataPools->setHidden(true);

   this->mpc_TreeItemRootLists = new QTreeWidgetItem();
   this->mpc_TreeItemRootLists->setText(0, C_GtGetText::h_GetText("Lists"));
   this->mpc_TreeItemRootLists->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootLists);
   this->mpc_TreeItemRootLists->setHidden(true);

   this->mpc_TreeItemRootMessages = new QTreeWidgetItem();
   this->mpc_TreeItemRootMessages->setText(0, C_GtGetText::h_GetText("Messages"));
   this->mpc_TreeItemRootMessages->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootMessages);
   this->mpc_TreeItemRootMessages->setHidden(true);

   this->mpc_TreeItemRootDataElements = new QTreeWidgetItem();
   this->mpc_TreeItemRootDataElements->setText(0, C_GtGetText::h_GetText("Data elements"));
   this->mpc_TreeItemRootDataElements->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootDataElements);
   this->mpc_TreeItemRootDataElements->setHidden(true);

   this->mpc_TreeItemRootApplications = new QTreeWidgetItem();
   this->mpc_TreeItemRootApplications->setText(0, C_GtGetText::h_GetText("Data Blocks"));
   this->mpc_TreeItemRootApplications->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootApplications);
   this->mpc_TreeItemRootApplications->setHidden(true);

   this->mpc_TreeItemRootHalcChannels = new QTreeWidgetItem();
   this->mpc_TreeItemRootHalcChannels->setText(0, C_GtGetText::h_GetText("Channels"));
   this->mpc_TreeItemRootHalcChannels->setFlags(Qt::ItemIsEnabled); // item is not selectable
   this->addTopLevelItem(this->mpc_TreeItemRootHalcChannels);
   this->mpc_TreeItemRootHalcChannels->setHidden(true);
}
