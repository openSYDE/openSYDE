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
#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
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
const bool C_SdNdeDpViewWidget::mhaq_StorageIndicatorActive[3] =
{
   false, // DIAG
   true,  // NVM
   false  // COM
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
   mu32_NodeIndex(0),
   mu32_LastKnownDataPoolIndex(0),
   me_ActiveDataPoolType(stw_opensyde_core::C_OSCNodeDataPool::eDIAG),
   msn_ActiveDataPoolWidget(-1),
   mq_Maximized(true),
   mq_IndexValid(false)
{
   sint32 s32_Counter;

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_WidgetDpDiag->InitWidget("DIAG",
                                             C_GtGetText::h_GetText("DIAG - Diagnostic Datapools"),
                                             C_GtGetText::h_GetText(
                                                "- The use case of a DIAG Datapool is monitoring live values of a system \n"
                                                "- The lifetime of the variables is during operation only"),
                                             "pc_WidgetDpDiag");
   this->mpc_Ui->pc_WidgetDpNvm->InitWidget("NVM",
                                            C_GtGetText::h_GetText("NVM - Non Volatile Memory Datapools"),
                                            C_GtGetText::h_GetText(
                                               "- The use case of NVM Datapool is to parametrize the system \n"
                                               "- The NVM parameters are located in a non volatile memory, for example in the EEPROM of the controller \n"
                                               "- Depending on the users application strategy, the parameters can be saved on shutdown and can be restored on start up"),
                                            "pc_WidgetDpNvm");
   this->mpc_Ui->pc_WidgetDpCom->InitWidget("COMM",
                                            C_GtGetText::h_GetText("CAN communication Datapools"),
                                            C_GtGetText::h_GetText(
                                               "- The use case of COMM Datapools is the exchange of data between nodes \n"
                                               "- The lifetime of the variables is during operation only \n"
                                               "- Only one Datapool per each communication protocol is possible"),
                                            "pc_WidgetDpCom");

   this->mpc_Ui->pc_BtnMinMaxDbSelector->SetToolTipInformation(C_GtGetText::h_GetText("Switch View"),
                                                               C_GtGetText::h_GetText("Show more or less Datapool "
                                                                                      "information."));

   // TODO icon states: svg icons; 4 different states; also update in m_MinMaxClicked
   this->mpc_Ui->pc_BtnMinMaxDbSelector->SetCustomIcons(":images/IconDataPoolSelOpenView.png",
                                                        ":images/IconDataPoolSelOpenView.png",
                                                        ":images/IconDataPoolSelOpenView.png",
                                                        ":images/IconDataPoolSelOpenView.png");

   // save pointer in an array for easy usage
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eDIAG] = this->mpc_Ui->pc_WidgetDpDiag;
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eNVM] = this->mpc_Ui->pc_WidgetDpNvm;
   this->mapc_Selectors[stw_opensyde_core::C_OSCNodeDataPool::eCOM] = this->mpc_Ui->pc_WidgetDpCom;

   for (s32_Counter = 0; s32_Counter < 3; ++s32_Counter)
   {
      this->mapc_Selectors[s32_Counter]->SetActive(false);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigWidgetFocused,
              this, &C_SdNdeDpViewWidget::m_SubWidgetFocused);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigChanged,
              this, &C_SdNdeDpViewWidget::SigChanged);
      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigUpdateLists,
              this, &C_SdNdeDpViewWidget::SigUpdateLists);

      connect(this->mapc_Selectors[s32_Counter], &C_SdNdeDpSelectorWidget::SigNoDataPoolSelected,
              this, &C_SdNdeDpViewWidget::m_NoDataPoolSelected);
   }

   connect(this->mpc_Ui->pc_BtnMinMaxDbSelector, &stw_opensyde_gui_elements::C_OgePubIconOnly::clicked,
           this, &C_SdNdeDpViewWidget::m_MinMaxClicked);
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
   const C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   sint32 s32_Counter;
   bool q_DataPoolAdded = false;

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
               this->mu32_LastKnownDataPoolIndex = u32_ItDataPool;
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
   //Important flag
   this->mq_IndexValid = true;

   for (s32_Counter = 0; s32_Counter < 3; ++s32_Counter)
   {
      const bool q_Return = this->mapc_Selectors[s32_Counter]->SetTypeAndNode(
         static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(s32_Counter), ou32_NodeIndex,
         mhaq_StorageIndicatorActive[s32_Counter]);

      if (q_RestoreDataPoolSelection == false)
      {
         if ((q_DataPoolAdded == false) &&
             (q_Return == true))
         {
            // a datapool was added
            // the first category "wins"
            q_DataPoolAdded = true;
            this->mapc_Selectors[s32_Counter]->SetActive(true);
            this->m_EmitActualDataPool(static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(s32_Counter), 0);
         }
      }
      else
      {
         if (static_cast<stw_opensyde_core::C_OSCNodeDataPool::E_Type>(s32_Counter) == e_RestoredDataPoolType)
         {
            const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(ou32_NodeIndex,
                                                                                         e_RestoredDataPoolType,
                                                                                         u32_RestoredDataPoolTypeIndex);
            q_DataPoolAdded = true;
            if (s32_DpIndex >= 0)
            {
               this->mapc_Selectors[s32_Counter]->SetDataPoolActive(static_cast<uint32>(s32_DpIndex));
            }
            else
            {
               this->mapc_Selectors[s32_Counter]->SetActive(true);
            }
            //No emit of selection necessary because datapool selection will trigger the signal implicitly
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the conflict state of the active datapool

   \param[in] oq_Active     Flag if conlfict is active or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetActualDataPoolConflict(const bool oq_Active) const
{
   this->mapc_Selectors[this->me_ActiveDataPoolType]->SetCurrentDataPoolConflict(oq_Active);
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
/*! \brief   Activate a concrete datapool

   \param[in] ou32_DataPoolIndex      Datapool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::SetActualDataPool(const uint32 ou32_DataPoolIndex) const
{
   sint32 s32_Counter;

   for (s32_Counter = 0; s32_Counter < 3; ++s32_Counter)
   {
      // try to activate the datapool
      const bool q_Result = this->mapc_Selectors[s32_Counter]->SetDataPoolActive(ou32_DataPoolIndex);
      if (this->mapc_Selectors[s32_Counter]->IsActive() != q_Result)
      {
         this->mapc_Selectors[s32_Counter]->SetActive(q_Result);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpViewWidget::~C_SdNdeDpViewWidget()
{
   m_StoreToUserSettings();
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_SubWidgetFocused(const C_OSCNodeDataPool::E_Type oe_DataPoolType,
                                             const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_ForceChange)
{
   // was an other widget clicked?
   if (this->me_ActiveDataPoolType != oe_DataPoolType)
   {
      sint32 s32_Counter;

      // deactivate all first
      for (s32_Counter = 0; s32_Counter < 3; ++s32_Counter)
      {
         this->mapc_Selectors[s32_Counter]->SetActive(false);
      }

      // activate and adapt the style of the clicked widget
      this->mapc_Selectors[oe_DataPoolType]->SetActive(true);
   }

   if ((oq_ForceChange == true) || //It might be necessary to trigger a reload for the same index
       (this->me_ActiveDataPoolType != oe_DataPoolType) ||
       (oe_DataPoolType == C_OSCNodeDataPool::eCOM) || // special case: COM lists can change if the datapool is the same
       (this->msn_ActiveDataPoolWidget != osn_DataPoolWidgetIndex))
   {
      this->m_EmitActualDataPool(oe_DataPoolType, osn_DataPoolWidgetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_MinMaxClicked(void)
{
   sint32 s32_Counter;

   // change the maximized state
   this->mq_Maximized = !this->mq_Maximized;

   if (this->mq_Maximized == true)
   {
      //update icon
      this->mpc_Ui->pc_BtnMinMaxDbSelector->SetCustomIcons(":images/IconDataPoolSelOpenView.png",
                                                           ":images/IconDataPoolSelOpenView.png",
                                                           ":images/IconDataPoolSelOpenView.png",
                                                           ":images/IconDataPoolSelOpenView.png");
   }
   else
   {
      //update icon
      this->mpc_Ui->pc_BtnMinMaxDbSelector->SetCustomIcons(":images/IconDataPoolSelCloseView.png",
                                                           ":images/IconDataPoolSelCloseView.png",
                                                           ":images/IconDataPoolSelCloseView.png",
                                                           ":images/IconDataPoolSelCloseView.png");
   }

   // Adapt the widgets
   for (s32_Counter = 0; s32_Counter < 3; ++s32_Counter)
   {
      this->mapc_Selectors[s32_Counter]->SetMaximized(this->mq_Maximized);
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
      Q_EMIT this->SigDataPoolSelected(static_cast<uint32>(s32_DataPoolIndex));
      this->mu32_LastKnownDataPoolIndex = static_cast<uint32>(s32_DataPoolIndex);
   }
   else
   {
      Q_EMIT this->SigNoDataPoolSelected();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_NoDataPoolSelected(void)
{
   this->msn_ActiveDataPoolWidget = -1;

   Q_EMIT this->SigNoDataPoolSelected();
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
   Q_EMIT this->SigErrorChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current selection to user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewWidget::m_StoreToUserSettings(void) const
{
   if (this->mq_IndexValid == true)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         if (this->mu32_LastKnownDataPoolIndex < pc_Node->c_DataPools.size())
         {
            const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[this->mu32_LastKnownDataPoolIndex];
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedDatapoolName(
               pc_Node->c_Properties.c_Name.c_str(), rc_DataPool.c_Name.c_str());
         }
      }
   }
}
