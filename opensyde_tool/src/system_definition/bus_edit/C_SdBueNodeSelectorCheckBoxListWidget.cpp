//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handling all node selector widgets in a kind of list (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

#include "C_SdBueNodeSelectorCheckBoxListWidget.h"

#include "ui_C_SdBueNodeSelectorCheckBoxListWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

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
                                                     const std::vector<uint32> & orc_Indexes,
                                                     const std::vector<uint32> & orc_SubIndexes)
{
   uint32 u32_Counter;
   sintn sn_Counter;
   QSpacerItem * const pc_Spacer = new QSpacerItem(0, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);
   const sintn sn_ItemCount = this->mpc_Ui->pc_CbxVerticalLayout->count();

   // clear all previous nodes
   for (sn_Counter = (sn_ItemCount - 1); sn_Counter >= 0; --sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
         dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget *>(pc_Item->widget());
      this->mpc_Ui->pc_CbxVerticalLayout->removeItem(pc_Item);

      if (pc_SelectorItem != NULL)
      {
         // delete the widget
         //lint -e{64, 918, 1025, 1703}  false positive because of C++11 use of Qt
         disconnect(pc_SelectorItem, &C_SdBueNodeSelectorCheckBoxItemWidget::SigNodeToggled,
                    this, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled);
         delete pc_SelectorItem;
      }

      // delete the layout item
      delete pc_Item;
   }

   if ((orc_Names.size() == orc_Indexes.size()) &&
       (orc_Names.size() == orc_SubIndexes.size()))
   {
      // add new nodes
      for (u32_Counter = 0U; u32_Counter < orc_Names.size(); ++u32_Counter)
      {
         this->m_AddNode(orc_Names[u32_Counter], orc_Indexes[u32_Counter], orc_SubIndexes[u32_Counter]);
      }

      this->mpc_Ui->pc_CbxVerticalLayout->addSpacerItem(pc_Spacer);
      this->mpc_Ui->pc_CbxVerticalLayout->setStretch(this->mpc_Ui->pc_CbxVerticalLayout->count() - 1, 1);
   }
   //lint -e{429}  no memory leak because of the parent of pc_Spacer by addSpacerItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks nodes in the checkbox list

   \param[in]     orc_Indexes        Indexes of Nodes
   \param[in]     orc_SubIndexes     Subindexes of Nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::CheckNodes(const std::vector<uint32> & orc_Indexes,
                                                       const std::vector<uint32> & orc_SubIndexes) const
{
   if (orc_Indexes.size() == orc_SubIndexes.size())
   {
      sintn sn_Counter;
      uint32 u32_Index;
      uint32 u32_SubIndex;
      uint32 u32_CounterIndex;
      const sintn sn_ItemCount = this->mpc_Ui->pc_CbxVerticalLayout->count();

      // check all checkboxes which have these indexes and uncheck all other checkboxes
      for (sn_Counter = (sn_ItemCount - 1); sn_Counter >= 0; --sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(sn_Counter);
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
void C_SdBueNodeSelectorCheckBoxListWidget::CheckSpecificNode(const uint32 ou32_Index, const uint32 ou32_SubIndex,
                                                              const bool oq_Checked) const
{
   sintn sn_Counter;

   // check all checkboxes which have these indexes and uncheck all other checkboxes
   for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_CbxVerticalLayout->count(); ++sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdBueNodeSelectorCheckBoxItemWidget * const pc_SelectorItem =
         dynamic_cast<C_SdBueNodeSelectorCheckBoxItemWidget *>(pc_Item->widget());

      if (pc_SelectorItem != NULL)
      {
         uint32 u32_Index;
         uint32 u32_SubIndex;

         pc_SelectorItem->GetIndexes(u32_Index, u32_SubIndex);

         if ((u32_Index == ou32_Index) &&
             (u32_SubIndex == ou32_SubIndex))
         {
            if (pc_SelectorItem->IsChecked() != oq_Checked)
            {
               pc_SelectorItem->SetChecked(oq_Checked);
            }
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual protocol to mark the protocol using nodes

   \param[in]     oe_Protocol     Protocol id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol)
const
{
   for (sint32 s32_ItItem = 0; s32_ItItem < this->mpc_Ui->pc_CbxVerticalLayout->count(); ++s32_ItItem)
   {
      QLayoutItem * const pc_Layout = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_ItItem);
      if (pc_Layout != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
   for (sint32 s32_ItItem = 0; s32_ItItem < this->mpc_Ui->pc_CbxVerticalLayout->count(); ++s32_ItItem)
   {
      QLayoutItem * const pc_Layout = this->mpc_Ui->pc_CbxVerticalLayout->itemAt(s32_ItItem);
      if (pc_Layout != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
void C_SdBueNodeSelectorCheckBoxListWidget::m_AddNode(const QString & orc_Name, const uint32 ou32_Index,
                                                      const uint32 ou32_SubIndex)
{
   C_SdBueNodeSelectorCheckBoxItemWidget * pc_SelectorItem = new C_SdBueNodeSelectorCheckBoxItemWidget(ou32_Index,
                                                                                                       ou32_SubIndex,
                                                                                                       orc_Name,
                                                                                                       this);

   //lint -e{64, 918, 1025, 1703}  false positive because of C++11 use of Qt
   connect(pc_SelectorItem, &C_SdBueNodeSelectorCheckBoxItemWidget::SigNodeToggled,
           this, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled);

   this->mpc_Ui->pc_CbxVerticalLayout->addWidget(pc_SelectorItem);

   //lint -e{429}  no memory leak because of the parent of pc_SelectorItem by addWidget and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxListWidget::m_ScrollBarRangeChanged(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->hide();
   }
   else
   {
      this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->show();
   }
}
