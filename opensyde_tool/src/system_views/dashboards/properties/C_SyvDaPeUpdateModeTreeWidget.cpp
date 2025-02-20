//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element update mode tree (implementation)

   Widget for system view dashboard data element update mode tree

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDaPeUpdateModeTableView.hpp"
#include "C_SyvDaPeUpdateModeNodeHeader.hpp"
#include "C_SyvDaPeUpdateModeTreeWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SyvDaPeUpdateModeTreeWidget::C_SyvDaPeUpdateModeTreeWidget(QWidget * const opc_Parent) :
   QTreeWidget(opc_Parent)
{
   //UI Settings
   this->setEditTriggers(QAbstractItemView::NoEditTriggers);
   this->setDragEnabled(false);
   this->setDragDropMode(QAbstractItemView::NoDragDrop);
   this->setDefaultDropAction(Qt::TargetMoveAction);
   this->setAlternatingRowColors(true);
   this->setSelectionMode(QAbstractItemView::NoSelection);
   this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   this->setAutoExpandDelay(500);
   this->setIndentation(15);
   this->setRootIsDecorated(false);
   this->setAnimated(true);
   this->setHeaderHidden(true);
   this->setAcceptDrops(false);
   this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
   this->setAutoScroll(false);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   //Avoid styling table inside
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->verticalScrollBar(), "C_SyvDaPeUpdateModeTreeWidget", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->horizontalScrollBar(), "C_SyvDaPeUpdateModeTreeWidget", true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Connect
   connect(this, &C_SyvDaPeUpdateModeTreeWidget::collapsed, this, &C_SyvDaPeUpdateModeTreeWidget::m_OnCollapse);
   connect(this, &C_SyvDaPeUpdateModeTreeWidget::expanded, this, &C_SyvDaPeUpdateModeTreeWidget::m_OnExpand);
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SyvDaPeUpdateModeTreeWidget::m_ScrollBarRangeChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeTreeWidget::~C_SyvDaPeUpdateModeTreeWidget(void)
{
   m_Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize update mode content

   \param[in] ou32_ViewIndex View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::Init(const uint32_t ou32_ViewIndex)
{
   std::vector<uint8_t> c_NodeActiveFlags;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      ou32_ViewIndex,
      c_NodeActiveFlags);

   m_Clear();
   if (s32_Retval == C_NO_ERR)
   {
      //Step 1: Initialize all necessary models
      this->mc_Models.reserve(c_NodeActiveFlags.size());
      for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeActiveFlags.size(); ++u32_ItNode)
      {
         C_SyvDaPeUpdateModeTableModel * const pc_Tmp = new C_SyvDaPeUpdateModeTableModel(ou32_ViewIndex, u32_ItNode);
         //Only store if relevant
         if (pc_Tmp->rowCount() > 0)
         {
            this->mc_Models.push_back(pc_Tmp);
         }
         else
         {
            delete pc_Tmp;
         }
      } //lint !e593    //Cleaned up at later point
      //Step 2: Based on model count initialize internal tree structure and assign widgets (Qt takes ownership)
      this->clear();
      for (uint32_t u32_ItModel = 0; u32_ItModel < this->mc_Models.size(); ++u32_ItModel)
      {
         C_SyvDaPeUpdateModeTableModel * const pc_Model = this->mc_Models[u32_ItModel];

         //Check valid entry
         if (pc_Model != NULL)
         {
            const C_OscNodeDataPoolListElementId * const pc_Id = pc_Model->GetIndex(0);
            if (pc_Id != NULL)
            {
               QTreeWidgetItem * const pc_TopLevelItem =
                  new QTreeWidgetItem(static_cast<int32_t>(QTreeWidgetItem::UserType));
               QTreeWidgetItem * const pc_ViewItem =
                  new QTreeWidgetItem(pc_TopLevelItem, static_cast<int32_t>(QTreeWidgetItem::UserType));
               C_SyvDaPeUpdateModeTableView * const pc_Table = new C_SyvDaPeUpdateModeTableView(ou32_ViewIndex, this,
                                                                                                pc_ViewItem);
               C_SyvDaPeUpdateModeNodeHeader * const pc_Header = new C_SyvDaPeUpdateModeNodeHeader(ou32_ViewIndex,
                                                                                                   pc_Id->u32_NodeIndex,
                                                                                                   pc_TopLevelItem);
               //Set up table
               pc_Table->setModel(pc_Model);
               //Init after model
               pc_Table->InitColumns();
               pc_Table->AdjustToItems(true, (u32_ItModel == (static_cast<uint32_t>(this->mc_Models.size()) - 1UL)));

               //Connects
               connect(pc_Header, &C_SyvDaPeUpdateModeNodeHeader::SigExpand, this,
                       &C_SyvDaPeUpdateModeTreeWidget::m_OnExpandRequested);
               connect(pc_Model, &C_SyvDaPeUpdateModeTableModel::SigTransmissionCountChange, pc_Header,
                       &C_SyvDaPeUpdateModeNodeHeader::UpdateCount);

               //Insert header
               this->addTopLevelItem(pc_TopLevelItem);
               this->setItemWidget(pc_TopLevelItem, 0, pc_Header);

               //Insert table
               this->setItemWidget(pc_ViewItem, 0, pc_Table);
            } //lint !e429  //Cleaned up via Qt parent mechanism
         }
      }
      //Final step
      this->setItemsExpandable(false);
      this->expandAll();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select specified ID

   \param[in] orc_Id ID to select
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::Select(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   if (orc_Id.GetIsValid() == true)
   {
      //Find node
      for (uint32_t u32_It = 0; u32_It < this->mc_Models.size(); ++u32_It)
      {
         const stw::opensyde_gui_logic::C_SyvDaPeUpdateModeTableModel * const pc_Model = this->mc_Models[u32_It];
         if (pc_Model != NULL)
         {
            const C_OscNodeDataPoolListElementId * const pc_ElementId = pc_Model->GetIndex(0);
            if (pc_ElementId != NULL)
            {
               if (orc_Id.u32_NodeIndex == pc_ElementId->u32_NodeIndex)
               {
                  //Match node
                  const QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(static_cast<int32_t>(u32_It));
                  if (pc_TopLevelItem != NULL)
                  {
                     QTreeWidgetItem * const pc_ViewItem = pc_TopLevelItem->child(0);

                     if (pc_ViewItem != NULL)
                     {
                        C_SyvDaPeUpdateModeTableView * const pc_Table =
                           dynamic_cast<C_SyvDaPeUpdateModeTableView * const>(this->itemWidget(pc_ViewItem, 0));
                        if (pc_Table != NULL)
                        {
                           //Find row
                           for (int32_t s32_ItRow = 0; s32_ItRow < pc_Model->rowCount(); ++s32_ItRow)
                           {
                              const C_OscNodeDataPoolListElementId * const pc_CurElementId = pc_Model->GetIndex(
                                 s32_ItRow);
                              if (pc_CurElementId != NULL)
                              {
                                 if (orc_Id == *pc_CurElementId)
                                 {
                                    const QModelIndex c_TopLevelIndex = this->model()->index(u32_It, 0);
                                    //Match row
                                    pc_Table->selectRow(s32_ItRow);

                                    // Manual scrolling on the top level layer necessary
                                    this->scrollTo(c_TopLevelIndex);

                                    if (u32_It == static_cast<uint32_t>(this->mc_Models.size() - 1))
                                    {
                                       // Last table view. It has no own scroll bar.
                                       // The main scroll bar must handle this. The problem: The normal scroll functions
                                       // does not work here. The position must be set manually
                                       QScrollBar * const pc_ScrollBar = this->verticalScrollBar();
                                       if (pc_ScrollBar != NULL)
                                       {
                                          const int32_t s32_SliderPos = pc_ScrollBar->sliderPosition();
                                          const int32_t s32_SliderMax = pc_ScrollBar->maximum();
                                          const int32_t s32_ItemRelPos = (s32_ItRow * 100) / pc_Model->rowCount();
                                          const int32_t s32_SliderOffset =
                                             ((s32_SliderMax - s32_SliderPos) * s32_ItemRelPos) / 100;
                                          pc_ScrollBar->setSliderPosition(s32_SliderPos + s32_SliderOffset);
                                       }
                                    }
                                    break;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply data from internal storage to global storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::ApplyData(void) const
{
   for (uint32_t u32_ItModel = 0; u32_ItModel < this->mc_Models.size(); ++u32_ItModel)
   {
      const C_SyvDaPeUpdateModeTableModel * const pc_Model = this->mc_Models[u32_ItModel];
      if (pc_Model != NULL)
      {
         pc_Model->ApplyData();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::m_Clear(void)
{
   for (uint32_t u32_ItModel = 0; u32_ItModel < this->mc_Models.size(); ++u32_ItModel)
   {
      delete (this->mc_Models[u32_ItModel]);
   }
   this->mc_Models.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle expand request

   \param[in] opc_Item  Affected tree item widget
   \param[in] oq_Expand true: expand
                        false collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::m_OnExpandRequested(QTreeWidgetItem * const opc_Item, const bool oq_Expand)
{
   if (oq_Expand == true)
   {
      this->expand(this->indexFromItem(opc_Item));
   }
   else
   {
      this->collapse(this->indexFromItem(opc_Item));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle collapse event

   \param[in] orc_Index Collapsed item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::m_OnCollapse(const QModelIndex & orc_Index) const
{
   const C_SyvDaPeUpdateModeNodeHeader * const pc_Widget =
      dynamic_cast<C_SyvDaPeUpdateModeNodeHeader * const>(this->indexWidget(orc_Index));

   if (pc_Widget != NULL)
   {
      pc_Widget->RegisterExpandOrCollapse(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle expand event

   \param[in] orc_Index Expanded item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::m_OnExpand(const QModelIndex & orc_Index) const
{
   const C_SyvDaPeUpdateModeNodeHeader * const pc_Widget =
      dynamic_cast<C_SyvDaPeUpdateModeNodeHeader * const>(this->indexWidget(orc_Index));

   if (pc_Widget != NULL)
   {
      pc_Widget->RegisterExpandOrCollapse(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTreeWidget::m_ScrollBarRangeChanged(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}
