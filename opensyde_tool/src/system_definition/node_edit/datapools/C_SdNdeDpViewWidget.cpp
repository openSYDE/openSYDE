//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all available datapools of a node

   The widget shows all datapools seperated in the three categories:
   - DIAG
   - NVM
   - COM

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDpViewWidget.h"

#include "ui_C_SdNdeDpViewWidget.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_SdNdeDpUtil.h"
#include "C_OSCNode.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
// configuration for the showing of storage usage indicators
const bool C_SdNdeDpViewWidget::mhaq_StorageIndicatorActive[static_cast<stw_types::sintn>(stw_opensyde_core::
                                                                                          C_OSCNodeDataPool::
                                                                                          eHALC) + 1] =
{
   false, // DIAG
   true,  // NVM
   false, // COM
   false  // HALC
};

const bool C_SdNdeDpViewWidget::mhaq_AddButtonVisible[static_cast<stw_types::sintn>(stw_opensyde_core::
                                                                                    C_OSCNodeDataPool::
                                                                                    eHALC) + 1] =
{
   true, // DIAG
   true, // NVM
   true, // COM
   false // HALC
};

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
C_SdNdeDpViewWidget::C_SdNdeDpViewWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpViewWidget),
   mpc_UsageBar(NULL),
   mu32_NodeIndex(0),
   mu32_LastKnownDataPoolIndex(0),
   me_ActiveDataPoolType(stw_opensyde_core::C_OSCNodeDataPool::eDIAG),
   msn_ActiveDataPoolWidget(-1)
{
   sint32 s32_Counter;

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_WidgetDpDiag->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OSCNodeDataPool::eDIAG),
                                             C_GtGetText::h_GetText("DIAG - Diagnostic Datapools"),
                                             C_GtGetText::h_GetText(
                                                "- The use case of DIAG Datapools is monitoring live values of a system \n"
                                                "- The lifetime of the variables is during operation only"),
                                             "pc_WidgetDpDiag");
   this->mpc_Ui->pc_WidgetDpNvm->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OSCNodeDataPool::eNVM),
                                            C_GtGetText::h_GetText("NVM - Non Volatile Memory Datapools"),
                                            C_GtGetText::h_GetText(
                                               "- The use case of NVM Datapools is to parametrize the system \n"
                                               "- The NVM parameters are located in a non volatile memory, for example in the EEPROM of the controller \n"
                                               "- Depending on the users application strategy, the parameters can be saved on shutdown and can be restored on start up"),
                                            "pc_WidgetDpNvm");
   this->mpc_Ui->pc_WidgetDpCom->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OSCNodeDataPool::eCOM),
                                            C_GtGetText::h_GetText("COMM - CAN Communication Datapools"),
                                            C_GtGetText::h_GetText(
                                               "- The use case of COMM Datapools is the exchange of data between nodes \n"
                                               "- The lifetime of the variables is during operation only"),
                                            "pc_WidgetDpCom");
   this->mpc_Ui->pc_WidgetDpHalc->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OSCNodeDataPool::eHALC),
                                             C_GtGetText::h_GetText("HAL - Hardware Configuration Datapools"),
                                             C_GtGetText::h_GetText(
                                                "- The use case of HAL Datapools is to configure, monitor, and control hardware parts of the node \n"
                                                "- The lifetime of the variables is during operation only"),
                                             "pc_WidgetDpHalc");

   // save pointer in an array for easy usage
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eDIAG] = this->mpc_Ui->pc_WidgetDpDiag;
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eNVM] = this->mpc_Ui->pc_WidgetDpNvm;
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eCOM] = this->mpc_Ui->pc_WidgetDpCom;
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eHALC] = this->mpc_Ui->pc_WidgetDpHalc;

   for (s32_Counter = 0; s32_Counter <= static_cast<sint32>(stw_opensyde_core::C_OSCNodeDataPool::eHALC); ++s32_Counter)
   {
      this->mapc_Selectors[s32_Counter]->SetSelected(false);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigWidgetFocused,
              this, &C_SdNdeDpViewWidget::m_SubWidgetFocused);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigOpenDataPoolContent,
              this, &C_SdNdeDpViewWidget::m_EmitActualDataPool);
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigUpdateLists,
              this, &C_SdNdeDpViewWidget::SigUpdateLists);
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigNoDataPoolSelected,
              this, &C_SdNdeDpViewWidget::m_NoDataPoolSelected);
   }

   connect(this->mpc_Ui->pc_WidgetDpCom, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
   connect(this->mpc_Ui->pc_WidgetDpDiag, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
   connect(this->mpc_Ui->pc_WidgetDpNvm, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated node

   \param[in] ou32_NodeIndex     Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetNode(const uint32 ou32_NodeIndex)
{
   bool q_RestoreDataPoolSelection = false;

   C_OSCNodeDataPool::E_Type e_RestoredDataPoolType = C_OSCNodeDataPool::eDIAG;
   uint32 u32_RestoredDataPoolTypeIndex = 0UL;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   sint32 s32_Counter;
   bool q_RestoredDataPoolFound = false;

   //User settings store
   m_StoreToUserSettings();
   //User settings restores
   if (pc_Node != NULL)
   {
      const C_UsNode c_UserSettingsNode = C_UsHandler::h_GetInstance()->GetProjSdNode(
         pc_Node->c_Properties.c_Name.c_str());
      const QString c_SelectedDataPoolName = c_UserSettingsNode.GetSelectedDatapoolName();
      if (c_SelectedDataPoolName.compare("") != 0)
      {
         for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
         {
            const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
            if (rc_DataPool.c_Name == c_SelectedDataPoolName.toStdString().c_str())
            {
               //Match found
               q_RestoreDataPoolSelection = true;
               e_RestoredDataPoolType = rc_DataPool.e_Type;

               //Iterate to specific type index
               for (uint32 u32_ItPrevDataPool = 0; u32_ItPrevDataPool < u32_ItDataPool; ++u32_ItPrevDataPool)
               {
                  const C_OSCNodeDataPool & rc_PrevDataPool = pc_Node->c_DataPools[u32_ItPrevDataPool];
                  if (rc_DataPool.e_Type == rc_PrevDataPool.e_Type)
                  {
                     ++u32_RestoredDataPoolTypeIndex;
                  }
               }
               break;
            }
         }
      }
   }

   this->mu32_NodeIndex = ou32_NodeIndex;

   if (this->mpc_UsageBar == NULL)
   {
      this->mpc_UsageBar = new C_SdNdeDpSelectorUsageWidget(this);
   }

   this->m_UpdateUsageBarSize();

   for (s32_Counter = 0; s32_Counter <= static_cast<sint32>(stw_opensyde_core::C_OSCNodeDataPool::eHALC); ++s32_Counter)
   {
      const bool q_Return = this->mapc_Selectors[s32_Counter]->SetTypeAndNode(
         static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(s32_Counter), ou32_NodeIndex,
         mhaq_StorageIndicatorActive[s32_Counter],
         mhaq_AddButtonVisible[s32_Counter]);

      if ((q_Return == true) && (q_RestoreDataPoolSelection == true))
      {
         if (static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(s32_Counter) == e_RestoredDataPoolType)
         {
            const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(ou32_NodeIndex,
                                                                                         e_RestoredDataPoolType,
                                                                                         u32_RestoredDataPoolTypeIndex);
            if (s32_DpIndex >= 0)
            {
               q_RestoredDataPoolFound = true;
            }
         }
      }

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigDataPoolChanged,
              this, &C_SdNdeDpViewWidget::m_DpChangedUpdateUsageView);
   }

   this->m_DpUpdateUsageView();

   if (q_RestoredDataPoolFound == true)
   {
      this->m_EmitActualDataPool(e_RestoredDataPoolType, u32_RestoredDataPoolTypeIndex);
      this->mapc_Selectors[this->me_ActiveDataPoolType]->SetSelectedDataPool(this->msn_ActiveDataPoolWidget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the conflict state of the active datapool

   \param[in] oq_Active     Flag if conlfict is active or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetActualDataPoolConflict(const bool oq_Active) const
{
   this->mapc_Selectors[this->me_ActiveDataPoolType]->SetCurrentDataPoolConflict(this->msn_ActiveDataPoolWidget,
                                                                                 oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the widgets for the actual datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::UpdateActualDataPool(void) const
{
   this->mapc_Selectors[this->me_ActiveDataPoolType]->UpdateActualDataPool();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a concrete datapool

   \param[in] ou32_DataPoolIndex      Datapool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetActualDataPool(const uint32 ou32_DataPoolIndex)
{
   sintn sn_Counter;
   uint32 u32_DpType;
   sint32 s32_DpIndex;
   bool q_Found = false;

   for (u32_DpType = 0U; u32_DpType <= static_cast<uint32>(stw_opensyde_core::C_OSCNodeDataPool::eHALC);
        ++u32_DpType)
   {
      // search the index
      for (sn_Counter = 0U; sn_Counter <= static_cast<sintn>(ou32_DataPoolIndex); ++sn_Counter)
      {
         s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(
            this->mu32_NodeIndex,
            static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(u32_DpType),
            static_cast<uint32>(sn_Counter));

         if ((s32_DpIndex >= 0) &&
             (static_cast<uint32>(s32_DpIndex) == ou32_DataPoolIndex))
         {
            // save the actual datpool
            this->msn_ActiveDataPoolWidget = sn_Counter;
            // save the actual active datapool type
            this->me_ActiveDataPoolType = static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(u32_DpType);
            q_Found = true;
            break;
         }
      }

      if (q_Found == true)
      {
         break;
      }
   }

   if (q_Found == false)
   {
      this->msn_ActiveDataPoolWidget = -1;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Navigate one Datapool forward or backward.

   \param[in]  oq_Forwards    true: select next Datapool
                              false: select previous Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::NavigateToNextDataPool(const bool oq_Forwards)
{
   if (this->msn_ActiveDataPoolWidget >= 0)
   {
      const sint32 s32_NewDpIndex =
         C_SdNdeDpUtil::h_GetNextDiagOrNvmDpIndex(this->mu32_NodeIndex, this->mu32_LastKnownDataPoolIndex, oq_Forwards);

      tgl_assert(s32_NewDpIndex >= 0);
      if (s32_NewDpIndex >= 0)
      {
         this->SetActualDataPool(s32_NewDpIndex); // first: update internal indices
         this->m_EmitActualDataPool(this->me_ActiveDataPoolType, this->msn_ActiveDataPoolWidget);
         this->mapc_Selectors[this->me_ActiveDataPoolType]->SetSelectedDataPool(this->msn_ActiveDataPoolWidget);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current set Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetNoActualDataPool(void)
{
   this->msn_ActiveDataPoolWidget = -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_UsageBar and the Qt memory management
C_SdNdeDpViewWidget::~C_SdNdeDpViewWidget()
{
   m_StoreToUserSettings();
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_UsageBar and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Resize usage widget size

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   this->m_UpdateUsageBarSize();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_SubWidgetFocused(const C_OSCNodeDataPool::E_Type oe_DataPoolType) const
{
   sint32 s32_Counter;

   // deactivate the other widgets
   for (s32_Counter = 0; s32_Counter <= static_cast<sint32>(stw_opensyde_core::C_OSCNodeDataPool::eHALC);
        ++s32_Counter)
   {
      if (s32_Counter != static_cast<sint32>(oe_DataPoolType))
      {
         this->mapc_Selectors[s32_Counter]->SetSelected(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_EmitActualDataPool(const C_OSCNodeDataPool::E_Type oe_DataPoolType,
                                               const sintn osn_DataPoolWidgetIndex)
{
   sint32 s32_DataPoolIndex;

   // save the actual datpool
   this->msn_ActiveDataPoolWidget = osn_DataPoolWidgetIndex;
   // save the actual active datapool type
   this->me_ActiveDataPoolType = oe_DataPoolType;

   s32_DataPoolIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex, oe_DataPoolType,
                                                                         osn_DataPoolWidgetIndex);

   if (s32_DataPoolIndex >= 0)
   {
      Q_EMIT (this->SigOpenDataPoolContent(static_cast<uint32>(s32_DataPoolIndex)));
      this->mu32_LastKnownDataPoolIndex = static_cast<uint32>(s32_DataPoolIndex);
   }
   else
   {
      Q_EMIT (this->SigNoDataPoolSelected());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_NoDataPoolSelected(void)
{
   this->msn_ActiveDataPoolWidget = -1;

   Q_EMIT (this->SigNoDataPoolSelected());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Recheck error for all data pools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_ErrorCheck(void) const
{
   this->mpc_Ui->pc_WidgetDpCom->ErrorCheck();
   this->mpc_Ui->pc_WidgetDpDiag->ErrorCheck();
   this->mpc_Ui->pc_WidgetDpNvm->ErrorCheck();
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current selection to user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_StoreToUserSettings(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      if ((this->msn_ActiveDataPoolWidget >= 0) &&
          (this->mu32_LastKnownDataPoolIndex < pc_Node->c_DataPools.size()))
      {
         const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[this->mu32_LastKnownDataPoolIndex];
         C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedDatapoolName(
            pc_Node->c_Properties.c_Name.c_str(), rc_DataPool.c_Name.c_str());
      }
      else
      {
         // In case of an invalid or not shown Datapool, reset the entry
         C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedDatapoolName(
            pc_Node->c_Properties.c_Name.c_str(), "");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for a changed Datapool configuration to update the usage view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_DpChangedUpdateUsageView(void)
{
   Q_EMIT (this->SigChanged());
   this->m_DpUpdateUsageView();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the usage view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_DpUpdateUsageView(void)
{
   if (this->mpc_UsageBar != NULL)
   {
      // update the usage view
      std::vector<uint32> c_VecUsedSize;
      std::vector<QString> c_VecDatapoolNames;
      uint32 u32_Percentage;
      uint32 u32_SumNvmSize = 0;
      uint32 u32_DpCounter;
      const stw_opensyde_core::C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
         this->mu32_NodeIndex);

      if (pc_Node != NULL)
      {
         const stw_opensyde_core::C_OSCDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
         tgl_assert(pc_DevDef != NULL);
         if (pc_DevDef != NULL)
         {
            QString c_LabelTooltip = static_cast<QString>("%1% ") + C_GtGetText::h_GetText("reserved by Datapools") +
                                     static_cast<QString>(
               " (%2 / %3)");
            // check all datapools of the lists
            for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
            {
               if (pc_Node->c_DataPools[u32_DpCounter].e_Type == stw_opensyde_core::C_OSCNodeDataPool::eNVM)
               {
                  // save the name of the datapool
                  c_VecDatapoolNames.push_back(static_cast<QString>(pc_Node->c_DataPools[u32_DpCounter].c_Name.c_str()));
                  c_VecUsedSize.push_back(pc_Node->c_DataPools[u32_DpCounter].u32_NvMSize);
                  u32_SumNvmSize += pc_Node->c_DataPools[u32_DpCounter].u32_NvMSize;
               }
            }

            u32_Percentage = this->mpc_UsageBar->SetUsage(pc_DevDef->u32_UserEepromSizeBytes, c_VecUsedSize,
                                                          c_VecDatapoolNames);
            // show the percentage
            this->mpc_Ui->pc_LabelUsagePercentage->setText(
               C_GtGetText::h_GetText("Memory Usage: ") +
               QString::number(u32_Percentage) + "%");
            c_LabelTooltip = c_LabelTooltip.arg(QString::number(u32_Percentage),
                                                C_Uti::h_GetByteCountAsString(u32_SumNvmSize),
                                                C_Uti::h_GetByteCountAsString(pc_DevDef->u32_UserEepromSizeBytes));

            this->mpc_Ui->pc_LabelUsagePercentage->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                            "Memory Statistics"), c_LabelTooltip);

            if (u32_SumNvmSize > pc_DevDef->u32_UserEepromSizeBytes)
            {
               C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsagePercentage, "Conflict", true);
            }
            else
            {
               C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsagePercentage, "Conflict", false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the size and position of the usage bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_UpdateUsageBarSize(void)
{
   if (this->mpc_UsageBar != NULL)
   {
      this->mpc_UsageBar->setGeometry(0, this->height() - 17, this->width() - 9, 17);
   }
}
