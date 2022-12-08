//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all existing node entries (implementation)

   List for all existing node entries

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>

#include "stwerrors.hpp"

#include "TglUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDcExistingNodeWidget.hpp"
#include "C_SyvDcExistingNodeList.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
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
C_SyvDcExistingNodeList::C_SyvDcExistingNodeList(QWidget * const opc_Parent) :
   QListWidget(opc_Parent),
   mu32_ViewIndex(0),
   mq_ShowAssignment(false),
   mq_GridSizeSet(false),
   mu32_CommunicatingNodeCount(0U)
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

   \param[in]  ou32_Index           New view index
   \param[in]  oq_ShowAssignment    Show assignment flag

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDcExistingNodeList::SetView(const uint32_t ou32_Index, const bool oq_ShowAssignment)
{
   this->mu32_ViewIndex = ou32_Index;
   this->mq_ShowAssignment = oq_ShowAssignment;

   return m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do assignment for specified node and serial number

   \param[in] ou32_NodeIndex              Node index
   \param[in] orc_SerialNumber            Serial number
   \param[in] orc_SubNodeIdsToOldNodeIds  Detected sub node ids and the associated used node ids
                                          with same serial number
                                          - In case of a normal node, exact one sub node id which should be 0
                                          - In case of a multiple CPU, at least two sub node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::ConnectSerialNumber(const uint32_t ou32_NodeIndex,
                                                  const C_OscProtocolSerialNumber & orc_SerialNumber,
                                                  const std::map<uint8_t,
                                                                 C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
const
{
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(s32_It)));
      if ((pc_Widget != NULL) && (pc_Widget->CompareIndex(ou32_NodeIndex) == true))
      {
         pc_Widget->ConnectSerialNumber(orc_SerialNumber, orc_SubNodeIdsToOldNodeIds);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect assignment for specified node and serial number

   \param[in] ou32_NodeIndex         Node index
   \param[in] orc_SerialNumber       Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::DisconnectSerialNumber(const uint32_t ou32_NodeIndex,
                                                     const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber)
const
{
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(s32_It)));
      if ((pc_Widget != NULL) && (pc_Widget->CompareIndex(ou32_NodeIndex) == true))
      {
         pc_Widget->DisconnectSerialNumber(orc_SerialNumber);
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of communicating nodes

   Relevant nodes for communication (normal nodes and sub nodes of multi CPU nodes)

   \return
   Number of communicating nodes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDcExistingNodeList::GetCommunicatingNodeCount(void) const
{
   return this->mu32_CommunicatingNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of nodes which have an assigned serial number

   \return
   Number of nodes which have an assigned serial number
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDcExistingNodeList::GetAssignmentCount(void) const
{
   uint32_t u32_Retval = 0;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(s32_It)));
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
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(s32_It)));
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

   \param[in]  orc_DeviceName       Device name (device type)
   \param[in]  oq_DeviceNameValid   Flag if device name is valid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const
{
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(s32_It)));
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
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const C_SyvDcExistingNodeWidget * const pc_Widget =
         dynamic_cast<const C_SyvDcExistingNodeWidget * const>(this->itemWidget(this->item(s32_It)));
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
int32_t C_SyvDcExistingNodeList::m_Init(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   int32_t s32_Return = C_NO_ERR;

   //Init/Reinit UI
   this->clear();
   this->mu32_CommunicatingNodeCount = 0U;

   //No point if PC not connected
   if ((pc_View != NULL) && (pc_View->GetPcData().GetConnected() == true))
   {
      const std::vector<uint8_t> & rc_NodeActiveFlags = pc_View->GetNodeActiveFlags();
      for (uint32_t u32_ItNode = 0; u32_ItNode < rc_NodeActiveFlags.size(); ++u32_ItNode)
      {
         //Active
         if (rc_NodeActiveFlags[u32_ItNode] == true)
         {
            uint32_t u32_SquadIndex;
            const C_OscNodeSquad * pc_Squad = NULL;
            std::vector<uint32_t> c_RelevantNodeIndexes;
            std::set<uint32_t> c_FirstSubNodeConnectedInterfaces;
            uint32_t u32_SubNodeCounter;
            bool q_AllSubNodesAvailable = true;

            // Check for squad and get all node indexes which are part of the squad
            // The indexes of the squad must be in order, so skip all the nodes after the first one in the first loop
            if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(u32_ItNode, u32_SquadIndex) == C_NO_ERR)
            {
               pc_Squad = C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_SquadIndex);
               tgl_assert(pc_Squad != NULL);
               if (pc_Squad != NULL)
               {
                  c_RelevantNodeIndexes = pc_Squad->c_SubNodeIndexes;
               }
            }
            else
            {
               // No squad, a normal node
               c_RelevantNodeIndexes.push_back(u32_ItNode);
            }

            tgl_assert(c_RelevantNodeIndexes.size() > 0);

            for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < c_RelevantNodeIndexes.size(); ++u32_SubNodeCounter)
            {
               const uint32_t u32_CurNodeIndex = c_RelevantNodeIndexes[u32_SubNodeCounter];
               const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_CurNodeIndex);
               bool q_Connected = false;

               if (pc_Node != NULL)
               {
                  tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
                  tgl_assert(pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size());
                  for (uint32_t u32_ItInterface = 0; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size();
                       ++u32_ItInterface)
                  {
                     const C_OscNodeComInterfaceSettings & rc_Interface =
                        pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
                     //Connected to current bus
                     if (((rc_Interface.GetBusConnected() == true) &&
                          (rc_Interface.u32_BusIndex == pc_View->GetPcData().GetBusIndex())) &&
                         (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].IsUpdateAvailable(
                             rc_Interface.e_InterfaceType) == true))
                     {
                        if (u32_SubNodeCounter == 0U)
                        {
                           // First sub node: Save all its connected interfaces. Only interfaces with all connected
                           // sub nodes are supported
                           c_FirstSubNodeConnectedInterfaces.insert(u32_ItInterface);
                           q_Connected = true;
                        }
                        else
                        {
                           if (c_FirstSubNodeConnectedInterfaces.find(u32_ItInterface) !=
                               c_FirstSubNodeConnectedInterfaces.end())
                           {
                              // Interface is used by previous sub node(s) too
                              q_Connected = true;
                           }
                        }
                     }
                  }
               }

               if (q_Connected == false)
               {
                  // At least one sub node is not available, so it will be filtered out
                  q_AllSubNodesAvailable = false;
                  break;
               }
            }

            //Check flashloader of all sub nodes or the normal node available
            if (q_AllSubNodesAvailable == true)
            {
               //All checks passed at node
               const int32_t s32_NodeReturn = m_AppendNode(u32_ItNode, (pc_Squad != NULL));

               // Save the error case
               if (s32_Return == C_NO_ERR)
               {
                  s32_Return = s32_NodeReturn;
               }

               // Count all communicating sub nodes and normal nodes
               this->mu32_CommunicatingNodeCount += c_RelevantNodeIndexes.size();
            }

            // Skip the other sub nodes in case of a squad. Only the first node index of a squad will be used
            if (pc_Squad != NULL)
            {
               //lint -e{850} Index modified for skipping index step
               u32_ItNode += (static_cast<uint32_t>(c_RelevantNodeIndexes.size()) - 1U);
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append node to list

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  oq_PartOfSquad          Flag if node is part of a squad

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDcExistingNodeList::m_AppendNode(const uint32_t ou32_NodeIndex, const bool oq_PartOfSquad)
{
   C_SyvDcExistingNodeWidget * const pc_Widget = new C_SyvDcExistingNodeWidget(this);
   int32_t s32_Return;

   this->addItem("");

   //First step: add widget
   this->setItemWidget(this->item(this->count() - 1), pc_Widget);

   //Second step: configure widget
   s32_Return = pc_Widget->SetIndex(this->mu32_ViewIndex, ou32_NodeIndex, oq_PartOfSquad,
                                    this->item(this->count() - 1), this->mq_ShowAssignment);

   //Style first
   C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "First", this->count() == 1);

   if (this->mq_GridSizeSet == false)
   {
      // Set the grid size for one time. The size hint seems not to be enough since Qt 5.15.2
      this->mq_GridSizeSet = true;
      this->setGridSize(QSize(pc_Widget->width(), pc_Widget->height()));
   }

   //Connects
   connect(pc_Widget, &C_SyvDcExistingNodeWidget::SigConnect, this, &C_SyvDcExistingNodeList::SigConnect);
   connect(pc_Widget, &C_SyvDcExistingNodeWidget::SigDisconnect, this, &C_SyvDcExistingNodeList::SigDisconnect);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeList::m_ScrollBarRangeChangedVer(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scroll bar to stop resizing the parent widget when showing or hiding the scroll
   // bar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}
