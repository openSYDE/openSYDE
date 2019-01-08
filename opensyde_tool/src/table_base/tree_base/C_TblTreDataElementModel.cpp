//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tree model for data elements (implementation)

   Tree model for data elements

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "constants.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SyvRoRouteCalculation.h"
#include "C_TblTreDataElementModel.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_TblTreDataElementModel::mhc_IconNode = ":/images/system_definition/IconNode.svg";
const QString C_TblTreDataElementModel::mhc_IconDatapool = ":/images/system_definition/IconDataPoolSmall.svg";
const QString C_TblTreDataElementModel::mhc_IconList = ":/images/system_definition/IconDatapoolList.svg";
const QString C_TblTreDataElementModel::mhc_IconVariable = ":/images/system_definition/IconVariable.svg";
const QString C_TblTreDataElementModel::mhc_IconParameter = ":/images/system_definition/IconParameter.svg";
const QString C_TblTreDataElementModel::mhc_IconSignal = ":/images/system_definition/IconSignal.svg";
const QString C_TblTreDataElementModel::mhc_IconEthernet = ":/images/system_definition/IconBus.svg";
const QString C_TblTreDataElementModel::mhc_IconCAN = ":/images/system_definition/IconBus.svg";
const QString C_TblTreDataElementModel::mhc_IconMessage = ":/images/system_definition/IconMessage.svg";
const QString C_TblTreDataElementModel::mhc_AdditionalDataPoolInfo = " (Already assigned)";
const QString C_TblTreDataElementModel::mhc_AdditionalWriteOnlyInfo = " (Not supported, read only)";
const QString C_TblTreDataElementModel::mhc_AdditionalArrayInfo = " (Not supported, array or string type)";
const QString C_TblTreDataElementModel::mhc_Additional64BitInfo = " (Not supported, 64 bit value)";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblTreDataElementModel::C_TblTreDataElementModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   me_Mode(eDATAPOOL_ELEMENT)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the active node index

   \param[in] ou32_NodeIndex               Active node index
   \param[in] os32_SkipApplicationIndex    Application index to not display as used
   \param[in] orc_UsedDataPoolIndicesIndex Data pools to always display as used

   \created     01.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementModel::InitSD(const uint32 ou32_NodeIndex, const sint32 os32_SkipApplicationIndex,
                                      const std::vector<uint32> & orc_UsedDataPoolIndicesIndex)
{
   C_TblTreItem * const pc_NodeItem = new C_TblTreItem();
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   bool q_NodeValid = false;

   this->beginResetModel();
   this->me_Mode = C_TblTreDataElementModel::eDATAPOOLS;
   //Clear
   for (std::vector<C_TblTreItem *>::const_iterator c_It = this->mc_InvisibleRootItem.c_Children.begin();
        c_It != this->mc_InvisibleRootItem.c_Children.end(); ++c_It)
   {
      delete (*c_It);
   }
   this->mc_InvisibleRootItem.c_Children.clear();
   //Init

   //Static
   pc_NodeItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconNode);
   //Node
   if (pc_Node != NULL)
   {
      //Data pool type nodes
      C_TblTreItem * const pc_DiagItem = new C_TblTreItem();
      C_TblTreItem * const pc_NvmItem = new C_TblTreItem();
      C_TblTreItem * const pc_ComItem = new C_TblTreItem();
      bool q_DataPoolDiagValid = false;
      bool q_DataPoolNvmValid = false;
      bool q_DataPoolComValid = false;
      pc_DiagItem->c_Name = C_GtGetText::h_GetText("DIAG Datapools");
      pc_NvmItem->c_Name = C_GtGetText::h_GetText("NVM Datapools");
      pc_ComItem->c_Name = C_GtGetText::h_GetText("COMM Datapools");
      pc_DiagItem->c_ToolTipHeading = C_GtGetText::h_GetText("DIAG Datapools");
      pc_NvmItem->c_ToolTipHeading = C_GtGetText::h_GetText("NVM Datapools");
      pc_ComItem->c_ToolTipHeading = C_GtGetText::h_GetText("COMM Datapools");
      pc_DiagItem->q_Selectable = false;
      pc_NvmItem->q_Selectable = false;
      pc_ComItem->q_Selectable = false;
      //Use some unique index for expansion restoration mechanism
      pc_DiagItem->u32_Index = 0;
      pc_NvmItem->u32_Index = 1;
      pc_ComItem->u32_Index = 2;
      //Init current node
      pc_NodeItem->u32_Index = ou32_NodeIndex;
      pc_NodeItem->q_Selectable = false;
      pc_NodeItem->c_Name = pc_Node->c_Properties.c_Name.c_str();
      pc_NodeItem->c_ToolTipHeading = pc_Node->c_Properties.c_Name.c_str();
      pc_NodeItem->c_ToolTipContent = pc_Node->c_Properties.c_Comment.c_str();
      //Data pools
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         C_TblTreItem * const pc_DataPoolItem = new C_TblTreItem();
         //Init current node
         pc_DataPoolItem->u32_Index = u32_ItDataPool;
         pc_DataPoolItem->c_Name = rc_DataPool.c_Name.c_str();
         pc_DataPoolItem->c_ToolTipHeading = rc_DataPool.c_Name.c_str();
         pc_DataPoolItem->c_ToolTipContent = rc_DataPool.c_Comment.c_str();
         pc_DataPoolItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconDatapool);
         if ((rc_DataPool.s32_RelatedDataBlockIndex < 0) ||
             ((os32_SkipApplicationIndex >= 0) && (rc_DataPool.s32_RelatedDataBlockIndex == os32_SkipApplicationIndex)))
         {
            bool q_Found = false;
            for (uint32 u32_It = 0UL; u32_It < orc_UsedDataPoolIndicesIndex.size(); ++u32_It)
            {
               if (u32_ItDataPool == orc_UsedDataPoolIndicesIndex[u32_It])
               {
                  q_Found = true;
               }
            }
            if (q_Found == false)
            {
               pc_DataPoolItem->q_Selectable = true;
            }
            else
            {
               pc_DataPoolItem->q_Enabled = false;
               pc_DataPoolItem->q_Selectable = false;
               pc_DataPoolItem->c_Name += C_TblTreDataElementModel::mhc_AdditionalDataPoolInfo;
            }
         }
         else
         {
            pc_DataPoolItem->q_Enabled = false;
            pc_DataPoolItem->q_Selectable = false;
            pc_DataPoolItem->c_Name += C_TblTreDataElementModel::mhc_AdditionalDataPoolInfo;
         }
         //Flags
         q_NodeValid = true;
         switch (rc_DataPool.e_Type)
         {
         case C_OSCNodeDataPool::eDIAG:
            q_DataPoolDiagValid = true;
            pc_DiagItem->AddChild(pc_DataPoolItem);
            break;
         case C_OSCNodeDataPool::eNVM:
            q_DataPoolNvmValid = true;
            pc_NvmItem->AddChild(pc_DataPoolItem);
            break;
         case C_OSCNodeDataPool::eCOM:
            q_DataPoolComValid = true;
            pc_ComItem->AddChild(pc_DataPoolItem);
            break;
         default:
            delete (pc_DataPoolItem);
            break;
         }
      }
      if (q_DataPoolDiagValid == true)
      {
         pc_NodeItem->AddChild(pc_DiagItem);
      }
      else
      {
         delete (pc_DiagItem);
      }
      if (q_DataPoolNvmValid == true)
      {
         pc_NodeItem->AddChild(pc_NvmItem);
      }
      else
      {
         delete (pc_NvmItem);
      }
      if (q_DataPoolComValid == true)
      {
         pc_NodeItem->AddChild(pc_ComItem);
      }
      else
      {
         delete (pc_ComItem);
      }
   }
   if (q_NodeValid == true)
   {
      this->mc_InvisibleRootItem.AddChild(pc_NodeItem);
   }
   else
   {
      delete (pc_NodeItem);
   }
   this->endResetModel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize tree structure

   \param[in] ou32_ViewIndex           View index
   \param[in] oe_Mode                  Item mode
   \param[in] oq_ShowOnlyWriteElements Optional flag to show only writable elements
   \param[in] oq_ShowArrayElements     Optional flag to hide all array elements (if false)
   \param[in] oq_Show64BitValues       Optional flag to hide all 64 bit elements (if false)

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementModel::InitSV(const uint32 ou32_ViewIndex, const E_Mode oe_Mode,
                                      const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                      const bool oq_Show64BitValues)
{
   this->me_Mode = oe_Mode;
   this->beginResetModel();
   //Clear
   for (std::vector<C_TblTreItem *>::const_iterator c_It = this->mc_InvisibleRootItem.c_Children.begin();
        c_It != this->mc_InvisibleRootItem.c_Children.end(); ++c_It)
   {
      delete (*c_It);
   }
   this->mc_InvisibleRootItem.c_Children.clear();
   //Init
   switch (this->me_Mode)
   {
   case eDATAPOOL_ELEMENT:
      m_InitDatapoolElement(ou32_ViewIndex, oq_ShowOnlyWriteElements, oq_ShowArrayElements, oq_Show64BitValues);
      break;
   case eBUS_SIGNAL:
      m_InitBusSignal(ou32_ViewIndex, oq_ShowOnlyWriteElements, oq_ShowArrayElements, oq_Show64BitValues);
      break;
   case eNVM_LIST:
      m_InitNvmList(ou32_ViewIndex);
      break;
   default:
      //NO SV use case
      tgl_assert(false);
   }
   this->endResetModel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data elements

   \param[in] orc_Index Index

   \return
   Data elements

   \created     06.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::GetDataElements(const QModelIndex & orc_Index)
const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   if (orc_Index.isValid() == true)
   {
      switch (this->me_Mode)
      {
      case eDATAPOOLS:
         c_Retval = m_GetDatapools(orc_Index);
         break;
      case eDATAPOOL_ELEMENT:
         c_Retval = m_GetDatapoolElements(orc_Index);
         break;
      case eBUS_SIGNAL:
         c_Retval = m_GetBusSignals(orc_Index);
         break;
      case eNVM_LIST:
         c_Retval = m_GetNvmList(orc_Index);
         break;
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get tree column count

   \param[in] orc_Parent Parent

   \return
   Column count

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_TblTreDataElementModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert generic item representation to model index

   \param[in] orc_ItemIndices Generic item representation

   \return
   Model index

   \created     19.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QModelIndex C_TblTreDataElementModel::GetIndexForItem(const std::vector<uint32> & orc_ItemIndices) const
{
   QModelIndex c_Retval;

   if (orc_ItemIndices.size() > 0UL)
   {
      QModelIndex c_PreviousParent;
      uint32 u32_Counter = 0UL;
      bool q_FoundSomething;

      do
      {
         q_FoundSomething = false;
         for (sintn sn_ItChild = 0; sn_ItChild < this->rowCount(c_PreviousParent); ++sn_ItChild)
         {
            const sintn sn_Column = 0;
            const QModelIndex c_Tmp = this->index(sn_ItChild, sn_Column, c_PreviousParent);
            //lint -e{925}  Result of Qt interface restrictions, set by index function
            const C_TblTreItem * const pc_TreeItem =
               static_cast<const C_TblTreItem * const>(c_Tmp.internalPointer());
            if (pc_TreeItem != NULL)
            {
               if (pc_TreeItem->u32_Index == orc_ItemIndices[u32_Counter])
               {
                  c_PreviousParent = c_Tmp;
                  ++u32_Counter;
                  q_FoundSomething = true;
                  if (u32_Counter == orc_ItemIndices.size())
                  {
                     c_Retval = c_Tmp;
                  }
                  //Stop otherwise the counter is invalid and we already have found the correct element
                  break;
               }
            }
         }
      }
      while ((u32_Counter < orc_ItemIndices.size()) && (q_FoundSomething == true));
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert model index to generic item representation

   \param[in] orc_ItemIndex Model index

   \return
   Generic item representation

   \created     19.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_TblTreDataElementModel::GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex)
const
{
   std::vector<uint32> c_Retval;
   QModelIndex c_CurItem = orc_ItemIndex;
   while (c_CurItem.isValid() == true)
   {
      //lint -e{925}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem = static_cast<const C_TblTreItem * const>(c_CurItem.internalPointer());
      if (pc_TreeItem != NULL)
      {
         c_Retval.insert(c_Retval.begin(), pc_TreeItem->u32_Index);
         c_CurItem = c_CurItem.parent();
      }
      else
      {
         break;
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Translate signal index to data pool index

   WARNING: Only works if message contains at least one signal

   \param[in] orc_Indices      Message identification indices
   \param[in] ou32_SignalIndex Signal index

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId C_TblTreDataElementModel::mh_Translate(
   const C_OSCCanMessageIdentificationIndices & orc_Indices, const uint32 ou32_SignalIndex)
{
   C_PuiSvDbNodeDataPoolListElementId c_Retval(0, 0, 0, 0, C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                               false);
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPool(
      orc_Indices.u32_NodeIndex, orc_Indices.e_ComProtocol);

   if (pc_DataPool != NULL)
   {
      uint32 u32_ListIndex;
      if (C_OSCCanProtocol::h_GetComListIndex(*pc_DataPool, orc_Indices.u32_InterfaceIndex, orc_Indices.q_MessageIsTx,
                                              u32_ListIndex) == C_NO_ERR)
      {
         const C_OSCCanMessageContainer * const pc_Container =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_Indices.u32_NodeIndex,
                                                                            orc_Indices.e_ComProtocol,
                                                                            orc_Indices.u32_InterfaceIndex);
         const C_OSCCanProtocol * const pc_Protocol = C_PuiSdHandler::h_GetInstance()->GetCanProtocol(
            orc_Indices.u32_NodeIndex, orc_Indices.e_ComProtocol);
         if ((pc_Container != NULL) && (pc_Protocol != NULL))
         {
            const uint32 u32_SignalDataStartIndex = pc_Container->GetMessageSignalDataStartIndex(
               orc_Indices.q_MessageIsTx,
               orc_Indices.u32_MessageIndex);
            c_Retval = C_PuiSvDbNodeDataPoolListElementId(orc_Indices.u32_NodeIndex, pc_Protocol->u32_DataPoolIndex,
                                                          u32_ListIndex, u32_SignalDataStartIndex + ou32_SignalIndex,
                                                          C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL);
         }
      }
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize tree structure for bus signals

   \param[in] ou32_ViewIndex           View index
   \param[in] oq_ShowOnlyWriteElements Optional flag to show only writable elements
   \param[in] oq_ShowArrayElements     Optional flag to hide all array elements (if false)
   \param[in] oq_Show64BitValues       Optional flag to hide all 64 bit elements (if false)

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementModel::m_InitBusSignal(const uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                                               const bool oq_ShowArrayElements, const bool oq_Show64BitValues)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if ((pc_View != NULL) && (pc_View->GetPcData().GetConnected() == true))
   {
      bool q_BusValid;
      bool q_ProtocolValid;
      bool q_MessageValid;

      this->mc_MessageSyncManagers.reserve(3U);
      //Init sync managers
      //Busses
      if (C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(ou32_ViewIndex,
                                                            pc_View->GetPcData().GetBusIndex()) == false)
      {
         C_TblTreItem * const pc_BusItem = new C_TblTreItem();
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
            pc_View->GetPcData().GetBusIndex());
         //Flag
         q_BusValid = false;
         //Node
         if (pc_Bus != NULL)
         {
            //Init current node
            pc_BusItem->u32_Index = pc_View->GetPcData().GetBusIndex();
            pc_BusItem->c_Name = pc_Bus->c_Name.c_str();
            pc_BusItem->q_Selectable = false;
            pc_BusItem->c_ToolTipHeading = pc_Bus->c_Name.c_str();
            pc_BusItem->c_ToolTipContent = pc_Bus->c_Comment.c_str();
            switch (pc_Bus->e_Type)
            {
            case C_OSCSystemBus::eCAN:
               pc_BusItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconCAN);
               break;
            case C_OSCSystemBus::eETHERNET:
               pc_BusItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconEthernet);
               break;
            }
            //Protocols
            for (uint8 u8_ItProtocol = 0U; u8_ItProtocol < 3U; ++u8_ItProtocol)
            {
               C_PuiSdNodeCanMessageSyncManager c_SyncManager;
               C_OSCCanProtocol::E_Type e_Type;
               std::vector<C_OSCCanMessageIdentificationIndices> c_UniqueMessages;
               C_TblTreItem * const pc_ProtocolItem = new C_TblTreItem();
               //Init current node
               pc_ProtocolItem->u32_Index = static_cast<uint32>(u8_ItProtocol);
               pc_ProtocolItem->q_Selectable = false;
               switch (u8_ItProtocol)
               {
               case 0U:
                  e_Type = C_OSCCanProtocol::eLAYER2;
                  pc_ProtocolItem->c_Name = "LAYER2 protocol";
                  pc_ProtocolItem->c_ToolTipHeading = pc_ProtocolItem->c_Name;
                  break;
               case 1U:
                  e_Type = C_OSCCanProtocol::eECES;
                  pc_ProtocolItem->c_Name = "ECES protocol";
                  pc_ProtocolItem->c_ToolTipHeading = pc_ProtocolItem->c_Name;
                  break;
               case 2U:
                  e_Type = C_OSCCanProtocol::eCAN_OPEN_SAFETY;
                  pc_ProtocolItem->c_Name = "CAN_OPEN_SAFETY protocol";
                  pc_ProtocolItem->c_ToolTipHeading = pc_ProtocolItem->c_Name;
                  break;
               default:
                  e_Type = C_OSCCanProtocol::eLAYER2;
                  tgl_assert(true);
                  break;
               }
               c_SyncManager.Init(pc_View->GetPcData().GetBusIndex(), e_Type);
               c_UniqueMessages = c_SyncManager.GetUniqueMessages();
               this->mc_MessageSyncManagers.push_back(c_SyncManager);
               //Flag
               q_ProtocolValid = false;

               //Messages
               for (uint32 u32_ItMessage = 0U; u32_ItMessage < c_UniqueMessages.size(); ++u32_ItMessage)
               {
                  C_TblTreItem * const pc_MessageItem = new C_TblTreItem();
                  const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                     c_UniqueMessages[u32_ItMessage]);
                  //Flag
                  q_MessageValid = false;
                  if (pc_Message != NULL)
                  {
                     //Init current node
                     pc_MessageItem->u32_Index = u32_ItMessage;
                     pc_MessageItem->c_Name =
                        QString("%1 (0x%2)").arg(pc_Message->c_Name.c_str()).arg(QString::number(pc_Message->u32_CanId,
                                                                                                 16));
                     pc_MessageItem->c_ToolTipHeading = pc_MessageItem->c_Name;
                     pc_MessageItem->c_ToolTipContent =
                        C_SdUtil::h_GetToolTipContentMessage(c_UniqueMessages[u32_ItMessage]);
                     pc_MessageItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconMessage);
                     //Signals
                     for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
                     {
                        const C_OSCNodeDataPoolListElement * const pc_Element =
                           C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(
                              c_UniqueMessages[u32_ItMessage],
                              u32_ItSignal);
                        if (pc_Element != NULL)
                        {
                           C_TblTreItem * const pc_ElementItem =
                              new C_TblTreItem();
                           //Init current node
                           pc_ElementItem->u32_Index = u32_ItSignal;
                           pc_ElementItem->c_Name = pc_Element->c_Name.c_str();
                           pc_ElementItem->c_ToolTipHeading = pc_Element->c_Name.c_str();
                           // todo
                           pc_ElementItem->c_ToolTipContent =
                              C_SdUtil::h_GetToolTipContentSignal(c_UniqueMessages[u32_ItMessage], u32_ItSignal);
                           pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconSignal);
                           if ((pc_Element->GetArray() == false) || (oq_ShowArrayElements == true))
                           {
                              if ((((pc_Element->GetType() != C_OSCNodeDataPoolContent::eFLOAT64) &&
                                    (pc_Element->GetType() != C_OSCNodeDataPoolContent::eUINT64)) &&
                                   (pc_Element->GetType() != C_OSCNodeDataPoolContent::eSINT64)) ||
                                  (oq_Show64BitValues == true))
                              {
                                 if ((pc_Element->e_Access == C_OSCNodeDataPoolListElement::eACCESS_RW) ||
                                     (oq_ShowOnlyWriteElements == false))
                                 {
                                    //Valid no adaptation
                                 }
                                 else
                                 {
                                    pc_ElementItem->q_Enabled = false;
                                    pc_ElementItem->q_Selectable = false;
                                    //Explanation
                                    pc_ElementItem->c_Name +=
                                       C_TblTreDataElementModel::mhc_AdditionalWriteOnlyInfo;
                                 }
                              }
                              else
                              {
                                 pc_ElementItem->q_Enabled = false;
                                 pc_ElementItem->q_Selectable = false;
                                 //Explanation
                                 pc_ElementItem->c_Name += C_TblTreDataElementModel::mhc_Additional64BitInfo;
                              }
                           }
                           else
                           {
                              pc_ElementItem->q_Enabled = false;
                              pc_ElementItem->q_Selectable = false;
                              //Explanation
                              pc_ElementItem->c_Name += C_TblTreDataElementModel::mhc_AdditionalArrayInfo;
                           }
                           //Signal
                           q_BusValid = true;
                           q_ProtocolValid = true;
                           q_MessageValid = true;
                           //Add
                           pc_MessageItem->AddChild(pc_ElementItem);
                        }
                     }
                  }
                  if (q_MessageValid == true)
                  {
                     pc_ProtocolItem->AddChild(pc_MessageItem);
                  }
                  else
                  {
                     delete (pc_MessageItem);
                  }
               }
               if (q_ProtocolValid == true)
               {
                  pc_BusItem->AddChild(pc_ProtocolItem);
               }
               else
               {
                  delete (pc_ProtocolItem);
               }
            }
         }
         if (q_BusValid == true)
         {
            this->mc_InvisibleRootItem.AddChild(pc_BusItem);
         }
         else
         {
            delete (pc_BusItem);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize tree structure for data pool elements

   \param[in] ou32_ViewIndex           View index
   \param[in] oq_ShowOnlyWriteElements Optional flag to show only writable elements
   \param[in] oq_ShowArrayElements     Optional flag to hide all array elements (if false)
   \param[in] oq_Show64BitValues       Optional flag to hide all 64 bit elements (if false)

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementModel::m_InitDatapoolElement(const uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                                                     const bool oq_ShowArrayElements, const bool oq_Show64BitValues)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      const std::vector<uint8> & rc_NodeActiveFlags = pc_View->GetNodeActiveFlags();
      bool q_NodeValid;
      bool q_DataPoolDiagValid;
      bool q_DataPoolNvmValid;
      bool q_DataPoolComValid;
      bool q_DataPoolValid;
      bool q_ListValid;
      const uint32 u32_NodeSize = C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize();

      //Nodes
      tgl_assert(rc_NodeActiveFlags.size() == u32_NodeSize);
      for (uint32 u32_ItNode = 0; u32_ItNode < u32_NodeSize; ++u32_ItNode)
      {
         if ((rc_NodeActiveFlags[u32_ItNode] == true) && (mh_CheckNodeDiagnostic(ou32_ViewIndex, u32_ItNode) == true))
         {
            C_TblTreItem * const pc_NodeItem = new C_TblTreItem();
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItNode);
            q_NodeValid = false;
            //Static
            pc_NodeItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconNode);
            //Node
            if (pc_Node != NULL)
            {
               //Data pool type nodes
               C_TblTreItem * const pc_DiagItem = new C_TblTreItem();
               C_TblTreItem * const pc_NvmItem = new C_TblTreItem();
               C_TblTreItem * const pc_ComItem = new C_TblTreItem();
               pc_DiagItem->c_Name = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_Name = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_Name = C_GtGetText::h_GetText("COMM Datapools");
               pc_DiagItem->c_ToolTipHeading = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_ToolTipHeading = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_ToolTipHeading = C_GtGetText::h_GetText("COMM Datapools");
               pc_DiagItem->q_Selectable = false;
               pc_NvmItem->q_Selectable = false;
               pc_ComItem->q_Selectable = false;
               //Use some unique index for expansion restoration mechanism
               pc_DiagItem->u32_Index = 0;
               pc_NvmItem->u32_Index = 1;
               pc_ComItem->u32_Index = 2;
               q_DataPoolDiagValid = false;
               q_DataPoolNvmValid = false;
               q_DataPoolComValid = false;
               //Init current node
               pc_NodeItem->u32_Index = u32_ItNode;
               pc_NodeItem->q_Selectable = false;
               pc_NodeItem->c_Name = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipHeading = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipContent = pc_Node->c_Properties.c_Comment.c_str();
               //Data pools
               for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
               {
                  const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
                  C_TblTreItem * const pc_DataPoolItem = new C_TblTreItem();
                  //Init current node
                  pc_DataPoolItem->u32_Index = u32_ItDataPool;
                  pc_DataPoolItem->c_Name = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipHeading = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipContent = rc_DataPool.c_Comment.c_str();
                  pc_DataPoolItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconDatapool);
                  pc_DataPoolItem->q_Selectable = false;
                  //Flag
                  q_DataPoolValid = false;
                  //Data pool
                  //Lists
                  for (uint32 u32_ItList = 0; u32_ItList < rc_DataPool.c_Lists.size(); ++u32_ItList)
                  {
                     const C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u32_ItList];
                     C_TblTreItem * const pc_ListItem = new C_TblTreItem();
                     //Init current node
                     pc_ListItem->u32_Index = u32_ItList;
                     pc_ListItem->c_Name = rc_List.c_Name.c_str();
                     pc_ListItem->c_ToolTipHeading = rc_List.c_Name.c_str();
                     // tooltip content: do not use h_GetToolTipContenList because we do not want so much info and
                     // consistency with superior tree items
                     pc_ListItem->c_ToolTipContent = rc_List.c_Comment.c_str();
                     pc_ListItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconList);
                     pc_ListItem->q_Selectable = false;
                     //Flag
                     q_ListValid = false;
                     //Elements
                     for (uint32 u32_ItElement = 0; u32_ItElement < rc_List.c_Elements.size(); ++u32_ItElement)
                     {
                        const C_OSCNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u32_ItElement];
                        C_TblTreItem * const pc_ElementItem =
                           new C_TblTreItem();
                        const C_OSCNodeDataPoolListElementId c_NodeDpListElement(u32_ItNode, u32_ItDataPool, u32_ItList,
                                                                                 u32_ItElement);
                        //Init current node
                        pc_ElementItem->u32_Index = u32_ItElement;
                        pc_ElementItem->c_Name = rc_Element.c_Name.c_str();
                        pc_ElementItem->c_ToolTipHeading = rc_Element.c_Name.c_str();
                        pc_ElementItem->c_ToolTipContent = C_SdUtil::h_GetToolTipContentDpListElement(
                           c_NodeDpListElement);
                        //Signal
                        q_NodeValid = true;
                        q_DataPoolValid = true;
                        q_ListValid = true;
                        //Type
                        switch (rc_DataPool.e_Type)
                        {
                        case C_OSCNodeDataPool::eDIAG:
                           q_DataPoolDiagValid = true;
                           pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconVariable);
                           break;
                        case C_OSCNodeDataPool::eNVM:
                           q_DataPoolNvmValid = true;
                           pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconParameter);
                           break;
                        case C_OSCNodeDataPool::eCOM:
                           q_DataPoolComValid = true;
                           pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconSignal);
                           break;
                        default:
                           break;
                        }
                        if ((rc_Element.GetArray() == false) || (oq_ShowArrayElements == true))
                        {
                           if ((((rc_Element.GetType() != C_OSCNodeDataPoolContent::eFLOAT64) &&
                                 (rc_Element.GetType() != C_OSCNodeDataPoolContent::eUINT64)) &&
                                (rc_Element.GetType() != C_OSCNodeDataPoolContent::eSINT64)) ||
                               (oq_Show64BitValues == true))
                           {
                              if ((rc_Element.e_Access == C_OSCNodeDataPoolListElement::eACCESS_RW) ||
                                  (oq_ShowOnlyWriteElements == false))
                              {
                                 //Valid no adaptation
                              }
                              else
                              {
                                 pc_ElementItem->q_Enabled = false;
                                 pc_ElementItem->q_Selectable = false;
                                 //Explanation
                                 pc_ElementItem->c_Name += C_TblTreDataElementModel::mhc_AdditionalWriteOnlyInfo;
                              }
                           }
                           else
                           {
                              pc_ElementItem->q_Enabled = false;
                              pc_ElementItem->q_Selectable = false;
                              //Explanation
                              pc_ElementItem->c_Name += C_TblTreDataElementModel::mhc_Additional64BitInfo;
                           }
                        }
                        else
                        {
                           pc_ElementItem->q_Enabled = false;
                           pc_ElementItem->q_Selectable = false;
                           //Explanation
                           pc_ElementItem->c_Name += C_TblTreDataElementModel::mhc_AdditionalArrayInfo;
                        }
                        //Add
                        pc_ListItem->AddChild(pc_ElementItem);
                     }
                     if (q_ListValid == true)
                     {
                        pc_DataPoolItem->AddChild(pc_ListItem);
                     }
                     else
                     {
                        delete (pc_ListItem);
                     }
                  }
                  if (q_DataPoolValid == true)
                  {
                     switch (rc_DataPool.e_Type)
                     {
                     case C_OSCNodeDataPool::eDIAG:
                        pc_DiagItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OSCNodeDataPool::eNVM:
                        pc_NvmItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OSCNodeDataPool::eCOM:
                        pc_ComItem->AddChild(pc_DataPoolItem);
                        break;
                     default:
                        delete (pc_DataPoolItem);
                        break;
                     }
                  }
                  else
                  {
                     delete (pc_DataPoolItem);
                  }
               }
               if (q_DataPoolDiagValid == true)
               {
                  pc_NodeItem->AddChild(pc_DiagItem);
               }
               else
               {
                  delete (pc_DiagItem);
               }
               if (q_DataPoolNvmValid == true)
               {
                  pc_NodeItem->AddChild(pc_NvmItem);
               }
               else
               {
                  delete (pc_NvmItem);
               }
               if (q_DataPoolComValid == true)
               {
                  pc_NodeItem->AddChild(pc_ComItem);
               }
               else
               {
                  delete (pc_ComItem);
               }
            }
            if (q_NodeValid == true)
            {
               this->mc_InvisibleRootItem.AddChild(pc_NodeItem);
            }
            else
            {
               delete (pc_NodeItem);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize tree structure for data pool lists

   \param[in] ou32_ViewIndex View index

   \created     08.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementModel::m_InitNvmList(const uint32 ou32_ViewIndex)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      const std::vector<uint8> & rc_NodeActiveFlags = pc_View->GetNodeActiveFlags();
      bool q_NodeValid;
      bool q_DataPoolDiagValid;
      bool q_DataPoolNvmValid;
      bool q_DataPoolComValid;
      bool q_DataPoolValid;
      const uint32 u32_NodeSize = C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize();

      //Nodes
      tgl_assert(rc_NodeActiveFlags.size() == u32_NodeSize);
      for (uint32 u32_ItNode = 0; u32_ItNode < u32_NodeSize; ++u32_ItNode)
      {
         if ((rc_NodeActiveFlags[u32_ItNode] == true) && (mh_CheckNodeDiagnostic(ou32_ViewIndex, u32_ItNode) == true))
         {
            C_TblTreItem * const pc_NodeItem = new C_TblTreItem();
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItNode);
            q_NodeValid = false;
            //Static
            pc_NodeItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconNode);
            //Node
            if (pc_Node != NULL)
            {
               //Data pool type nodes
               C_TblTreItem * const pc_DiagItem = new C_TblTreItem();
               C_TblTreItem * const pc_NvmItem = new C_TblTreItem();
               C_TblTreItem * const pc_ComItem = new C_TblTreItem();
               pc_DiagItem->c_Name = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_Name = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_Name = C_GtGetText::h_GetText("COMM Datapools");
               pc_DiagItem->c_ToolTipHeading = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_ToolTipHeading = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_ToolTipHeading = C_GtGetText::h_GetText("COMM Datapools");
               pc_DiagItem->q_Selectable = false;
               pc_NvmItem->q_Selectable = false;
               pc_ComItem->q_Selectable = false;
               //Probably invalid index to detect errors
               pc_DiagItem->u32_Index = 12345678;
               pc_NvmItem->u32_Index = 12345678;
               pc_ComItem->u32_Index = 12345678;
               q_DataPoolDiagValid = false;
               q_DataPoolNvmValid = false;
               q_DataPoolComValid = false;
               //Init current node
               pc_NodeItem->u32_Index = u32_ItNode;
               pc_NodeItem->q_Selectable = false;
               pc_NodeItem->c_Name = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipHeading = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipContent = pc_Node->c_Properties.c_Comment.c_str();
               //Data pools
               for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
               {
                  const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
                  C_TblTreItem * const pc_DataPoolItem = new C_TblTreItem();
                  //Init current node
                  pc_DataPoolItem->u32_Index = u32_ItDataPool;
                  pc_DataPoolItem->q_Selectable = false;
                  pc_DataPoolItem->c_Name = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipHeading = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipContent = rc_DataPool.c_Comment.c_str();
                  pc_DataPoolItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconDatapool);
                  //Flag
                  q_DataPoolValid = false;
                  //Data pool
                  //Lists
                  for (uint32 u32_ItList = 0; u32_ItList < rc_DataPool.c_Lists.size(); ++u32_ItList)
                  {
                     const C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u32_ItList];
                     C_TblTreItem * const pc_ListItem = new C_TblTreItem();
                     //Init current node
                     pc_ListItem->u32_Index = u32_ItList;
                     pc_ListItem->c_Name = rc_List.c_Name.c_str();
                     pc_ListItem->c_ToolTipHeading = rc_List.c_Name.c_str();
                     pc_ListItem->c_ToolTipContent =
                        C_SdUtil::h_GetToolTipContentDpList(u32_ItNode, u32_ItDataPool, u32_ItList);
                     pc_ListItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_IconList);
                     //Elements
                     if (rc_List.c_Elements.size() > 0)
                     {
                        //Signal
                        q_NodeValid = true;
                        q_DataPoolValid = true;
                        switch (rc_DataPool.e_Type)
                        {
                        case C_OSCNodeDataPool::eNVM:
                           q_DataPoolNvmValid = true;
                           //Check usage
                           if (pc_View->CheckNvmParamListUsage(C_OSCNodeDataPoolListId(u32_ItNode, u32_ItDataPool,
                                                                                       u32_ItList)) == true)
                           {
                              pc_ListItem->c_Name += C_GtGetText::h_GetText(" (Already used)");
                              pc_ListItem->q_Selectable = false;
                              pc_ListItem->q_Enabled = false;
                           }
                           else
                           {
                              pc_ListItem->q_Selectable = true;
                              pc_ListItem->q_Enabled = true;
                           }
                           break;
                        default:
                           break;
                        }
                        //Add
                        pc_DataPoolItem->AddChild(pc_ListItem);
                     }
                     else
                     {
                        delete (pc_ListItem);
                     }
                  }
                  if (q_DataPoolValid == true)
                  {
                     switch (rc_DataPool.e_Type)
                     {
                     case C_OSCNodeDataPool::eDIAG:
                        pc_DiagItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OSCNodeDataPool::eNVM:
                        pc_NvmItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OSCNodeDataPool::eCOM:
                        pc_ComItem->AddChild(pc_DataPoolItem);
                        break;
                     default:
                        delete (pc_DataPoolItem);
                        break;
                     }
                  }
                  else
                  {
                     delete (pc_DataPoolItem);
                  }
               }
               //lint -e{948}  Kept because of similarities to other init functions
               if (q_DataPoolDiagValid == true)
               {
                  pc_NodeItem->AddChild(pc_DiagItem);
               }
               else
               {
                  delete (pc_DiagItem);
               }
               if (q_DataPoolNvmValid == true)
               {
                  pc_NodeItem->AddChild(pc_NvmItem);
               }
               else
               {
                  delete (pc_NvmItem);
               }
               //lint -e{948}  Kept because of similarities to other init functions
               if (q_DataPoolComValid == true)
               {
                  pc_NodeItem->AddChild(pc_ComItem);
               }
               else
               {
                  delete (pc_ComItem);
               }
            }
            if (q_NodeValid == true)
            {
               this->mc_InvisibleRootItem.AddChild(pc_NodeItem);
            }
            else
            {
               delete (pc_NodeItem);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get bus signals

   \param[in] orc_Index Index

   \return
   Bus signals

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetBusSignals(const QModelIndex & orc_Index)
const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{925}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreItem * const pc_FirstParent = pc_TreeItem->pc_Parent;
      if (pc_FirstParent->pc_Parent != NULL)
      {
         const C_TblTreItem * const pc_SecondParent = pc_FirstParent->pc_Parent;
         if (pc_SecondParent->pc_Parent != NULL)
         {
            const C_TblTreItem * const pc_ThirdParent = pc_SecondParent->pc_Parent;
            if (pc_ThirdParent->pc_Parent != NULL)
            {
               const C_TblTreItem * const pc_FourthParent = pc_ThirdParent->pc_Parent;
               if (pc_FourthParent->pc_Parent != NULL)
               {
                  //Should not happen
                  tgl_assert(false);
               }
               else
               {
                  //Signal element
                  //4: Invisible item
                  //3: Bus item
                  //2: Protocol item
                  //1: Message item

                  //3 Sync managers, second stores index of type
                  const uint32 u32_MessageVectorIndex = pc_SecondParent->u32_Index;
                  if (u32_MessageVectorIndex < this->mc_MessageSyncManagers.size())
                  {
                     const C_PuiSdNodeCanMessageSyncManager & rc_MessageSyncManager =
                        this->mc_MessageSyncManagers[u32_MessageVectorIndex];
                     const std::vector<C_OSCCanMessageIdentificationIndices> c_Messages =
                        rc_MessageSyncManager.GetUniqueMessages();
                     if (pc_FirstParent->u32_Index < c_Messages.size())
                     {
                        c_Retval.push_back(mh_Translate(c_Messages[pc_FirstParent->u32_Index],
                                                        pc_TreeItem->u32_Index));
                     }
                  }
               }
            }
            else
            {
               //Message
               //3: Invisible item
               //2: Bus item
               //1: Protocol item
               //TODO ?
            }
         }
         else
         {
            //Protocol
            //2: Invisible item
            //1: Bus item
            //TODO ?
         }
      }
      else
      {
         //Bus
         //1: Invisible item
         //No valid parent
         //TODO ?
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pools

   \param[in] orc_Index Index

   \return
   Data pools

   \created     01.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetDatapools(const QModelIndex & orc_Index)
const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{925}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreItem * const pc_FirstParent = pc_TreeItem->pc_Parent;
      if (pc_FirstParent->pc_Parent != NULL)
      {
         const C_TblTreItem * const pc_SecondParent = pc_FirstParent->pc_Parent;
         if (pc_SecondParent->pc_Parent != NULL)
         {
            const C_TblTreItem * const pc_ThirdParent = pc_SecondParent->pc_Parent;
            if (pc_ThirdParent->pc_Parent != NULL)
            {
               //Should not happen
               tgl_assert(false);
            }
            else
            {
               //Data pool
               //3: Invisible item
               //2: Node item
               //1: Data pool type item
               c_Retval.push_back(C_PuiSvDbNodeDataPoolListElementId(pc_SecondParent->u32_Index,
                                                                     pc_TreeItem->u32_Index, 0UL, 0UL,
                                                                     C_PuiSvDbNodeDataPoolListElementId::
                                                                     eDATAPOOL_ELEMENT));
            }
         }
         else
         {
            //Data pool type
            //2: Invisible item
            //1: Node item
            //TODO ?
         }
      }
      else
      {
         //Node
         //1: Invisible item
         //No valid parent
         //TODO ?
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool elements

   \param[in] orc_Index Index

   \return
   Data pool elements

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetDatapoolElements(
   const QModelIndex & orc_Index) const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{925}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreItem * const pc_FirstParent = pc_TreeItem->pc_Parent;
      if (pc_FirstParent->pc_Parent != NULL)
      {
         const C_TblTreItem * const pc_SecondParent = pc_FirstParent->pc_Parent;
         if (pc_SecondParent->pc_Parent != NULL)
         {
            const C_TblTreItem * const pc_ThirdParent = pc_SecondParent->pc_Parent;
            if (pc_ThirdParent->pc_Parent != NULL)
            {
               const C_TblTreItem * const pc_FourthParent = pc_ThirdParent->pc_Parent;
               if (pc_FourthParent->pc_Parent != NULL)
               {
                  const C_TblTreItem * const pc_FifthParent = pc_FourthParent->pc_Parent;
                  if (pc_FifthParent->pc_Parent != NULL)
                  {
                     //Should not happen
                     tgl_assert(false);
                  }
                  else
                  {
                     //Data element
                     //5: Invisible item
                     //4: Node item
                     //3: Data pool type item
                     //2: Data pool item
                     //1: List item
                     c_Retval.push_back(C_PuiSvDbNodeDataPoolListElementId(pc_FourthParent->u32_Index,
                                                                           pc_SecondParent->u32_Index,
                                                                           pc_FirstParent->u32_Index,
                                                                           pc_TreeItem->u32_Index,
                                                                           C_PuiSvDbNodeDataPoolListElementId::
                                                                           eDATAPOOL_ELEMENT));
                  }
               }
               else
               {
                  //List
                  //4: Invisible item
                  //3: Node item
                  //2: Data pool type item
                  //1: Data pool item
                  //TODO ?
               }
            }
            else
            {
               //Data pool
               //3: Invisible item
               //2: Node item
               //1: Data pool type item
               //TODO ?
            }
         }
         else
         {
            //Data pool type
            //2: Invisible item
            //1: Node item
            //TODO ?
         }
      }
      else
      {
         //Node
         //1: Invisible item
         //No valid parent
         //TODO ?
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool elements

   \param[in] orc_Index Index

   \return
   Data pool elements

   \created     08.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetNvmList(const QModelIndex & orc_Index)
const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{925}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreItem * const pc_FirstParent = pc_TreeItem->pc_Parent;
      if (pc_FirstParent->pc_Parent != NULL)
      {
         const C_TblTreItem * const pc_SecondParent = pc_FirstParent->pc_Parent;
         if (pc_SecondParent->pc_Parent != NULL)
         {
            const C_TblTreItem * const pc_ThirdParent = pc_SecondParent->pc_Parent;
            if (pc_ThirdParent->pc_Parent != NULL)
            {
               const C_TblTreItem * const pc_FourthParent = pc_ThirdParent->pc_Parent;
               if (pc_FourthParent->pc_Parent != NULL)
               {
                  //Should not happen
                  tgl_assert(false);
               }
               else
               {
                  //List
                  //4: Invisible item
                  //3: Node item
                  //2: Data pool type item
                  //1: Data pool item
                  const uint32 u32_NodeIndex = pc_ThirdParent->u32_Index;
                  const uint32 u32_DataPoolIndex = pc_FirstParent->u32_Index;
                  const uint32 u32_ListIndex = pc_TreeItem->u32_Index;
                  const C_OSCNodeDataPoolList * const pc_OSCList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                     u32_NodeIndex, u32_DataPoolIndex, u32_ListIndex);
                  if (pc_OSCList != NULL)
                  {
                     for (uint32 u32_ItElement = 0; u32_ItElement < pc_OSCList->c_Elements.size(); ++u32_ItElement)
                     {
                        c_Retval.push_back(C_PuiSvDbNodeDataPoolListElementId(u32_NodeIndex,
                                                                              u32_DataPoolIndex,
                                                                              u32_ListIndex,
                                                                              u32_ItElement,
                                                                              C_PuiSvDbNodeDataPoolListElementId::
                                                                              eDATAPOOL_ELEMENT));
                     }
                  }
                  else
                  {
                     //Should not happen
                     tgl_assert(false);
                  }
               }
            }
            else
            {
               //Data pool
               //3: Invisible item
               //2: Node item
               //1: Data pool type item
               //TODO ?
            }
         }
         else
         {
            //Data pool type
            //2: Invisible item
            //1: Node item
            //TODO ?
         }
      }
      else
      {
         //Node
         //1: Invisible item
         //No valid parent
         //TODO ?
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if diagnostic mode activated

   \param[in] ou32_ViewIndex View index
   \param[in] ou32_NodeIndex Node index

   \return
   True  Diagnostic mode activated
   False No diagnostic mode activated

   \created     07.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_TblTreDataElementModel::mh_CheckNodeDiagnostic(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex)
{
   bool q_Retval = false;
   const C_SyvRoRouteCalculation c_RouteCalculation(ou32_ViewIndex, ou32_NodeIndex,
                                                    C_OSCRoutingCalculation::eDIAGNOSTIC);

   const sint32 s32_Retval = c_RouteCalculation.GetState();

   if (s32_Retval == C_NO_ERR)
   {
      q_Retval = true;
   }

   return q_Retval;
}
