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
#include "C_SdNdeDpViewWidget.hpp"

#include "ui_C_SdNdeDpViewWidget.h"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_UsHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_OscNode.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
// configuration for the showing of storage usage indicators
const bool C_SdNdeDpViewWidget::mhaq_STORAGE_INDICATOR_ACTIVE[static_cast<int32_t>(stw::opensyde_core::
                                                                                   C_OscNodeDataPool::
                                                                                   eHALC_NVM) + 1] =
{
   false, // DIAG
   true,  // NVM
   false, // COM
   false, // HALC
   true   // HALC_NVM
};

const bool C_SdNdeDpViewWidget::mhaq_ADD_BUTTON_INVISIBLE[static_cast<int32_t>(stw::opensyde_core::
                                                                               C_OscNodeDataPool::
                                                                               eHALC_NVM) + 1] =
{
   true,  // DIAG
   true,  // NVM
   true,  // COM
   false, // HALC
   false  // HALC_NVM
};

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpViewWidget::C_SdNdeDpViewWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpViewWidget),
   mpc_UsageBar(NULL),
   mu32_NodeIndex(0),
   mu32_LastKnownDataPoolIndex(0),
   me_ActiveDataPoolType(stw::opensyde_core::C_OscNodeDataPool::eDIAG),
   ms32_ActiveDataPoolWidget(-1),
   mq_HalcNvmBased(false)
{
   int32_t s32_Counter;

   mpc_Ui->setupUi(this);

   // Prepare UI
   this->InitStaticNames();
   this->mpc_Ui->pc_LabelUsagePercentage->SetForegroundColor(7);
   this->mpc_Ui->pc_LabelUsagePercentage->SetFontPixel(16, false, false);
   this->mpc_Ui->pc_PushButtonAutoStartAddress->setCheckable(true);
   this->mpc_Ui->pc_PushButtonAutoStartAddress->setChecked(true);
   this->m_UpdateAutoStartAddressSvg();

   this->mpc_Ui->pc_WidgetDpDiag->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OscNodeDataPool::eDIAG),
                                             C_GtGetText::h_GetText("DIAG - Diagnostic Datapools"),
                                             C_GtGetText::h_GetText(
                                                "- The use case of DIAG Datapools is monitoring live values of a system \n"
                                                "- The lifetime of the variables is during operation only"),
                                             "pc_WidgetDpDiag");
   this->mpc_Ui->pc_WidgetDpNvm->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OscNodeDataPool::eNVM),
                                            C_GtGetText::h_GetText("NVM - Non Volatile Memory Datapools"),
                                            C_GtGetText::h_GetText(
                                               "- The use case of NVM Datapools is to parametrize the system \n"
                                               "- The NVM parameters are located in a non volatile memory, for example in the EEPROM of the controller \n"
                                               "- Depending on the users application strategy, the parameters can be saved on shutdown and can be restored on start up"),
                                            "pc_WidgetDpNvm");
   this->mpc_Ui->pc_WidgetDpCom->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OscNodeDataPool::eCOM),
                                            C_GtGetText::h_GetText("COMM - CAN Communication Datapools"),
                                            C_GtGetText::h_GetText(
                                               "- The use case of COMM Datapools is the exchange of data between nodes \n"
                                               "- The lifetime of the variables is during operation only"),
                                            "pc_WidgetDpCom");
   this->mpc_Ui->pc_WidgetDpHalc->InitWidget(C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OscNodeDataPool::eHALC),
                                             C_GtGetText::h_GetText("HAL - Hardware Configuration Datapools"),
                                             C_GtGetText::h_GetText(
                                                "- The use case of HAL Datapools is to configure, monitor, and control hardware parts of the node \n"
                                                "- The lifetime of the variables is during operation only"),
                                             "pc_WidgetDpHalc");
   this->mpc_Ui->pc_WidgetDpHalcNvm->InitWidget(
      C_PuiSdUtil::h_ConvertDataPoolTypeToString(C_OscNodeDataPool::eHALC_NVM),
      C_GtGetText::h_GetText("HAL - Hardware Configuration Datapools"),
      C_GtGetText::h_GetText(
         "- The use case of HAL Datapools is to configure, monitor, and control hardware parts of the node \n"
         "- The lifetime of the variables is during operation only"),
      "pc_WidgetDpHalcNvm");

   // save pointer in an array for easy usage
   this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eDIAG] = this->mpc_Ui->pc_WidgetDpDiag;
   this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eNVM] = this->mpc_Ui->pc_WidgetDpNvm;
   this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eCOM] = this->mpc_Ui->pc_WidgetDpCom;
   this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eHALC] = this->mpc_Ui->pc_WidgetDpHalc;
   this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM] = this->mpc_Ui->pc_WidgetDpHalcNvm;

   // Special case: The HALC NVM Datapool will be visible if a HALC NVM based HALC Description will be loaded
   this->mpc_Ui->pc_WidgetDpHalcNvm->setVisible(false);

   for (s32_Counter = 0; s32_Counter <= static_cast<int32_t>(stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM);
        ++s32_Counter)
   {
      this->mapc_Selectors[s32_Counter]->SetSelected(false);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigWidgetFocused,
              this, &C_SdNdeDpViewWidget::m_SubWidgetFocused);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigOpenDataPoolContent,
              this, &C_SdNdeDpViewWidget::m_EmitActualDataPool);
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigUpdateLists,
              this, &C_SdNdeDpViewWidget::SigUpdateLists);
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigUpdateFollowingLists,
              this, &C_SdNdeDpViewWidget::m_UpdateFollowingLists);
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigNoDataPoolSelected,
              this, &C_SdNdeDpViewWidget::m_NoDataPoolSelected);
   }

   connect(this->mpc_Ui->pc_WidgetDpCom, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
   connect(this->mpc_Ui->pc_WidgetDpDiag, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
   connect(this->mpc_Ui->pc_WidgetDpNvm, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
   connect(this->mpc_Ui->pc_WidgetDpHalc, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);
   connect(this->mpc_Ui->pc_WidgetDpHalcNvm, &C_SdNdeDpSelectorWidget::SigErrorCheck, this,
           &C_SdNdeDpViewWidget::m_ErrorCheck);

   connect(this->mpc_Ui->pc_PushButtonAutoStartAddress, &stw::opensyde_gui_elements::C_OgePubSvgIconOnly::clicked,
           this, &C_SdNdeDpViewWidget::m_AutoStartAddressClicked);
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::InitStaticNames(void) const
{
   const QString c_AutoStartAddressHeading = C_GtGetText::h_GetText("Auto Start Address");
   const QString c_AutoStartAddressDescription =
      C_GtGetText::h_GetText("On: All start addresses of NVM Datapools will be calculated "
                             "automatically without any gaps between the Datapools.\n\n"
                             "Off: The start addresses of NVM Datapools will not be calculated "
                             "automatically. The start addresses must be set by the user. Gaps and overlaps "
                             "between the Datapools are not corrected automatically.");

   this->mpc_Ui->pc_LabelAutoStartAddress->setText(C_GtGetText::h_GetText("Auto Start Address"));

   this->mpc_Ui->pc_LabelAutoStartAddress->SetToolTipInformation(c_AutoStartAddressHeading,
                                                                 c_AutoStartAddressDescription);

   this->mpc_Ui->pc_PushButtonAutoStartAddress->SetToolTipInformation(c_AutoStartAddressHeading,
                                                                      c_AutoStartAddressDescription);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated node

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   bool q_RestoreDataPoolSelection = false;

   C_OscNodeDataPool::E_Type e_RestoredDataPoolType = C_OscNodeDataPool::eDIAG;
   uint32_t u32_RestoredDataPoolTypeIndex = 0UL;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
   int32_t s32_Counter;
   bool q_RestoredDataPoolFound = false;

   //User settings store
   m_StoreToUserSettings();
   if (pc_Node != NULL)
   {
      //User settings restores
      const C_UsNode c_UserSettingsNode = C_UsHandler::h_GetInstance()->GetProjSdNode(
         pc_Node->c_Properties.c_Name.c_str());
      const QString c_SelectedDataPoolName = c_UserSettingsNode.GetSelectedDatapoolName();

      if (pc_Node->c_HalcConfig.IsClear() == false)
      {
         // Node has a HALC Configuration loaded
         this->mq_HalcNvmBased = pc_Node->c_HalcConfig.q_NvmBasedConfig;

         if (pc_Node->c_HalcConfig.q_NvmBasedConfig == false)
         {
            this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eHALC]->setVisible(true);
            this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM]->setVisible(false);
         }
         else
         {
            this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eHALC]->setVisible(false);
            this->mapc_Selectors[stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM]->setVisible(true);
         }
      }

      if (c_SelectedDataPoolName.compare("") != 0)
      {
         for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
         {
            const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
            if (rc_DataPool.c_Name == c_SelectedDataPoolName.toStdString().c_str())
            {
               //Match found
               q_RestoreDataPoolSelection = true;
               e_RestoredDataPoolType = rc_DataPool.e_Type;

               //Iterate to specific type index
               for (uint32_t u32_ItPrevDataPool = 0; u32_ItPrevDataPool < u32_ItDataPool; ++u32_ItPrevDataPool)
               {
                  const C_OscNodeDataPool & rc_PrevDataPool = pc_Node->c_DataPools[u32_ItPrevDataPool];
                  if (rc_DataPool.e_Type == rc_PrevDataPool.e_Type)
                  {
                     ++u32_RestoredDataPoolTypeIndex;
                  }
               }
               break;
            }
         }
      }

      // Restore setting for auto start address mode
      this->mpc_Ui->pc_PushButtonAutoStartAddress->setChecked(pc_Node->q_DatapoolAutoNvmStartAddress);
      this->m_UpdateAutoStartAddressSvg();
   }

   this->mu32_NodeIndex = ou32_NodeIndex;

   if (this->mpc_UsageBar == NULL)
   {
      this->mpc_UsageBar = new C_SdNdeDpViewUsageWidget(this);
   }

   this->m_UpdateUsageBarSize();

   for (s32_Counter = 0; s32_Counter <= static_cast<int32_t>(stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM);
        ++s32_Counter)
   {
      const bool q_Return = this->mapc_Selectors[s32_Counter]->SetTypeAndNode(
         static_cast<stw::opensyde_core::C_OscNodeDataPool::E_Type>(s32_Counter), ou32_NodeIndex,
         mhaq_STORAGE_INDICATOR_ACTIVE[s32_Counter],
         mhaq_ADD_BUTTON_INVISIBLE[s32_Counter]);

      if ((q_Return == true) && (q_RestoreDataPoolSelection == true))
      {
         if (static_cast<stw::opensyde_core::C_OscNodeDataPool::E_Type>(s32_Counter) == e_RestoredDataPoolType)
         {
            const int32_t s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(ou32_NodeIndex,
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
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigDataPoolHoverStateChanged,
              this->mpc_UsageBar, &C_SdNdeDpViewUsageWidget::DataPoolHoverStateChanged);
   }

   this->m_DpUpdateUsageView();

   if (q_RestoredDataPoolFound == true)
   {
      this->m_EmitActualDataPool(e_RestoredDataPoolType, u32_RestoredDataPoolTypeIndex);
      this->mapc_Selectors[this->me_ActiveDataPoolType]->SetSelectedDataPool(this->ms32_ActiveDataPoolWidget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the conflict state of the active datapool

   \param[in]  oq_Active   Flag if conlfict is active or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetActualDataPoolConflict(const bool oq_Active) const
{
   this->mapc_Selectors[this->me_ActiveDataPoolType]->SetCurrentDataPoolConflict(this->ms32_ActiveDataPoolWidget,
                                                                                 oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the widget for the all Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::UpdateDataPools(void)
{
   int32_t s32_Counter;

   for (s32_Counter = 0; s32_Counter <= static_cast<int32_t>(stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM);
        ++s32_Counter)
   {
      this->mapc_Selectors[s32_Counter]->UpdateDataPools();
   }
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

   \param[in]  ou32_DataPoolIndex   Datapool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetActualDataPool(const uint32_t ou32_DataPoolIndex)
{
   int32_t s32_Counter;
   uint32_t u32_DpType;
   int32_t s32_DpIndex;
   bool q_Found = false;

   for (u32_DpType = 0U; u32_DpType <= static_cast<uint32_t>(stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM);
        ++u32_DpType)
   {
      // search the index
      for (s32_Counter = 0U; s32_Counter <= static_cast<int32_t>(ou32_DataPoolIndex); ++s32_Counter)
      {
         s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(
            this->mu32_NodeIndex,
            static_cast<stw::opensyde_core::C_OscNodeDataPool::E_Type>(u32_DpType),
            static_cast<uint32_t>(s32_Counter));

         if ((s32_DpIndex >= 0) &&
             (static_cast<uint32_t>(s32_DpIndex) == ou32_DataPoolIndex))
         {
            // save the actual datpool
            this->ms32_ActiveDataPoolWidget = s32_Counter;
            // save the actual active datapool type
            this->me_ActiveDataPoolType = static_cast<stw::opensyde_core::C_OscNodeDataPool::E_Type>(u32_DpType);
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
      this->ms32_ActiveDataPoolWidget = -1;
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
   if (this->ms32_ActiveDataPoolWidget >= 0)
   {
      const int32_t s32_NewDpIndex =
         C_SdNdeDpUtil::h_GetNextDiagOrNvmDpIndex(this->mu32_NodeIndex, this->mu32_LastKnownDataPoolIndex, oq_Forwards);

      tgl_assert(s32_NewDpIndex >= 0);
      if (s32_NewDpIndex >= 0)
      {
         this->SetActualDataPool(s32_NewDpIndex); // first: update internal indices
         this->m_EmitActualDataPool(this->me_ActiveDataPoolType, this->ms32_ActiveDataPoolWidget);
         this->mapc_Selectors[this->me_ActiveDataPoolType]->SetSelectedDataPool(this->ms32_ActiveDataPoolWidget);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current set Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetNoActualDataPool(void)
{
   this->ms32_ActiveDataPoolWidget = -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Resize usage widget size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   this->m_UpdateUsageBarSize();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_SubWidgetFocused(const C_OscNodeDataPool::E_Type oe_DataPoolType) const
{
   int32_t s32_Counter;

   // deactivate the other widgets
   for (s32_Counter = 0; s32_Counter <= static_cast<int32_t>(stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM);
        ++s32_Counter)
   {
      if (s32_Counter != static_cast<int32_t>(oe_DataPoolType))
      {
         this->mapc_Selectors[s32_Counter]->SetSelected(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_EmitActualDataPool(const C_OscNodeDataPool::E_Type oe_DataPoolType,
                                               const int32_t os32_DataPoolWidgetIndex)
{
   int32_t s32_DataPoolIndex;

   // save the actual datpool
   this->ms32_ActiveDataPoolWidget = os32_DataPoolWidgetIndex;
   // save the actual active datapool type
   this->me_ActiveDataPoolType = oe_DataPoolType;

   s32_DataPoolIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex, oe_DataPoolType,
                                                                         os32_DataPoolWidgetIndex);

   if (s32_DataPoolIndex >= 0)
   {
      Q_EMIT (this->SigOpenDataPoolContent(static_cast<uint32_t>(s32_DataPoolIndex)));
      this->mu32_LastKnownDataPoolIndex = static_cast<uint32_t>(s32_DataPoolIndex);
   }
   else
   {
      Q_EMIT (this->SigNoDataPoolSelected());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_NoDataPoolSelected(void)
{
   this->ms32_ActiveDataPoolWidget = -1;

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
   this->mpc_Ui->pc_WidgetDpHalc->ErrorCheck();
   this->mpc_Ui->pc_WidgetDpHalcNvm->ErrorCheck();
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current selection to user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_StoreToUserSettings(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      if ((this->ms32_ActiveDataPoolWidget >= 0) &&
          (this->mu32_LastKnownDataPoolIndex < pc_Node->c_DataPools.size()))
      {
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[this->mu32_LastKnownDataPoolIndex];
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
      const stw::opensyde_core::C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
         this->mu32_NodeIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         const stw::opensyde_core::C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
         const uint32_t u32_SubDeviceIndex = pc_Node->u32_SubDeviceIndex;
         tgl_assert(pc_DevDef != NULL);
         if (pc_DevDef != NULL)
         {
            tgl_assert(u32_SubDeviceIndex < pc_DevDef->c_SubDevices.size());
            if (u32_SubDeviceIndex < pc_DevDef->c_SubDevices.size())
            {
               uint32_t u32_Percentage;
               uint32_t u32_SumNvmSize = 0;
               std::vector<C_PuiSdHandler::C_PuiSdHandlerNodeLogicNvmArea> c_Areas;
               QString c_LabelTooltip = static_cast<QString>("%1% ") + C_GtGetText::h_GetText("reserved by Datapools") +
                                        static_cast<QString>(
                  " (%2 / %3)");

               tgl_assert(C_PuiSdHandler::h_GetInstance()->GetNodeNvmDataPoolAreas(this->mu32_NodeIndex,
                                                                                   c_Areas) == C_NO_ERR);

               this->mpc_UsageBar->SetUsage(this->mu32_NodeIndex,
                                            pc_DevDef->c_SubDevices[u32_SubDeviceIndex].u32_UserEepromSizeBytes,
                                            c_Areas,
                                            u32_Percentage, u32_SumNvmSize);

               // show the percentage
               this->mpc_Ui->pc_LabelUsagePercentage->setText(
                  C_GtGetText::h_GetText("Memory Usage: ") +
                  QString::number(u32_Percentage) + "%");
               c_LabelTooltip = c_LabelTooltip.arg(QString::number(u32_Percentage),
                                                   C_Uti::h_GetByteCountAsString(u32_SumNvmSize),
                                                   C_Uti::h_GetByteCountAsString(pc_DevDef->c_SubDevices[
                                                                                    u32_SubDeviceIndex
                                                                                 ].u32_UserEepromSizeBytes));

               if (u32_SumNvmSize > pc_DevDef->c_SubDevices[u32_SubDeviceIndex].u32_UserEepromSizeBytes)
               {
                  this->mpc_Ui->pc_LabelUsagePercentage->SetForegroundColor(24);
                  this->mpc_Ui->pc_LabelUsagePercentage->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                                  "Memory Statistics"), c_LabelTooltip,
                                                                               C_NagToolTip::eERROR);
               }
               else
               {
                  this->mpc_Ui->pc_LabelUsagePercentage->SetForegroundColor(7);
                  this->mpc_Ui->pc_LabelUsagePercentage->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                                  "Memory Statistics"), c_LabelTooltip);
               }
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
      this->mpc_UsageBar->setGeometry(0, this->height() - 16, this->width() - 14, 16);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for button auto start address

   \param[in]  oq_Enabled  Flag if auto start address is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_AutoStartAddressClicked(const bool oq_Enabled)
{
   if ((oq_Enabled == true) &&
       (this->mq_HalcNvmBased == true))
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);

      // In case of a NVM HALC based HALC description the automatic start address mode is not activatable
      this->mpc_Ui->pc_PushButtonAutoStartAddress->setChecked(false);

      c_Message.SetHeading(C_GtGetText::h_GetText("Auto Start Address mode"));

      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Hardware description file with enabled \"nvm-config\" property is loaded. \n"
                                  "In this case the automatic start address mode cannot be used."));
      c_Message.SetCustomMinHeight(200, 250);
      c_Message.Execute();
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

      c_Message.SetHeading(C_GtGetText::h_GetText("Auto Start Address mode"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));
      c_Message.SetCustomMinHeight(250, 250);

      if (oq_Enabled == true)
      {
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Do you really want to activate the automatic start address mode?\n"
                                     "This is not revertible.\n\n"
                                     "After activating, the start addresses of Datapools located in NVM"
                                     " will be recalculated."));

         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Activate"));
      }
      else
      {
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Do you really want to deactivate the automatic start address mode?\n\n"
                                     "After deactivating, the start addresses of Datapools located in NVM must "
                                     "be managed manually."));

         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Deactivate"));
      }

      if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
      {
         C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNode(this->mu32_NodeIndex);

         if (pc_Node != NULL)
         {
            pc_Node->q_DatapoolAutoNvmStartAddress = oq_Enabled;
            this->m_UpdateAutoStartAddressSvg();

            if (pc_Node->q_DatapoolAutoNvmStartAddress == true)
            {
               // Recalculate the addresses
               pc_Node->RecalculateAddress();
               this->UpdateDataPools();
               this->m_DpUpdateUsageView();
               this->m_ErrorCheck();
            }
         }
      }
      else
      {
         this->mpc_Ui->pc_PushButtonAutoStartAddress->setChecked(!oq_Enabled);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the icon of the button for auto start address
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_UpdateAutoStartAddressSvg(void) const
{
   if (this->mpc_Ui->pc_PushButtonAutoStartAddress->isChecked() == true)
   {
      this->mpc_Ui->pc_PushButtonAutoStartAddress->SetSvg("://images/system_views/IconConnected.svg");
   }
   else
   {
      this->mpc_Ui->pc_PushButtonAutoStartAddress->SetSvg("://images/system_views/IconDisconnected.svg");
   }
   this->mpc_Ui->pc_PushButtonAutoStartAddress->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all lists after the list with the specified Datapool type

   \param[in]       oe_DataPoolType     List with Datapool type which sent the update request
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_UpdateFollowingLists(const C_OscNodeDataPool::E_Type oe_DataPoolType) const
{
   int32_t s32_Counter;

   for (s32_Counter = static_cast<int32_t>(oe_DataPoolType) + 1;
        s32_Counter <= static_cast<int32_t>(stw::opensyde_core::C_OscNodeDataPool::eHALC_NVM);
        ++s32_Counter)
   {
      this->mapc_Selectors[s32_Counter]->ReloadDataPools();
   }
}
