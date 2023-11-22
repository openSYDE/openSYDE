//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for tree for navigation and configuration of CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QKeyEvent>
#include <QScrollBar>

#include "C_Uti.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CieUtil.hpp"
#include "C_PuiUtil.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscImportEdsDcf.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeCoConfigTreeView.hpp"
#include "C_CieImportReportWidget.hpp"
#include "C_CieDataPoolListAdapter.hpp"
#include "C_SdNdeCoAddDeviceDialog.hpp"
#include "C_OscCanInterfaceId.hpp"
#include "C_UsNode.hpp"
#include "C_UsHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor/destructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoConfigTreeDelegate::C_SdNdeCoConfigTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Here: Draw special background of channel ID and paint parent selected state.

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Option
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                        const QModelIndex & orc_Index) const
{
   // paint parent of current item in selected style
   QStyleOptionViewItem c_Option = orc_Option;

   if (orc_Index == mc_Index)
   {
      c_Option.state.setFlag(QStyle::State_Selected, true);
      c_Option.state.setFlag(QStyle::State_Enabled, true);
   }

   // draw original
   QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remember current index.

   \param[in]  orc_Index   Relevant model index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeDelegate::SetCurrentIndexParentRow(const QModelIndex & orc_Index)
{
   this->mc_Index = orc_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoConfigTreeView::C_SdNdeCoConfigTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mu32_NodeIndex(0U),
   mq_Initialized(false),
   mpc_ContextMenu(new C_OgeContextMenu),
   mpc_AddAction(NULL),
   mpc_RemoveAction(NULL)
{
   this->C_SdNdeCoConfigTreeView::setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   this->setDragEnabled(false);
   this->setHeaderHidden(true);
   this->setSelectionMode(QAbstractItemView::SingleSelection);
   this->m_SetupContextMenu();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigCommDatapoolsChanged, this,
           &C_SdNdeCoConfigTreeView::SigCommDatapoolsChanged);
   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigErrorChange, this,
           &C_SdNdeCoConfigTreeView::SigErrorChange);
   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigSelectManager, this,
           &C_SdNdeCoConfigTreeView::m_SelectManager);
   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigManagerUnchecked, this,
           &C_SdNdeCoConfigTreeView::m_HandleManagerUnchecked);
   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigDeviceRemoved, this,
           &C_SdNdeCoConfigTreeView::m_HandleDeviceRemoved);

   connect(this, &C_SdNdeCoConfigTreeView::expanded, this, &C_SdNdeCoConfigTreeView::m_OnExpanded);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of all elements and the Qt memory management
C_SdNdeCoConfigTreeView::~C_SdNdeCoConfigTreeView(void)
{
   delete this->mpc_ContextMenu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active node id

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SetNodeId(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mc_Model.SetNodeId(ou32_NodeIndex);
   this->mq_Initialized = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset delegate to update parent selection look.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::ResetDelegate(void)
{
   this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::LoadUserSettings(void)
{
   C_OscCanInterfaceId c_NodeId;

   const uint32_t u32_NodeIndex = this->mc_Model.GetNodeIndex();
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

   if (pc_Node != NULL)
   {
      uint8_t u8_InterfaceNumber;
      const C_UsNode c_UsNode =
         C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
      const std::map<uint8_t, C_OscCanOpenManagerInfo> c_CanOpenManagers = pc_Node->c_CanOpenManagers;

      for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager = c_CanOpenManagers.begin();
           c_ItManager != c_CanOpenManagers.end(); ++c_ItManager)
      {
         const QModelIndex c_InterfaceModelIndex = this->mc_Model.GetInterfaceModelIndex(c_ItManager->first);
         const QModelIndex c_DevicesModelIndex = this->mc_Model.GetDevicesModelIndex(c_ItManager->first);

         const C_OscCanOpenManagerInfo c_CanOpenManagerInfo = c_ItManager->second;
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo> & rc_Devices = c_CanOpenManagerInfo.c_CanOpenDevices;

         const std::map<uint8_t, bool> c_Interfaces = c_UsNode.GetExpandedCanOpenManager();
         for (std::map<uint8_t, bool>::const_iterator c_ItInterfaces = c_Interfaces.begin();
              c_ItInterfaces != c_Interfaces.end(); ++c_ItInterfaces)
         {
            if (c_ItInterfaces->first == c_ItManager->first)
            {
               if (c_ItInterfaces->second == true)
               {
                  for (std::map<C_OscCanInterfaceId,
                                C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevices = rc_Devices.begin();
                       c_ItDevices != rc_Devices.end(); ++c_ItDevices)
                  {
                     const std::map<uint8_t, bool> c_Devices = c_UsNode.GetExpandedCanOpenDevices();
                     for (std::map<uint8_t, bool>::const_iterator c_ItUsDevices = c_Devices.begin();
                          c_ItUsDevices != c_Devices.end(); ++c_ItUsDevices)
                     {
                        const QModelIndex c_DeviceModelIndex =
                           this->mc_Model.GetDeviceModelIndex(c_ItManager->first, c_ItDevices->first);

                        if (c_ItUsDevices->first == c_ItDevices->first.u8_InterfaceNumber)
                        {
                           if (c_ItUsDevices->second == true)
                           {
                              const C_OscNode * const pc_DeviceNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                                 c_ItDevices->first.u32_NodeIndex);

                              if (pc_DeviceNode != NULL)
                              {
                                 const std::map<std::pair<uint8_t, std::pair<uint8_t,
                                                                             stw::scl::C_SclString> >,
                                                bool> c_Device = c_UsNode.GetExpandedCanOpenDevice();
                                 for (std::map<std::pair<uint8_t,
                                                         std::pair<uint8_t, stw::scl::C_SclString> >,
                                               bool>::const_iterator c_ItDevice = c_Device.begin();
                                      c_ItDevice != c_Device.end();
                                      ++c_ItDevice)
                                 {
                                    if (pc_DeviceNode->c_Properties.c_Name == c_ItDevice->first.second.second)
                                    {
                                       this->setExpanded(c_DeviceModelIndex, c_ItDevice->second);
                                    }
                                 }
                              }
                              this->setExpanded(c_DevicesModelIndex, c_ItUsDevices->second);
                           }
                           else
                           {
                              this->setExpanded(c_DevicesModelIndex, false);
                           }
                        }
                        else
                        {
                           this->setExpanded(c_DevicesModelIndex, false);
                        }
                     }
                  }
                  this->setExpanded(c_InterfaceModelIndex, c_ItInterfaces->second);
               }
               else
               {
                  this->setExpanded(c_InterfaceModelIndex, false);
               }
            }
            else
            {
               this->setExpanded(c_InterfaceModelIndex, false);
            }
         }
      }

      // last known selected item
      u8_InterfaceNumber = c_UsNode.GetSelectedCanOpenManager();

      if (c_UsNode.GetCanOpenSelectedUseCaseOrInterface() == false)
      {
         this->SetInterfaceSelected(u8_InterfaceNumber);
      }
      else
      {
         const QString c_DeviceNodeName = c_UsNode.GetSelectedCanOpenDeviceNodeName();

         c_NodeId.u8_InterfaceNumber = c_UsNode.GetSelectedCanOpenDeviceInterfaceNumber();

         for (uint32_t u32_Counter = 0; u32_Counter < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); u32_Counter++)
         {
            const C_OscNode * const pc_DeviceNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_Counter);
            if ((pc_DeviceNode != NULL) && (pc_DeviceNode->c_Properties.c_Name.c_str() == c_DeviceNodeName))
            {
               uint32_t u32_UseCaseIndex;

               c_NodeId.u32_NodeIndex = u32_Counter;
               u32_UseCaseIndex = c_UsNode.GetSelectedCanOpenDeviceUseCaseIndex();
               this->SetDeviceSelected(u8_InterfaceNumber, c_NodeId, u32_UseCaseIndex);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SaveUserSettings(void) const
{
   C_OscCanInterfaceId c_NodeId;
   const uint32_t u32_NodeIndex = this->mc_Model.GetNodeIndex();

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

   if (pc_Node != NULL)
   {
      std::map<uint8_t, bool> c_SaveInterface;
      std::map<uint8_t, bool> c_SaveDevices;
      std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >, bool> c_SaveDevice;

      const std::map<uint8_t, C_OscCanOpenManagerInfo> c_CanOpenManagers = pc_Node->c_CanOpenManagers;

      for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager = c_CanOpenManagers.begin();
           c_ItManager != c_CanOpenManagers.end(); ++c_ItManager)
      {
         const QModelIndex c_InterfaceModelIndex = this->mc_Model.GetInterfaceModelIndex(c_ItManager->first);
         const QModelIndex c_DevicesModelIndex = this->mc_Model.GetDevicesModelIndex(c_ItManager->first);

         const C_OscCanOpenManagerInfo c_CanOpenManagerInfo = c_ItManager->second;
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo> & rc_Devices = c_CanOpenManagerInfo.c_CanOpenDevices;
         for (std::map<C_OscCanInterfaceId,
                       C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevices = rc_Devices.begin();
              c_ItDevices != rc_Devices.end(); ++c_ItDevices)
         {
            const QModelIndex c_DeviceModelIndex =
               this->mc_Model.GetDeviceModelIndex(c_ItManager->first, c_ItDevices->first);

            if (this->isExpanded(c_DeviceModelIndex) == true)
            {
               const C_OscNode * const pc_DeviceNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                  c_ItDevices->first.u32_NodeIndex);
               if (pc_DeviceNode != NULL)
               {
                  const std::pair<uint8_t, stw::scl::C_SclString> c_PairInterfaceId(
                     c_ItDevices->first.u8_InterfaceNumber, pc_DeviceNode->c_Properties.c_Name.c_str());
                  const std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> > c_Pair(
                     c_ItManager->first, c_PairInterfaceId);
                  c_SaveDevice[c_Pair] = true;
               }
            }
         }

         if (this->isExpanded(c_InterfaceModelIndex) == true)
         {
            c_SaveInterface[c_ItManager->first] = true;
         }

         if (this->isExpanded(c_DevicesModelIndex) == true)
         {
            c_SaveDevices[c_ItManager->first] = true;
         }
      }

      C_UsHandler::h_GetInstance()->SetProjSdNodeExpandedCanOpenTree(
         pc_Node->c_Properties.c_Name.c_str(), c_SaveInterface, c_SaveDevices, c_SaveDevice);

      if (this->currentIndex().isValid() == true)
      {
         uint8_t u8_InterfaceNumber;
         bool q_Reset = true;

         if ((this->mc_Model.GetInterfaceIdForModelIndex(this->currentIndex(), u8_InterfaceNumber) == C_NO_ERR))
         {
            bool q_IsUseCaseSelected = false;

            if (this->mc_Model.GetDeviceIndexForModelIndex(this->currentIndex(), u8_InterfaceNumber,
                                                           c_NodeId) == C_NO_ERR)
            {
               uint32_t u32_UseCaseIndex;

               if (this->mc_Model.GetDeviceAndUseCaseIndexForModelIndex(this->currentIndex(),
                                                                        u8_InterfaceNumber,
                                                                        c_NodeId,
                                                                        u32_UseCaseIndex) == C_NO_ERR)
               {
                  C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenDeviceUseCaseIndex(
                     pc_Node->c_Properties.c_Name.c_str(), u32_UseCaseIndex);
                  q_IsUseCaseSelected = true;
                  C_UsHandler::h_GetInstance()->SetProjSdNodeCanOpenSelectedUseCaseOrInterface(
                     pc_Node->c_Properties.c_Name.c_str(), q_IsUseCaseSelected);
               }

               const C_OscNode * const pc_InterfaceIdNode =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeId.u32_NodeIndex);

               C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenDevice(
                  pc_Node->c_Properties.c_Name.c_str(), c_NodeId.u8_InterfaceNumber,
                  pc_InterfaceIdNode->c_Properties.c_Name.c_str());
            }

            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenManager(
               pc_Node->c_Properties.c_Name.c_str(), u8_InterfaceNumber);

            if (q_IsUseCaseSelected == false)
            {
               C_UsHandler::h_GetInstance()->SetProjSdNodeCanOpenSelectedUseCaseOrInterface(
                  pc_Node->c_Properties.c_Name.c_str(), q_IsUseCaseSelected);
            }

            q_Reset = false;
         }
         else
         {
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenManager(
               pc_Node->c_Properties.c_Name.c_str(), 0);
         }

         // reset if none is selected
         if (q_Reset == true)
         {
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenDeviceUseCaseIndex(
               pc_Node->c_Properties.c_Name.c_str(), 0);
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenDevice(
               pc_Node->c_Properties.c_Name.c_str(), 0, "");
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedCanOpenManager(
               pc_Node->c_Properties.c_Name.c_str(), 0);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select specific interface in CANopen tree view.

   \param[in]  ou8_InterfaceNumber  interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SetInterfaceSelected(const uint8_t ou8_InterfaceNumber)
{
   const QModelIndex c_Index = this->mc_Model.GetInterfaceModelIndex(ou8_InterfaceNumber);

   this->setCurrentIndex(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select specific device in CANopen tree view.

   \param[in]  ou8_InterfaceNumber  interface number
   \param[in]  orc_NodeId           node index
   \param[in]  ou32_UseCaseIndex    index of use case (EDS_FILE | Configuration | PDOs)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SetDeviceSelected(const uint8_t ou8_InterfaceNumber,
                                                const C_OscCanInterfaceId & orc_NodeId,
                                                const uint32_t ou32_UseCaseIndex)
{
   const QModelIndex c_Index = this->mc_Model.GetDeviceUseCaseModelIndex(ou8_InterfaceNumber, orc_NodeId,
                                                                         ou32_UseCaseIndex);

   this->setCurrentIndex(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Navigate to CANopen Manager configuration of specific interface number

   \param[in]  ou8_InterfaceNumber    Interface number of manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::OpenManagerConfiguration(const uint8_t ou8_InterfaceNumber)
{
   const QModelIndex c_Index = this->mc_Model.GetInterfaceModelIndex(ou8_InterfaceNumber);

   this->setCurrentIndex(c_Index);
   this->expand(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Navigate to CANopen device in tree

   \param[in]  ou32_DeviceNodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::OpenDeviceConfiguration(const uint32_t ou32_DeviceNodeIndex)
{
   const C_OscNode * const pc_Manager = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Manager != NULL);
   if (pc_Manager != NULL)
   {
      uint8_t u8_InterfaceNumber;
      C_OscCanInterfaceId c_DeviceNodeId;
      int32_t s32_Return;
      s32_Return = C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerOfDeviceAndId(ou32_DeviceNodeIndex,
                                                                                   NULL,
                                                                                   &u8_InterfaceNumber,
                                                                                   &c_DeviceNodeId);

      tgl_assert(s32_Return == C_NO_ERR);
      if (s32_Return == C_NO_ERR)
      {
         this->SetDeviceSelected(u8_InterfaceNumber, c_DeviceNodeId,
                                 C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Rechecks the tree elements for errors of current selected interface and adapt the icons

   Must be triggered from outside. Will not be called be tree
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::CheckError(void)
{
   if (this->mq_Initialized == true)
   {
      const QModelIndexList & rc_SelectedIndexes = this->selectionModel()->selectedIndexes();

      if (rc_SelectedIndexes.size() == 1)
      {
         const QModelIndex & rc_CurrentIndex = rc_SelectedIndexes[0];

         if (this->mc_Model.hasChildren(rc_CurrentIndex) == false)
         {
            if ((rc_CurrentIndex.parent().isValid() == true) &&
                (rc_CurrentIndex.parent().parent().isValid() == true) &&
                (rc_CurrentIndex.parent().parent().parent().isValid() == true))
            {
               // Device page
               this->mc_Model.CheckError(rc_CurrentIndex.parent().parent().parent());
            }
         }
         else
         {
            // Manager page
            this->mc_Model.CheckError(rc_CurrentIndex);
         }
      }
      else
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

         // Nothing selected, check all
         if (pc_Node != NULL)
         {
            uint32_t u32_IntfCounter;

            for (u32_IntfCounter = 0U; u32_IntfCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                 ++u32_IntfCounter)
            {
               const C_OscNodeComInterfaceSettings & rc_Intf = pc_Node->c_Properties.c_ComInterfaces[u32_IntfCounter];
               if (rc_Intf.e_InterfaceType == C_OscSystemBus::eCAN)
               {
                  const QModelIndex c_ModelInterfaceIndex = this->mc_Model.GetInterfaceModelIndex(
                     rc_Intf.u8_InterfaceNumber);

                  this->mc_Model.CheckError(c_ModelInterfaceIndex);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create new device

   \param[in]  orc_EdsPath    Eds path

   \return
   New device
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanOpenManagerDeviceInfo C_SdNdeCoConfigTreeView::h_CreateNewDevice(const QString & orc_EdsPath)
{
   C_OscCanOpenManagerDeviceInfo c_Config;
   const QFileInfo c_FileInfo(orc_EdsPath);

   c_Config.c_EdsFileName = TglExtractFileName(orc_EdsPath.toStdString());
   tgl_assert(c_Config.c_EdsFileContent.LoadFromFile(orc_EdsPath.toStdString()) == C_NO_ERR);
   tgl_assert(c_FileInfo.exists());
   C_SdNdeCoConfigTreeView::mh_InitMappableSignals(c_Config.c_EdsFileMappableSignals, c_Config.c_EdsFileContent,
                                                   c_FileInfo.suffix().toLower() == "eds");
   C_SdNdeCoConfigTreeView::mh_InitNewDeviceContent(c_Config);
   return c_Config;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Selection changed

   \param[in]  orc_SelectedItems    Selected items
   \param[in]  orc_DeselectedItems  Deselected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::selectionChanged(const QItemSelection & orc_SelectedItems,
                                               const QItemSelection & orc_DeselectedItems)
{
   if ((orc_SelectedItems.size() == 1) &&
       (orc_SelectedItems.indexes().size() == 0))
   {
      // Special scenario: When clicking on an element which is enabled (necessary for checkbox) but is not selectable,
      // the selection changed signal of Qt is emitted anyway. The selected item is only selected on "view" layer.
      // orc_SelectedItems equals 1 but the model index size is zero. This scenario does not make any sense.
      // This would cause in case of a following real selection of this element by checking the checkbox
      // an not emitted selection changed signal of Qt.
      // The solution: Revert the pretended selection
      if ((orc_DeselectedItems.size() == 1) &&
          (orc_DeselectedItems.indexes().size() == 1))
      {
         // The previous selection was a specific element
         this->setCurrentIndex(orc_DeselectedItems.indexes().at(0));
      }
      else
      {
         // Coming from the overview, nothing was selected before
         this->setCurrentIndex(QModelIndex());
      }
   }
   else
   {
      C_OgeTreeViewToolTipBase::selectionChanged(orc_SelectedItems, orc_DeselectedItems);
      this->m_OnItemSelected();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: Draw the separator lines and background for selected items

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::paintEvent(QPaintEvent * const opc_Event)
{
   if (this->viewport() != NULL)
   {
      QRect c_Rect;
      QPainter c_Painter(this->viewport());
      const QModelIndexList & rc_SelectedIndexes = this->selectionModel()->selectedIndexes();

      c_Painter.setPen(Qt::transparent);

      if (rc_SelectedIndexes.size() == 1)
      {
         const QModelIndex & rc_CurrentIndex = rc_SelectedIndexes[0];
         const QModelIndex c_DeviceItemParent = rc_CurrentIndex.parent();

         // draw background of selected items on complete row width
         c_Rect = this->visualRect(rc_CurrentIndex);
         c_Rect.setX(0);
         c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_10);
         c_Painter.drawRect(c_Rect);

         // draw the special lines
         if ((this->mc_Model.hasChildren(rc_CurrentIndex) == false) &&
             (c_DeviceItemParent.isValid() == true) &&
             (this->isExpanded(c_DeviceItemParent) == true))
         {
            const QModelIndex c_DevicesItemParent = c_DeviceItemParent.parent();

            if ((c_DevicesItemParent.isValid() == true) &&
                (this->isExpanded(c_DevicesItemParent) == true))
            {
               const QModelIndex c_IntfItemParent = c_DevicesItemParent.parent();
               if ((c_IntfItemParent.isValid() == true) &&
                   (this->isExpanded(c_IntfItemParent) == true))
               {
                  // Draw the lines only if all parent items are expanded
                  QModelIndex c_LastIndex;

                  c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_7);

                  c_LastIndex =
                     this->mc_Model.index(this->mc_Model.rowCount(c_DeviceItemParent) - 1, 0, c_DeviceItemParent);

                  // draw the top line
                  c_Rect = this->visualRect(c_DeviceItemParent);
                  c_Painter.drawRect(0, c_Rect.y(), this->width(), 2);

                  // draw the bottom line
                  c_Rect = this->visualRect(c_LastIndex);
                  c_Painter.drawRect(0, c_Rect.y() + c_Rect.height(), this->width(), 2);
               }
            }
         }
      }
   }

   C_OgeTreeViewToolTipBase::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_SetupContextMenu()
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeCoConfigTreeView::customContextMenuRequested,
           this, &C_SdNdeCoConfigTreeView::m_OnCustomContextMenuRequested);

   this->mpc_AddAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add CANopen Device"),
                                                          this, &C_SdNdeCoConfigTreeView::m_OnAddDevice);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"), this, &C_SdNdeCoConfigTreeView::expandAll);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"), this,
                                    &C_SdNdeCoConfigTreeView::collapseAll);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_RemoveAction = this->mpc_ContextMenu->addAction("Unknown",
                                                             this, &C_SdNdeCoConfigTreeView::m_OnRemoveDevice);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QModelIndex c_Index = this->indexAt(orc_Pos);

   if (c_Index.isValid() == true)
   {
      uint8_t u8_InterfaceNumber;
      C_OscCanInterfaceId c_NodeId;
      const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

      this->mc_LastKnownContextMenuPos = orc_Pos;
      if (this->mc_Model.GetInterfaceIdForModelIndex(c_Index, u8_InterfaceNumber) == C_NO_ERR)
      {
         if (C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(this->mu32_NodeIndex, u8_InterfaceNumber) != NULL)
         {
            this->mpc_AddAction->setEnabled(true);
         }
         else
         {
            this->mpc_AddAction->setEnabled(false);
         }
      }

      if (this->mc_Model.GetDeviceIndexForModelIndex(c_Index, u8_InterfaceNumber, c_NodeId) == C_NO_ERR)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeId.u32_NodeIndex);
         if (pc_Node != NULL)
         {
            this->mpc_RemoveAction->setVisible(true);
            this->mpc_RemoveAction->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                    "Remove CANopen Device \"%1\"")).arg(pc_Node->
                                                                                                         c_Properties.
                                                                                                         c_Name.c_str()));
         }
         else
         {
            this->mpc_RemoveAction->setVisible(false);
         }
      }
      else
      {
         this->mpc_RemoveAction->setVisible(false);
      }

      // popup
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add device action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnAddDevice(void)
{
   const QModelIndex c_Index = this->indexAt(this->mc_LastKnownContextMenuPos);
   uint8_t u8_InterfaceNumber;

   if (this->mc_Model.GetInterfaceIdForModelIndex(c_Index, u8_InterfaceNumber) == C_NO_ERR)
   {
      const QPointer<C_OgePopUpDialog> c_PopUp = new C_OgePopUpDialog(this, this);
      C_SdNdeCoAddDeviceDialog * const pc_AddDialog = new C_SdNdeCoAddDeviceDialog(*c_PopUp,
                                                                                   this->mu32_NodeIndex,
                                                                                   u8_InterfaceNumber);

      //Resize
      const QSize c_SIZE(1200, 829);
      c_PopUp->SetSize(c_SIZE);

      Q_UNUSED(pc_AddDialog)

      if (c_PopUp->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         // Let user select node and interface
         uint32_t u32_NodeIndex = 0;
         uint32_t u32_InterfaceIndex = 0;
         QString c_EdsPath;
         if (pc_AddDialog->GetNodeSelection(u32_NodeIndex, u32_InterfaceIndex, c_EdsPath) == C_RANGE)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Add CANopen Device"));
            c_Message.SetDescription(C_GtGetText::h_GetText(
                                        "Cannot add CANopen Device. There is no free Node available."));
            c_Message.SetCustomMinHeight(180, 250);
            c_Message.Execute();
         }
         else if (!TglFileExists(pc_AddDialog->GetEdsFile()))
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("EDS File"));
            c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                             "Could not load specified EDS File.")));
            c_Message.SetDetails(C_GtGetText::h_GetText("Specified EDS File does not exist."));
            c_Message.SetCustomMinHeight(180, 250);
            c_Message.Execute();
         }
         else
         {
            this->m_OnAddDeviceReport(u32_NodeIndex, u32_InterfaceIndex, u8_InterfaceNumber, c_EdsPath);
         }
      }

      if (c_PopUp != NULL)
      {
         c_PopUp->HideOverlay();
         c_PopUp->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add device report

   \param[in]  ou32_SelectedNodeIndex           Selected node index
   \param[in]  ou32_SelectedNodeInterfaceIndex  Selected node interface index
   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_EdsPath                      Eds path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnAddDeviceReport(const uint32_t ou32_SelectedNodeIndex,
                                                  const uint32_t ou32_SelectedNodeInterfaceIndex,
                                                  const uint8_t ou8_OriginalNodeInterfaceNumber,
                                                  const QString & orc_EdsPath)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_SelectedNodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      tgl_assert(ou32_SelectedNodeInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      if (ou32_SelectedNodeInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         QString c_ParsingError;
         C_CieImportDataAssignment c_NodeAssignment;
         C_CieImportDataAssignment c_InvalidNodeAssignment;
         const C_OscNodeComInterfaceSettings & rc_CurInterface =
            pc_Node->c_Properties.c_ComInterfaces[ou32_SelectedNodeInterfaceIndex];
         const int32_t s32_ImportResult = C_CieUtil::h_GetDeviceInfo(ou32_SelectedNodeIndex,
                                                                     ou32_SelectedNodeInterfaceIndex,
                                                                     this->mu32_NodeIndex,
                                                                     ou8_OriginalNodeInterfaceNumber,
                                                                     orc_EdsPath,
                                                                     c_NodeAssignment,
                                                                     c_InvalidNodeAssignment,
                                                                     c_ParsingError);
         if (s32_ImportResult == C_NO_ERR)
         {
            const std::vector<C_CieImportDataAssignment> c_NodeAssignmentVector = {c_NodeAssignment};

            const std::vector<C_CieImportDataAssignment> c_InvalidNodeAssignmentVector = {c_InvalidNodeAssignment};

            const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
            C_CieImportReportWidget * const pc_Dialog =
               new C_CieImportReportWidget(*c_New, orc_EdsPath, rc_CurInterface.u32_BusIndex,
                                           C_OscCanProtocol::eCAN_OPEN,
                                           c_NodeAssignmentVector, c_InvalidNodeAssignmentVector,
                                           &pc_Node->c_Properties.c_Name, true);

            Q_UNUSED(pc_Dialog)

            //Resize
            c_New->SetSize(mc_POPUP_REPORT_SIZE);

            if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
            {
               this->m_OnAddDeviceReportAccepted(ou32_SelectedNodeIndex, rc_CurInterface.u8_InterfaceNumber,
                                                 ou8_OriginalNodeInterfaceNumber, orc_EdsPath);
            }

            if (c_New != NULL)
            {
               c_New->HideOverlay();
               c_New->deleteLater();
            }
         } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
         else
         {
            C_CieUtil::h_ReportEdsImportError(this, s32_ImportResult, c_ParsingError, rc_CurInterface.u8_NodeId);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add device report accepted

   \param[in]  ou32_SelectedNodeIndex           Selected node index
   \param[in]  ou8_SelectedNodeInterfaceNumber  Selected node interface number
   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_EdsPath                      Eds path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnAddDeviceReportAccepted(const uint32_t ou32_SelectedNodeIndex,
                                                          const uint8_t ou8_SelectedNodeInterfaceNumber,
                                                          const uint8_t ou8_OriginalNodeInterfaceNumber,
                                                          const QString & orc_EdsPath)
{
   const C_OscCanOpenManagerDeviceInfo c_Config = C_SdNdeCoConfigTreeView::h_CreateNewDevice(orc_EdsPath);
   const C_OscCanInterfaceId c_SelectedDeviceId(ou32_SelectedNodeIndex, ou8_SelectedNodeInterfaceNumber);

   this->mc_Model.AddDevice(ou32_SelectedNodeIndex, ou8_SelectedNodeInterfaceNumber, ou8_OriginalNodeInterfaceNumber,
                            c_Config);
   this->m_SelectDevice(ou8_OriginalNodeInterfaceNumber, c_SelectedDeviceId);
   this->m_DisplayDeviceAddFinishedMessage();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select device

   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_DeviceId                     Device id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_SelectDevice(const uint8_t ou8_OriginalNodeInterfaceNumber,
                                             const C_OscCanInterfaceId & orc_DeviceId)
{
   const QModelIndex c_DeviceIndex = this->mc_Model.GetDeviceUseCaseModelIndex(ou8_OriginalNodeInterfaceNumber,
                                                                               orc_DeviceId,
                                                                               C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION);

   this->setCurrentIndex(c_DeviceIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Display device add finished message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_DisplayDeviceAddFinishedMessage()
{
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eINFORMATION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Add CANopen Device"));
   c_Message.SetDescription(C_GtGetText::h_GetText("CANopen Device added successfully"));
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle remove device action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnRemoveDevice()
{
   const QModelIndex c_Index = this->indexAt(this->mc_LastKnownContextMenuPos);
   uint8_t u8_InterfaceNumber;
   C_OscCanInterfaceId c_NodeId;

   if (this->mc_Model.GetDeviceIndexForModelIndex(c_Index, u8_InterfaceNumber, c_NodeId) == C_NO_ERR)
   {
      this->mc_Model.RemoveDevice(u8_InterfaceNumber, c_NodeId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle item selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnItemSelected(void)
{
   const QModelIndexList c_SelectedItems = this->selectedIndexes();

   if (c_SelectedItems.size() >= 1)
   {
      const QModelIndex c_Current =  c_SelectedItems.at(0);
      tgl_assert(c_SelectedItems.size() == 1);
      if (c_Current.isValid() == true)
      {
         if ((c_Current.parent().isValid() == true) &&
             (this->mc_Model.hasChildren(c_Current) == true))
         {
            // Special case: A device was selected. Select its first sub element, the configuration widget
            if (this->isExpanded(c_Current) == false)
            {
               // If not already expand, expand the item first before selecting the sub element
               // Otherwise the current device item could lose the select style (device and sub element are styled
               // as selected)
               this->expand(c_Current);
            }
            this->setCurrentIndex(c_Current.child(C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION,
                                                  0));
         }
         else
         {
            uint8_t u8_ManagerInterfaceNumber;

            if ((this->mc_Model.hasChildren(c_Current) == false) &&
                (c_Current.parent().isValid() == true) &&
                (this->isExpanded(c_Current.parent()) == true))
            {
               // delegate needs to know parent row of current index
               this->mc_Delegate.SetCurrentIndexParentRow(c_Current.parent());
            }
            else
            {
               this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
            }

            if (this->mc_Model.GetInterfaceIdForModelIndex(c_Current, u8_ManagerInterfaceNumber) == C_NO_ERR)
            {
               C_OscCanInterfaceId c_DeviceNodeId;
               uint32_t u32_UseCaseIndex;

               if (this->mc_Model.GetDeviceAndUseCaseIndexForModelIndex(c_Current, u8_ManagerInterfaceNumber,
                                                                        c_DeviceNodeId,
                                                                        u32_UseCaseIndex) == C_NO_ERR)
               {
                  Q_EMIT (this->SigDeviceSelected(u8_ManagerInterfaceNumber, c_DeviceNodeId, u32_UseCaseIndex));
               }
               else
               {
                  Q_EMIT (this->SigInterfaceSelected(u8_ManagerInterfaceNumber));
               }
            }
         }
      }
      else
      {
         this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
      }
   }
   else
   {
      this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init mappable signals

   \param[in,out]  orc_MappableSignals    Mappable signals
   \param[in]      orc_EdsDictionary      Eds dictionary
   \param[in]      oq_IsEds               Flag if current file is an EDS file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::mh_InitMappableSignals(
   std::vector<C_OscCanOpenManagerMappableSignal> & orc_MappableSignals,
   const C_OscCanOpenObjectDictionary & orc_EdsDictionary, const bool oq_IsEds)
{
   std::map<uint32_t, std::vector<uint32_t> > c_MappableObjects;
   orc_EdsDictionary.GetMappableObjects(c_MappableObjects);
   for (std::map<uint32_t, std::vector<uint32_t> >::const_iterator c_It = c_MappableObjects.begin();
        c_It != c_MappableObjects.end(); ++c_It)
   {
      for (uint32_t u32_ItSig = 0UL; u32_ItSig < c_It->second.size(); ++u32_ItSig)
      {
         C_OscCanOpenManagerMappableSignal c_Entry;
         if (C_OscImportEdsDcf::h_ParseSignalContent(orc_EdsDictionary.c_Objects, c_It->first,
                                                     c_It->second[u32_ItSig], 0UL,
                                                     true, oq_IsEds, c_Entry.c_SignalData,
                                                     c_Entry.c_DatapoolData,
                                                     c_Entry.q_AutoMinMaxUsed) == C_NO_ERR)
         {
            C_CieUtil::h_AdaptName(c_Entry.c_DatapoolData.c_Name, c_Entry.c_DatapoolData.c_Comment, false);
            orc_MappableSignals.push_back(c_Entry);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select manager

   \param[in]  orc_ManagerIndex  Manager index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_SelectManager(const QModelIndex & orc_ManagerIndex)
{
   this->setCurrentIndex(orc_ManagerIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handling an unchecked manager

   \param[in]  orc_ManagerIndex  Manager index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_HandleManagerUnchecked(const QModelIndex & orc_ManagerIndex)
{
   const QModelIndex c_CurrentSelection = this->currentIndex();
   const bool q_ManagerIsSelected = (c_CurrentSelection == orc_ManagerIndex);

   // Three scenarios in case of an unchecked manager:
   // * Interface of this manager is selected: the selection must be cleared and jump to the overview
   // * A widget of one device of the manager was selected. The tree entries are deleted already, the selection is
   //    already invalid, jump to overview
   // * The overview is already visible, it must be updated
   if ((q_ManagerIsSelected == true) ||
       (c_CurrentSelection.isValid() == false))
   {
      if (q_ManagerIsSelected == true)
      {
         this->setCurrentIndex(QModelIndex());
      }
      Q_EMIT (this->SigOpenOverview());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handling a removed device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_HandleDeviceRemoved(void) const
{
   const QModelIndex c_CurrentSelection = this->currentIndex();

   // Three scenarios in case of a removed device:
   // * Sub widget of this device is selected: the selection is already invalid but the widget is still visible
   //    jump to the overview
   // * A widget of an other device or the manager interface was selected. Selection is still valid, nothing to do
   // * The overview is already visible, it must be updated
   if (c_CurrentSelection.isValid() == false)
   {
      Q_EMIT (this->SigOpenOverview());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for expanded signal of tree view

   \param[in]  orc_ExpandedIndex    Index of expanded element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnExpanded(const QModelIndex & orc_ExpandedIndex)
{
   // Expand all children
   this->expandRecursively(orc_ExpandedIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init new device content

   \param[in,out]  orc_Device    Device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::mh_InitNewDeviceContent(C_OscCanOpenManagerDeviceInfo & orc_Device)
{
   const int32_t s32_HB_OFF_VALUE = 0;
   const QString c_FileName = orc_Device.c_EdsFileName.UpperCase().c_str();
   const bool q_IsEds = c_FileName.endsWith(".EDS");

   //heartbeat producing supported?
   if (orc_Device.c_EdsFileContent.IsHeartbeatProducerSupported() == true)
   {
      //supported
      // check for read-only
      bool q_HbProducerRo = true;
      tgl_assert(orc_Device.c_EdsFileContent.IsHeartbeatProducerRo(q_HbProducerRo) == C_NO_ERR);

      // get default value first (100 ms)
      const int32_t s32_DefaultHeartbeatProducerTimeMs =
         static_cast<int32_t>(orc_Device.u16_HeartbeatProducerTimeMs);
      int32_t s32_HeartbeatProducerTimeMs = s32_DefaultHeartbeatProducerTimeMs;

      // get heartbeat producer time from EDS
      const C_OscCanOpenObject * const pc_OscCanOpenObject = orc_Device.c_EdsFileContent.GetCanOpenObject(
         C_OscCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_PRODUCER);
      if (pc_OscCanOpenObject != NULL)
      {
         const stw::scl::C_SclString c_Value = C_OscImportEdsDcf::h_GetCoObjectValue(*pc_OscCanOpenObject, q_IsEds);
         // in case there is an EDS entry
         if (c_Value.IsEmpty() == false)
         {
            try
            {
               // get value from EDS file
               s32_HeartbeatProducerTimeMs = c_Value.ToInt();
            }
            catch (...)
            {
               const stw::scl::C_SclString c_Info = "Could not convert the following number: \"" +
                                                    c_Value + "\". Switching heartbeat off.";
               s32_HeartbeatProducerTimeMs = s32_HB_OFF_VALUE;
               osc_write_log_warning("Read CANopen EDS Heartbeat Producer default value", c_Info);
            }
         }
      }

      // on/off handling if default value from EDS is 0
      if (s32_HeartbeatProducerTimeMs == s32_HB_OFF_VALUE)
      {
         orc_Device.u16_HeartbeatProducerTimeMs = static_cast<uint16_t>(s32_DefaultHeartbeatProducerTimeMs); // default
                                                                                                             // value,
                                                                                                             // since 0
         // is not allowed
         orc_Device.q_EnableHeartbeatProducing = false;
      }
      else
      {
         orc_Device.q_EnableHeartbeatProducing = !q_HbProducerRo;
         orc_Device.u16_HeartbeatProducerTimeMs = static_cast<uint16_t>(s32_HeartbeatProducerTimeMs);
      }
   }
   else
   {
      //no support
      orc_Device.q_EnableHeartbeatProducing = false;
   }

   if (orc_Device.c_EdsFileContent.GetNumHeartbeatConsumers() != 0)
   {
      // check for read-only
      bool q_HbConsumerRo = true;
      tgl_assert(orc_Device.c_EdsFileContent.IsHeartbeatConsumerRo(q_HbConsumerRo) == C_NO_ERR);

      orc_Device.q_EnableHeartbeatConsuming = !q_HbConsumerRo;

      if (q_HbConsumerRo == true)
      {
         // get default value first (100 ms)
         const int32_t s32_DefaultHeartbeatConsumerTimeMs =
            static_cast<int32_t>(orc_Device.u16_HeartbeatConsumerTimeMs);
         int32_t s32_HeartbeatConsumerTimeMs = s32_DefaultHeartbeatConsumerTimeMs;

         // In case of read only, the default value of EDS file must be set
         // get heartbeat consumer time from EDS
         // Checking the first consumer because this is the entry which will be used for the user specified
         // value when it is not read-only too
         const C_OscCanOpenObject * const pc_OscCanOpenObject =
            orc_Device.c_EdsFileContent.GetCanOpenSubIndexObject(
               C_OscCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_CONSUMER, 1);
         if (pc_OscCanOpenObject != NULL)
         {
            const stw::scl::C_SclString c_Value =
               C_OscImportEdsDcf::h_GetCoObjectValue(*pc_OscCanOpenObject, q_IsEds);
            // in case there is an EDS entry
            if (c_Value.IsEmpty() == false)
            {
               try
               {
                  // get value from EDS file
                  s32_HeartbeatConsumerTimeMs = c_Value.ToInt();
               }
               catch (...)
               {
                  const stw::scl::C_SclString c_Info = "Could not convert the following number: \"" +
                                                       c_Value +
                                                       "\". Switching heartbeat off.";
                  s32_HeartbeatConsumerTimeMs = s32_HB_OFF_VALUE;
                  osc_write_log_warning("CANopen EDS Heartbeat Consumer default value", c_Info);
               }
            }
         }

         // on/off handling if default value from EDS is 0
         if (s32_HeartbeatConsumerTimeMs == s32_HB_OFF_VALUE)
         {
            orc_Device.u16_HeartbeatConsumerTimeMs = static_cast<uint16_t>(s32_DefaultHeartbeatConsumerTimeMs); // default
                                                                                                                // value,
            // since 0
            // is not allowed
            orc_Device.q_EnableHeartbeatConsuming = false;
         }
         else
         {
            orc_Device.u16_HeartbeatConsumerTimeMs = static_cast<uint16_t>(s32_HeartbeatConsumerTimeMs);
            orc_Device.q_EnableHeartbeatConsuming = true;
         }
         // In any case the automatic is deactivated
         orc_Device.q_EnableHeartbeatConsumingAutoCalculation = false;
      }
   }
   else
   {
      //no support or manager HB producing is disabled
      orc_Device.q_EnableHeartbeatConsuming = false;
   }
}
