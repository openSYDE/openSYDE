//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for data elements (implementation)

   Tree model for data elements

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscHalcMagicianUtil.hpp"
#include "C_TblTreDataElementItem.hpp"
#include "C_SyvRoRouteCalculation.hpp"
#include "C_TblTreDataElementModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_TblTreDataElementModel::mhc_ICON_NODE = ":/images/system_definition/IconNode.svg";
const QString C_TblTreDataElementModel::mhc_ICON_DATAPOOL = ":/images/system_definition/IconDataPoolSmall.svg";
const QString C_TblTreDataElementModel::mhc_ICON_LIST = ":/images/system_definition/IconDatapoolList.svg";
const QString C_TblTreDataElementModel::mhc_ICON_VARIABLE = ":/images/system_definition/IconVariable.svg";
const QString C_TblTreDataElementModel::mhc_ICON_PARAMETER = ":/images/system_definition/IconParameter.svg";
const QString C_TblTreDataElementModel::mhc_ICON_SIGNAL = ":/images/system_definition/IconSignal.svg";
const QString C_TblTreDataElementModel::mhc_ICON_ETHERNET = ":/images/system_definition/IconBus.svg";
const QString C_TblTreDataElementModel::mhc_ICON_CAN = ":/images/system_definition/IconBus.svg";
const QString C_TblTreDataElementModel::mhc_ICON_MESSAGE = ":/images/system_definition/IconMessage.svg";
const QString C_TblTreDataElementModel::mhc_ICON_HALC_INPUT =
   "://images/system_definition/NodeEdit/halc/InputSmallActive.svg";
const QString C_TblTreDataElementModel::mhc_ICON_HALC_OUTPUT =
   "://images/system_definition/NodeEdit/halc/OutputSmallActive.svg";
const QString C_TblTreDataElementModel::mhc_ICON_HALC_MISC =
   "://images/system_definition/NodeEdit/halc/OtherSmallActive.svg";
const QString C_TblTreDataElementModel::mhc_ADDITIONAL_DATA_POOL_INFO = " (Already assigned)";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
QMap<std::vector<uint32_t>,
     C_TblTreDataElementModel::C_TblTreDataElementModelState> C_TblTreDataElementModel::mhc_ViewSetupsNl;
QMap<std::vector<uint32_t>,
     C_TblTreDataElementModel::C_TblTreDataElementModelState> C_TblTreDataElementModel::mhc_ViewSetupsDe;
QMap<std::vector<uint32_t>,
     C_TblTreDataElementModel::C_TblTreDataElementModelState> C_TblTreDataElementModel::mhc_ViewSetupsBs;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementModel::C_TblTreDataElementModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   me_Mode(eDATAPOOL_ELEMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementModel::~C_TblTreDataElementModel(void)
{
   CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the active node index

   \param[in]  ou32_NodeIndex                Active node index
   \param[in]  os32_SkipApplicationIndex     Application index to not display as used
   \param[in]  orc_UsedDataPoolIndicesIndex  Data pools to always display as used
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::InitSd(const uint32_t ou32_NodeIndex, const int32_t os32_SkipApplicationIndex,
                                      const std::vector<uint32_t> & orc_UsedDataPoolIndicesIndex)
{
   C_TblTreItem * const pc_NodeItem = new C_TblTreItem();
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
   bool q_NodeValid = false;

   this->beginResetModel();
   this->me_Mode = C_TblTreDataElementModel::eDATAPOOLS;

   //Clear
   CleanUpLastModel();
   this->mpc_InvisibleRootItem = new C_TblTreItem();

   //Static
   pc_NodeItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_NODE);

   //Node
   if (pc_Node != NULL)
   {
      //Data pool type nodes
      C_TblTreItem * const pc_DiagItem = new C_TblTreItem();
      C_TblTreItem * const pc_NvmItem = new C_TblTreItem();
      C_TblTreItem * const pc_ComItem = new C_TblTreItem();
      C_TblTreItem * const pc_HalcItem = new C_TblTreItem();
      bool q_DataPoolDiagValid = false;
      bool q_DataPoolNvmValid = false;
      bool q_DataPoolComValid = false;
      bool q_DataPoolHalcValid = false;
      pc_DiagItem->c_Name = C_GtGetText::h_GetText("DIAG Datapools");
      pc_NvmItem->c_Name = C_GtGetText::h_GetText("NVM Datapools");
      pc_ComItem->c_Name = C_GtGetText::h_GetText("COMM Datapools");
      pc_HalcItem->c_Name = C_GtGetText::h_GetText("HAL Datapools");
      pc_DiagItem->c_ToolTipHeading = C_GtGetText::h_GetText("DIAG Datapools");
      pc_NvmItem->c_ToolTipHeading = C_GtGetText::h_GetText("NVM Datapools");
      pc_ComItem->c_ToolTipHeading = C_GtGetText::h_GetText("COMM Datapools");
      pc_HalcItem->c_ToolTipHeading = C_GtGetText::h_GetText("HAL Datapools");
      pc_DiagItem->q_Selectable = false;
      pc_NvmItem->q_Selectable = false;
      pc_ComItem->q_Selectable = false;
      pc_HalcItem->q_Selectable = false;

      //Use some unique index for expansion restoration mechanism
      pc_DiagItem->u32_Index = 0;
      pc_NvmItem->u32_Index = 1;
      pc_ComItem->u32_Index = 2;
      pc_HalcItem->u32_Index = 3;

      //Init current node
      pc_NodeItem->u32_Index = ou32_NodeIndex;
      pc_NodeItem->q_Selectable = false;
      pc_NodeItem->c_Name = pc_Node->c_Properties.c_Name.c_str();
      pc_NodeItem->c_ToolTipHeading = pc_Node->c_Properties.c_Name.c_str();
      pc_NodeItem->c_ToolTipContent = pc_Node->c_Properties.c_Comment.c_str();

      //Data pools
      pc_NodeItem->ReserveChildrenSpace(4UL);
      pc_DiagItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
      pc_NvmItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
      pc_ComItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
      pc_HalcItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         C_TblTreItem * const pc_DataPoolItem = new C_TblTreItem();

         //Init current Datapool
         pc_DataPoolItem->u32_Index = u32_ItDataPool;
         pc_DataPoolItem->c_Name = rc_DataPool.c_Name.c_str();
         pc_DataPoolItem->c_ToolTipHeading = rc_DataPool.c_Name.c_str();
         pc_DataPoolItem->c_ToolTipContent = rc_DataPool.c_Comment.c_str();
         pc_DataPoolItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_DATAPOOL);

         // Append protocol type if COMM datapool
         if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
         {
            pc_DataPoolItem->c_Name += C_GtGetText::h_GetText(" (Protocol: ");
            pc_DataPoolItem->c_Name +=
               C_PuiSdUtil::h_ConvertProtocolTypeToString(C_PuiSdUtil::h_GetRelatedCanProtocolType(ou32_NodeIndex,
                                                                                                   u32_ItDataPool));
            pc_DataPoolItem->c_Name +=  ")";
            pc_DataPoolItem->c_ToolTipHeading = pc_DataPoolItem->c_Name;
         }

         // Set disabled/enabled
         if ((rc_DataPool.s32_RelatedDataBlockIndex < 0) ||
             ((os32_SkipApplicationIndex >= 0) && (rc_DataPool.s32_RelatedDataBlockIndex == os32_SkipApplicationIndex)))
         {
            bool q_Found = false;
            for (uint32_t u32_It = 0UL; u32_It < orc_UsedDataPoolIndicesIndex.size(); ++u32_It)
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
               pc_DataPoolItem->c_Name += C_TblTreDataElementModel::mhc_ADDITIONAL_DATA_POOL_INFO;
            }
         }
         else
         {
            pc_DataPoolItem->q_Enabled = false;
            pc_DataPoolItem->q_Selectable = false;
            pc_DataPoolItem->c_Name += C_TblTreDataElementModel::mhc_ADDITIONAL_DATA_POOL_INFO;
         }

         //Flags
         q_NodeValid = true;
         switch (rc_DataPool.e_Type)
         {
         case C_OscNodeDataPool::eDIAG:
            q_DataPoolDiagValid = true;
            pc_DiagItem->AddChild(pc_DataPoolItem);
            break;
         case C_OscNodeDataPool::eNVM:
            q_DataPoolNvmValid = true;
            pc_NvmItem->AddChild(pc_DataPoolItem);
            break;
         case C_OscNodeDataPool::eCOM:
            q_DataPoolComValid = true;
            pc_ComItem->AddChild(pc_DataPoolItem);
            break;
         case C_OscNodeDataPool::eHALC:
         case C_OscNodeDataPool::eHALC_NVM:
            q_DataPoolHalcValid = true;
            pc_HalcItem->AddChild(pc_DataPoolItem);
            break;
         default:
            delete (pc_DataPoolItem);
            break;
         }
      } //lint !e593  //no memory leak because of the Qt memory management
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
      if (q_DataPoolHalcValid == true)
      {
         pc_NodeItem->AddChild(pc_HalcItem);
      }
      else
      {
         delete (pc_HalcItem);
      }
   }
   if (q_NodeValid == true)
   {
      this->mpc_InvisibleRootItem->AddChild(pc_NodeItem);
   }
   else
   {
      delete (pc_NodeItem);
   }
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize tree structure

   \param[in]  ou32_ViewIndex             View index
   \param[in]  oe_Mode                    Item mode
   \param[in]  oq_ShowOnlyWriteElements   Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements       Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements  Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues         Optional flag to hide all 64 bit elements (if false)
   \param[in]  opc_AlreasyUsedElements    Optional pointer to vector with already used elements. All added elements
                                          will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::InitSv(const uint32_t ou32_ViewIndex, const E_Mode oe_Mode,
                                      const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                      const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                      const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   QMap<std::vector<uint32_t>, C_TblTreDataElementModelState>::const_iterator c_It;
   const std::vector<uint32_t> c_Hashes = C_TblTreDataElementModel::mh_GetViewSdHash(ou32_ViewIndex);

   this->me_Mode = oe_Mode;
   this->beginResetModel();
   //Clear
   CleanUpLastModel();
   //Init
   switch (this->me_Mode)
   {
   case eDATAPOOL_ELEMENT:
      c_It = C_TblTreDataElementModel::mhc_ViewSetupsDe.find(c_Hashes);
      if (c_It != C_TblTreDataElementModel::mhc_ViewSetupsDe.end())
      {
         const C_TblTreDataElementModelState & rc_It = c_It.value();
         this->mpc_InvisibleRootItem = rc_It.pc_Tree;
         C_TblTreDataElementModel::mh_UpdateDatapoolElement(oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                            oq_ShowArrayIndexElements,
                                                            oq_Show64BitValues, opc_AlreasyUsedElements,
                                                            this->mpc_InvisibleRootItem);
      }
      else
      {
         this->mpc_InvisibleRootItem = new C_TblTreItem();
         m_InitDatapoolElements(ou32_ViewIndex, oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                oq_ShowArrayIndexElements, oq_Show64BitValues, opc_AlreasyUsedElements);
         //Clean up (old values probably not necessary in future);
         mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsDe);
         //Directly store the model (after filling it-> for sync managers)
         C_TblTreDataElementModel::mhc_ViewSetupsDe.insert(
            c_Hashes,
            static_cast<C_TblTreDataElementModel::C_TblTreDataElementModelState>(this->mpc_InvisibleRootItem));
      }
      break;
   case eBUS_SIGNAL:
      c_It = C_TblTreDataElementModel::mhc_ViewSetupsBs.find(c_Hashes);
      if (c_It != C_TblTreDataElementModel::mhc_ViewSetupsBs.end())
      {
         const C_TblTreDataElementModelState & rc_It = c_It.value();
         this->mpc_InvisibleRootItem = rc_It.pc_Tree;
         C_TblTreDataElementModel::mh_UpdateDatapoolElement(oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                            oq_ShowArrayIndexElements,
                                                            oq_Show64BitValues, opc_AlreasyUsedElements,
                                                            this->mpc_InvisibleRootItem);
      }
      else
      {
         this->mpc_InvisibleRootItem = new C_TblTreItem();
         m_InitBusSignal(ou32_ViewIndex, oq_ShowOnlyWriteElements, oq_ShowArrayElements, oq_ShowArrayIndexElements,
                         oq_Show64BitValues, opc_AlreasyUsedElements);
         //Clean up (old values probably not necessary in future);
         mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsBs);
         //Directly store the model (after filling it-> for sync managers)
         C_TblTreDataElementModel::mhc_ViewSetupsBs.insert(
            c_Hashes,
            static_cast<C_TblTreDataElementModel::C_TblTreDataElementModelState>(this->mpc_InvisibleRootItem));
      }
      break;
   case eNVM_LIST:
      c_It = C_TblTreDataElementModel::mhc_ViewSetupsNl.find(c_Hashes);
      if (c_It != C_TblTreDataElementModel::mhc_ViewSetupsNl.end())
      {
         const C_TblTreDataElementModelState & rc_It = c_It.value();
         this->mpc_InvisibleRootItem = rc_It.pc_Tree;
         C_TblTreDataElementModel::mh_UpdateDatapoolElement(oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                            oq_ShowArrayIndexElements,
                                                            oq_Show64BitValues, NULL,
                                                            this->mpc_InvisibleRootItem);
      }
      else
      {
         //Clean up (old values probably not necessary in future);
         mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsNl);
         this->mpc_InvisibleRootItem = new C_TblTreItem();
         m_InitNvmList(ou32_ViewIndex);
         //Directly store the model (after filling it-> for sync managers)
         C_TblTreDataElementModel::mhc_ViewSetupsNl.insert(
            c_Hashes,
            static_cast<C_TblTreDataElementModel::C_TblTreDataElementModelState>(this->mpc_InvisibleRootItem));
      }
      break;
   case eDATAPOOLS:
   default:
      //NO SV use case
      tgl_assert(false);
      break;
   }
   this->endResetModel();

   osc_write_log_performance_stop(u16_TimerId, "Setup data element tree");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize tree structure

   \param[in]  ou32_SdDataLoggerUseCaseNodeIndex   System definition data logger use case: node index
   \param[in]  oq_ShowOnlyWriteElements            Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements                Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements           Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues                  Optional flag to hide all 64 bit elements (if false)
   \param[in]  opc_AlreasyUsedElements             Optional pointer to vector with already used elements. All added elements
                                                   will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::InitSdDatapoolElements(const uint32_t ou32_SdDataLoggerUseCaseNodeIndex,
                                                      const bool oq_ShowOnlyWriteElements,
                                                      const bool oq_ShowArrayElements,
                                                      const bool oq_ShowArrayIndexElements,
                                                      const bool oq_Show64BitValues,
                                                      const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();
   const std::vector<uint32_t> c_Hashes = C_TblTreDataElementModel::mh_GetViewSdHash(0U);

   this->beginResetModel();
   //Clear
   CleanUpLastModel();
   this->mpc_InvisibleRootItem = new C_TblTreItem();
   m_InitDatapoolElements(0U, oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                          oq_ShowArrayIndexElements, oq_Show64BitValues, opc_AlreasyUsedElements, false,
                          ou32_SdDataLoggerUseCaseNodeIndex);
   //Clean up (old values probably not necessary in future);
   mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsDe);
   //Directly store the model (after filling it-> for sync managers)
   C_TblTreDataElementModel::mhc_ViewSetupsDe.insert(
      c_Hashes,
      static_cast<C_TblTreDataElementModel::C_TblTreDataElementModelState>(this->mpc_InvisibleRootItem));
   this->endResetModel();

   osc_write_log_performance_stop(u16_TimerId, "Setup sd datapool element tree");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data elements

   \param[in]  orc_Index   Index

   \return
   Data elements
*/
//----------------------------------------------------------------------------------------------------------------------
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
         c_Retval = m_GetAnyDatapoolElements(orc_Index);
         break;
      case eBUS_SIGNAL:
         c_Retval = m_GetAnyDatapoolElements(orc_Index);
         break;
      case eNVM_LIST:
         c_Retval = m_GetNvmList(orc_Index);
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Preparation for final clean up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::h_CleanUp(void)
{
   mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsBs);
   C_TblTreDataElementModel::mhc_ViewSetupsBs.clear();
   mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsDe);
   C_TblTreDataElementModel::mhc_ViewSetupsDe.clear();
   mh_CleanUp(C_TblTreDataElementModel::mhc_ViewSetupsNl);
   C_TblTreDataElementModel::mhc_ViewSetupsNl.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_TblTreDataElementModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert generic item representation to model index

   \param[in]  orc_ItemIndices   Generic item representation

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreDataElementModel::GetIndexForItem(const std::vector<uint32_t> & orc_ItemIndices) const
{
   QModelIndex c_Retval;

   if (orc_ItemIndices.size() > 0UL)
   {
      QModelIndex c_PreviousParent;
      uint32_t u32_Counter = 0UL;
      bool q_FoundSomething;

      do
      {
         q_FoundSomething = false;
         for (int32_t s32_ItChild = 0; s32_ItChild < this->rowCount(c_PreviousParent); ++s32_ItChild)
         {
            const int32_t s32_COLUMN = 0;
            const QModelIndex c_Tmp = this->index(s32_ItChild, s32_COLUMN, c_PreviousParent);
            //lint -e{9079}  Result of Qt interface restrictions, set by index function
            const C_TblTreItem * const pc_TreeItem =
               static_cast<const C_TblTreItem *>(c_Tmp.internalPointer());
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert model index to generic item representation

   \param[in]  orc_ItemIndex  Model index

   \return
   Generic item representation
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_TblTreDataElementModel::GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex)
const
{
   std::vector<uint32_t> c_Retval;
   QModelIndex c_CurItem = orc_ItemIndex;
   while (c_CurItem.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem = static_cast<const C_TblTreItem *>(c_CurItem.internalPointer());
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Tree   Tree layout to remember
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementModel::C_TblTreDataElementModelState::C_TblTreDataElementModelState(
   C_TblTreSimpleItem * const opc_Tree) :
   pc_Tree(opc_Tree)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Preparation for final clean up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::C_TblTreDataElementModelState::CleanUp(void)
{
   delete (this->pc_Tree);
   this->pc_Tree = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up current model

   Warning: pointer might be invalid after call of this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::CleanUpLastModel(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      //Check if current model is stored, only discard if not stored
      if (mh_Contains(C_TblTreDataElementModel::mhc_ViewSetupsBs, this->mpc_InvisibleRootItem) == false)
      {
         if (mh_Contains(C_TblTreDataElementModel::mhc_ViewSetupsDe, this->mpc_InvisibleRootItem) == false)
         {
            if (mh_Contains(C_TblTreDataElementModel::mhc_ViewSetupsNl, this->mpc_InvisibleRootItem) == false)
            {
               delete (this->mpc_InvisibleRootItem);
            }
         }
      }
      // Setting always to NULL. The saved items for each case will be assigned again when necessary.
      // But it could be that the pointer will get invalid, if a new dialog will replace the saved item and delete
      // the old one.
      this->mpc_InvisibleRootItem = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate signal index to data pool index

   WARNING: Only works if message contains at least one signal

   \param[in]  orc_Indices       Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
   \param[in]  oe_IdType         Id type

   \return
   Parsed element ID
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId C_TblTreDataElementModel::mh_Translate(
   const C_OscCanMessageIdentificationIndices & orc_Indices, const uint32_t ou32_SignalIndex,
   const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_IdType)
{
   C_PuiSvDbNodeDataPoolListElementId c_Retval(0, 0, 0, 0, C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                               false, 0UL, false);
   const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPool(
      orc_Indices.u32_NodeIndex, orc_Indices.e_ComProtocol, orc_Indices.u32_DatapoolIndex);

   if (pc_DataPool != NULL)
   {
      uint32_t u32_ListIndex;
      if (C_OscCanProtocol::h_GetComListIndex(*pc_DataPool, orc_Indices.u32_InterfaceIndex, orc_Indices.q_MessageIsTx,
                                              u32_ListIndex) == C_NO_ERR)
      {
         const C_OscCanMessageContainer * const pc_Container =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_Indices.u32_NodeIndex,
                                                                            orc_Indices.e_ComProtocol,
                                                                            orc_Indices.u32_InterfaceIndex,
                                                                            orc_Indices.u32_DatapoolIndex);

         const C_OscCanProtocol * const pc_Protocol = C_PuiSdHandler::h_GetInstance()->GetCanProtocol(
            orc_Indices.u32_NodeIndex, orc_Indices.e_ComProtocol, orc_Indices.u32_DatapoolIndex);

         if ((pc_Container != NULL) && (pc_Protocol != NULL))
         {
            const uint32_t u32_SignalDataStartIndex = pc_Container->GetMessageSignalDataStartIndex(
               orc_Indices.q_MessageIsTx,
               orc_Indices.u32_MessageIndex);
            c_Retval = C_PuiSvDbNodeDataPoolListElementId(orc_Indices.u32_NodeIndex, pc_Protocol->u32_DataPoolIndex,
                                                          u32_ListIndex, u32_SignalDataStartIndex + ou32_SignalIndex,
                                                          oe_IdType, false, 0UL);
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize tree structure for bus signals

   \param[in]  ou32_ViewIndex             View index
   \param[in]  oq_ShowOnlyWriteElements   Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements       Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements  Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues         Optional flag to hide all 64 bit elements (if false)
   \param[in]  opc_AlreasyUsedElements    Optional pointer to vector with already used elements. All added elements
                                          will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::m_InitBusSignal(const uint32_t ou32_ViewIndex,  const bool oq_ShowOnlyWriteElements,
                                               const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                                               const bool oq_Show64BitValues,
                                               const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if ((pc_View != NULL) && (pc_View->GetOscPcData().GetConnected() == true))
   {
      //Init sync managers
      //Busses
      if (C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(ou32_ViewIndex,
                                                            pc_View->GetOscPcData().GetBusIndex()) == false)
      {
         C_TblTreItem * const pc_BusItem = new C_TblTreItem();
         const C_OscSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOscBus(pc_View->GetOscPcData().GetBusIndex());
         bool q_BusValid = false;
         //Node
         if (pc_Bus != NULL)
         {
            //Init current node
            pc_BusItem->u32_Index = pc_View->GetOscPcData().GetBusIndex();
            pc_BusItem->c_Name = pc_Bus->c_Name.c_str();
            pc_BusItem->q_Selectable = false;
            pc_BusItem->c_ToolTipHeading = pc_Bus->c_Name.c_str();
            pc_BusItem->c_ToolTipContent = pc_Bus->c_Comment.c_str();
            switch (pc_Bus->e_Type)
            {
            case C_OscSystemBus::eCAN:
               pc_BusItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_CAN);
               break;
            case C_OscSystemBus::eETHERNET:
               pc_BusItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_ETHERNET);
               break;
            default:
               tgl_assert(false);
               break;
            }
            //Protocols
            pc_BusItem->ReserveChildrenSpace(static_cast<uint32_t>(C_OscCanProtocol::hc_ALL_PROTOCOLS.size()));

            for (uint8_t u8_ItProtocol = 0U;
                 u8_ItProtocol < static_cast<uint8_t>(C_OscCanProtocol::hc_ALL_PROTOCOLS.size());
                 ++u8_ItProtocol)
            {
               bool q_ProtocolValid;
               C_PuiSdNodeCanMessageSyncManager * const pc_SyncManager = new C_PuiSdNodeCanMessageSyncManager();
               C_OscCanProtocol::E_Type e_Type;
               std::vector<C_OscCanMessageIdentificationIndices> c_UniqueMessages;
               C_TblTreItem * const pc_ProtocolItem = new C_TblTreItem();
               //Init current node
               pc_ProtocolItem->u32_Index = static_cast<uint32_t>(u8_ItProtocol);
               pc_ProtocolItem->q_Selectable = false;
               switch (u8_ItProtocol)
               {
               case 0U:
                  e_Type = C_OscCanProtocol::eLAYER2;
                  break;
               case 1U:
                  e_Type = C_OscCanProtocol::eECES;
                  break;
               case 2U:
                  e_Type = C_OscCanProtocol::eCAN_OPEN_SAFETY;
                  break;
               case 3U:
                  e_Type = C_OscCanProtocol::eCAN_OPEN;
                  break;
               case 4U:
                  e_Type = C_OscCanProtocol::eJ1939;
                  break;
               default:
                  e_Type = C_OscCanProtocol::eLAYER2;
                  tgl_assert(false);
                  break;
               }
               pc_ProtocolItem->c_Name = C_PuiSdUtil::h_ConvertProtocolTypeToString(e_Type);
               pc_ProtocolItem->c_ToolTipHeading = pc_ProtocolItem->c_Name;
               pc_SyncManager->Init(pc_View->GetOscPcData().GetBusIndex(), e_Type);
               c_UniqueMessages = pc_SyncManager->GetUniqueMessages();
               //Flag
               q_ProtocolValid = false;
               //Messages
               if (mh_AddCommMessageItems(pc_ProtocolItem, c_UniqueMessages,
                                          C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL, oq_ShowOnlyWriteElements,
                                          oq_ShowArrayElements, oq_ShowArrayIndexElements, oq_Show64BitValues,
                                          opc_AlreasyUsedElements) == true)
               {
                  q_BusValid = true;
                  q_ProtocolValid = true;
               }
               if (q_ProtocolValid == true)
               {
                  pc_BusItem->AddChild(pc_ProtocolItem);
               }
               else
               {
                  delete (pc_ProtocolItem);
               }
            } //lint !e429  //Deleted at a later point
         }
         if (q_BusValid == true)
         {
            this->mpc_InvisibleRootItem->AddChild(pc_BusItem);
         }
         else
         {
            delete (pc_BusItem);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize tree structure for data pool elements

   \param[in]  ou32_ViewIndex                      View index
   \param[in]  oq_ShowOnlyWriteElements            Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements                Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements           Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues                  Optional flag to hide all 64 bit elements (if false)
   \param[in]  opc_AlreasyUsedElements             Optional pointer to vector with already used elements. All added elements
                                                   will be marked as used an will be disabled
   \param[in]  oq_IsModelUsedInSysViews            True if the model is used for a view in system views, false otherwise
   \param[in]  ou32_SdDataLoggerUseCaseNodeIndex   System definition data logger use case: node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::m_InitDatapoolElements(const uint32_t ou32_ViewIndex,
                                                      const bool oq_ShowOnlyWriteElements,
                                                      const bool oq_ShowArrayElements,
                                                      const bool oq_ShowArrayIndexElements,
                                                      const bool oq_Show64BitValues,
                                                      const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, const bool oq_IsModelUsedInSysViews,
                                                      const uint32_t ou32_SdDataLoggerUseCaseNodeIndex)
{
   std::vector<uint8_t> c_NodeActiveFlags;
   const int32_t s32_Retval = C_TblTreDataElementModel::mh_GetCurrentNodeActiveFlags(ou32_ViewIndex,
                                                                                     oq_IsModelUsedInSysViews,
                                                                                     c_NodeActiveFlags);

   if (s32_Retval == C_NO_ERR)
   {
      bool q_NodeValid;
      bool q_DataPoolDiagValid;
      bool q_DataPoolNvmValid;
      bool q_DataPoolComValid;
      bool q_DataPoolHalcValid;
      bool q_DataPoolValid;
      bool q_ListValid;
      const uint32_t u32_NodeSize = C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();

      //Nodes
      this->mpc_InvisibleRootItem->ReserveChildrenSpace(u32_NodeSize);
      tgl_assert(c_NodeActiveFlags.size() == u32_NodeSize);

      for (uint32_t u32_ItNode = 0; u32_ItNode < u32_NodeSize; ++u32_ItNode)
      {
         bool q_Check;

         // Model used in a different context than system views e.g. system definition
         if (oq_IsModelUsedInSysViews == false)
         {
            q_Check = C_PuiSdUtil::h_CheckXappNodeReachable(ou32_SdDataLoggerUseCaseNodeIndex, u32_ItNode);
         }
         else
         {
            q_Check = mh_SvCheckNodeDiagnostic(ou32_ViewIndex, u32_ItNode);
         }

         if ((c_NodeActiveFlags[u32_ItNode] == 1U) && (q_Check == true))
         {
            C_TblTreItem * const pc_NodeItem = new C_TblTreItem();
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
            q_NodeValid = false;
            //Static
            pc_NodeItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_NODE);
            //Node
            if ((pc_Node != NULL) && (pc_Node->c_Properties.e_DiagnosticServer == C_OscNodeProperties::eDS_OPEN_SYDE))
            {
               //Data pool type nodes
               C_TblTreItem * const pc_DiagItem = new C_TblTreItem();
               C_TblTreItem * const pc_NvmItem = new C_TblTreItem();
               C_TblTreItem * const pc_ComItem = new C_TblTreItem();
               C_TblTreItem * const pc_HalcItem = new C_TblTreItem();
               pc_DiagItem->c_Name = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_Name = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_Name = C_GtGetText::h_GetText("COMM Datapools");
               pc_HalcItem->c_Name = C_GtGetText::h_GetText("HAL Datapools");
               pc_DiagItem->c_ToolTipHeading = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_ToolTipHeading = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_ToolTipHeading = C_GtGetText::h_GetText("COMM Datapools");
               pc_HalcItem->c_ToolTipHeading = C_GtGetText::h_GetText("HAL Datapools");
               pc_DiagItem->q_Selectable = false;
               pc_NvmItem->q_Selectable = false;
               pc_ComItem->q_Selectable = false;
               pc_HalcItem->q_Selectable = false;
               //Use some unique index for expansion restoration mechanism
               pc_DiagItem->u32_Index = 0;
               pc_NvmItem->u32_Index = 1;
               pc_ComItem->u32_Index = 2;
               pc_HalcItem->u32_Index = 3;
               q_DataPoolDiagValid = false;
               q_DataPoolNvmValid = false;
               q_DataPoolComValid = false;
               q_DataPoolHalcValid = false;
               //Init current node
               pc_NodeItem->u32_Index = u32_ItNode;
               pc_NodeItem->q_Selectable = false;
               pc_NodeItem->c_Name = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipHeading = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipContent = pc_Node->c_Properties.c_Comment.c_str();
               //Data pools
               pc_NodeItem->ReserveChildrenSpace(4UL);
               pc_DiagItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               pc_NvmItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               pc_ComItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               pc_HalcItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
               {
                  const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
                  C_TblTreItem * const pc_DataPoolItem = new C_TblTreItem();
                  //Init current Datapool
                  pc_DataPoolItem->u32_Index = u32_ItDataPool;
                  pc_DataPoolItem->c_Name = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipHeading = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipContent = rc_DataPool.c_Comment.c_str();
                  pc_DataPoolItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_DATAPOOL);
                  pc_DataPoolItem->q_Selectable = false;
                  // Append protocol type if COMM datapool
                  if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
                  {
                     pc_DataPoolItem->c_Name += C_GtGetText::h_GetText(" (Protocol: ");
                     pc_DataPoolItem->c_Name +=
                        C_PuiSdUtil::h_ConvertProtocolTypeToString(C_PuiSdUtil::h_GetRelatedCanProtocolType(
                                                                      u32_ItNode, u32_ItDataPool));
                     pc_DataPoolItem->c_Name +=  ")";
                     pc_DataPoolItem->c_ToolTipHeading = pc_DataPoolItem->c_Name;
                  }
                  //Flag
                  //Data pool
                  q_DataPoolValid = false;
                  C_TblTreDataElementModel::mh_InitDatapoolElementsHalc(pc_DataPoolItem, q_DataPoolValid, *pc_Node,
                                                                        rc_DataPool, u32_ItNode, u32_ItDataPool,
                                                                        oq_ShowOnlyWriteElements,
                                                                        oq_ShowArrayElements,
                                                                        oq_ShowArrayIndexElements,
                                                                        oq_Show64BitValues,
                                                                        opc_AlreasyUsedElements);
                  if (q_DataPoolValid == true)
                  {
                     q_NodeValid = true;
                     q_DataPoolHalcValid = true;
                  }
                  else
                  {
                     C_TblTreDataElementModel::mh_InitDatapoolElementsComm(pc_DataPoolItem, q_DataPoolValid, *pc_Node,
                                                                           rc_DataPool, u32_ItNode, u32_ItDataPool,
                                                                           oq_ShowOnlyWriteElements,
                                                                           oq_ShowArrayElements,
                                                                           oq_ShowArrayIndexElements,
                                                                           oq_Show64BitValues,
                                                                           opc_AlreasyUsedElements);
                     if (q_DataPoolValid == true)
                     {
                        q_NodeValid = true;
                        q_DataPoolComValid = true;
                     }
                     else
                     {
                        //Lists (NVM and DIAG only)
                        if ((rc_DataPool.e_Type == C_OscNodeDataPool::eDIAG) ||
                            (rc_DataPool.e_Type == C_OscNodeDataPool::eNVM))
                        {
                           pc_DataPoolItem->ReserveChildrenSpace(rc_DataPool.c_Lists.size());
                           for (uint32_t u32_ItList = 0; u32_ItList < rc_DataPool.c_Lists.size(); ++u32_ItList)
                           {
                              const C_OscNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u32_ItList];
                              C_TblTreItem * const pc_ListItem = new C_TblTreItem();
                              //Init current node
                              pc_ListItem->u32_Index = u32_ItList;
                              pc_ListItem->c_Name = rc_List.c_Name.c_str();
                              pc_ListItem->c_ToolTipHeading = rc_List.c_Name.c_str();
                              // tooltip content: do not use h_GetToolTipContentDpList because we do not want so much
                              // info and
                              // consistency with superior tree items
                              pc_ListItem->c_ToolTipContent = rc_List.c_Comment.c_str();
                              pc_ListItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_LIST);
                              pc_ListItem->q_Selectable = false;
                              //Flag
                              q_ListValid = false;
                              //Elements
                              pc_ListItem->ReserveChildrenSpace(rc_List.c_Elements.size());
                              for (uint32_t u32_ItElement = 0; u32_ItElement < rc_List.c_Elements.size();
                                   ++u32_ItElement)
                              {
                                 const C_PuiSvDbNodeDataPoolListElementId c_Id(
                                    u32_ItNode, u32_ItDataPool, u32_ItList, u32_ItElement,
                                    C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                    false, 0UL, true);
                                 const C_OscNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u32_ItElement];
                                 const bool oq_IsString = rc_Element.q_InterpretAsString;
                                 C_TblTreDataElementItem * const pc_ElementItem =
                                    new C_TblTreDataElementItem(false, rc_Element.GetArray(), rc_Element.c_Name.c_str(),
                                                                rc_Element.GetType(), rc_Element.e_Access, oq_IsString,
                                                                c_Id);
                                 const C_OscNodeDataPoolListElementId c_NodeDpListElement(u32_ItNode, u32_ItDataPool,
                                                                                          u32_ItList, u32_ItElement);
                                 //Init current node
                                 pc_ElementItem->u32_Index = u32_ItElement;
                                 pc_ElementItem->c_ToolTipHeading = rc_Element.c_Name.c_str();
                                 pc_ElementItem->c_ToolTipContent = C_SdUtil::h_GetToolTipContentDpListElement(
                                    c_NodeDpListElement);
                                 //Signal
                                 q_NodeValid = true;
                                 q_DataPoolValid = true;
                                 q_ListValid = true;
                                 //Type
                                 switch (rc_DataPool.e_Type) //lint !e788 //check for diag or nvm was already done
                                 {
                                 case C_OscNodeDataPool::eDIAG:
                                    q_DataPoolDiagValid = true;
                                    pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_VARIABLE);
                                    break;
                                 case C_OscNodeDataPool::eNVM:
                                    q_DataPoolNvmValid = true;
                                    pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_PARAMETER);
                                    break;
                                 default:
                                    break;
                                 }

                                 // Adapt the element item
                                 pc_ElementItem->ConfigureDynamicName(oq_ShowOnlyWriteElements,
                                                                      oq_ShowArrayElements,
                                                                      oq_ShowArrayIndexElements,
                                                                      oq_Show64BitValues,
                                                                      opc_AlreasyUsedElements);

                                 //Array elements and check for usage
                                 C_TblTreDataElementModel::mh_CreateArrayElementNodes(
                                    oq_ShowOnlyWriteElements,
                                    oq_ShowArrayElements,
                                    oq_ShowArrayIndexElements,
                                    oq_Show64BitValues, rc_Element,
                                    oq_IsString, pc_ElementItem,
                                    c_Id,
                                    opc_AlreasyUsedElements);

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
                        }
                     }
                  }
                  if (q_DataPoolValid == true)
                  {
                     switch (rc_DataPool.e_Type)
                     {
                     case C_OscNodeDataPool::eDIAG:
                        pc_DiagItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OscNodeDataPool::eNVM:
                        pc_NvmItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OscNodeDataPool::eCOM:
                        pc_ComItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OscNodeDataPool::eHALC:
                     case C_OscNodeDataPool::eHALC_NVM:
                        pc_HalcItem->AddChild(pc_DataPoolItem);
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
               if (q_DataPoolHalcValid == true)
               {
                  pc_NodeItem->AddChild(pc_HalcItem);
               }
               else
               {
                  delete (pc_HalcItem);
               }
            }
            if (q_NodeValid == true)
            {
               this->mpc_InvisibleRootItem->AddChild(pc_NodeItem);
            }
            else
            {
               delete (pc_NodeItem);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init datapool element HALC

   \param[in,out]  opc_DpItem                   Datapool item
   \param[out]     orq_HalcValid                HALC valid
   \param[in]      orc_Node                     Node
   \param[in]      orc_Dp                       Datapool
   \param[in]      ou32_NodeIndex               Node index
   \param[in]      ou32_DpIndex                 Datapool index
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      opc_AlreasyUsedElements      Optional pointer to vector with already used elements. All added elements
                                                will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_InitDatapoolElementsHalc(C_TblTreItem * const opc_DpItem, bool & orq_HalcValid,
                                                           const C_OscNode & orc_Node, const C_OscNodeDataPool & orc_Dp,
                                                           const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex,
                                                           const bool oq_ShowOnlyWriteElements,
                                                           const bool oq_ShowArrayElements,
                                                           const bool oq_ShowArrayIndexElements,
                                                           const bool oq_Show64BitValues,
                                                           const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   if ((opc_DpItem != NULL) &&
       ((orc_Dp.e_Type == C_OscNodeDataPool::eHALC) || (orc_Dp.e_Type == C_OscNodeDataPool::eHALC_NVM)))
   {
      const C_OscHalcMagicianDatapoolListHandler c_DpHandler(orc_Node.c_HalcConfig, C_OscHalcDefDomain::eVA_PARAM,
                                                             orc_Dp.q_IsSafety);
      uint32_t u32_CounterParam = 0UL;
      uint32_t u32_CounterInput = 0UL;
      uint32_t u32_CounterOutput = 0UL;
      uint32_t u32_CounterStatus = 0UL;
      C_TblTreItem * const pc_UnusedChannelsItem = new C_TblTreItem();
      bool q_UnusedNodeValid = false;
      //Init current channels
      pc_UnusedChannelsItem->u32_Index = 0UL;
      pc_UnusedChannelsItem->c_Name = C_GtGetText::h_GetText("Unused Channels");
      pc_UnusedChannelsItem->c_ToolTipHeading = C_GtGetText::h_GetText("Unused Channels");
      pc_UnusedChannelsItem->c_ToolTipContent = C_GtGetText::h_GetText("Channels which are not used by this datapool");
      pc_UnusedChannelsItem->q_Selectable = false;
      for (uint32_t u32_ItDomain = 0UL; u32_ItDomain < orc_Node.c_HalcConfig.GetDomainSize(); ++u32_ItDomain)
      {
         const C_OscHalcDefDomain * const pc_DomainDef = orc_Node.c_HalcConfig.GetDomainDefDataConst(u32_ItDomain);
         const C_OscHalcConfigDomain * const pc_DomainConfig = orc_Node.c_HalcConfig.GetDomainConfigDataConst(
            u32_ItDomain);
         if ((pc_DomainConfig != NULL) && (pc_DomainDef != NULL))
         {
            const uint32_t u32_RelevantChannels = c_DpHandler.CountRelevantItems(
               pc_DomainConfig->c_ChannelConfigs, pc_DomainConfig->c_DomainConfig);
            if (pc_DomainConfig->c_ChannelConfigs.size() > 0UL)
            {
               uint32_t u32_RelevantChannelIndex = 0UL;
               uint32_t u32_CurCounterParam = u32_CounterParam;
               uint32_t u32_CurCounterInput = u32_CounterInput;
               uint32_t u32_CurCounterOutput = u32_CounterOutput;
               uint32_t u32_CurCounterStatus = u32_CounterStatus;
               for (uint32_t u32_ItConfig = 0UL; u32_ItConfig < pc_DomainConfig->c_ChannelConfigs.size();
                    ++u32_ItConfig)
               {
                  const C_OscHalcConfigChannel & rc_Config = pc_DomainConfig->c_ChannelConfigs[u32_ItConfig];
                  if (c_DpHandler.CheckChanPresent(rc_Config))
                  {
                     //Move section to appropriate node
                     C_TblTreItem * const pc_BaseItem =
                        c_DpHandler.CheckChanRelevant(rc_Config) ? opc_DpItem : pc_UnusedChannelsItem;
                     bool * const pq_Valid =
                        c_DpHandler.CheckChanRelevant(rc_Config) ? &orq_HalcValid : &q_UnusedNodeValid;
                     //Each iteration these should be same value
                     u32_CurCounterParam = u32_CounterParam;
                     u32_CurCounterInput = u32_CounterInput;
                     u32_CurCounterOutput = u32_CounterOutput;
                     u32_CurCounterStatus = u32_CounterStatus;
                     C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfig(pc_BaseItem, *pq_Valid,
                                                                                 *pc_DomainDef, *pc_DomainConfig,
                                                                                 rc_Config,
                                                                                 pc_DomainDef->c_ChannelValues, orc_Dp,
                                                                                 ou32_NodeIndex, ou32_DpIndex,
                                                                                 u32_ItConfig,
                                                                                 u32_RelevantChannelIndex,
                                                                                 u32_CurCounterParam,
                                                                                 u32_CurCounterInput,
                                                                                 u32_CurCounterOutput,
                                                                                 u32_CurCounterStatus,
                                                                                 u32_RelevantChannels,
                                                                                 oq_ShowOnlyWriteElements,
                                                                                 oq_ShowArrayElements,
                                                                                 oq_ShowArrayIndexElements,
                                                                                 oq_Show64BitValues, c_DpHandler,
                                                                                 opc_AlreasyUsedElements);
                     //Iterate channel
                     ++u32_RelevantChannelIndex;
                  }
               }
               //Update global values
               u32_CounterParam = u32_CurCounterParam;
               u32_CounterInput = u32_CurCounterInput;
               u32_CounterOutput = u32_CurCounterOutput;
               u32_CounterStatus = u32_CurCounterStatus;
            }
            else
            {
               if (c_DpHandler.CheckChanPresent(pc_DomainConfig->c_DomainConfig))
               {
                  //Move section to appropriate node
                  C_TblTreItem * const pc_BaseItem =
                     c_DpHandler.CheckChanRelevant(pc_DomainConfig->c_DomainConfig) ? opc_DpItem : pc_UnusedChannelsItem;
                  bool * const pq_Valid =
                     c_DpHandler.CheckChanRelevant(pc_DomainConfig->c_DomainConfig) ? &orq_HalcValid :
                     &q_UnusedNodeValid;
                  C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfig(pc_BaseItem, *pq_Valid,
                                                                              *pc_DomainDef, *pc_DomainConfig,
                                                                              pc_DomainConfig->c_DomainConfig,
                                                                              pc_DomainDef->c_DomainValues, orc_Dp,
                                                                              ou32_NodeIndex, ou32_DpIndex,
                                                                              0UL, 0UL, u32_CounterParam,
                                                                              u32_CounterInput,
                                                                              u32_CounterOutput,
                                                                              u32_CounterStatus,
                                                                              u32_RelevantChannels,
                                                                              oq_ShowOnlyWriteElements,
                                                                              oq_ShowArrayElements,
                                                                              oq_ShowArrayIndexElements,
                                                                              oq_Show64BitValues, c_DpHandler,
                                                                              opc_AlreasyUsedElements);
               }
            }
         }
      }
      //lint -e{774,948} Invalid warning
      if (q_UnusedNodeValid == true)
      {
         opc_DpItem->AddChild(pc_UnusedChannelsItem);
         orq_HalcValid = true;
      }
   } //lint !e429 !e593 //Deleted at a later point
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init datapool elements HALC config

   \param[in,out]  opc_DpItem                   Datapool item
   \param[in,out]  orq_HalcValid                HALC valid
   \param[in]      orc_DomainDef                Domain def
   \param[in]      orc_DomainConfig             Domain config
   \param[in]      orc_ChannelConfig            Channel config
   \param[in]      orc_Values                   Values
   \param[in]      orc_Dp                       Datapool
   \param[in]      ou32_NodeIndex               Node index
   \param[in]      ou32_DpIndex                 Datapool index
   \param[in]      ou32_ChannelIndex            Channel index
   \param[in]      ou32_ChannelArrayIndex       Channel array index
   \param[in,out]  oru32_CounterParam           Counter param
   \param[in,out]  oru32_CounterInput           Counter input
   \param[in,out]  oru32_CounterOutput          Counter output
   \param[in,out]  oru32_CounterStatus          Counter status
   \param[in]      ou32_RelevantChannelNumber   Relevant channel number
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      orc_DpHandler                Dp handler
   \param[in]      opc_AlreasyUsedElements      Optional pointer to vector with already used elements. All added elements
                                                will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfig(C_TblTreItem * const opc_DpItem, bool & orq_HalcValid,
                                                                 const C_OscHalcDefDomain & orc_DomainDef,
                                                                 const C_OscHalcConfigDomain & orc_DomainConfig,
                                                                 const C_OscHalcConfigChannel & orc_ChannelConfig,
                                                                 const C_OscHalcDefChannelValues & orc_Values,
                                                                 const C_OscNodeDataPool & orc_Dp,
                                                                 const uint32_t ou32_NodeIndex,
                                                                 const uint32_t ou32_DpIndex,
                                                                 const uint32_t ou32_ChannelIndex,
                                                                 const uint32_t ou32_ChannelArrayIndex,
                                                                 uint32_t & oru32_CounterParam,
                                                                 uint32_t & oru32_CounterInput,
                                                                 uint32_t & oru32_CounterOutput,
                                                                 uint32_t & oru32_CounterStatus,
                                                                 const uint32_t ou32_RelevantChannelNumber,
                                                                 const bool oq_ShowOnlyWriteElements,
                                                                 const bool oq_ShowArrayElements,
                                                                 const bool oq_ShowArrayIndexElements,
                                                                 const bool oq_Show64BitValues,
                                                                 const stw::opensyde_core::C_OscHalcMagicianDatapoolListHandler & orc_DpHandler,
                                                                 const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   const bool q_ChanNumVarNecessary = orc_DpHandler.CheckChanNumVariableNecessary(orc_DomainConfig);
   const bool q_UseCaseVarNecessary = orc_DpHandler.CheckUseCaseVariableNecessary(orc_DomainConfig);
   const bool q_SafetyFlagVarNecessary = orc_DpHandler.CheckSafetyFlagVariableNecessary();
   C_TblTreItem * const pc_ChannelItem = new C_TblTreItem();
   bool q_ChannelValid = false;
   QString c_HalChannelOrDomainName = orc_DomainDef.c_SingularName.c_str();

   //Init current channel
   pc_ChannelItem->u32_Index = ou32_ChannelIndex;
   pc_ChannelItem->c_Name = orc_ChannelConfig.c_Name.c_str();
   if (ou32_ChannelIndex < orc_DomainDef.c_Channels.size())
   {
      const C_OscHalcDefChannelDef & rc_Channel = orc_DomainDef.c_Channels[ou32_ChannelIndex];
      pc_ChannelItem->c_Name += static_cast<QString>(" (%1)").arg(rc_Channel.c_Name.c_str());
      c_HalChannelOrDomainName = rc_Channel.c_Name.c_str();
   }
   pc_ChannelItem->c_ToolTipHeading = orc_ChannelConfig.c_Name.c_str();
   pc_ChannelItem->c_ToolTipContent = orc_ChannelConfig.c_Comment.c_str();
   switch (orc_DomainDef.e_Category)
   {
   case C_OscHalcDefDomain::eCA_INPUT:
      pc_ChannelItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_HALC_INPUT);
      break;
   case C_OscHalcDefDomain::eCA_OUTPUT:
      pc_ChannelItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_HALC_OUTPUT);
      break;
   case C_OscHalcDefDomain::eCA_OTHER:
      pc_ChannelItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_HALC_MISC);
      break;
   default:
      tgl_assert(false);
      break;
   }
   pc_ChannelItem->q_Enabled = true;
   pc_ChannelItem->q_Selectable = false;
   //Fill
   for (uint32_t u32_ItList = 0UL; u32_ItList < orc_Dp.c_Lists.size(); ++u32_ItList)
   {
      const C_OscNodeDataPoolList & rc_List = orc_Dp.c_Lists[u32_ItList];
      if (rc_List.c_Name == C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_PARAM))
      {
         C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfigList(pc_ChannelItem, q_ChannelValid,
                                                                         orc_Values.c_Parameters, rc_List,
                                                                         ou32_ChannelArrayIndex, ou32_NodeIndex,
                                                                         ou32_DpIndex, u32_ItList,
                                                                         orc_ChannelConfig.u32_UseCaseIndex,
                                                                         oru32_CounterParam, ou32_RelevantChannelNumber,
                                                                         q_ChanNumVarNecessary,
                                                                         q_UseCaseVarNecessary,
                                                                         q_SafetyFlagVarNecessary, true,
                                                                         orc_DomainDef.c_SingularName,
                                                                         oq_ShowOnlyWriteElements,
                                                                         oq_ShowArrayElements,
                                                                         oq_ShowArrayIndexElements,
                                                                         oq_Show64BitValues, opc_AlreasyUsedElements,
                                                                         c_HalChannelOrDomainName);
      }
      else if (rc_List.c_Name == C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_INPUT))
      {
         C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfigList(pc_ChannelItem, q_ChannelValid,
                                                                         orc_Values.c_InputValues, rc_List,
                                                                         ou32_ChannelArrayIndex, ou32_NodeIndex,
                                                                         ou32_DpIndex, u32_ItList,
                                                                         orc_ChannelConfig.u32_UseCaseIndex,
                                                                         oru32_CounterInput, ou32_RelevantChannelNumber,
                                                                         q_ChanNumVarNecessary,
                                                                         q_UseCaseVarNecessary,
                                                                         q_SafetyFlagVarNecessary, false,
                                                                         orc_DomainDef.c_SingularName,
                                                                         oq_ShowOnlyWriteElements,
                                                                         oq_ShowArrayElements,
                                                                         oq_ShowArrayIndexElements,
                                                                         oq_Show64BitValues, opc_AlreasyUsedElements,
                                                                         c_HalChannelOrDomainName);
      }
      else if (rc_List.c_Name == C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_OUTPUT))
      {
         C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfigList(pc_ChannelItem, q_ChannelValid,
                                                                         orc_Values.c_OutputValues, rc_List,
                                                                         ou32_ChannelArrayIndex, ou32_NodeIndex,
                                                                         ou32_DpIndex, u32_ItList,
                                                                         orc_ChannelConfig.u32_UseCaseIndex,
                                                                         oru32_CounterOutput,
                                                                         ou32_RelevantChannelNumber,
                                                                         q_ChanNumVarNecessary,
                                                                         q_UseCaseVarNecessary,
                                                                         q_SafetyFlagVarNecessary, false,
                                                                         orc_DomainDef.c_SingularName,
                                                                         oq_ShowOnlyWriteElements,
                                                                         oq_ShowArrayElements,
                                                                         oq_ShowArrayIndexElements,
                                                                         oq_Show64BitValues, opc_AlreasyUsedElements,
                                                                         c_HalChannelOrDomainName);
      }
      else if (rc_List.c_Name == C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_STATUS))
      {
         C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfigList(pc_ChannelItem, q_ChannelValid,
                                                                         orc_Values.c_StatusValues, rc_List,
                                                                         ou32_ChannelArrayIndex, ou32_NodeIndex,
                                                                         ou32_DpIndex, u32_ItList,
                                                                         orc_ChannelConfig.u32_UseCaseIndex,
                                                                         oru32_CounterStatus,
                                                                         ou32_RelevantChannelNumber,
                                                                         q_ChanNumVarNecessary,
                                                                         q_UseCaseVarNecessary,
                                                                         q_SafetyFlagVarNecessary, false,
                                                                         orc_DomainDef.c_SingularName,
                                                                         oq_ShowOnlyWriteElements,
                                                                         oq_ShowArrayElements,
                                                                         oq_ShowArrayIndexElements,
                                                                         oq_Show64BitValues, opc_AlreasyUsedElements,
                                                                         c_HalChannelOrDomainName);
      }
      else
      {
         //Unexpected list
         tgl_assert(false);
      }
   }
   //Append
   if (q_ChannelValid)
   {
      orq_HalcValid = true;
      opc_DpItem->AddChild(pc_ChannelItem);
   }
   else
   {
      delete (pc_ChannelItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init datapool elements HALC config list

   \param[in,out]  opc_ChannelItem              Channel item
   \param[out]     orq_ChannelValid             Channel valid
   \param[in]      orc_Values                   Values
   \param[in]      orc_List                     List
   \param[in]      ou32_ChannelArrayIndex       Channel array index
   \param[in]      ou32_NodeIndex               Node index
   \param[in]      ou32_DpIndex                 Datapool index
   \param[in]      ou32_ListIndex               List index
   \param[in]      ou32_UseCase                 Use case
   \param[in,out]  oru32_Counter                Counter
   \param[in]      ou32_RelevantChannelNumber   Relevant channel number
   \param[in]      oq_ChanNumVarNecessary       Chan num variable necessary
   \param[in]      oq_UseCaseVarNecessary       Use case variable necessary
   \param[in]      oq_SafetyFlagVarNecessary    Safety flag var necessary
   \param[in]      oq_IsParam                   Is parameter
   \param[in]      orc_DomainSingularName       Domain singular name
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      opc_AlreasyUsedElements      Optional pointer to vector with already used elements. All added elements
                                                will be marked as used an will be disabled
   \param[in]      orc_HalChannelOrDomainName   Hal channel or domain name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_InitDatapoolElementsHalcConfigList(C_TblTreItem * const opc_ChannelItem,
                                                                     bool & orq_ChannelValid,
                                                                     const std::vector<C_OscHalcDefStruct> & orc_Values,
                                                                     const C_OscNodeDataPoolList & orc_List,
                                                                     const uint32_t ou32_ChannelArrayIndex,
                                                                     const uint32_t ou32_NodeIndex,
                                                                     const uint32_t ou32_DpIndex,
                                                                     const uint32_t ou32_ListIndex,
                                                                     const uint32_t ou32_UseCase,
                                                                     uint32_t & oru32_Counter,
                                                                     const uint32_t ou32_RelevantChannelNumber,
                                                                     const bool oq_ChanNumVarNecessary,
                                                                     const bool oq_UseCaseVarNecessary,
                                                                     const bool oq_SafetyFlagVarNecessary,
                                                                     const bool oq_IsParam,
                                                                     const stw::scl::C_SclString & orc_DomainSingularName, const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues, const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements,
                                                                     const QString & orc_HalChannelOrDomainName)
{
   C_TblTreItem * const pc_ListItem = new C_TblTreItem();
   C_TblTreItem * const pc_OtherItem = new C_TblTreItem();
   bool q_ListValid = false;
   bool q_OtherValid = false;

   //Init current list
   pc_ListItem->u32_Index = ou32_ListIndex;
   pc_ListItem->c_Name = orc_List.c_Name.c_str();
   pc_ListItem->c_ToolTipHeading = orc_List.c_Name.c_str();
   pc_ListItem->c_ToolTipContent = orc_List.c_Comment.c_str();
   pc_ListItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_LIST);
   pc_ListItem->q_Enabled = true;
   pc_ListItem->q_Selectable = false;
   //Init other item
   pc_OtherItem->u32_Index = 0UL;
   pc_OtherItem->c_Name = C_GtGetText::h_GetText("Unused Dataelements");
   pc_OtherItem->c_ToolTipHeading = C_GtGetText::h_GetText("Unused Dataelements");
   pc_OtherItem->c_ToolTipContent = C_GtGetText::h_GetText(
      "Unused Dataelements which are not relevant for current use-case");
   pc_OtherItem->q_Enabled = true;
   pc_OtherItem->q_Selectable = false;
   //Fill
   if (oq_IsParam)
   {
      if (oq_ChanNumVarNecessary)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id(ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex, oru32_Counter,
                                                 C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                 ou32_RelevantChannelNumber != 1UL, ou32_ChannelArrayIndex, true);
         c_Id.SetHalChannelName(orc_HalChannelOrDomainName.toStdString());
         C_TblTreDataElementModel::mh_AddHalcTreeItem(pc_ListItem,
                                                      C_OscHalcMagicianUtil::h_GetChanNumVariableName(
                                                         orc_DomainSingularName).c_str(), c_Id, ou32_RelevantChannelNumber != 1UL, false,
                                                      oq_ShowOnlyWriteElements,
                                                      oq_ShowArrayElements,
                                                      oq_ShowArrayIndexElements,
                                                      oq_Show64BitValues,
                                                      opc_AlreasyUsedElements);
         //Iterate
         ++oru32_Counter;
         //Mark valid
         q_ListValid = true;
      }
      if (oq_SafetyFlagVarNecessary)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id(ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex, oru32_Counter,
                                                 C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                 ou32_RelevantChannelNumber != 1UL, ou32_ChannelArrayIndex, true);
         c_Id.SetHalChannelName(orc_HalChannelOrDomainName.toStdString());
         C_TblTreDataElementModel::mh_AddHalcTreeItem(pc_ListItem,
                                                      C_OscHalcMagicianUtil::h_GetSafetyFlagVariableName(
                                                         orc_DomainSingularName).c_str(), c_Id, ou32_RelevantChannelNumber != 1UL, false,
                                                      oq_ShowOnlyWriteElements,
                                                      oq_ShowArrayElements,
                                                      oq_ShowArrayIndexElements,
                                                      oq_Show64BitValues,
                                                      opc_AlreasyUsedElements);
         //Iterate
         ++oru32_Counter;
         //Mark valid
         q_ListValid = true;
      }
      if (oq_UseCaseVarNecessary)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id(ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex, oru32_Counter,
                                                 C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                 ou32_RelevantChannelNumber != 1UL, ou32_ChannelArrayIndex, true);
         c_Id.SetHalChannelName(orc_HalChannelOrDomainName.toStdString());
         C_TblTreDataElementModel::mh_AddHalcTreeItem(pc_ListItem,
                                                      C_OscHalcMagicianUtil::h_GetUseCaseVariableName(
                                                         orc_DomainSingularName).c_str(), c_Id, ou32_RelevantChannelNumber != 1UL, false,
                                                      oq_ShowOnlyWriteElements,
                                                      oq_ShowArrayElements,
                                                      oq_ShowArrayIndexElements,
                                                      oq_Show64BitValues,
                                                      opc_AlreasyUsedElements);
         //Iterate
         ++oru32_Counter;
         //Mark valid
         q_ListValid = true;
      }
   }
   for (uint32_t u32_ItStruct = 0UL; u32_ItStruct < orc_Values.size(); ++u32_ItStruct)
   {
      const C_OscHalcDefStruct & rc_Struct = orc_Values[u32_ItStruct];
      bool q_IsRelevant = false;
      if (rc_Struct.c_UseCaseAvailabilities.size() > 0UL)
      {
         for (uint32_t u32_ItUseCase = 0UL; u32_ItUseCase < rc_Struct.c_UseCaseAvailabilities.size(); ++u32_ItUseCase)
         {
            if (rc_Struct.c_UseCaseAvailabilities[u32_ItUseCase] == ou32_UseCase)
            {
               q_IsRelevant = true;
               break;
            }
         }
      }
      else
      {
         q_IsRelevant = true;
      }
      if (rc_Struct.c_StructElements.size() > 0UL)
      {
         for (uint32_t u32_ItEl = 0UL; u32_ItEl < rc_Struct.c_StructElements.size(); ++u32_ItEl)
         {
            if (q_IsRelevant)
            {
               C_TblTreDataElementModel::mh_AddHalcItem(pc_ListItem, rc_Struct.c_StructElements[u32_ItEl],
                                                        ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex,
                                                        oru32_Counter, ou32_RelevantChannelNumber,
                                                        ou32_ChannelArrayIndex,
                                                        oq_ShowOnlyWriteElements,
                                                        oq_ShowArrayElements,
                                                        oq_ShowArrayIndexElements,
                                                        oq_Show64BitValues, opc_AlreasyUsedElements,
                                                        orc_HalChannelOrDomainName);
            }
            else
            {
               C_TblTreDataElementModel::mh_AddHalcItem(pc_OtherItem, rc_Struct.c_StructElements[u32_ItEl],
                                                        ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex,
                                                        oru32_Counter, ou32_RelevantChannelNumber,
                                                        ou32_ChannelArrayIndex,
                                                        oq_ShowOnlyWriteElements,
                                                        oq_ShowArrayElements,
                                                        oq_ShowArrayIndexElements,
                                                        oq_Show64BitValues, opc_AlreasyUsedElements,
                                                        orc_HalChannelOrDomainName);
               //Mark valid
               q_OtherValid = true;
            }
            //Mark valid
            q_ListValid = true;
         }
      }
      else
      {
         if (q_IsRelevant)
         {
            C_TblTreDataElementModel::mh_AddHalcItem(pc_ListItem, rc_Struct,
                                                     ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex,
                                                     oru32_Counter, ou32_RelevantChannelNumber, ou32_ChannelArrayIndex,
                                                     oq_ShowOnlyWriteElements,
                                                     oq_ShowArrayElements,
                                                     oq_ShowArrayIndexElements,
                                                     oq_Show64BitValues, opc_AlreasyUsedElements,
                                                     orc_HalChannelOrDomainName);
         }
         else
         {
            C_TblTreDataElementModel::mh_AddHalcItem(pc_OtherItem, rc_Struct,
                                                     ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex,
                                                     oru32_Counter, ou32_RelevantChannelNumber, ou32_ChannelArrayIndex,
                                                     oq_ShowOnlyWriteElements,
                                                     oq_ShowArrayElements,
                                                     oq_ShowArrayIndexElements,
                                                     oq_Show64BitValues, opc_AlreasyUsedElements,
                                                     orc_HalChannelOrDomainName);
            //Mark valid
            q_OtherValid = true;
         }
         //Mark valid
         q_ListValid = true;
      }
   }
   //Append
   if (q_OtherValid)
   {
      pc_ListItem->AddChild(pc_OtherItem);
   }
   else
   {
      delete (pc_OtherItem);
   }
   //Append
   if (q_ListValid)
   {
      orq_ChannelValid = true;
      opc_ChannelItem->AddChild(pc_ListItem);
   }
   else
   {
      delete (pc_ListItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add HALC item

   \param[in,out]  opc_BaseItem                 Base item
   \param[in]      orc_HalcItem                 HALC item
   \param[in]      ou32_NodeIndex               Node index
   \param[in]      ou32_DpIndex                 Datapool index
   \param[in]      ou32_ListIndex               List index
   \param[in,out]  oru32_Counter                Counter
   \param[in]      ou32_RelevantChannelNumber   Relevant channel number
   \param[in]      ou32_ChannelArrayIndex       Channel array index
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      opc_AlreasyUsedElements      Alreasy used elements
   \param[in]      orc_HalChannelOrDomainName   Hal channel or domain name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_AddHalcItem(C_TblTreItem * const opc_BaseItem,
                                              const C_OscHalcDefElement & orc_HalcItem, const uint32_t ou32_NodeIndex,
                                              const uint32_t ou32_DpIndex, const uint32_t ou32_ListIndex,
                                              uint32_t & oru32_Counter, const uint32_t ou32_RelevantChannelNumber,
                                              const uint32_t ou32_ChannelArrayIndex,
                                              const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                              const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                              const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements,
                                              const QString & orc_HalChannelOrDomainName)
{
   C_PuiSvDbNodeDataPoolListElementId c_Id(ou32_NodeIndex, ou32_DpIndex, ou32_ListIndex, oru32_Counter,
                                           C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                           (orc_HalcItem.GetComplexType() == C_OscHalcDefContent::eCT_STRING) || (ou32_RelevantChannelNumber != 1UL), ou32_ChannelArrayIndex,
                                           true);

   c_Id.SetHalChannelName(orc_HalChannelOrDomainName.toStdString());

   C_TblTreDataElementModel::mh_AddHalcTreeItem(opc_BaseItem, orc_HalcItem.c_Display.c_str(), c_Id,
                                                (orc_HalcItem.GetComplexType() == C_OscHalcDefContent::eCT_STRING) ||
                                                (ou32_RelevantChannelNumber != 1UL),
                                                orc_HalcItem.GetComplexType() == C_OscHalcDefContent::eCT_STRING,
                                                oq_ShowOnlyWriteElements,
                                                oq_ShowArrayElements,
                                                oq_ShowArrayIndexElements,
                                                oq_Show64BitValues,
                                                opc_AlreasyUsedElements);
   //Iterate
   oru32_Counter += C_OscHalcMagicianDatapoolListHandler::h_CountElements(orc_HalcItem, ou32_RelevantChannelNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add HALC item

   \param[in,out]  opc_ListItem                 List item
   \param[in]      orc_Name                     Name
   \param[in]      orc_Id                       Id
   \param[in]      oq_IsArray                   Is array
   \param[in]      oq_IsString                  Is string
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      opc_AlreasyUsedElements      Optional pointer to vector with already used elements. All added elements
                                                will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_AddHalcTreeItem(C_TblTreItem * const opc_ListItem, const QString & orc_Name,
                                                  const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                                  const bool oq_IsArray, const bool oq_IsString,
                                                  const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                                  const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                                  const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id);

   if (pc_OscElement != NULL)
   {
      C_TblTreDataElementItem * const pc_ElementItem =
         new C_TblTreDataElementItem(oq_IsArray && (!oq_IsString), oq_IsArray, orc_Name,
                                     pc_OscElement->GetType(), pc_OscElement->e_Access, oq_IsString, orc_Id);

      //Init current node
      pc_ElementItem->u32_Index = orc_Id.u32_ElementIndex;
      pc_ElementItem->c_ToolTipHeading = orc_Name;
      pc_ElementItem->c_ToolTipContent = C_SdUtil::h_GetToolTipContentDpListElement(orc_Id);

      // Adapt the element item
      pc_ElementItem->ConfigureDynamicName(oq_ShowOnlyWriteElements,
                                           oq_ShowArrayElements,
                                           oq_ShowArrayIndexElements,
                                           oq_Show64BitValues,
                                           opc_AlreasyUsedElements);
      //Append
      opc_ListItem->AddChild(pc_ElementItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize COMM Datapool elements

   \param[in,out]  opc_DpItem                   Datapool tree item
   \param[out]     orq_ComValid                 COM Datapool valid
   \param[in]      orc_Node                     Node
   \param[in]      orc_Dp                       Datapool
   \param[in]      ou32_NodeIndex               Node index
   \param[in]      ou32_DpIndex                 Datapool index
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      opc_AlreasyUsedElements      Optional pointer to vector with already used elements. All added elements
                                                will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_InitDatapoolElementsComm(C_TblTreItem * const opc_DpItem, bool & orq_ComValid,
                                                           const C_OscNode & orc_Node, const C_OscNodeDataPool & orc_Dp,
                                                           const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex,
                                                           const bool oq_ShowOnlyWriteElements,
                                                           const bool oq_ShowArrayElements,
                                                           const bool oq_ShowArrayIndexElements,
                                                           const bool oq_Show64BitValues,
                                                           const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   if ((opc_DpItem != NULL) && (orc_Dp.e_Type == C_OscNodeDataPool::eCOM))
   {
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> c_MessageIds;
      C_OscCanProtocol::E_Type e_Protocol = C_OscCanProtocol::eLAYER2;

      tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanProtocolType(ou32_NodeIndex, ou32_DpIndex,
                                                                     e_Protocol) == C_NO_ERR);

      // get all message ids
      for (uint32_t u32_ItInterface = 0; u32_ItInterface < orc_Node.c_Properties.c_ComInterfaces.size();
           u32_ItInterface++)
      {
         const C_OscNodeComInterfaceSettings & rc_CurInterface = orc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
         if (rc_CurInterface.e_InterfaceType == C_OscSystemBus::eCAN)
         {
            const std::vector<const C_OscCanMessageContainer *> c_Containers =
               C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainers(ou32_NodeIndex, e_Protocol,
                                                                                u32_ItInterface);
            for (std::vector<const C_OscCanMessageContainer *>::const_iterator c_It = c_Containers.begin();
                 c_It != c_Containers.end(); ++c_It)
            {
               const C_OscCanMessageContainer * const pc_Container = *c_It;
               if (pc_Container != NULL)
               {
                  // Add Tx message identification indices
                  for (uint32_t u32_ItTxMessage = 0; u32_ItTxMessage < pc_Container->c_TxMessages.size();
                       u32_ItTxMessage++)
                  {
                     const C_OscCanMessageIdentificationIndices c_MessageId(ou32_NodeIndex, e_Protocol, u32_ItInterface,
                                                                            ou32_DpIndex, true, u32_ItTxMessage);
                     c_MessageIds.push_back(c_MessageId);
                  }

                  // Add Rx message identification indices
                  for (uint32_t u32_ItRxMessage = 0; u32_ItRxMessage < pc_Container->c_RxMessages.size();
                       u32_ItRxMessage++)
                  {
                     const C_OscCanMessageIdentificationIndices c_MessageId(ou32_NodeIndex, e_Protocol, u32_ItInterface,
                                                                            ou32_DpIndex, false, u32_ItRxMessage);
                     c_MessageIds.push_back(c_MessageId);
                  }
               }
            }
         }
      }

      // Finally add all messages to tree
      if (C_TblTreDataElementModel::mh_AddCommMessageItems(opc_DpItem, c_MessageIds,
                                                           C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                           oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                           oq_ShowArrayIndexElements, oq_Show64BitValues,
                                                           opc_AlreasyUsedElements) == true)
      {
         orq_ComValid = true;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add COMM message item

   \param[in,out]  opc_BaseItem                 Base item
   \param[in]      orc_MessageIds               Message identification indices
   \param[in]      oe_IdType                    Id type
   \param[in]      oq_ShowOnlyWriteElements     Show only write elements
   \param[in]      oq_ShowArrayElements         Show array elements
   \param[in]      oq_ShowArrayIndexElements    Show array index elements
   \param[in]      oq_Show64BitValues           Show64 bit values
   \param[in]      opc_AlreasyUsedElements      Optional pointer to vector with already used elements. All added elements
                                                will be marked as used an will be disabled

   \retval  true     At least one message with at least one signal was added to opc_BaseItem
   \retval  false    Nothing was added.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementModel::mh_AddCommMessageItems(C_TblTreItem * const opc_BaseItem, const std::vector<C_OscCanMessageIdentificationIndices> &
                                                      orc_MessageIds,
                                                      const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_IdType,
                                                      const bool oq_ShowOnlyWriteElements,
                                                      const bool oq_ShowArrayElements,
                                                      const bool oq_ShowArrayIndexElements,
                                                      const bool oq_Show64BitValues,
                                                      const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   bool q_Valid = false;

   if (opc_BaseItem != NULL)
   {
      opc_BaseItem->ReserveChildrenSpace(orc_MessageIds.size());
      for (uint32_t u32_ItMessage = 0U; u32_ItMessage < orc_MessageIds.size(); ++u32_ItMessage)
      {
         C_TblTreItem * const pc_MessageItem = new C_TblTreItem();
         const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
            orc_MessageIds[u32_ItMessage]);
         //Flag
         bool q_MessageValid = false;
         if (pc_Message != NULL)
         {
            //Init current node
            pc_MessageItem->u32_Index = u32_ItMessage;
            pc_MessageItem->c_Name =
               static_cast<QString>("%1 (0x%2)").arg(pc_Message->c_Name.c_str()).arg(QString::number(pc_Message->
                                                                                                     u32_CanId, 16));
            pc_MessageItem->c_ToolTipHeading = pc_MessageItem->c_Name;
            pc_MessageItem->c_ToolTipContent =
               C_SdUtil::h_GetToolTipContentMessage(orc_MessageIds[u32_ItMessage]);
            pc_MessageItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_MESSAGE);
            pc_MessageItem->q_Selectable = false;
            //Signals
            pc_MessageItem->ReserveChildrenSpace(pc_Message->c_Signals.size());
            for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
            {
               const C_OscNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(orc_MessageIds[u32_ItMessage],
                                                                                u32_ItSignal);
               if (pc_Element != NULL)
               {
                  C_TblTreDataElementItem * const pc_ElementItem =
                     new C_TblTreDataElementItem(false, false, pc_Element->c_Name.c_str(),
                                                 pc_Element->GetType(), pc_Element->e_Access, false,
                                                 mh_Translate(orc_MessageIds[u32_ItMessage], u32_ItSignal, oe_IdType));
                  //Init current node
                  pc_ElementItem->u32_Index = u32_ItSignal;
                  pc_ElementItem->c_Name = pc_Element->c_Name.c_str();
                  pc_ElementItem->c_ToolTipHeading = pc_Element->c_Name.c_str();
                  pc_ElementItem->c_ToolTipContent =
                     C_SdUtil::h_GetToolTipContentSignal(orc_MessageIds[u32_ItMessage], u32_ItSignal);
                  pc_ElementItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_SIGNAL);
                  pc_ElementItem->ConfigureDynamicName(oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                       oq_ShowArrayIndexElements, oq_Show64BitValues,
                                                       opc_AlreasyUsedElements);
                  //Signal found
                  q_Valid = true;
                  q_MessageValid = true;
                  //Add
                  pc_MessageItem->AddChild(pc_ElementItem);
               }
            }
         }
         if (q_MessageValid == true)
         {
            opc_BaseItem->AddChild(pc_MessageItem);
         }
         else
         {
            delete (pc_MessageItem);
         }
      }
   }
   return q_Valid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapts the properties of already existing tree structure for data pool elements

   \param[in]  oq_ShowOnlyWriteElements   Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements       Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements  Show array index elements
   \param[in]  oq_Show64BitValues         Optional flag to hide all 64 bit elements (if false)
   \param[in]  opc_AlreasyUsedElements    Optional pointer to vector with already used elements. All added elements
                                          will be marked as used an will be disabled
   \param[in]  opc_Tree                   Tree
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_UpdateDatapoolElement(const bool oq_ShowOnlyWriteElements,
                                                        const bool oq_ShowArrayElements,
                                                        const bool oq_ShowArrayIndexElements,
                                                        const bool oq_Show64BitValues,
                                                        const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements,
                                                        C_TblTreSimpleItem * const opc_Tree)
{
   if (opc_Tree != NULL)
   {
      C_TblTreDataElementItem * const pc_AdaptableItem = dynamic_cast<C_TblTreDataElementItem * const>(opc_Tree);
      if (pc_AdaptableItem != NULL)
      {
         pc_AdaptableItem->ConfigureDynamicName(oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                oq_ShowArrayIndexElements, oq_Show64BitValues, opc_AlreasyUsedElements);
      }
      for (uint32_t u32_It = 0UL; u32_It < opc_Tree->c_Children.size(); ++u32_It)
      {
         C_TblTreDataElementModel::mh_UpdateDatapoolElement(oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                                            oq_ShowArrayIndexElements, oq_Show64BitValues,
                                                            opc_AlreasyUsedElements,
                                                            opc_Tree->c_Children[u32_It]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle array element section

   \param[in]  oq_ShowOnlyWriteElements   Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements       Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements  Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues         Optional flag to hide all 64 bit elements (if false)
   \param[in]  orc_Element                Datapool element of tree item
   \param[in]  oq_IsStringElement         Flag if current item is an string element
   \param[in]  opc_ElementItem            Tree item for datapool element
   \param[in]  orc_ParentId               Parent id
   \param[in]  opc_AlreasyUsedElements    Optional pointer to vector with already used elements. All added elements
                                          will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_CreateArrayElementNodes(const bool oq_ShowOnlyWriteElements,
                                                          const bool oq_ShowArrayElements,
                                                          const bool oq_ShowArrayIndexElements,
                                                          const bool oq_Show64BitValues,
                                                          const C_OscNodeDataPoolListElement & orc_Element,
                                                          const bool oq_IsStringElement,
                                                          C_TblTreItem * const opc_ElementItem,
                                                          const C_PuiSvDbNodeDataPoolListElementId & orc_ParentId,
                                                          const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   if (opc_ElementItem != NULL)
   {
      if ((orc_Element.GetArray()) && (oq_IsStringElement == false))
      {
         //Create new
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize();
              ++u32_ArrayIndex)
         {
            const C_PuiSvDbNodeDataPoolListElementId c_Id(orc_ParentId.u32_NodeIndex, orc_ParentId.u32_DataPoolIndex,
                                                          orc_ParentId.u32_ListIndex, orc_ParentId.u32_ElementIndex,
                                                          C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                          true, u32_ArrayIndex, true);
            C_TblTreDataElementItem * const pc_ArrayItem =
               new C_TblTreDataElementItem(true, false, static_cast<QString>("[%1]").arg(u32_ArrayIndex),
                                           orc_Element.GetType(), orc_Element.e_Access, false, c_Id);
            //Init current node
            pc_ArrayItem->u32_Index = u32_ArrayIndex;
            pc_ArrayItem->c_Name = static_cast<QString>("[%1]").arg(u32_ArrayIndex);
            pc_ArrayItem->c_ToolTipHeading = pc_ArrayItem->c_Name;
            pc_ArrayItem->c_ToolTipContent =
               static_cast<QString>(C_GtGetText::h_GetText("Array element %1 of \"%2\"")).arg(u32_ArrayIndex).arg(
                  orc_Element.c_Name.c_str());

            //Configure
            pc_ArrayItem->ConfigureDynamicName(oq_ShowOnlyWriteElements,
                                               oq_ShowArrayElements,
                                               oq_ShowArrayIndexElements,
                                               oq_Show64BitValues,
                                               opc_AlreasyUsedElements);

            //Add
            opc_ElementItem->AddChild(pc_ArrayItem);
         }
      }
      else
      {
         opc_ElementItem->ClearChildren();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize tree structure for data pool lists

   \param[in]  ou32_ViewIndex    View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::m_InitNvmList(const uint32_t ou32_ViewIndex)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   std::vector<uint8_t> c_NodeActiveFlags;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      ou32_ViewIndex,
      c_NodeActiveFlags);

   if ((pc_View != NULL) &&
       (s32_Retval == C_NO_ERR))
   {
      bool q_NodeValid;
      bool q_DataPoolDiagValid;
      bool q_DataPoolNvmValid;
      bool q_DataPoolComValid;
      bool q_DataPoolHalcValid;
      bool q_DataPoolValid;
      const uint32_t u32_NodeSize = C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();

      //Nodes
      this->mpc_InvisibleRootItem->ReserveChildrenSpace(u32_NodeSize);
      tgl_assert(c_NodeActiveFlags.size() == u32_NodeSize);
      for (uint32_t u32_ItNode = 0; u32_ItNode < u32_NodeSize; ++u32_ItNode)
      {
         if ((c_NodeActiveFlags[u32_ItNode] == 1U) && (mh_SvCheckNodeDiagnostic(ou32_ViewIndex, u32_ItNode) == true))
         {
            C_TblTreItem * const pc_NodeItem = new C_TblTreItem();
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
            q_NodeValid = false;
            //Static
            pc_NodeItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_NODE);
            //Node
            if ((pc_Node != NULL) && (pc_Node->c_Properties.e_DiagnosticServer == C_OscNodeProperties::eDS_OPEN_SYDE))
            {
               //Data pool type nodes
               C_TblTreItem * const pc_DiagItem = new C_TblTreItem();
               C_TblTreItem * const pc_NvmItem = new C_TblTreItem();
               C_TblTreItem * const pc_ComItem = new C_TblTreItem();
               C_TblTreItem * const pc_HalcItem = new C_TblTreItem();
               pc_DiagItem->c_Name = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_Name = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_Name = C_GtGetText::h_GetText("COMM Datapools");
               pc_HalcItem->c_Name = C_GtGetText::h_GetText("HAL Datapools");
               pc_DiagItem->c_ToolTipHeading = C_GtGetText::h_GetText("DIAG Datapools");
               pc_NvmItem->c_ToolTipHeading = C_GtGetText::h_GetText("NVM Datapools");
               pc_ComItem->c_ToolTipHeading = C_GtGetText::h_GetText("COMM Datapools");
               pc_HalcItem->c_ToolTipHeading = C_GtGetText::h_GetText("HAL Datapools");
               pc_DiagItem->q_Selectable = false;
               pc_NvmItem->q_Selectable = false;
               pc_ComItem->q_Selectable = false;
               pc_HalcItem->q_Selectable = false;
               //Probably invalid index to detect errors
               pc_DiagItem->u32_Index = 12345678;
               pc_NvmItem->u32_Index = 12345678;
               pc_ComItem->u32_Index = 12345678;
               pc_HalcItem->u32_Index = 12345678;
               q_DataPoolDiagValid = false;
               q_DataPoolNvmValid = false;
               q_DataPoolComValid = false;
               q_DataPoolHalcValid = false;
               //Init current node
               pc_NodeItem->u32_Index = u32_ItNode;
               pc_NodeItem->q_Selectable = false;
               pc_NodeItem->c_Name = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipHeading = pc_Node->c_Properties.c_Name.c_str();
               pc_NodeItem->c_ToolTipContent = pc_Node->c_Properties.c_Comment.c_str();
               //Data pools
               pc_NodeItem->ReserveChildrenSpace(3UL);
               pc_DiagItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               pc_NvmItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               pc_ComItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               pc_HalcItem->ReserveChildrenSpace(pc_Node->c_DataPools.size());
               for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
               {
                  const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
                  C_TblTreItem * const pc_DataPoolItem = new C_TblTreItem();
                  //Init current node
                  pc_DataPoolItem->u32_Index = u32_ItDataPool;
                  pc_DataPoolItem->q_Selectable = false;
                  pc_DataPoolItem->c_Name = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipHeading = rc_DataPool.c_Name.c_str();
                  pc_DataPoolItem->c_ToolTipContent = rc_DataPool.c_Comment.c_str();
                  pc_DataPoolItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_DATAPOOL);
                  //Flag
                  q_DataPoolValid = false;
                  //Data pool
                  //Lists
                  pc_DataPoolItem->ReserveChildrenSpace(rc_DataPool.c_Lists.size());
                  for (uint32_t u32_ItList = 0; u32_ItList < rc_DataPool.c_Lists.size(); ++u32_ItList)
                  {
                     const C_OscNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u32_ItList];
                     C_TblTreItem * const pc_ListItem = new C_TblTreItem();
                     //Init current node
                     pc_ListItem->u32_Index = u32_ItList;
                     pc_ListItem->c_Name = rc_List.c_Name.c_str();
                     pc_ListItem->c_ToolTipHeading = rc_List.c_Name.c_str();
                     pc_ListItem->c_ToolTipContent =
                        C_SdUtil::h_GetToolTipContentDpList(u32_ItNode, u32_ItDataPool, u32_ItList);
                     pc_ListItem->c_Icon = QIcon(C_TblTreDataElementModel::mhc_ICON_LIST);
                     //Elements
                     if (rc_List.c_Elements.size() > 0)
                     {
                        //Signal
                        q_NodeValid = true;
                        q_DataPoolValid = true;
                        if ((rc_DataPool.e_Type == C_OscNodeDataPool::eNVM) ||
                            (rc_DataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
                        {
                           //Check usage
                           if (pc_View->CheckNvmParamListUsage(C_OscNodeDataPoolListId(u32_ItNode, u32_ItDataPool,
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
                           if (rc_DataPool.e_Type == C_OscNodeDataPool::eNVM)
                           {
                              q_DataPoolNvmValid = true;
                           }
                           else
                           {
                              q_DataPoolHalcValid = true;
                           }
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
                     case C_OscNodeDataPool::eDIAG:
                        pc_DiagItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OscNodeDataPool::eNVM:
                        pc_NvmItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OscNodeDataPool::eCOM:
                        pc_ComItem->AddChild(pc_DataPoolItem);
                        break;
                     case C_OscNodeDataPool::eHALC:
                     case C_OscNodeDataPool::eHALC_NVM:
                        pc_HalcItem->AddChild(pc_DataPoolItem);
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
               } //lint !e593  //no memory leak because of the Qt memory management
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
               if (q_DataPoolHalcValid == true)
               {
                  pc_NodeItem->AddChild(pc_HalcItem);
               }
               else
               {
                  delete (pc_HalcItem);
               }
            }
            if (q_NodeValid == true)
            {
               this->mpc_InvisibleRootItem->AddChild(pc_NodeItem);
            }
            else
            {
               delete (pc_NodeItem);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data pools

   \param[in]  orc_Index   Index

   \return
   Data pools
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetDatapools(const QModelIndex & orc_Index)
const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{9079}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem *>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreItem * const pc_FirstParent = dynamic_cast<const C_TblTreItem * const>(pc_TreeItem->pc_Parent);
      if ((pc_FirstParent != NULL) && (pc_FirstParent->pc_Parent != NULL))
      {
         const C_TblTreItem * const pc_SecondParent =
            dynamic_cast<const C_TblTreItem * const>(pc_FirstParent->pc_Parent);
         if ((pc_SecondParent != NULL) && (pc_SecondParent->pc_Parent != NULL))
         {
            const C_TblTreItem * const pc_ThirdParent =
               dynamic_cast<const C_TblTreItem * const>(pc_SecondParent->pc_Parent);
            if ((pc_ThirdParent != NULL) && (pc_ThirdParent->pc_Parent != NULL))
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
               c_Retval.emplace_back(C_PuiSvDbNodeDataPoolListElementId(pc_SecondParent->u32_Index,
                                                                        pc_TreeItem->u32_Index, 0UL, 0UL,
                                                                        C_PuiSvDbNodeDataPoolListElementId::
                                                                        eDATAPOOL_ELEMENT, false, 0UL));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get any data pool elements

   \param[in]  orc_Index   Index

   \return
   Any data pool elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetAnyDatapoolElements(
   const QModelIndex & orc_Index) const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{9079}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem *>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreDataElementItem * const pc_DataElementItem =
         dynamic_cast<const C_TblTreDataElementItem * const>(pc_TreeItem);
      if (pc_DataElementItem != NULL)
      {
         c_Retval.push_back(pc_DataElementItem->GetId());
      }
      else
      {
         //Parent, currently not supported
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data pool elements

   \param[in]  orc_Index   Index

   \return
   Data pool elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementModel::m_GetNvmList(const QModelIndex & orc_Index)
const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   //lint -e{9079}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem =
      static_cast<const C_TblTreItem *>(orc_Index.internalPointer());

   if (pc_TreeItem != NULL)
   {
      const C_TblTreItem * const pc_FirstParent = dynamic_cast<const C_TblTreItem * const>(pc_TreeItem->pc_Parent);
      if ((pc_FirstParent != NULL) && (pc_FirstParent->pc_Parent != NULL))
      {
         const C_TblTreItem * const pc_SecondParent =
            dynamic_cast<const C_TblTreItem * const>(pc_FirstParent->pc_Parent);
         if ((pc_SecondParent != NULL) && (pc_SecondParent->pc_Parent != NULL))
         {
            const C_TblTreItem * const pc_ThirdParent =
               dynamic_cast<const C_TblTreItem * const>(pc_SecondParent->pc_Parent);
            if ((pc_ThirdParent != NULL) && (pc_ThirdParent->pc_Parent != NULL))
            {
               const C_TblTreItem * const pc_FourthParent =
                  dynamic_cast<const C_TblTreItem * const>(pc_ThirdParent->pc_Parent);
               if ((pc_FourthParent != NULL) && (pc_FourthParent->pc_Parent != NULL))
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
                  const uint32_t u32_NodeIndex = pc_ThirdParent->u32_Index;
                  const uint32_t u32_DataPoolIndex = pc_FirstParent->u32_Index;
                  const uint32_t u32_ListIndex = pc_TreeItem->u32_Index;
                  const C_OscNodeDataPoolList * const pc_OscList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
                     u32_NodeIndex, u32_DataPoolIndex, u32_ListIndex);
                  if (pc_OscList != NULL)
                  {
                     for (uint32_t u32_ItElement = 0; u32_ItElement < pc_OscList->c_Elements.size(); ++u32_ItElement)
                     {
                        c_Retval.emplace_back(C_PuiSvDbNodeDataPoolListElementId(u32_NodeIndex,
                                                                                 u32_DataPoolIndex,
                                                                                 u32_ListIndex,
                                                                                 u32_ItElement,
                                                                                 C_PuiSvDbNodeDataPoolListElementId::
                                                                                 eDATAPOOL_ELEMENT, false, 0UL));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up map

   \param[in]  orc_Map  Map to clean up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementModel::mh_CleanUp(QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> & orc_Map)
{
   for (QMap<std::vector<uint32_t>, C_TblTreDataElementModelState>::iterator c_It = orc_Map.begin();
        c_It != orc_Map.end(); ++c_It)
   {
      c_It.value().CleanUp();
   }
   orc_Map.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if item in map

   \param[in]  orc_Map     Map to look in
   \param[in]  opc_Item    Item to look for

   \return
   True  Found
   False Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementModel::mh_Contains(const QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> & orc_Map,
                                           const C_TblTreSimpleItem * const opc_Item)
{
   bool q_Retval = false;

   for (QMap<std::vector<uint32_t>, C_TblTreDataElementModelState>::const_iterator c_It = orc_Map.begin();
        c_It != orc_Map.end(); ++c_It)
   {
      if (c_It.value().pc_Tree == opc_Item)
      {
         q_Retval = true;
         break;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get view and system definition hash in combination

   \param[in]  ou32_ViewIndex    View index

   \return
   View and system definition hash in combination
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_TblTreDataElementModel::mh_GetViewSdHash(const uint32_t ou32_ViewIndex)
{
   std::vector<uint32_t> c_Retval;
   uint32_t u32_Hash;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
   std::vector<uint8_t> c_NodeActiveFlags;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      ou32_ViewIndex,
      c_NodeActiveFlags);

   if ((pc_View != NULL) &&
       (s32_Retval == C_NO_ERR))
   {
      bool q_Data;
      uint32_t u32_Data;
      uint32_t u32_DashboardCounter;
      const std::vector<C_PuiSvDashboard> & rc_Dashboards = pc_View->GetDashboards();

      u32_Hash = 0xFFFFFFFFUL;
      //Active flags
      for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeActiveFlags.size(); ++u32_ItNode)
      {
         q_Data = static_cast<bool>(c_NodeActiveFlags[u32_ItNode]);
         stw::scl::C_SclChecksums::CalcCRC32(&q_Data, sizeof(q_Data), u32_Hash);
      }
      //Relevant PC data
      q_Data = pc_View->GetOscPcData().GetConnected();
      u32_Data = pc_View->GetOscPcData().GetBusIndex();
      stw::scl::C_SclChecksums::CalcCRC32(&q_Data, sizeof(q_Data), u32_Hash);
      stw::scl::C_SclChecksums::CalcCRC32(&u32_Data, sizeof(u32_Data), u32_Hash);

      // The parameter widget has an unique configuration which has side effects to the selection model too and
      // must be added to the view hash
      for (u32_DashboardCounter = 0U; u32_DashboardCounter < rc_Dashboards.size(); ++u32_DashboardCounter)
      {
         uint32_t u32_ParamCounter;
         const std::vector<C_PuiSvDbParam> & rc_Params = rc_Dashboards[u32_DashboardCounter].GetParams();

         for (u32_ParamCounter = 0U; u32_ParamCounter < rc_Params.size(); ++u32_ParamCounter)
         {
            uint32_t u32_ElementConfigCounter;
            for (u32_ElementConfigCounter = 0U;
                 u32_ElementConfigCounter < rc_Params[u32_ParamCounter].c_DataPoolElementsConfig.size();
                 ++u32_ElementConfigCounter)
            {
               // Only the element configuration of the parameter widget is relevant for this hash calculation
               rc_Params[u32_ParamCounter].c_DataPoolElementsConfig[u32_ElementConfigCounter].CalcHash(u32_Hash);
            }
         }
      }
   }
   else
   {
      u32_Hash = 0UL;
   }
   c_Retval.push_back(u32_Hash);
   u32_Hash = C_PuiSdHandler::h_GetInstance()->CalcHashSystemDefinition();
   c_Retval.push_back(u32_Hash);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if diagnostic mode activated for views

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index

   \return
   True  Diagnostic mode activated
   False No diagnostic mode activated
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementModel::mh_SvCheckNodeDiagnostic(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex)
{
   bool q_Retval = false;
   const C_SyvRoRouteCalculation c_RouteCalculation(ou32_ViewIndex, ou32_NodeIndex,
                                                    C_OscRoutingCalculation::eDIAGNOSTIC);

   const int32_t s32_Retval = c_RouteCalculation.GetState();

   if (s32_Retval == C_NO_ERR)
   {
      q_Retval = true;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current node active flags

   \param[in]      ou32_ViewIndex            View index
   \param[in]      oq_IsModelUsedInSysViews  True if the model is used for a view in system views, false otherwise
   \param[in,out]  orc_NodeActiveFlags       Node active flags

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_TblTreDataElementModel::mh_GetCurrentNodeActiveFlags(const uint32_t ou32_ViewIndex,
                                                               const bool oq_IsModelUsedInSysViews,
                                                               std::vector<uint8_t> & orc_NodeActiveFlags)
{
   int32_t s32_Retval;
   const uint32_t u32_NodeSize = C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();

   // Model used in a different context than system views e.g. system definition
   if (oq_IsModelUsedInSysViews == false)
   {
      s32_Retval = C_NO_ERR;
      orc_NodeActiveFlags.resize(u32_NodeSize, 1U);
   }
   else
   {
      s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
         ou32_ViewIndex,
         orc_NodeActiveFlags);
   }
   return s32_Retval;
}
