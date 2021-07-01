//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for the toolbox (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDrag>
#include <QEvent>
#include <QMimeData>
#include <QHelpEvent>
#include <QFileInfo>

#include "stwtypes.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_SdTopologyListWidget.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const sintn msn_GRID_HEIGHT = 70;
static const sintn msn_GRID_WIDTH = 90;
static const sintn msn_ICON_HEIGHT = 48;
static const sintn msn_ICON_WIDTH = 48;
const QString C_SdTopologyListWidget::hc_GroupName = "application/stw/opensyde/system/element";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyListWidget::C_SdTopologyListWidget(QWidget * const opc_Parent) :
   C_OgeListWidgetToolTipBase(opc_Parent),
   mq_AdaptMaximumHeight(true),
   mpc_ContextMenu(new C_OgeContextMenu(this)),
   mc_Position(0, 0)
{
   this->setSelectionMode(QAbstractItemView::SingleSelection);
   this->setViewMode(QListView::IconMode);
   this->setFlow(QListView::LeftToRight);
   this->setResizeMode(QListView::Adjust);
   this->setGridSize(QSize(msn_GRID_WIDTH, msn_GRID_HEIGHT));
   this->setIconSize(QSize(msn_ICON_WIDTH, msn_ICON_HEIGHT));
   this->setMovement(QListView::Static);
   this->setDragEnabled(true);
   this->setDropIndicatorShown(false);

   this->setFrameShape(QFrame::NoFrame);
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   this->setAutoScroll(false);
   this->mq_MinimumSize = true;

   //set no focus policy to hide focus rectangle of an selected item
   this->setFocusPolicy(Qt::NoFocus);

   this->mpc_DeleteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete                     "));
   connect(this->mpc_DeleteAction, &QAction::triggered, this, &C_SdTopologyListWidget::m_DeleteTriggered);

   // connect left mouse click to open context menu
   connect(this, &C_SdTopologyListWidget::customContextMenuRequested, this,
           &C_SdTopologyListWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent all elements and the Qt memory management
C_SdTopologyListWidget::~C_SdTopologyListWidget()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates the adaption of the maximum height if the size changes

   \param[in]     oq_Active      Flag if adaption is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::SetMaximumHeightAdaption(const bool oq_Active)
{
   this->mq_AdaptMaximumHeight = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the dark mode

   \param[in] oq_Active Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::ApplyDarkMode(const bool oq_Active) const
{
   for (sintn sn_ItItem = 0; sn_ItItem < this->count(); ++sn_ItItem)
   {
      QListWidgetItem * const pc_Item = this->item(sn_ItItem);
      if (pc_Item != NULL)
      {
         QPixmap c_Pixmap;
         if (oq_Active == false)
         {
            c_Pixmap = pc_Item->data(msn_USER_ROLE_PIXMAP_BRIGHT_MODE).value<QPixmap>();
         }
         else
         {
            c_Pixmap = pc_Item->data(msn_USER_ROLE_PIXMAP_DARK_MODE).value<QPixmap>();
         }
         if (c_Pixmap.isNull() == false)
         {
            QIcon c_Icon;
            c_Icon.addPixmap(c_Pixmap, QIcon::Normal);
            c_Icon.addPixmap(c_Pixmap, QIcon::Selected);
            pc_Item->setIcon(c_Icon);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set group name

   \param[in] orc_Name Group name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::SetGroupName(const QString & orc_Name)
{
   this->mc_Name = orc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Automatic adapting the minimum height
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::UpdateSize(void)
{
   // Items on each row
   sintn sn_ItemsRow = this->width() / msn_GRID_WIDTH;
   sintn sn_RowCount;

   if (this->mq_MinimumSize == true)
   {
      if (sn_ItemsRow < 1)
      {
         sn_ItemsRow = 1;
      }

      // calculate the necessary rows
      sn_RowCount = this->count() / sn_ItemsRow;

      // correct rounding error
      if ((this->count() % sn_ItemsRow) > 0)
      {
         ++sn_RowCount;
      }

      this->setMinimumHeight(sn_RowCount * msn_GRID_HEIGHT);
      // in some cases it is not necessary to adapt the maximum height
      if (this->mq_AdaptMaximumHeight == true)
      {
         this->setMaximumHeight(sn_RowCount * msn_GRID_HEIGHT);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getter function for mq_MinimumSize

   \retval   mq_MinimumSize   True or false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyListWidget::GetMinimumSize() const
{
   return this->mq_MinimumSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Setter function for mq_MinimumSize

   \param[in,out]   oq_MinimumSize   True or false
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::SetMinimumSize(const bool oq_MinimumSize)
{
   this->mq_MinimumSize = oq_MinimumSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event

   \param[in,out] oc_SupportedActions Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::startDrag(const Qt::DropActions oc_SupportedActions)
{
   QList<QListWidgetItem *> c_SelectedItems = this->selectedItems();

   if (c_SelectedItems.size() > 0)
   {
      QListWidgetItem * const pc_Item = c_SelectedItems.at(0);
      if (pc_Item != NULL)
      {
         QPixmap c_Pix;
         QDrag * const pc_Drag = new QDrag(this);
         QMimeData * const pc_MimeData = new QMimeData();

         pc_MimeData->setText(pc_Item->data(msn_USER_ROLE_ADDITIONAL_INFORMATION).toString());
         pc_MimeData->setData(C_SdTopologyListWidget::hc_GroupName, this->mc_Name.toStdString().c_str());
         c_Pix = c_SelectedItems[0]->icon().pixmap(msn_ICON_WIDTH, msn_ICON_HEIGHT);
         pc_Drag->setPixmap(c_Pix);
         pc_Drag->setHotSpot(c_Pix.rect().center());
         pc_Drag->setMimeData(pc_MimeData);

         pc_Drag->exec(oc_SupportedActions);
         //Clean up left over selection
         this->clearSelection();
      } //lint !e429  //no memory leak because of the parent and the Qt memory management
        //              and QDrag does take ownership on setMimeData
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Automatic adapting the minimum height.

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   this->UpdateSize();
   QListWidget::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize custom context menu functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   this->mc_Position = orc_Pos;

   QListWidgetItem * const pc_Item = this->itemAt(orc_Pos);

   // add action shall be shown only if no item concrete was clicked
   if (pc_Item != NULL)
   {
      this->mpc_DeleteAction->setVisible(true);
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
   else
   {
      this->mpc_DeleteAction->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of delete button triggered
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyListWidget::m_DeleteTriggered(void)
{
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Delete User Nodes"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                  "Do you really want to delete this user node?"));
   c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
   c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
   c_MessageBox.SetCustomMinHeight(180, 180);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      Q_EMIT SigDelete(this->mc_Position);
   }
}
