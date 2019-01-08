//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       List widget for the toolbox (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QDrag>
#include <QEvent>
#include <QMimeData>
#include <QHelpEvent>

#include "stwtypes.h"
#include "constants.h"
#include "C_SdTopologyListWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
static const sintn msn_GRID_HEIGHT = 70;
static const sintn msn_GRID_WIDTH = 90;
static const sintn msn_ICON_HEIGHT = 48;
static const sintn msn_ICON_WIDTH = 48;
const QString C_SdTopologyListWidget::hc_GroupName = "application/stw/opensyde/system/element";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent        Optional pointer to parent

   \created     10.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdTopologyListWidget::C_SdTopologyListWidget(QWidget * const opc_Parent) :
   C_OgeListWidgetToolTipBase(opc_Parent),
   mq_AdaptMaximumHeight(true)
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

   if (this->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
   {
      this->setAutoScroll(false);
      this->mq_MinimumSize = true;
   }
   else
   {
      this->setAutoScroll(true);
      this->mq_MinimumSize = false;
   }

   //set no focus policy to hide focus rectangle of an selected item
   this->setFocusPolicy(Qt::NoFocus);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     10.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdTopologyListWidget::~C_SdTopologyListWidget()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates or deactivates the adaption of the maximum height if the size changes

   \param[in]     oq_Active      Flag if adaption is active

   \created     17.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdTopologyListWidget::SetMaximumHeightAdaption(const bool oq_Active)
{
   this->mq_AdaptMaximumHeight = oq_Active;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the dark mode

   \param[in] oq_Active Dark mode active

   \created     04.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set group name

   \param[in] orc_Name Group name

   \created     07.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdTopologyListWidget::SetGroupName(const QString & orc_Name)
{
   this->mc_Name = orc_Name;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten start drag event

   \param[in,out] opc_Event Event identification and information

   \created     10.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdTopologyListWidget::startDrag(const Qt::DropActions oc_SupportedActions)
{
   QList<QListWidgetItem *> c_SelectedItems = this->selectedItems();

   if (c_SelectedItems.size() > 0)
   {
      QListWidgetItem * const pc_Item = c_SelectedItems.at(0);
      if (pc_Item != NULL)
      {
         QPixmap c_Pix;
         QDrag * pc_Drag = new QDrag(this);
         QMimeData * pc_MimeData = new QMimeData();

         pc_MimeData->setText(pc_Item->data(msn_USER_ROLE_ADDITIONAL_INFORMATION).toString());
         pc_MimeData->setData(C_SdTopologyListWidget::hc_GroupName, this->mc_Name.toStdString().c_str());
         c_Pix = c_SelectedItems[0]->icon().pixmap(msn_ICON_WIDTH, msn_ICON_HEIGHT);
         pc_Drag->setPixmap(c_Pix);
         pc_Drag->setHotSpot(c_Pix.rect().center());
         pc_Drag->setMimeData(pc_MimeData);

         pc_Drag->exec(oc_SupportedActions);
         //Clean up left over selection
         this->clearSelection();
         //lint -e{429}  no memory leak because of the parent and the Qt memory management
         //              and QDrag does take ownership on setMimeData
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event

   Automatic adapting the minimum height.

   \param[in,out] opc_Event Event identification and information

   \created     12.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdTopologyListWidget::resizeEvent(QResizeEvent * const opc_Event)
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

   QListWidget::resizeEvent(opc_Event);
}
