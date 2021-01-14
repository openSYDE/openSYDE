//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all connected node entries (implementation)

   List for all connected node entries

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDrag>
#include <QMimeData>
#include <QScrollBar>
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDcConnectedNodeList.h"
#include "C_SyvDcConnectedNodeWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDcConnectedNodeList::mhc_MimeData = "stw_opensyde_connected_node";
const QString C_SyvDcConnectedNodeList::mhc_MimeDataDevice = "stw_opensyde_connected_node_device";
const QString C_SyvDcConnectedNodeList::mhc_MimeDataDeviceValid = "stw_opensyde_connected_node_device_valid";

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
C_SyvDcConnectedNodeList::C_SyvDcConnectedNodeList(QWidget * const opc_Parent) :
   QListWidget(opc_Parent)
{
   //UI Settings
   this->setEditTriggers(QAbstractItemView::NoEditTriggers);
   this->setDragEnabled(true);
   this->setDragDropMode(QAbstractItemView::DragOnly);
   this->setDefaultDropAction(Qt::TargetMoveAction);
   this->setAlternatingRowColors(true);
   this->setSelectionMode(QAbstractItemView::SingleSelection);
   this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   this->setAcceptDrops(false);
   this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Connects
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SyvDcConnectedNodeList::m_ScrollBarRangeChangedVer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data

   \param[in] orc_Infos Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::SetData(const std::vector<C_SyvDcDeviceInformation> & orc_Infos)
{
   this->mc_Data = orc_Infos;
   m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable serial number

   \param[in] orc_SerialNumber Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::EnableSerialNumber(const QString & orc_SerialNumber) const
{
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {

      C_SyvDcConnectedNodeWidget * const pc_Widget =
         dynamic_cast<C_SyvDcConnectedNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if ((pc_Widget != NULL) && (pc_Widget->CompareSerialNumber(orc_SerialNumber) == true))
      {
         pc_Widget->setEnabled(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable serial number

   \param[in] orc_SerialNumber Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::DisableSerialNumber(const QString & orc_SerialNumber) const
{
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {

      C_SyvDcConnectedNodeWidget * const pc_Widget =
         dynamic_cast<C_SyvDcConnectedNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if ((pc_Widget != NULL) && (pc_Widget->CompareSerialNumber(orc_SerialNumber) == true))
      {
         pc_Widget->setEnabled(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event slot

   Here: Start manual drag

   \param[in,out]  oc_Actions  Drop actions type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::startDrag(const Qt::DropActions oc_Actions)
{
   QListWidgetItem * const pc_Item = this->currentItem();

   C_SyvDcConnectedNodeWidget * const pc_Widget =
      dynamic_cast<C_SyvDcConnectedNodeWidget * const>(this->itemWidget(pc_Item));

   if ((pc_Widget != NULL) && (pc_Widget->isEnabled() == true))
   {
      const QRect c_Rect = pc_Widget->rect();
      QList<QListWidgetItem *> c_Items;
      QPixmap c_Screenshot(c_Rect.size());
      //Manual drag
      QDrag * const pc_Drag = new QDrag(this);

      Q_EMIT (this->SigStartDrag(pc_Widget->GetDeviceName(), pc_Widget->GetDeviceNameValid()));

      // create the screenshot
      pc_Widget->render(&c_Screenshot, QPoint(), QRegion(c_Rect));

      pc_Drag->setPixmap(c_Screenshot);

      c_Items.push_back(pc_Item);
      pc_Drag->setMimeData(this->mimeData(c_Items));
      pc_Drag->exec(oc_Actions, this->defaultDropAction());

      // pc_Drag->exec will start its own event loop. It will return when the mouse button is released.
      // No Qt event or signal will detect this on drop exit on one common position
      // That is the only solution to handle a drag exit always.
      Q_EMIT (this->SigStopDrag());

   }  //lint !e429  //no memory leak because of the parent of pc_Drag and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mime data request event slot

   Here: Add additional information

   \param[in] oc_Items Affected items

   \return
   Mime data for drag event
*/
//----------------------------------------------------------------------------------------------------------------------
QMimeData * C_SyvDcConnectedNodeList::mimeData(const QList<QListWidgetItem *> oc_Items) const
{
   QMimeData * const pc_Retval = QListWidget::mimeData(oc_Items);

   if (pc_Retval != NULL)
   {
      if (oc_Items.size() > 0)
      {

         C_SyvDcConnectedNodeWidget * const pc_Widget =
            dynamic_cast<C_SyvDcConnectedNodeWidget * const>(this->itemWidget(oc_Items[0]));
         if (pc_Widget != NULL)
         {
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MimeData,
                               pc_Widget->GetSerialNumberString().toStdString().c_str());
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MimeDataDevice,
                               pc_Widget->GetDeviceName().toStdString().c_str());
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MimeDataDeviceValid,
                               QString::number(pc_Widget->GetDeviceNameValid()).toStdString().c_str());
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init step
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::m_Init(void)
{
   //Init/Reinit UI
   this->clear();
   //No point if pc not connected
   for (uint32 u32_ItData = 0; u32_ItData < this->mc_Data.size(); ++u32_ItData)
   {
      m_AppendNode(this->mc_Data[u32_ItData]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append node to list

   \param[in] ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::m_AppendNode(const C_SyvDcDeviceInformation & orc_Info)
{
   C_SyvDcConnectedNodeWidget * pc_Widget;

   this->addItem("");

   pc_Widget = new C_SyvDcConnectedNodeWidget(this->item(this->count() - 1), orc_Info, this);

   this->setItemWidget(this->item(this->count() - 1), pc_Widget);

   //Style first
   C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "First", this->count() == 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::m_ScrollBarRangeChangedVer(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}
