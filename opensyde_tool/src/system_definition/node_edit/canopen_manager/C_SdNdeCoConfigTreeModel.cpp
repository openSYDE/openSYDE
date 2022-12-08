//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for tree for navigation and configuration of CANopen Manager

   Model for tree for navigation and configuration of CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDir>

#include "C_Uti.hpp"
#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "constants.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeCoConfigTreeModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION = 0UL;
const uint32_t C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_PDOS = 1UL;
const uint32_t C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_EDS_FILE = 2UL;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoConfigTreeModel::C_SdNdeCoConfigTreeModel(QWidget * const opc_Parent) :
   C_TblTreeModelCheckable(opc_Parent),
   mpc_Parent(opc_Parent),
   mu32_NodeIndex(0)
{
   this->m_InitIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor.

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoConfigTreeModel::~C_SdNdeCoConfigTreeModel()
{
   this->m_CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active node id

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::SetNodeId(const uint32_t ou32_NodeIndex)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   this->beginResetModel();
   this->m_CleanUpLastModel();
   this->mc_LookupTreeIndex.clear();
   if (pc_Node != NULL)
   {
      this->mu32_NodeIndex = ou32_NodeIndex;
      this->mpc_InvisibleRootItem = new C_TblTreeModelCheckableItem();
      for (uint32_t u32_ItInterface = 0UL; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size();
           ++u32_ItInterface)
      {
         const C_OscNodeComInterfaceSettings & rc_Interface = pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
         if (rc_Interface.e_InterfaceType == C_OscSystemBus::eCAN)
         {
            C_TblTreeModelCheckableItem * const pc_InterfaceEntry = new C_TblTreeModelCheckableItem();
            this->m_InitInterfaceNode(rc_Interface, *pc_Node, *pc_InterfaceEntry);
            this->mpc_InvisibleRootItem->AddChild(pc_InterfaceEntry);
            this->m_CheckError(pc_InterfaceEntry);
         }
      }
   }
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init

   \param[in]  orc_ElementIndices   Element indices

   \return
   STW error codes

   \retval   C_NO_ERR   Init successful
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoConfigTreeModel::Init(const std::vector<uint32_t> & orc_ElementIndices)
{
   Q_UNUSED(orc_ElementIndices)
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get checked items

   \param[in,out]  orc_ElementIndices           Element indices
   \param[in,out]  orc_ChildIndicesPerElement   Child indices per element
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in this implementation
void C_SdNdeCoConfigTreeModel::GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices,
                                               std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const
{
   Q_UNUSED(orc_ElementIndices)
   Q_UNUSED(orc_ChildIndicesPerElement)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get checked item count

   \return
   Checked item count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeCoConfigTreeModel::GetCheckedItemCount() const
{
   return 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoConfigTreeModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                       const int32_t os32_Role)
{
   bool q_Return = false;

   if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
   {
      if (orc_Index.isValid() == true)
      {
         //lint -e{9079}  Result of Qt interface restrictions, set by index function
         C_TblTreeModelCheckableItem * const pc_TreeItem =
            static_cast<C_TblTreeModelCheckableItem *>(orc_Index.internalPointer());

         if (pc_TreeItem != NULL)
         {
            uint32_t u32_ValidLayers = 0UL;
            const QModelIndex c_StartIndex = orc_Index;
            const QModelIndex c_EndIndex = orc_Index;

            tgl_assert(this->m_CountLayers(orc_Index, u32_ValidLayers) == C_NO_ERR);
            if (u32_ValidLayers == 1UL)
            {
               if (this->m_CheckInterface(orc_Index, *pc_TreeItem))
               {
                  // Switching from unchecked and partial to checked
                  pc_TreeItem->e_CheckState = (pc_TreeItem->e_CheckState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
               }
            }
            else
            {
               //Currently no other possibility
               tgl_assert(false);
            }

            Q_EMIT (this->dataChanged(c_StartIndex, c_EndIndex));
         }
      }
   }
   else
   {
      //Skip checkable model
      q_Return = C_TblTreModel::setData(orc_Index, orc_Value, os32_Role);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeCoConfigTreeModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interface id for model index

   \param[in]   orc_Index              Index
   \param[out]  oru8_InterfaceNumber   Interface number

   \return
   STW error codes

   \retval   C_NO_ERR   Interface found
   \retval   C_RANGE    Interface not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoConfigTreeModel::GetInterfaceIdForModelIndex(const QModelIndex & orc_Index,
                                                              uint8_t & oru8_InterfaceNumber) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Index.isValid())
   {
      QModelIndex c_Tmp = orc_Index;
      //Move to top level
      while (c_Tmp.parent().isValid())
      {
         c_Tmp = c_Tmp.parent();
      }
      {
         //Select interface
         //lint -e{9079} Result of Qt interface restrictions, set by index function
         C_TblTreItem * const pc_InterfaceItem = static_cast<C_TblTreItem *>(c_Tmp.internalPointer());
         if (pc_InterfaceItem != NULL)
         {
            oru8_InterfaceNumber = static_cast<uint8_t>(pc_InterfaceItem->u32_Index);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device index for model index

   \param[in]   orc_Index              Index
   \param[out]  oru8_InterfaceNumber   Interface number
   \param[in]   orc_DeviceInterfaceId  Device interface id

   \return
   STW error codes

   \retval   C_NO_ERR   Device found
   \retval   C_RANGE    Device not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoConfigTreeModel::GetDeviceIndexForModelIndex(const QModelIndex & orc_Index,
                                                              uint8_t & oru8_InterfaceNumber,
                                                              C_OscCanInterfaceId & orc_DeviceInterfaceId) const
{
   int32_t s32_Retval = this->GetInterfaceIdForModelIndex(orc_Index, oru8_InterfaceNumber);

   if (s32_Retval == C_NO_ERR)
   {
      if ((orc_Index.isValid() && orc_Index.parent().isValid()) && orc_Index.parent().parent().isValid())
      {
         QModelIndex c_Tmp = orc_Index;
         //Move to device level
         if (orc_Index.parent().parent().parent().isValid())
         {
            c_Tmp = c_Tmp.parent();
         }
         {
            //Select device
            //lint -e{9079} Result of Qt interface restrictions, set by index function
            C_TblTreItem * const pc_DeviceItem = static_cast<C_TblTreItem *>(c_Tmp.internalPointer());
            if (pc_DeviceItem != NULL)
            {
               s32_Retval = this->m_RetrieveInterfaceFromLookupTree(oru8_InterfaceNumber, pc_DeviceItem->u32_Index,
                                                                    orc_DeviceInterfaceId);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device and use case index for model index

   \param[in]      orc_Index                     Index
   \param[out]     oru8_ManagerInterfaceNumber   Interface number
   \param[in,out]  orc_DeviceInterfaceId         Device interface id
   \param[out]     oru32_UseCaseIndex            Use case index

   \return
   STW error codes

   \retval   C_NO_ERR   Device and use case found
   \retval   C_RANGE    Device or use case not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoConfigTreeModel::GetDeviceAndUseCaseIndexForModelIndex(const QModelIndex & orc_Index,
                                                                        uint8_t & oru8_ManagerInterfaceNumber,
                                                                        C_OscCanInterfaceId & orc_DeviceInterfaceId,
                                                                        uint32_t & oru32_UseCaseIndex) const
{
   int32_t s32_Retval =
      this->GetDeviceIndexForModelIndex(orc_Index, oru8_ManagerInterfaceNumber, orc_DeviceInterfaceId);

   if (s32_Retval == C_NO_ERR)
   {
      if ((orc_Index.isValid() && orc_Index.parent().isValid()) && orc_Index.parent().parent().isValid() &&
          orc_Index.parent().parent().parent().isValid())
      {
         //lint -e{9079} Result of Qt interface restrictions, set by index function
         C_TblTreItem * const pc_DeviceUseCaseItem = static_cast<C_TblTreItem *>(orc_Index.internalPointer());
         if (pc_DeviceUseCaseItem != NULL)
         {
            oru32_UseCaseIndex = pc_DeviceUseCaseItem->u32_Index;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add device

   \param[in]  ou32_SelectedNodeIndex           Selected node index
   \param[in]  ou8_SelectedNodeInterfaceNumber  Selected node interface number
   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_Config                       New config to add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::AddDevice(const uint32_t ou32_SelectedNodeIndex,
                                         const uint8_t ou8_SelectedNodeInterfaceNumber,
                                         const uint8_t ou8_OriginalNodeInterfaceNumber,
                                         const C_OscCanOpenManagerDeviceInfo & orc_Config)
{
   const C_OscCanInterfaceId c_DeviceId(ou32_SelectedNodeIndex, ou8_SelectedNodeInterfaceNumber);
   const QModelIndex c_Index = this->GetDevicesModelIndex(ou8_OriginalNodeInterfaceNumber);
   const uint32_t u32_Count = this->rowCount(c_Index);
   C_TblTreeModelCheckableItem * const pc_AllDevices = this->m_GetDevicesModelNode(ou8_OriginalNodeInterfaceNumber);

   this->beginInsertRows(c_Index, u32_Count, u32_Count);
   //Handle data
   tgl_assert(C_PuiSdHandler::h_GetInstance()->AddCanOpenManagerDevice(this->mu32_NodeIndex,
                                                                       ou8_OriginalNodeInterfaceNumber, c_DeviceId,
                                                                       orc_Config) == C_NO_ERR);
   //Handle tree
   if (pc_AllDevices != NULL)
   {
      uint32_t u32_ExpectedPos;
      C_TblTreeModelCheckableItem * const pc_NewDevice = new C_TblTreeModelCheckableItem();
      m_InitDeviceNode(ou8_OriginalNodeInterfaceNumber, c_DeviceId, *pc_NewDevice);
      u32_ExpectedPos = m_GetExpectedPosInTree(ou8_OriginalNodeInterfaceNumber, c_DeviceId);
      pc_AllDevices->InsertChild(u32_ExpectedPos, pc_NewDevice);
   }
   this->endInsertRows();
   //Update current node
   this->m_TriggerUpdateDeviceCount(ou8_OriginalNodeInterfaceNumber);
   Q_EMIT (this->SigErrorChange());

   // Update COMM Datapools due to own error check in Datapool view
   Q_EMIT (this->SigCommDatapoolsChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove device

   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_DeviceId                     Device ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::RemoveDevice(const uint8_t ou8_OriginalNodeInterfaceNumber,
                                            const C_OscCanInterfaceId & orc_DeviceId)
{
   const QModelIndex c_DevicesIndex = this->GetDevicesModelIndex(ou8_OriginalNodeInterfaceNumber);
   const QModelIndex c_DeviceIndex = this->GetDeviceModelIndex(ou8_OriginalNodeInterfaceNumber, orc_DeviceId);
   C_TblTreeModelCheckableItem * const pc_AllDevices = this->m_GetDevicesModelNode(ou8_OriginalNodeInterfaceNumber);

   this->beginRemoveRows(c_DevicesIndex, c_DeviceIndex.row(), c_DeviceIndex.row());
   //Handle data
   tgl_assert(C_PuiSdHandler::h_GetInstance()->DeleteCanOpenManagerDevice(this->mu32_NodeIndex,
                                                                          ou8_OriginalNodeInterfaceNumber,
                                                                          orc_DeviceId) == C_NO_ERR);
   //Handle tree
   if (pc_AllDevices != NULL)
   {
      pc_AllDevices->c_Children.erase(pc_AllDevices->c_Children.begin() + c_DeviceIndex.row());
   }
   this->endRemoveRows();
   //Update current node
   this->m_TriggerUpdateDeviceCount(ou8_OriginalNodeInterfaceNumber);
   Q_EMIT (this->SigDeviceRemoved());
   Q_EMIT (this->SigErrorChange());

   // Update COMM Datapools due to own error check in Datapool view
   Q_EMIT (this->SigCommDatapoolsChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interface model index

   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SdNdeCoConfigTreeModel::GetInterfaceModelIndex(const uint8_t ou8_InterfaceNumber) const
{
   QModelIndex c_Index;

   if (this->mpc_InvisibleRootItem != NULL)
   {
      for (uint32_t u32_ItChild = 0UL; u32_ItChild < this->mpc_InvisibleRootItem->c_Children.size(); ++u32_ItChild)
      {
         const C_TblTreeModelCheckableItem * const pc_Node =
            dynamic_cast<const C_TblTreeModelCheckableItem * const>(this->mpc_InvisibleRootItem->c_Children[u32_ItChild]);
         if (pc_Node->u32_Index == static_cast<uint32_t>(ou8_InterfaceNumber))
         {
            c_Index = this->index(u32_ItChild, 0);
            break;
         }
      }
   }

   return c_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device use case model index

   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device id
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SdNdeCoConfigTreeModel::GetDeviceUseCaseModelIndex(const uint8_t ou8_InterfaceNumber,
                                                                 const C_OscCanInterfaceId & orc_DeviceId,
                                                                 const uint32_t ou32_UseCaseIndex)
{
   uint32_t u32_ChildIndex = 0UL;
   const QModelIndex c_ParentIndex = this->GetDeviceModelIndex(ou8_InterfaceNumber, orc_DeviceId);
   C_TblTreeModelCheckableItem * const pc_ParentNode = this->m_GetDeviceModelNode(ou8_InterfaceNumber, orc_DeviceId);
   QModelIndex c_Index;

   if (pc_ParentNode != NULL)
   {
      for (uint32_t u32_ItChild = 0UL; u32_ItChild < pc_ParentNode->c_Children.size(); ++u32_ItChild)
      {
         C_TblTreeModelCheckableItem * const pc_ChildNode =
            dynamic_cast<C_TblTreeModelCheckableItem * const>(pc_ParentNode->c_Children[u32_ItChild]);
         if (pc_ChildNode != NULL)
         {
            if (pc_ChildNode->u32_Index == ou32_UseCaseIndex)
            {
               u32_ChildIndex = u32_ItChild;
               break;
            }
         }
      }
   }
   c_Index = this->index(static_cast<int32_t>(u32_ChildIndex), 0, c_ParentIndex);
   return c_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get devices model index

   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SdNdeCoConfigTreeModel::GetDevicesModelIndex(const uint8_t ou8_InterfaceNumber) const
{
   const QModelIndex c_ParentIndex = this->GetInterfaceModelIndex(ou8_InterfaceNumber);
   const QModelIndex c_Index = this->index(0, 0, c_ParentIndex);

   return c_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device model index

   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device id

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SdNdeCoConfigTreeModel::GetDeviceModelIndex(const uint8_t ou8_InterfaceNumber,
                                                          const C_OscCanInterfaceId & orc_DeviceId) const
{
   uint32_t u32_ChildIndex = 0UL;
   const QModelIndex c_ParentIndex = this->GetDevicesModelIndex(ou8_InterfaceNumber);
   C_TblTreeModelCheckableItem * const pc_ParentNode = this->m_GetDevicesModelNode(ou8_InterfaceNumber);
   QModelIndex c_Index;

   if (pc_ParentNode != NULL)
   {
      const QMap<uint8_t,
                 QMap<stw::opensyde_core::C_OscCanInterfaceId,
                      uint32_t> >::const_iterator c_ItInterface = this->mc_LookupTreeIndex.constFind(
         ou8_InterfaceNumber);
      if (c_ItInterface != this->mc_LookupTreeIndex.end())
      {
         const QMap<stw::opensyde_core::C_OscCanInterfaceId,
                    uint32_t>::const_iterator c_ItDevice = c_ItInterface.value().constFind(orc_DeviceId);
         if (c_ItInterface != this->mc_LookupTreeIndex.end())
         {
            for (uint32_t u32_ItChild = 0UL; u32_ItChild < pc_ParentNode->c_Children.size(); ++u32_ItChild)
            {
               C_TblTreeModelCheckableItem * const pc_ChildNode =
                  dynamic_cast<C_TblTreeModelCheckableItem * const>(pc_ParentNode->c_Children[u32_ItChild]);
               if (pc_ChildNode != NULL)
               {
                  if (pc_ChildNode->u32_Index == c_ItDevice.value())
                  {
                     u32_ChildIndex = u32_ItChild;
                     break;
                  }
               }
            }
         }
      }
   }
   c_Index = this->index(static_cast<int32_t>(u32_ChildIndex), 0, c_ParentIndex);
   return c_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Rechecks the tree elements for errors of current selected interface and adapt the icons

   Must be triggered from outside. Will not be called be model

   \param[in,out]  orc_InterfaceEntry     Interface entry index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::CheckError(const QModelIndex & orc_InterfaceEntry)
{
   tgl_assert(orc_InterfaceEntry.parent().isValid() == false);
   if (orc_InterfaceEntry.parent().isValid() == false)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      C_TblTreeModelCheckableItem * const pc_Item =
         static_cast<C_TblTreeModelCheckableItem *>(orc_InterfaceEntry.internalPointer());

      this->m_CheckError(orc_InterfaceEntry, pc_Item);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init interface node

   \param[in]      orc_Interface       Interface
   \param[in]      orc_Node            Node
   \param[in,out]  orc_InterfaceEntry  Interface entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitInterfaceNode(const C_OscNodeComInterfaceSettings & orc_Interface,
                                                   const C_OscNode & orc_Node,
                                                   C_TblTreeModelCheckableItem & orc_InterfaceEntry)
{
   const std::map<uint8_t,
                  C_OscCanOpenManagerInfo>::const_iterator c_ItCanOpenManager =
      orc_Node.c_CanOpenManagers.find(orc_Interface.
                                      u8_InterfaceNumber);

   this->m_InitInterfaceNodeContent(orc_Interface,
                                    c_ItCanOpenManager != orc_Node.c_CanOpenManagers.end(), orc_InterfaceEntry);
   if (c_ItCanOpenManager != orc_Node.c_CanOpenManagers.end())
   {
      C_TblTreeModelCheckableItem * const pc_DevicesEntry = new C_TblTreeModelCheckableItem();
      this->m_InitDevicesNode(orc_Interface.u8_InterfaceNumber, c_ItCanOpenManager->second, *pc_DevicesEntry);
      orc_InterfaceEntry.AddChild(pc_DevicesEntry);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init interface node content

   \param[in]      orc_Interface          Interface
   \param[in]      oq_ContainsChildren    Contains children
   \param[in,out]  orc_InterfaceEntry     Interface entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitInterfaceNodeContent(const C_OscNodeComInterfaceSettings & orc_Interface,
                                                          const bool oq_ContainsChildren,
                                                          C_TblTreeModelCheckableItem & orc_InterfaceEntry) const
{
   orc_InterfaceEntry.c_Name = static_cast<QString>(C_GtGetText::h_GetText("%1 - CANopen Manager")).arg(C_PuiSdUtil::h_GetInterfaceName(
                                                                                                           orc_Interface
                                                                                                           .
                                                                                                           e_InterfaceType,
                                                                                                           orc_Interface
                                                                                                           .
                                                                                                           u8_InterfaceNumber));
   orc_InterfaceEntry.q_CheckBoxVisible = true;
   if (oq_ContainsChildren)
   {
      orc_InterfaceEntry.e_CheckState = Qt::Checked;
   }
   else
   {
      orc_InterfaceEntry.e_CheckState = Qt::Unchecked;
   }
   orc_InterfaceEntry.q_Enabled = true;
   // Only selectable if the checkbox is checked
   orc_InterfaceEntry.q_Selectable = oq_ContainsChildren;
   orc_InterfaceEntry.c_Icon = this->mc_IconInterface;
   orc_InterfaceEntry.u32_Index = static_cast<uint32_t>(orc_Interface.u8_InterfaceNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init devices node

   \param[in]      ou8_InterfaceNumber    Interface number
   \param[in]      orc_CanOpenManager     CAN open manager
   \param[in,out]  orc_DevicesEntry       Devices entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitDevicesNode(const uint8_t ou8_InterfaceNumber,
                                                 const C_OscCanOpenManagerInfo & orc_CanOpenManager,
                                                 C_TblTreeModelCheckableItem & orc_DevicesEntry)
{
   this->m_InitDevicesNodeContent(orc_CanOpenManager, orc_DevicesEntry);
   for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
           orc_CanOpenManager.c_CanOpenDevices.cbegin();
        c_ItDevice != orc_CanOpenManager.c_CanOpenDevices.cend(); ++c_ItDevice)
   {
      C_TblTreeModelCheckableItem * const pc_DeviceEntry = new C_TblTreeModelCheckableItem();
      this->m_InitDeviceNode(ou8_InterfaceNumber, c_ItDevice->first, *pc_DeviceEntry);
      orc_DevicesEntry.AddChild(pc_DeviceEntry);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init devices node content

   \param[in]      orc_CanOpenManager  CAN open manager
   \param[in,out]  orc_DevicesEntry    Devices entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitDevicesNodeContent(const C_OscCanOpenManagerInfo & orc_CanOpenManager,
                                                        C_TblTreeModelCheckableItem & orc_DevicesEntry) const
{
   orc_DevicesEntry.c_Name =
      static_cast<QString>(C_GtGetText::h_GetText("CANopen Devices (%1)")).arg(
         orc_CanOpenManager.c_CanOpenDevices.size());
   orc_DevicesEntry.q_CheckBoxVisible = false;
   // Special case: All not selectable items shall not change the current selection of the tree
   // Must be disabled too, to avoid calling currentRowChanged
   orc_DevicesEntry.q_Enabled = false;
   orc_DevicesEntry.q_Selectable = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init device node

   \param[in]      ou8_InterfaceNumber    Interface number
   \param[in]      orc_DeviceInterfaceId  Device interface id
   \param[in,out]  orc_DeviceEntry        Device entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitDeviceNode(const uint8_t ou8_InterfaceNumber,
                                                const C_OscCanInterfaceId & orc_DeviceInterfaceId,
                                                C_TblTreeModelCheckableItem & orc_DeviceEntry)
{
   this->m_InitDeviceNodeContent(ou8_InterfaceNumber, orc_DeviceInterfaceId, orc_DeviceEntry);
   {
      C_TblTreeModelCheckableItem * const pc_ConfigurationEntry = new C_TblTreeModelCheckableItem();
      this->m_InitConfigurationNodeContent(*pc_ConfigurationEntry);
      orc_DeviceEntry.AddChild(pc_ConfigurationEntry);
   }
   {
      C_TblTreeModelCheckableItem * const pc_PdosEntry = new C_TblTreeModelCheckableItem();
      this->m_InitPdosNodeContent(*pc_PdosEntry);
      orc_DeviceEntry.AddChild(pc_PdosEntry);
   }
   {
      C_TblTreeModelCheckableItem * const pc_EdsFileEntry = new C_TblTreeModelCheckableItem();
      this->m_InitEdsFileNodeContent(*pc_EdsFileEntry);
      orc_DeviceEntry.AddChild(pc_EdsFileEntry);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init device node content

   \param[in]      ou8_InterfaceNumber    Interface number
   \param[in]      orc_DeviceInterfaceId  Device interface id
   \param[in,out]  orc_DeviceEntry        Device entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitDeviceNodeContent(const uint8_t ou8_InterfaceNumber,
                                                       const C_OscCanInterfaceId & orc_DeviceInterfaceId,
                                                       C_TblTreeModelCheckableItem & orc_DeviceEntry)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
      orc_DeviceInterfaceId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      orc_DeviceEntry.c_Name = pc_Node->c_Properties.c_Name.c_str();
   }
   orc_DeviceEntry.q_CheckBoxVisible = false;
   // Special case: All not selectable items shall not change the current selection of the tree
   // Must be disabled too, to avoid calling currentRowChanged
   orc_DeviceEntry.q_Enabled = true;
   orc_DeviceEntry.q_Selectable = true;
   orc_DeviceEntry.c_Icon = this->mc_IconNode;
   orc_DeviceEntry.u32_Index = this->m_InsertNewInterfaceIntoLookupTree(ou8_InterfaceNumber, orc_DeviceInterfaceId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init EDS file node content

   \param[in,out]  orc_EdsFileEntry    EDS file entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitEdsFileNodeContent(C_TblTreeModelCheckableItem & orc_EdsFileEntry) const
{
   orc_EdsFileEntry.c_Name = C_GtGetText::h_GetText("EDS File");
   orc_EdsFileEntry.q_CheckBoxVisible = false;
   orc_EdsFileEntry.q_Enabled = true;
   orc_EdsFileEntry.q_Selectable = true;
   orc_EdsFileEntry.c_Icon = this->mc_IconNodeItems;
   orc_EdsFileEntry.u32_Index = C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_EDS_FILE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init configuration node content

   \param[in,out]  orc_ConfigurationEntry    Configuration entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitConfigurationNodeContent(C_TblTreeModelCheckableItem & orc_ConfigurationEntry)
const
{
   orc_ConfigurationEntry.c_Name = C_GtGetText::h_GetText("Configuration");
   orc_ConfigurationEntry.q_CheckBoxVisible = false;
   orc_ConfigurationEntry.q_Enabled = true;
   orc_ConfigurationEntry.q_Selectable = true;
   orc_ConfigurationEntry.c_Icon = this->mc_IconNodeItems;
   orc_ConfigurationEntry.u32_Index = C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init PDOs node content

   \param[in,out]  orc_PdosEntry    PD os entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitPdosNodeContent(C_TblTreeModelCheckableItem & orc_PdosEntry) const
{
   orc_PdosEntry.c_Name = C_GtGetText::h_GetText("PDOs");
   orc_PdosEntry.q_CheckBoxVisible = false;
   orc_PdosEntry.q_Enabled = true;
   orc_PdosEntry.q_Selectable = true;
   orc_PdosEntry.c_Icon = this->mc_IconNodeItems;
   orc_PdosEntry.u32_Index = C_SdNdeCoConfigTreeModel::hu32_INDEX_DEVICE_USE_CASE_PDOS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init icons
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_InitIcons()
{
   mc_IconInterface = C_SdUtil::h_InitStaticIconSvg("://images/system_definition/IconBusInactive.svg", mc_ICON_SIZE_16);
   // use different colored icon for active state
   mc_IconInterface.addPixmap(QIcon("://images/system_definition/IconBus.svg").pixmap(
                                 mc_ICON_SIZE_16), QIcon::Selected, QIcon::On);
   mc_IconInterface.addPixmap(QIcon("://images/system_definition/IconBus.svg").pixmap(
                                 mc_ICON_SIZE_16), QIcon::Selected, QIcon::Off);
   mc_IconInterfaceError =
      C_SdUtil::h_InitStaticIconSvg("://images/system_definition/IconBusError.svg", mc_ICON_SIZE_16);

   mc_IconNode = C_SdUtil::h_InitStaticIconSvg("://images/system_definition/IconNodeInactive.svg", mc_ICON_SIZE_16);
   // use different colored icon for active state
   mc_IconNode.addPixmap(QIcon("://images/system_definition/IconNode.svg").pixmap(
                            mc_ICON_SIZE_16), QIcon::Selected, QIcon::On);
   mc_IconNode.addPixmap(QIcon("://images/system_definition/IconNode.svg").pixmap(
                            mc_ICON_SIZE_16), QIcon::Selected, QIcon::Off);
   mc_IconNodeError = C_SdUtil::h_InitStaticIconSvg("://images/system_definition/IconNodeError.svg", mc_ICON_SIZE_16);

   mc_IconNodeItems = C_SdUtil::h_InitStaticIconSvg("://images/system_definition/NodeEdit/halc/OtherSmallInactive.svg",
                                                    mc_ICON_SIZE_16);
   // use different colored icon for active state
   mc_IconNodeItems.addPixmap(QIcon("://images/system_definition/NodeEdit/halc/OtherSmallActive.svg").pixmap(
                                 mc_ICON_SIZE_16), QIcon::Selected, QIcon::On);
   mc_IconNodeItems.addPixmap(QIcon("://images/system_definition/NodeEdit/halc/OtherSmallActive.svg").pixmap(
                                 mc_ICON_SIZE_16), QIcon::Selected, QIcon::Off);
   mc_IconNodeItemsError = C_SdUtil::h_InitStaticIconSvg(
      "://images/system_definition/NodeEdit/halc/OtherSmallError.svg", mc_ICON_SIZE_16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check interface

   \param[in]      orc_Index           Index
   \param[in,out]  orc_InterfaceItem   Interface item

   \return
   Flags

   \retval   True    Action executed
   \retval   False   Action aborted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoConfigTreeModel::m_CheckInterface(const QModelIndex & orc_Index,
                                                C_TblTreeModelCheckableItem & orc_InterfaceItem)
{
   bool q_Retval = true;

   if (orc_InterfaceItem.e_CheckState == Qt::Checked)
   {
      C_OgeWiCustomMessage c_Message(this->mpc_Parent, C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_Output;
      c_Message.SetHeading(C_GtGetText::h_GetText("Deactivate CANopen manager"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Do you really want to deactivate the CANopen manager?\n"
                                  "Any configuration for this manager will be permanently discarded."));
      e_Output = c_Message.Execute();
      if (e_Output == C_OgeWiCustomMessage::eYES)
      {
         this->m_HandleUncheckInterfaceAction(orc_Index, orc_InterfaceItem);

         Q_EMIT (this->SigManagerUnchecked(orc_Index));

         // Only selectable if the checkbox is checked
         orc_InterfaceItem.q_Selectable = false;
      }
      else
      {
         q_Retval = false;
      }
   }
   else
   {
      // check if manager is the only one on this bus
      const bool q_CoManagerCanBeActivated = m_CheckIfCoManagerCanBeActivated(this->mu32_NodeIndex,
                                                                              orc_InterfaceItem.u32_Index);

      if (q_CoManagerCanBeActivated == true)
      {
         // Manager interface is active, selectable again
         orc_InterfaceItem.q_Selectable = true;

         this->m_HandleCheckInterfaceAction(orc_Index, orc_InterfaceItem);

         Q_EMIT (this->SigSelectManager(orc_Index));
      }
      else
      {
         q_Retval = false;
      }
   }

   Q_EMIT (this->SigErrorChange());

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle check interface action

   \param[in]      orc_Index           Index
   \param[in,out]  orc_InterfaceItem   Interface item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_HandleCheckInterfaceAction(const QModelIndex & orc_Index,
                                                            C_TblTreeModelCheckableItem & orc_InterfaceItem)
{
   const C_OscCanOpenManagerInfo c_NewManager;
   bool q_DatapoolChanged;

   C_TblTreeModelCheckableItem * const pc_DeviceEntry = new C_TblTreeModelCheckableItem();

   this->beginInsertRows(orc_Index, 0, 0);

   this->m_InitDevicesNodeContent(c_NewManager, *pc_DeviceEntry);
   orc_InterfaceItem.AddChild(pc_DeviceEntry);
   tgl_assert(C_PuiSdHandler::h_GetInstance()->AddCanOpenManager(this->mu32_NodeIndex,
                                                                 static_cast<uint8_t>(orc_InterfaceItem.u32_Index),
                                                                 c_NewManager, q_DatapoolChanged) == C_NO_ERR);

   // Update COMM Datapools due to own error check in Datapool view and a new Datapool could be created
   Q_EMIT (this->SigCommDatapoolsChanged());
   this->endInsertRows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle uncheck interface action

   \param[in]      orc_Index           Index
   \param[in,out]  orc_InterfaceItem   Interface item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_HandleUncheckInterfaceAction(const QModelIndex & orc_Index,
                                                              C_TblTreeModelCheckableItem & orc_InterfaceItem)
{
   bool q_DatapoolChanged;

   this->beginRemoveRows(orc_Index, 0, 0);
   orc_InterfaceItem.ClearChildren();
   tgl_assert(C_PuiSdHandler::h_GetInstance()->DeleteCanOpenManager(this->mu32_NodeIndex,
                                                                    static_cast<uint8_t>(orc_InterfaceItem.u32_Index),
                                                                    true, q_DatapoolChanged) ==
              C_NO_ERR);

   // Update COMM Datapools due to own error check in Datapool view and a new Datapool could be created
   Q_EMIT (this->SigCommDatapoolsChanged());

   this->endRemoveRows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert new interface into lookup tree

   \param[in]  ou8_InterfaceNumber     Interface number
   \param[in]  orc_DeviceInterfaceId   Device interface id

   \return
   Index of tree item
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeCoConfigTreeModel::m_InsertNewInterfaceIntoLookupTree(const uint8_t ou8_InterfaceNumber,
                                                                      const C_OscCanInterfaceId & orc_DeviceInterfaceId)
{
   uint32_t u32_Retval = 0UL;

   const QMap<uint8_t,
              QMap<stw::opensyde_core::C_OscCanInterfaceId,
                   uint32_t> >::iterator c_ItManager =
      this->mc_LookupTreeIndex.find(ou8_InterfaceNumber);

   if (c_ItManager != this->mc_LookupTreeIndex.end())
   {
      const QMap<stw::opensyde_core::C_OscCanInterfaceId,
                 uint32_t>::iterator c_ItInternalMap = c_ItManager.value().find(
         orc_DeviceInterfaceId);
      if (c_ItInternalMap != c_ItManager.value().end())
      {
         //Exists
         u32_Retval = c_ItInternalMap.value();
      }
      else
      {
         //New
         const uint32_t u32_NewValue = C_SdNdeCoConfigTreeModel::mh_SearchNewUniqueValue(c_ItManager.value());
         //Insert and store
         u32_Retval = u32_NewValue;
         c_ItManager.value().insert(orc_DeviceInterfaceId, u32_Retval);
      }
   }
   else
   {
      //New
      QMap<stw::opensyde_core::C_OscCanInterfaceId,
           uint32_t> c_InternalMap;
      c_InternalMap.insert(orc_DeviceInterfaceId, 0UL);
      this->mc_LookupTreeIndex.insert(ou8_InterfaceNumber, c_InternalMap);
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Search new unique value

   \param[in]  orc_Map  Map

   \return
   New unique value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeCoConfigTreeModel::mh_SearchNewUniqueValue(const QMap<C_OscCanInterfaceId, uint32_t> & orc_Map)
{
   uint32_t u32_NewValue = 0UL;
   bool q_Found;

   //Search new unique value
   do
   {
      q_Found = false;
      ++u32_NewValue;
      for (QMap<stw::opensyde_core::C_OscCanInterfaceId,
                uint32_t>::const_iterator c_ItInternalMapNew = orc_Map.cbegin();
           (c_ItInternalMapNew != orc_Map.cend()) && (q_Found == false); ++c_ItInternalMapNew)
      {
         if (c_ItInternalMapNew.value() == u32_NewValue)
         {
            q_Found = true;
         }
      }
   }
   while (q_Found == true);
   return u32_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get expected pos in tree

   \param[in]  ou8_InterfaceNumber     Interface number
   \param[in]  orc_DeviceInterfaceId   Device interface id

   \return
   Expected pos in tree
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeCoConfigTreeModel::m_GetExpectedPosInTree(const uint8_t ou8_InterfaceNumber,
                                                          const C_OscCanInterfaceId & orc_DeviceInterfaceId)
{
   uint32_t u32_Retval = 0UL;

   const QMap<uint8_t,
              QMap<stw::opensyde_core::C_OscCanInterfaceId,
                   uint32_t> >::iterator c_ItManager =
      this->mc_LookupTreeIndex.find(ou8_InterfaceNumber);

   if (c_ItManager != this->mc_LookupTreeIndex.end())
   {
      const QMap<stw::opensyde_core::C_OscCanInterfaceId,
                 uint32_t>::iterator c_ItInternalMap = c_ItManager.value().find(
         orc_DeviceInterfaceId);
      if (c_ItInternalMap != c_ItManager.value().end())
      {
         for (QMap<stw::opensyde_core::C_OscCanInterfaceId,
                   uint32_t>::const_iterator c_ItInternalMapNew = c_ItManager.value().cbegin();
              c_ItInternalMapNew != c_ItManager.value().cend(); ++c_ItInternalMapNew)
         {
            if (c_ItInternalMapNew.key() == orc_DeviceInterfaceId)
            {
               break;
            }
            else
            {
               ++u32_Retval;
            }
         }
      }
      else
      {
         //Should not happen
         tgl_assert(false);
      }
   }
   else
   {
      //Should not happen
      tgl_assert(false);
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Retrieve interface from lookup tree

   \param[in]      ou8_InterfaceNumber    Interface number
   \param[in]      ou32_TreeNodeIndex     Tree node index
   \param[in,out]  orc_DeviceInterfaceId  Device interface ID

   \return
   STW error codes

   \retval   C_NO_ERR   Detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoConfigTreeModel::m_RetrieveInterfaceFromLookupTree(const uint8_t ou8_InterfaceNumber,
                                                                    const uint32_t ou32_TreeNodeIndex,
                                                                    C_OscCanInterfaceId & orc_DeviceInterfaceId)
const
{
   int32_t s32_Retval = C_NO_ERR;

   const QMap<uint8_t,
              QMap<stw::opensyde_core::C_OscCanInterfaceId,
                   uint32_t> >::const_iterator c_ItManager =
      this->mc_LookupTreeIndex.find(ou8_InterfaceNumber);

   if (c_ItManager != this->mc_LookupTreeIndex.end())
   {
      const QMap<stw::opensyde_core::C_OscCanInterfaceId,
                 uint32_t> & rc_InternalMap = c_ItManager.value();
      bool q_Found = false;
      for (QMap<stw::opensyde_core::C_OscCanInterfaceId,
                uint32_t>::const_iterator c_ItInternalMap = rc_InternalMap.begin();
           c_ItInternalMap != rc_InternalMap.end(); ++c_ItInternalMap)
      {
         if (c_ItInternalMap.value() == ou32_TreeNodeIndex)
         {
            q_Found = true;
            orc_DeviceInterfaceId = c_ItInternalMap.key();
         }
      }
      if (q_Found == false)
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interface model node

   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   Model node
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreeModelCheckableItem * C_SdNdeCoConfigTreeModel::m_GetInterfaceModelNode(const uint8_t ou8_InterfaceNumber) const
{
   C_TblTreeModelCheckableItem * pc_Retval = NULL;

   if (this->mpc_InvisibleRootItem != NULL)
   {
      for (uint32_t u32_ItChild = 0UL; u32_ItChild < this->mpc_InvisibleRootItem->c_Children.size(); ++u32_ItChild)
      {
         C_TblTreeModelCheckableItem * const pc_Node =
            dynamic_cast<C_TblTreeModelCheckableItem * const>(this->mpc_InvisibleRootItem->c_Children[u32_ItChild]);
         if (pc_Node->u32_Index == static_cast<uint32_t>(ou8_InterfaceNumber))
         {
            pc_Retval = pc_Node;
            break;
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get devices model node

   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   Model node
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreeModelCheckableItem * C_SdNdeCoConfigTreeModel::m_GetDevicesModelNode(const uint8_t ou8_InterfaceNumber) const
{
   C_TblTreeModelCheckableItem * pc_Retval = NULL;
   C_TblTreeModelCheckableItem * const pc_ParentNode = this->m_GetInterfaceModelNode(ou8_InterfaceNumber);

   if (pc_ParentNode != NULL)
   {
      if (pc_ParentNode->c_Children.size() > 0UL)
      {
         pc_Retval = dynamic_cast<C_TblTreeModelCheckableItem *>(pc_ParentNode->c_Children[0UL]);
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device model node

   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device id

   \return
   Model node
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreeModelCheckableItem * C_SdNdeCoConfigTreeModel::m_GetDeviceModelNode(const uint8_t ou8_InterfaceNumber,
                                                                             const C_OscCanInterfaceId & orc_DeviceId)
{
   C_TblTreeModelCheckableItem * pc_Retval = NULL;
   C_TblTreeModelCheckableItem * const pc_ParentNode = this->m_GetDevicesModelNode(ou8_InterfaceNumber);

   if (pc_ParentNode != NULL)
   {
      const QMap<uint8_t,
                 QMap<stw::opensyde_core::C_OscCanInterfaceId,
                      uint32_t> >::ConstIterator c_ItTop = this->mc_LookupTreeIndex.constFind(
         ou8_InterfaceNumber);
      if (c_ItTop != this->mc_LookupTreeIndex.cend())
      {
         const QMap<stw::opensyde_core::C_OscCanInterfaceId,
                    uint32_t>::ConstIterator c_ItDevice = c_ItTop.value().constFind(
            orc_DeviceId);
         if (c_ItDevice != c_ItTop.value().cend())
         {
            for (uint32_t u32_ItChild = 0UL; u32_ItChild < pc_ParentNode->c_Children.size(); ++u32_ItChild)
            {
               C_TblTreeModelCheckableItem * const pc_ChildNode =
                  dynamic_cast<C_TblTreeModelCheckableItem * const>(pc_ParentNode->c_Children[u32_ItChild]);
               if (pc_ChildNode != NULL)
               {
                  if (pc_ChildNode->u32_Index == c_ItDevice.value())
                  {
                     pc_Retval = pc_ChildNode;
                     break;
                  }
               }
            }
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger update of device count

   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_TriggerUpdateDeviceCount(const uint8_t ou8_InterfaceNumber)
{
   C_TblTreeModelCheckableItem * const pc_AllDevices = this->m_GetDevicesModelNode(ou8_InterfaceNumber);

   if (pc_AllDevices != NULL)
   {
      const C_OscCanOpenManagerInfo * const pc_ManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
         this->mu32_NodeIndex, ou8_InterfaceNumber);
      if (pc_ManagerInfo != NULL)
      {
         const QModelIndex c_Index = this->GetDevicesModelIndex(ou8_InterfaceNumber);
         m_InitDevicesNodeContent(*pc_ManagerInfo, *pc_AllDevices);
         Q_EMIT (this->dataChanged(c_Index, c_Index,
                                   QVector<int32_t>() << static_cast<int32_t>(Qt::DisplayRole)));
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks for errors of an specific interface

   \param[in,out]  opc_InterfaceEntry     Interface entry item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_CheckError(C_TblTreeModelCheckableItem * const opc_InterfaceEntry)
{
   const uint8_t u8_InterfaceNumber = static_cast<uint8_t>(opc_InterfaceEntry->u32_Index);
   const QModelIndex c_InterfaceIndex = this->GetInterfaceModelIndex(u8_InterfaceNumber);

   if (c_InterfaceIndex.isValid() == true)
   {
      this->m_CheckError(c_InterfaceIndex, opc_InterfaceEntry);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks for errors of an specific interface

   \param[in,out]  opc_InterfaceEntry     Interface entry item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeModel::m_CheckError(const QModelIndex & orc_InterfaceIndex,
                                            C_TblTreeModelCheckableItem * const opc_InterfaceEntry)
{
   tgl_assert(opc_InterfaceEntry != NULL);
   if (opc_InterfaceEntry != NULL)
   {
      const uint8_t u8_InterfaceNumber = static_cast<uint8_t>(opc_InterfaceEntry->u32_Index);
      const C_OscCanOpenManagerInfo * const pc_Manager = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
         this->mu32_NodeIndex, u8_InterfaceNumber);
      bool q_ManagerErrorDetected = false;
      bool q_AtLeastOneDevice = false;
      C_OscCanInterfaceId c_LastDeviceId;
      QModelIndex c_LastElementIndex;

      // Null pointer is in this case not an error. This is the scenario if an interface is unchecked. So the icon
      // of the interface element must be updated too
      if (pc_Manager != NULL)
      {
         // Manager layer
         // Check only for the manager relevant error checks. All errors related to devices will be checked
         // in the next step to assign possible errors exactly to the relevant devices
         pc_Manager->CheckErrorManager(NULL, &q_ManagerErrorDetected, NULL, NULL, false);

         // Devices layer
         if (opc_InterfaceEntry->c_Children.size() == 1)
         {
            uint32_t u32_DeviceCounter;

            for (u32_DeviceCounter = 0U; u32_DeviceCounter < opc_InterfaceEntry->c_Children[0]->c_Children.size();
                 ++u32_DeviceCounter)
            {
               // Layer for each device
               C_TblTreeModelCheckableItem * const pc_DeviceItem = dynamic_cast<C_TblTreeModelCheckableItem *>(
                  opc_InterfaceEntry->c_Children[0]->c_Children[u32_DeviceCounter]);

               tgl_assert(pc_DeviceItem != NULL);
               if (pc_DeviceItem != NULL)
               {
                  C_OscCanInterfaceId c_DeviceId;
                  bool q_DeviceErrorDetected = false;
                  // Get the device Id for the specific Device
                  int32_t s32_Retval = this->m_RetrieveInterfaceFromLookupTree(u8_InterfaceNumber,
                                                                               pc_DeviceItem->u32_Index,
                                                                               c_DeviceId);

                  tgl_assert(s32_Retval == C_NO_ERR);
                  if (s32_Retval == C_NO_ERR)
                  {
                     // Check for any error of the device
                     bool q_NodeIdInvalid;
                     bool q_NodeIdConflict;

                     // Device configuration errors
                     s32_Retval = pc_Manager->CheckErrorDeviceCoNodeId(c_DeviceId, &q_NodeIdConflict,
                                                                       &q_NodeIdInvalid, true);
                     tgl_assert(s32_Retval == C_NO_ERR);
                     if (s32_Retval == C_NO_ERR)
                     {
                        bool q_HeartbeatInvalid;

                        s32_Retval = pc_Manager->CheckErrorDeviceHeartbeat(c_DeviceId, &q_HeartbeatInvalid);
                        tgl_assert(s32_Retval == C_NO_ERR);
                        if (s32_Retval == C_NO_ERR)
                        {
                           q_DeviceErrorDetected = (q_NodeIdInvalid == true) ||
                                                   (q_NodeIdConflict == true) ||
                                                   (q_HeartbeatInvalid == true);

                           // Get the device configuration entry
                           uint32_t u32_ChildCounter;

                           for (u32_ChildCounter = 0U; u32_ChildCounter < pc_DeviceItem->c_Children.size();
                                ++u32_ChildCounter)
                           {
                              C_TblTreeModelCheckableItem * const pc_SubItem = dynamic_cast<
                                 C_TblTreeModelCheckableItem *>(pc_DeviceItem->c_Children[u32_ChildCounter]);

                              tgl_assert(pc_SubItem != NULL);
                              if ((pc_SubItem != NULL) &&
                                  (pc_SubItem->u32_Index == hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION))
                              {
                                 if (q_DeviceErrorDetected == true)
                                 {
                                    pc_SubItem->c_Icon = this->mc_IconNodeItemsError;
                                 }
                                 else
                                 {
                                    pc_SubItem->c_Icon = this->mc_IconNodeItems;
                                 }
                                 break;
                              }
                           }
                           // Save the device id for sending the dataChanged signal later
                           q_AtLeastOneDevice = true;
                           c_LastDeviceId = c_DeviceId;
                        }
                     }
                  }

                  // Update of icon of device layer
                  if (q_DeviceErrorDetected == true)
                  {
                     pc_DeviceItem->c_Icon = this->mc_IconNodeError;
                  }
                  else
                  {
                     pc_DeviceItem->c_Icon = this->mc_IconNode;
                  }

                  // With at least one device error, the manager will be marked with an error too
                  //lint -e{514}  Using operator with a bool value was intended and is no accident
                  q_ManagerErrorDetected |= q_DeviceErrorDetected;
               }
            }
         }
      }

      // Update of icon of manager layer
      if (q_ManagerErrorDetected == true)
      {
         opc_InterfaceEntry->c_Icon = this->mc_IconInterfaceError;
      }
      else
      {
         opc_InterfaceEntry->c_Icon = this->mc_IconInterface;
      }

      if (q_AtLeastOneDevice == true)
      {
         // Only configuration has an error state for now
         c_LastElementIndex = GetDeviceUseCaseModelIndex(u8_InterfaceNumber, c_LastDeviceId,
                                                         hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION);
      }
      else
      {
         // Only manager available
         c_LastElementIndex = orc_InterfaceIndex;
      }

      Q_EMIT (dataChanged(orc_InterfaceIndex, c_LastElementIndex,
                          QVector<int32_t>() << static_cast<int32_t>(Qt::DecorationRole)));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function to check if CANopen Manager can be activated for current interface of node on a bus.

   Only one CANopen Manager per bus is permitted, so there are three possible scenarios to be checked for:
   1. CANopen Manager from given node is already activated on another interface connected to the same bus.
   2. CANopen Manager from another node is already activated on the same bus.
   3. The given interface of node is not connected to any bus.

   Scenario 3 is checked here, so that we do not have to do any checks on topology level when assigning an interface to
   a bus. For further details see https://redmine.sensor-technik.de/issues/78408.

   \param[in]       ou32_NodeIndex        current node index
   \param[out]      ou32_InterfaceIndex   current interface index of given node index

   \retval   true             Everything fine, CANopen Manager can be activated on this interface of given node
   \retval   false            CANopen Manager cannot be activated
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoConfigTreeModel::m_CheckIfCoManagerCanBeActivated(const uint32_t ou32_NodeIndex,
                                                                const uint32_t ou32_InterfaceIndex) const
{
   bool q_CoManagerCanBeActivated = true;

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   const C_OscNodeComInterfaceSettings c_ComInterface = pc_Node->c_Properties.c_ComInterfaces[ou32_InterfaceIndex];

   // scenario 3
   if (c_ComInterface.GetBusConnected() == false)
   {
      q_CoManagerCanBeActivated = false;
      C_OgeWiCustomMessage c_Message(this->mpc_Parent, C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Activate CANopen Manager"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "The CANopen Manager can't be activated for this interface because it is not connected to any bus."));
      c_Message.Execute();
   }

   if ((c_ComInterface.GetBusConnected() == true) &&
       (c_ComInterface.e_InterfaceType == C_OscSystemBus::eCAN))
   {
      const C_OscNode * const pc_ManagerNodeToCheck =
         C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerNodeOnBus(c_ComInterface.u32_BusIndex);

      // scenario 1 and 2
      if (pc_ManagerNodeToCheck != NULL)
      {
         q_CoManagerCanBeActivated = false;

         uint32_t u32_ManagerNodeIndex;
         uint8_t u8_ManagerIntfNumber;

         C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerNodeOnBus(c_ComInterface.u32_BusIndex, u32_ManagerNodeIndex,
                                                                     &u8_ManagerIntfNumber);

         const QString c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(c_ComInterface.e_InterfaceType,
                                                                         u8_ManagerIntfNumber);

         const stw::scl::C_SclString c_Text = "The CANopen Manager can't be activated for this interface because there " \
                                              "already exists a CANopen Manager on this bus with node \"" +
                                              pc_ManagerNodeToCheck->c_Properties.c_Name +
                                              "\" at interface \"" + c_InterfaceName.toStdString() + "\".";

         C_OgeWiCustomMessage c_Message(this->mpc_Parent, C_OgeWiCustomMessage::eINFORMATION);
         c_Message.SetHeading(C_GtGetText::h_GetText("Activate CANopen Manager"));
         c_Message.SetDescription(C_GtGetText::h_GetText(c_Text.c_str()));
         c_Message.Execute();
      }
   }

   return q_CoManagerCanBeActivated;
}
