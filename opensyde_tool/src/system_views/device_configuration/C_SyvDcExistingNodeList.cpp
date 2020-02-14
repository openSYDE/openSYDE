//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all existing node entries (implementation)

   List for all existing node entries

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>

#include "stwerrors.h"

#include "TGLUtils.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDcExistingNodeWidget.h"
#include "C_SyvDcExistingNodeList.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
C_SyvDcExistingNodeList::C_SyvDcExistingNodeList(QWidget * const opc_Parent) :
   QListWidget(opc_Parent),
   mu32_ViewIndex(0),
   mq_ShowAssignment(false)
{
   //UI Settings
   this->setEditTriggers(QAbstractItemView::NoEditTriggers);
   this->setDragEnabled(false);
   this->setDragDropMode(QAbstractItemView::NoDragDrop);
   this->setDefaultDropAction(Qt::TargetMoveAction);
   this->setAlternatingRowColors(false);
   this->setSelectionMode(QAbstractItemView::NoSelection);
   this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   this->setAcceptDrops(false);
   this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

   // configure the scroll bar to stop resizing the widget when showing or hiding the scroll bar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Connects
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SyvDcExistingNodeList::m_ScrollBarRangeChangedVer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view index

   \param[in] ou32_Index         New view index
   \param[in] oq_ShowAssignment  Show assignment flag

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcExistingNodeList::SetView(const uint32 ou32_Index, const bool oq_ShowAssignment)
{
   this->mu32_ViewIndex = ou32_Index;
   this->mq_ShowAssignment = oq_ShowAssignment;

   return m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do assignment for specified node and serial number

   \param[in] ou32_NodeIndex   Node index
   \param[in] orc_SerialNumber Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::ConnectSerialNumber(const uint32 ou32_NodeIndex, const QString & orc_SerialNumber) const
{
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if ((pc_Widget != NULL) && (pc_Widget->CompareIndex(ou32_NodeIndex) == true))
      {
         pc_Widget->ConnectSerialNumber(orc_SerialNumber);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect assignment for specified node and serial number

   \param[in] ou32_NodeIndex   Node index
   \param[in] orc_SerialNumber Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::DisconnectSerialNumber(const uint32 ou32_NodeIndex,
                                                     const QString & orc_SerialNumber) const
{
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if ((pc_Widget != NULL) && (pc_Widget->CompareIndex(ou32_NodeIndex) == true))
      {
         pc_Widget->DisconnectSerialNumber(orc_SerialNumber);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of nodes which have an assigned serial number

   \return
   Number of nodes which have an assigned serial number
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvDcExistingNodeList::GetAssignmentCount(void) const
{
   uint32 u32_Retval = 0;

   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if ((pc_Widget != NULL) && (pc_Widget->IsAssigned() == true))
      {
         ++u32_Retval;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all configs

   \return
   Current all configs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_SyvDcDeviceConfiguation> C_SyvDcExistingNodeList::GetConfigs(void) const
{
   std::vector<C_SyvDcDeviceConfiguation> c_Retval;
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if (pc_Widget != NULL)
      {
         tgl_assert(pc_Widget->IsAssigned() == true);
         pc_Widget->AppendDeviceConfig(c_Retval);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the widget for starting drag and drop of connected nodes

   \param[in]     orc_DeviceName         Device name (device type)
   \param[in]     oq_DeviceNameValid     Flag if device name is valid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const
{
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if (pc_Widget != NULL)
      {
         pc_Widget->StartDrag(orc_DeviceName, oq_DeviceNameValid);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handles the stop of drag and drop of connected nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::StopDrag(void) const
{
   for (sintn sn_It = 0; sn_It < this->count(); ++sn_It)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(sn_It)));
      if (pc_Widget != NULL)
      {
         pc_Widget->StopDrag();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init step

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcExistingNodeList::m_Init(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   sint32 s32_Return = C_NO_ERR;

   //Init/Reinit UI
   this->clear();
   //No point if PC not connected
   if ((pc_View != NULL) && (pc_View->GetPcData().GetConnected() == true))
   {
      sint32 s32_VisibleNodeIndex = 1;
      const std::vector<uint8> & rc_NodeActiveFlags = pc_View->GetNodeActiveFlags();
      for (uint32 u32_ItNode = 0; u32_ItNode < rc_NodeActiveFlags.size(); ++u32_ItNode)
      {
         //Active
         if (rc_NodeActiveFlags[u32_ItNode] == true)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItNode);
            if (pc_Node != NULL)
            {
               bool q_Connected = false;
               tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
               for (uint32 u32_ItInterface = 0; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size();
                    ++u32_ItInterface)
               {
                  const C_OSCNodeComInterfaceSettings & rc_Interface =
                     pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
                  //Connected to current bus
                  if (((rc_Interface.q_IsBusConnected == true) &&
                       (rc_Interface.u32_BusIndex == pc_View->GetPcData().GetBusIndex())) &&
                      (pc_Node->pc_DeviceDefinition->IsUpdateAvailable(rc_Interface.e_InterfaceType) == true))
                  {
                     q_Connected = true;
                  }
               }
               //Check flashloader available
               if (q_Connected == true)
               {
                  //All checks passed at node
                  const sint32 s32_NodeReturn = m_AppendNode(u32_ItNode, s32_VisibleNodeIndex);

                  // Save the error case
                  if (s32_Return == C_NO_ERR)
                  {
                     s32_Return = s32_NodeReturn;
                  }
               }
            }
            //Increment visible node index
            ++s32_VisibleNodeIndex;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append node to list

   \param[in] ou32_NodeIndex Node index

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcExistingNodeList::m_AppendNode(const uint32 ou32_NodeIndex, const sint32 os32_VisibleNodeIndex)
{
   C_SyvDcExistingNodeWidget * const pc_Widget = new C_SyvDcExistingNodeWidget(this);
   sint32 s32_Return;

   this->addItem("");

   //First step: add widget
   this->setItemWidget(this->item(this->count() - 1), pc_Widget);

   //Second step: configure widget
   s32_Return = pc_Widget->SetIndex(this->mu32_ViewIndex, ou32_NodeIndex, os32_VisibleNodeIndex, this->item(
                                       this->count() - 1), this->mq_ShowAssignment);

   //Style first
   C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "First", this->count() == 1);

   //Connects
   connect(pc_Widget, &C_SyvDcExistingNodeWidget::SigConnect, this, &C_SyvDcExistingNodeList::SigConnect);
   connect(pc_Widget, &C_SyvDcExistingNodeWidget::SigDisconnect, this, &C_SyvDcExistingNodeList::SigDisconnect);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::m_ScrollBarRangeChangedVer(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scroll bar to stop resizing the parent widget when showing or hiding the scroll
   // bar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}
