//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for view widgets (implementation)

   List widget for view widgets

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include "constants.hpp"
#include "C_NagViewList.hpp"
#include "C_PuiSvHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagViewList::C_NagViewList(QWidget * const opc_Parent) :
   QListView(opc_Parent)
{
   this->C_NagViewList::setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);
   this->mc_Delegate.SetListView(this);

   //Options
   this->setDragEnabled(true);
   this->setAcceptDrops(true);
   this->setDragDropMode(QAbstractItemView::DragDrop);
   this->setDropIndicatorShown(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagViewList::~C_NagViewList(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::Init(void)
{
   this->mc_Model.SetNumRows(static_cast<int32_t>(C_PuiSvHandler::h_GetInstance()->GetViewCount()));
   for (uint32_t u32_ItView = 0UL; u32_ItView < C_PuiSvHandler::h_GetInstance()->GetViewCount(); ++u32_ItView)
   {
      const QModelIndex c_Index = this->mc_Model.index(u32_ItView);
      C_NagViewItem * pc_ViewWidget = dynamic_cast<C_NagViewItem *>(this->indexWidget(c_Index));
      if (pc_ViewWidget == NULL)
      {
         //Add item widget
         pc_ViewWidget = new C_NagViewItem(this);
         this->setIndexWidget(c_Index, pc_ViewWidget);
         // scroll to new item
         this->scrollTo(c_Index);

         connect(pc_ViewWidget, &C_NagViewItem::SigStartDrag, this, &C_NagViewList::m_OnStartDrag);
         connect(pc_ViewWidget, &C_NagViewItem::SigSetName, this, &C_NagViewList::m_OnSetName);
         connect(pc_ViewWidget, &C_NagViewItem::SigExpanded, this, &C_NagViewList::m_OnExpand);
         connect(pc_ViewWidget, &C_NagViewItem::SigDuplicate, this, &C_NagViewList::m_OnDuplicate);
         connect(pc_ViewWidget, &C_NagViewItem::SigDelete, this, &C_NagViewList::m_OnDelete);
         connect(pc_ViewWidget, &C_NagViewItem::SigSelect, this, &C_NagViewList::m_OnSelect);
      }
      pc_ViewWidget->Init(u32_ItView);
      //lint -e429 Clean up via Qt parent mechanism
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::LoadUserSettings(void) const
{
   for (int32_t s32_ItView = 0UL; s32_ItView < this->mc_Model.rowCount(); ++s32_ItView)
   {
      C_NagViewItem * const pc_View = this->GetItemAt(s32_ItView);
      if (pc_View != NULL)
      {
         pc_View->LoadUserSettings();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::SaveUserSettings(void) const
{
   for (int32_t s32_ItView = 0UL; s32_ItView < this->mc_Model.rowCount(); ++s32_ItView)
   {
      const C_NagViewItem * const pc_View = this->GetItemAt(s32_ItView);
      if (pc_View != NULL)
      {
         pc_View->SaveUserSettings();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update all view names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::UpdateNames(void) const
{
   for (int32_t s32_ItView = 0UL; s32_ItView < this->mc_Model.rowCount(); ++s32_ItView)
   {
      C_NagViewItem * const pc_View = this->GetItemAt(s32_ItView);
      if (pc_View != NULL)
      {
         pc_View->UpdateName();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger update of decoration role

   \param[in]  oq_CheckOnlyThisView    Flag to reduce view error check to one item
   \param[in]  ou32_ViewIndex          Index to specify which view changed (only used if oq_CheckOnlyThisView set)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::UpdateDeco(const bool oq_CheckOnlyThisView, const uint32_t ou32_ViewIndex) const
{
   if (oq_CheckOnlyThisView == true)
   {
      C_NagViewItem * const pc_View = this->GetItemAt(static_cast<int32_t>(ou32_ViewIndex));
      if (pc_View != NULL)
      {
         pc_View->UpdateDeco();
      }
   }
   else
   {
      for (int32_t s32_ItView = 0UL; s32_ItView < this->mc_Model.rowCount(); ++s32_ItView)
      {
         C_NagViewItem * const pc_View = this->GetItemAt(s32_ItView);
         if (pc_View != NULL)
         {
            pc_View->UpdateDeco();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the maximum height required to display all items of this widget in their current state

   \return
   The maximum height required to display all items of this widget in their current state
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_NagViewList::GetMaximumRequiredHeight(void) const
{
   int32_t s32_Retval = 0;

   for (int32_t s32_ItRow = 0; s32_ItRow < this->mc_Model.rowCount(); ++s32_ItRow)
   {
      const QStyleOptionViewItem c_Style;
      const QModelIndex c_Index = this->mc_Model.index(s32_ItRow);
      const QSize c_Size = this->mc_Delegate.sizeHint(c_Style, c_Index);
      s32_Retval += c_Size.height();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active state

   \param[in]  ou32_ViewIndex    Active view index
   \param[in]  os32_SubMode      New active sub mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::SetActive(const uint32_t ou32_ViewIndex, const int32_t os32_SubMode) const
{
   for (int32_t s32_ItView = 0UL; s32_ItView < this->mc_Model.rowCount(); ++s32_ItView)
   {
      C_NagViewItem * const pc_View = this->GetItemAt(s32_ItView);
      if (pc_View != NULL)
      {
         if (ou32_ViewIndex == static_cast<uint32_t>(s32_ItView))
         {
            pc_View->SetActive(true, os32_SubMode);
         }
         else
         {
            pc_View->SetActive(false, os32_SubMode);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item widget at index

   \param[in]  os32_Index   Item index

   \return
   NULL Item not found
   Else Valid item
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagViewItem * C_NagViewList::GetItemAt(const int32_t os32_Index) const
{
   return dynamic_cast<C_NagViewItem *>(this->indexWidget(this->mc_Model.index(os32_Index)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle service mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::HandleServiceMode(void)
{
   this->Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Accept drag

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QListView::dragEnterEvent(opc_Event);
   if (pc_MimeData != NULL)
   {
      if (pc_MimeData->hasFormat(this->mc_Model.mimeTypes().at(0)) == true)
      {
         opc_Event->acceptProposedAction();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag move event slot

   Here: Accept drag

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QListView::dragMoveEvent(opc_Event);
   if (pc_MimeData != NULL)
   {
      if (pc_MimeData->hasFormat(this->mc_Model.mimeTypes().at(0)) == true)
      {
         opc_Event->acceptProposedAction();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Trigger view move

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::dropEvent(QDropEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QListView::dropEvent(opc_Event);
   if (pc_MimeData != NULL)
   {
      const QString c_MimeType = this->mc_Model.mimeTypes().at(0);
      if (pc_MimeData->hasFormat(c_MimeType) == true)
      {
         bool q_Ok;
         const QString c_Content(pc_MimeData->data(c_MimeType));
         const int32_t s32_Source = c_Content.toLong(&q_Ok);
         if (q_Ok == true)
         {
            const QListView::DropIndicatorPosition e_DropIndicator = this->dropIndicatorPosition();
            int32_t s32_TargetRow = this->indexAt(opc_Event->pos()).row();
            //If drag below inserted adapt target row
            if (s32_Source < s32_TargetRow)
            {
               --s32_TargetRow;
            }

            //If indicator below item insert at next index
            if (e_DropIndicator == QAbstractItemView::BelowItem)
            {
               ++s32_TargetRow;
            }

            if (e_DropIndicator == QAbstractItemView::OnViewport)
            {
               // special case: drop behind the last item in the free space
               s32_TargetRow = this->mc_Model.rowCount();
            }
            if ((s32_Source >= 0) && (s32_TargetRow >= 0))
            {
               Q_EMIT this->SigMoveView(static_cast<uint32_t>(s32_Source), static_cast<uint32_t>(s32_TargetRow));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event

   Here: send custom drag event

   \param[in]  oc_SupportedActions  Supported actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::startDrag(const Qt::DropActions oc_SupportedActions)
{
   const QModelIndexList c_SelectedIndices = this->selectedIndexes();

   Q_UNUSED(oc_SupportedActions)
   if (c_SelectedIndices.size() > 0)
   {
      this->m_StartDrag(c_SelectedIndices.at(0));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up UI after drag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_AfterDrag(void) const
{
   for (int32_t s32_ItView = 0UL; s32_ItView < this->mc_Model.rowCount(); ++s32_ItView)
   {
      C_NagViewItem * const pc_View = this->GetItemAt(s32_ItView);
      if (pc_View != NULL)
      {
         pc_View->SetDragged(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start drag action

   \param[in]  orc_Index   Selected index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_StartDrag(const QModelIndex & orc_Index)
{
   C_NagViewItem * const pc_Item = dynamic_cast<C_NagViewItem *>(this->indexWidget(orc_Index));

   if (pc_Item != NULL)
   {
      QModelIndexList c_IndexList;

      //start manual drag
      QDrag * const pc_Drag = new QDrag(this);
      QMimeData * pc_MimeData;
      const QRect c_Rect = QRect(QPoint(0, 0), pc_Item->sizeHint());
      QPixmap c_Screenshot(c_Rect.size() + QSize(3 + 3, 3 + 1));

      c_Screenshot.fill(mc_STYLE_GUIDE_COLOR_25);

      //Apply new state BEFORE rendering
      pc_Item->SetDragged(true);

      // create the screenshot
      pc_Item->render(&c_Screenshot, QPoint(3, 1), static_cast<QRegion>(c_Rect));

      //Overwrite first two rows
      {
         QImage c_Tmp = c_Screenshot.toImage();

         for (uint32_t u32_Row = 0; u32_Row < 3; ++u32_Row)
         {
            for (int32_t s32_Col = 0; s32_Col < c_Tmp.size().width(); ++s32_Col)
            {
               c_Tmp.setPixelColor(s32_Col, static_cast<int32_t>(u32_Row), mc_STYLE_GUIDE_COLOR_25.rgb());
            }
         }
         c_Screenshot.convertFromImage(c_Tmp);
      }

      pc_Drag->setPixmap(c_Screenshot);

      c_IndexList.push_back(orc_Index);
      pc_MimeData = this->mc_Model.mimeData(c_IndexList);
      pc_Drag->setMimeData(pc_MimeData);
      pc_Drag->exec();
      connect(pc_Drag, &QDrag::destroyed, this, &C_NagViewList::m_AfterDrag);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle start drag action

   \param[in]  opc_Sender  Sender
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_OnStartDrag(const C_NagViewItem * const opc_Sender)
{
   for (int32_t s32_It = 0; s32_It < this->mc_Model.rowCount(); ++s32_It)
   {
      if (this->indexWidget(this->mc_Model.index(s32_It)) == opc_Sender)
      {
         const QModelIndex c_Index = this->model()->index(s32_It, 0);
         this->m_StartDrag(c_Index);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle expand action

   \param[in]  opc_Sender  Sender
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_OnExpand(const C_NagViewItem * const opc_Sender)
{
   for (int32_t s32_It = 0; s32_It < this->mc_Model.rowCount(); ++s32_It)
   {
      if (this->indexWidget(this->mc_Model.index(s32_It)) == opc_Sender)
      {
         const QModelIndex c_Index = this->model()->index(s32_It, 0);
         this->mc_Delegate.RegisterExpand(c_Index);
         Q_EMIT this->SigSizeChange();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete action

   \param[in]  opc_Sender  Sender
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_OnDelete(const C_NagViewItem * const opc_Sender)
{
   for (int32_t s32_It = 0; s32_It < this->mc_Model.rowCount(); ++s32_It)
   {
      if (this->indexWidget(this->mc_Model.index(s32_It)) == opc_Sender)
      {
         Q_EMIT this->SigDelete(static_cast<uint32_t>(s32_It));
         Q_EMIT this->SigSizeChange();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle duplicate action

   \param[in]  opc_Sender  Sender
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_OnDuplicate(const C_NagViewItem * const opc_Sender)
{
   for (int32_t s32_It = 0; s32_It < this->mc_Model.rowCount(); ++s32_It)
   {
      if (this->indexWidget(this->mc_Model.index(s32_It)) == opc_Sender)
      {
         Q_EMIT this->SigDuplicate(static_cast<uint32_t>(s32_It));
         Q_EMIT this->SigSizeChange();
         // scroll to new item
         if (this->mc_Model.index(s32_It + 1).isValid() == true)
         {
            this->scrollTo(this->mc_Model.index(s32_It + 1));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle set name

   \param[in]  opc_Sender  Sender
   \param[in]  orc_Name    Name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_OnSetName(const C_NagViewItem * const opc_Sender, const QString & orc_Name)
{
   for (int32_t s32_It = 0; s32_It < this->mc_Model.rowCount(); ++s32_It)
   {
      if (this->indexWidget(this->mc_Model.index(s32_It)) == opc_Sender)
      {
         Q_EMIT this->SigSetName(static_cast<uint32_t>(s32_It), orc_Name);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle select

   \param[in]  opc_Sender           Sender
   \param[in]  os32_SubMode         Selected sub mode
   \param[in]  orc_Name             Name to display
   \param[in]  orc_SubSubItemName   Selected sub sub mode name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewList::m_OnSelect(const C_NagViewItem * const opc_Sender, const int32_t os32_SubMode,
                               const QString & orc_Name, const QString & orc_SubSubItemName)
{
   for (int32_t s32_It = 0; s32_It < this->mc_Model.rowCount(); ++s32_It)
   {
      if (this->indexWidget(this->mc_Model.index(s32_It)) == opc_Sender)
      {
         const uint32_t u32_ViewIndex = static_cast<uint32_t>(s32_It);
         Q_EMIT this->SigSelect(u32_ViewIndex, os32_SubMode, orc_Name, orc_SubSubItemName);
         Q_EMIT this->SigSizeChange();
      }
   }
}
