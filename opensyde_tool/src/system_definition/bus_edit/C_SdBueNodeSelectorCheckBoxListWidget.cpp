//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handling all node selector widgets in a kind of list (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

#include "TglUtils.hpp"
#include "C_SdBueNodeSelectorCheckBoxListWidget.hpp"

#include "ui_C_SdBueNodeSelectorCheckBoxListWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueNodeSelectorCheckBoxListWidget::C_SdBueNodeSelectorCheckBoxListWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueNodeSelectorCheckBoxListWidget)
{
   this->mpc_Ui->setupUi(this);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->mpc_Ui->pc_ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->mpc_Ui->pc_ScrollArea->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this->mpc_Ui->pc_ScrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdBueNodeSelectorCheckBoxListWidget::m_ScrollBarRangeChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueNodeSelectorCheckBoxListWidget::~C_SdBueNodeSelectorCheckBoxListWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds new nodes to the checkbox list

   \param[in]     orc_Names       Names of Nodes
   \param[in]     orc_Indexes     Indexes of Nodes
   \param[in]     orc_SubIndexes  Sub Indexes of Nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::AddNodes(const std::vector<QString> & orc_Names,
                                                     const std::vector<uint32_t> & orc_Indexes,
                                                     const std::vector<uint32_t> & orc_SubIndexes)
{
   QSpacerItem * const pc_Spacer = new QSpacerItem(0, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);
   const int32_t s32_ItemCount = this->mpc_Ui->pc_CbxVerticalLayout->count();

   // clear all previous nodes
   for (int32_t s32_Counter = (s32_ItemCount - 1); s32_Counter >= 0; --s32_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_Counter);

      C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
         dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget *>(pc_Item->widget());
      this->mpc_Ui->pc_CbxVerticalLayout->removeItem(pc_Item);

      if (pc_SelectorItem != NULL)
      {
         // delete the widget
         disconnect(pc_SelectorItem, &C_SdBueNodeSelectorCheckBoxItemWidget::SigNodeToggled,
                    this, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled);
         disconnect(pc_SelectorItem, &C_SdBueNodeSelectorCheckBoxItemWidget::SigSwitchToCoManager,
                    this, &C_SdBueNodeSelectorCheckBoxListWidget::SigSwitchToCoManager);
         delete pc_SelectorItem;
      }

      // delete the layout item
      delete pc_Item;
   }

   if ((orc_Names.size() == orc_Indexes.size()) &&
       (orc_Names.size() == orc_SubIndexes.size()))
   {
      // add new nodes
      for (uint32_t u32_Counter = 0U; u32_Counter < orc_Names.size(); ++u32_Counter)
      {
         this->m_AddNode(orc_Names[u32_Counter], orc_Indexes[u32_Counter], orc_SubIndexes[u32_Counter]);
      }

      this->mpc_Ui->pc_CbxVerticalLayout->addSpacerItem(pc_Spacer);
      this->mpc_Ui->pc_CbxVerticalLayout->setStretch(this->mpc_Ui->pc_CbxVerticalLayout->count() - 1, 1);
   }
} //lint !e429  //no memory leak because of the parent of pc_Spacer by addSpacerItem and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks nodes in the checkbox list

   \param[in]     orc_Indexes        Indexes of Nodes
   \param[in]     orc_SubIndexes     Subindexes of Nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::CheckNodes(const std::vector<uint32_t> & orc_Indexes,
                                                       const std::vector<uint32_t> & orc_SubIndexes) const
{
   if (orc_Indexes.size() == orc_SubIndexes.size())
   {
      int32_t s32_Counter;
      uint32_t u32_Index;
      uint32_t u32_SubIndex;
      uint32_t u32_CounterIndex;
      const int32_t s32_ItemCount = this->mpc_Ui->pc_CbxVerticalLayout->count();

      // check all checkboxes which have these indexes and uncheck all other checkboxes
      for (s32_Counter = (s32_ItemCount - 1); s32_Counter >= 0; --s32_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_Counter);

         C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
            dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget *>(pc_Item->widget());

         if (pc_SelectorItem != NULL)
         {
            bool q_Checked = false;

            pc_SelectorItem->GetIndexes(u32_Index, u32_SubIndex);

            // check the index
            for (u32_CounterIndex = 0U; u32_CounterIndex < orc_Indexes.size(); ++u32_CounterIndex)
            {
               if ((u32_Index == orc_Indexes[u32_CounterIndex]) &&
                   (u32_SubIndex == orc_SubIndexes[u32_CounterIndex]))
               {
                  q_Checked = true;
                  break;
               }
            }
            pc_SelectorItem->SetChecked(q_Checked);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a specific node in the checkbox list

   \param[in]     ou32_Index        Index of Node
   \param[in]     ou32_SubIndex     Subindex of Node
   \param[in]     oq_Checked        Flag for checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::CheckSpecificNode(const uint32_t ou32_Index, const uint32_t ou32_SubIndex,
                                                              const bool oq_Checked) const
{
   C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem = this->m_GetItemWidget(ou32_Index, ou32_SubIndex);

   tgl_assert(pc_SelectorItem != NULL);
   if ((pc_SelectorItem != NULL) &&
       (pc_SelectorItem->IsChecked() != oq_Checked))
   {
      pc_SelectorItem->SetChecked(oq_Checked);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a specific node in the checkbox list as CANopen Manager

   \param[in]     ou32_Index        Index of Node
   \param[in]     ou32_SubIndex     Subindex of Node
   \param[in]     oq_Manager        Flag if node is a CANopen Manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::SetSpecificNodeAsManager(const uint32_t ou32_Index,
                                                                     const uint32_t ou32_SubIndex,
                                                                     const bool oq_Manager) const
{
   C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem = this->m_GetItemWidget(ou32_Index, ou32_SubIndex);

   if (pc_SelectorItem != NULL)
   {
      pc_SelectorItem->SetNodeAsManager(oq_Manager);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a specific node in the checkbox list as CANopen Device

   \param[in]     ou32_Index              Index of Device Node
   \param[in]     ou32_SubIndex           Subindex of Device Node
   \param[in]     oq_Device               Flag if node is a CANopen Device
   \param[in]     opc_DeviceId            Device Id (NULL if not a device)
   \param[in]     ou32_ManagerNodeIndex   Index of Manager Node which the Device is assigned to (if oq_Device == true)
   \param[in]     ou32_ManagerIntfIndex   Interface index of Manager Node which the Device is
                                          assigned to (if oq_Device == true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::SetSpecificNodeAsDevice(const uint32_t ou32_Index,
                                                                    const uint32_t ou32_SubIndex, const bool oq_Device,
                                                                    const stw::opensyde_core::C_OscCanInterfaceId * const opc_DeviceId, const uint32_t ou32_ManagerNodeIndex,
                                                                    const uint32_t ou32_ManagerIntfIndex)
const
{
   C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem = this->m_GetItemWidget(ou32_Index, ou32_SubIndex);

   if (pc_SelectorItem != NULL)
   {
      pc_SelectorItem->SetNodeAsDevice(oq_Device, opc_DeviceId, ou32_ManagerNodeIndex, ou32_ManagerIntfIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual protocol to mark the protocol using nodes

   \param[in]     oe_Protocol     Protocol id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::SetProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol)
const
{
   for (int32_t s32_ItItem = 0; s32_ItItem < this->mpc_Ui->pc_CbxVerticalLayout->count(); ++s32_ItItem)
   {
      QLayoutItem * const pc_Layout = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_ItItem);
      if (pc_Layout != NULL)
      {
         C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
            dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget * const>(pc_Layout->widget());
         if (pc_SelectorItem != NULL)
         {
            pc_SelectorItem->SetProtocol(oe_Protocol);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates in case of changed the items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::Refresh(void) const
{
   for (int32_t s32_ItItem = 0; s32_ItItem < this->mpc_Ui->pc_CbxVerticalLayout->count(); ++s32_ItItem)
   {
      QLayoutItem * const pc_Layout = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_ItItem);
      if (pc_Layout != NULL)
      {
         C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
            dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget * const>(pc_Layout->widget());
         if (pc_SelectorItem != NULL)
         {
            pc_SelectorItem->UpdateToolTip();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new node to the checkbox list

   \param[in]     orc_Name       Name of Node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::m_AddNode(const QString & orc_Name, const uint32_t ou32_Index,
                                                      const uint32_t ou32_SubIndex)
{
   C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
      new C_SdBueNodeSelectorCheckBoxItemWidget(ou32_Index,
                                                ou32_SubIndex,
                                                orc_Name,
                                                this);

   connect(pc_SelectorItem, &C_SdBueNodeSelectorCheckBoxItemWidget::SigNodeToggled,
           this, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled);
   connect(pc_SelectorItem, &C_SdBueNodeSelectorCheckBoxItemWidget::SigSwitchToCoManager,
           this, &C_SdBueNodeSelectorCheckBoxListWidget::SigSwitchToCoManager);

   this->mpc_Ui->pc_CbxVerticalLayout->addWidget(pc_SelectorItem);
} //lint !e429  //no memory leak because of the parent of pc_SelectorItem by addWidget and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::m_ScrollBarRangeChanged(const int32_t os32_Min,
                                                                    const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->hide();
   }
   else
   {
      this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets a specific node in the checkbox list

   \param[in]     ou32_Index        Index of Node
   \param[in]     ou32_SubIndex     Subindex of Node

   \retval   Valid pointer   Item found
   \retval   NULL            No item with matching parameters found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueNodeSelectorCheckBoxItemWidget * C_SdBueNodeSelectorCheckBoxListWidget::m_GetItemWidget(
   const uint32_t ou32_Index, const uint32_t ou32_SubIndex)
const
{
   int32_t s32_Counter;
   C_SdBueNodeSelectorCheckBoxItemWidget * pc_Return = NULL;

   // check all checkboxes which have these indexes and uncheck all other checkboxes
   for (s32_Counter = 0; s32_Counter < this->mpc_Ui->pc_CbxVerticalLayout->count(); ++s32_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_Counter);

      C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
         dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget *>(pc_Item->widget());

      if (pc_SelectorItem != NULL)
      {
         uint32_t u32_Index;
         uint32_t u32_SubIndex;

         pc_SelectorItem->GetIndexes(u32_Index, u32_SubIndex);

         if ((u32_Index == ou32_Index) &&
             (u32_SubIndex == ou32_SubIndex))
         {
            pc_Return = pc_SelectorItem;
            break;
         }
      }
   }

   return pc_Return;
}
