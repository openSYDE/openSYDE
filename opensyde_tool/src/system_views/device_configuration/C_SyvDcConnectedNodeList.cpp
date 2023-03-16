//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all connected node entries (implementation)

   List for all connected node entries

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDrag>
#include <QMimeData>
#include <QScrollBar>
#include "constants.hpp"
#include "TglUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDcConnectedNodeList.hpp"
#include "C_SyvDcConnectedNodeWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDcConnectedNodeList::mhc_MIME_DATA = "stw_opensyde_connected_node";
const QString C_SyvDcConnectedNodeList::mhc_MIME_DATA_EXT_FORMAT = "stw_opensyde_connected_node_ext_format";
const QString C_SyvDcConnectedNodeList::mhc_MIME_DATA_MANUFACTURER_FORMAT = "stw_opensyde_connected_node_manu_format";
const QString C_SyvDcConnectedNodeList::mhc_MIME_DATA_DEVICE = "stw_opensyde_connected_node_device";
const QString C_SyvDcConnectedNodeList::mhc_MIME_DATA_DEVICE_VALID = "stw_opensyde_connected_node_device_valid";
const QString C_SyvDcConnectedNodeList::mhc_MIME_DATA_SUB_NODE_IDS_TO_OLD_NODE_IDS =
   "stw_opensyde_connected_node_subnodeids_to_nodeids";

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
   QListWidget(opc_Parent),
   mq_GridSizeSet(false)
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

   \param[in] orc_SerialNumber       Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::EnableSerialNumber(const C_OscProtocolSerialNumber & orc_SerialNumber) const
{
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      C_SyvDcConnectedNodeWidget * const pc_Widget =
         dynamic_cast<C_SyvDcConnectedNodeWidget * const>(this->itemWidget(this->item(s32_It)));
      if ((pc_Widget != NULL) &&
          (pc_Widget->CompareSerialNumber(orc_SerialNumber) == true))
      {
         pc_Widget->setEnabled(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable serial number

   \param[in] orc_SerialNumber       Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::DisableSerialNumber(
   const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber)
const
{
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      C_SyvDcConnectedNodeWidget * const pc_Widget =
         dynamic_cast<C_SyvDcConnectedNodeWidget * const>(this->itemWidget(this->item(s32_It)));
      if ((pc_Widget != NULL) &&
          (pc_Widget->CompareSerialNumber(orc_SerialNumber) == true))
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
      QRegion c_Region;

      Q_EMIT (this->SigStartDrag(pc_Widget->GetDeviceName(), pc_Widget->GetDeviceNameValid()));

      // create the screenshot
      c_Region.setRects(&c_Rect, 1);
      pc_Widget->render(&c_Screenshot, QPoint(), c_Region);

      pc_Drag->setPixmap(c_Screenshot);

      c_Items.push_back(pc_Item);
      pc_Drag->setMimeData(this->mimeData(c_Items));
      pc_Drag->exec(oc_Actions, this->defaultDropAction());

      // pc_Drag->exec will start its own event loop. It will return when the mouse button is released.
      // No Qt event or signal will detect this on drop exit on one common position
      // That is the only solution to handle a drag exit always.
      Q_EMIT (this->SigStopDrag());
   } //lint !e429  //no memory leak because of the parent of pc_Drag and the Qt memory management
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
            QString c_StringSubNodeIdsToOldNodeIds;
            const std::map<uint8_t, C_SyvDcDeviceOldComConfig> c_SubNodeIdsToOldNodeIds =
               pc_Widget->GetSubNodeIdsToOldNodeIds();
            std::map<uint8_t, C_SyvDcDeviceOldComConfig>::const_iterator c_ItIds;

            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MIME_DATA,
                               pc_Widget->GetPlainSerialNumberString().toStdString().c_str());
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MIME_DATA_EXT_FORMAT,
                               QString::number(static_cast<int32_t>(pc_Widget->GetExtFormat())).
                               toStdString().c_str());
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MIME_DATA_MANUFACTURER_FORMAT,
                               QString::number(static_cast<int32_t>(pc_Widget->GetManufacturerFormat())).
                               toStdString().c_str());
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MIME_DATA_DEVICE,
                               pc_Widget->GetDeviceName().toStdString().c_str());
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MIME_DATA_DEVICE_VALID,
                               QString::number(static_cast<int32_t>(pc_Widget->GetDeviceNameValid())).
                               toStdString().c_str());

            // Build a string to set the sub node id to node id mapping
            for (c_ItIds = c_SubNodeIdsToOldNodeIds.begin(); c_ItIds != c_SubNodeIdsToOldNodeIds.end(); ++c_ItIds)
            {
               // Format: sub node id,node id, ip valid flag(true), ip0:ip1:ip2:ip3;
               // sub node id, node id, ip valid flag(false);
               c_StringSubNodeIdsToOldNodeIds +=
                  QString::number(c_ItIds->first) + "," +
                  QString::number(c_ItIds->second.u8_OldNodeId) + "," +
                  QString::number(static_cast<uint8_t>(c_ItIds->second.q_OldIpAddressValid));

               if (c_ItIds->second.q_OldIpAddressValid == true)
               {
                  c_StringSubNodeIdsToOldNodeIds += ",";
                  c_StringSubNodeIdsToOldNodeIds += QString::number(c_ItIds->second.au8_OldIpAddress[0]) + ":" +
                                                    QString::number(c_ItIds->second.au8_OldIpAddress[1]) + ":" +
                                                    QString::number(c_ItIds->second.au8_OldIpAddress[2]) + ":" +
                                                    QString::number(c_ItIds->second.au8_OldIpAddress[3]);
               }
               c_StringSubNodeIdsToOldNodeIds += ";";
            }
            pc_Retval->setData(C_SyvDcConnectedNodeList::mhc_MIME_DATA_SUB_NODE_IDS_TO_OLD_NODE_IDS,
                               c_StringSubNodeIdsToOldNodeIds.toStdString().c_str());
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
   uint32_t u32_ItData;

   std::vector<C_SyvDcDeviceInformation> c_DataUnique;
   std::vector<std::map<uint8_t, C_SyvDcDeviceOldComConfig> > c_DataUniqueSubNodeIdsToOldNodeIds;

   //Init/Reinit UI
   this->clear();

   c_DataUnique.reserve(this->mc_Data.size());
   c_DataUniqueSubNodeIdsToOldNodeIds.reserve(this->mc_Data.size());

   // Check for Multi-CPU nodes
   for (u32_ItData = 0U; u32_ItData < this->mc_Data.size(); ++u32_ItData)
   {
      const C_SyvDcDeviceInformation & rc_Data = this->mc_Data[u32_ItData];
      uint32_t u32_ItDataUnique;
      bool q_MatchingSubNodeFound = false;

      // Check if already existing
      for (u32_ItDataUnique = 0U; u32_ItDataUnique < c_DataUnique.size(); ++u32_ItDataUnique)
      {
         const C_SyvDcDeviceInformation & rc_DataUnique = c_DataUnique[u32_ItDataUnique];

         if (rc_DataUnique.IsSerialNumberIdentical(rc_Data) == true)
         {
            // Serial number is identical, add the sub node id
            C_SyvDcDeviceOldComConfig c_OldComConfig;
            c_OldComConfig.SetContent(rc_Data.u8_NodeId, rc_Data.q_IpAddressValid, &rc_Data.au8_IpAddress[0]);

            c_DataUniqueSubNodeIdsToOldNodeIds[u32_ItDataUnique][rc_Data.u8_SubNodeId] = c_OldComConfig;
            q_MatchingSubNodeFound = true;
            break;
         }
      }

      if (q_MatchingSubNodeFound == false)
      {
         std::map<uint8_t, C_SyvDcDeviceOldComConfig> c_SubNodeIdToNodeId;
         C_SyvDcDeviceOldComConfig c_OldComConfig;
         c_OldComConfig.SetContent(rc_Data.u8_NodeId, rc_Data.q_IpAddressValid, &rc_Data.au8_IpAddress[0]);

         c_SubNodeIdToNodeId[rc_Data.u8_SubNodeId] = c_OldComConfig;

         // New serial number
         c_DataUnique.push_back(rc_Data);
         c_DataUniqueSubNodeIdsToOldNodeIds.push_back(c_SubNodeIdToNodeId);
      }
   }

   tgl_assert(c_DataUnique.size() == c_DataUniqueSubNodeIdsToOldNodeIds.size());

   //No point if pc not connected
   for (u32_ItData = 0U; u32_ItData < c_DataUnique.size(); ++u32_ItData)
   {
      m_AppendNode(c_DataUnique[u32_ItData], c_DataUniqueSubNodeIdsToOldNodeIds[u32_ItData]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append node to list

   \param[in] orc_Info                    Read device information
   \param[in] orc_SubNodeIdsToOldNodeIds  Detected sub node ids and the associated used node ids with same serial number
                                          - In case of a normal node, exact one sub node id which should be 0
                                          - In case of a multiple CPU, at least two sub node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::m_AppendNode(const C_SyvDcDeviceInformation & orc_Info, const std::map<uint8_t,
                                                                                                      C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
{
   C_SyvDcConnectedNodeWidget * pc_Widget;

   this->addItem("");

   pc_Widget = new C_SyvDcConnectedNodeWidget(this->item(this->count() - 1), orc_Info,
                                              orc_SubNodeIdsToOldNodeIds, this);

   this->setItemWidget(this->item(this->count() - 1), pc_Widget);

   //Style first
   C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "First", this->count() == 1);

   if (this->mq_GridSizeSet == false)
   {
      // Set the grid size for one time. The size hint seems not to be enough since Qt 5.15.2
      this->mq_GridSizeSet = true;
      this->setGridSize(QSize(pc_Widget->width(), pc_Widget->height()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeList::m_ScrollBarRangeChangedVer(const int32_t os32_Min, const int32_t os32_Max) const
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
