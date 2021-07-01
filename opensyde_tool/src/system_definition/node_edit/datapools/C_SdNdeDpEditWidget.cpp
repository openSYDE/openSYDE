//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition update

   detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_SdNdeDpEditWidget.h"
#include "ui_C_SdNdeDpEditWidget.h"
#include "C_HeHandler.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpEditWidget::C_SdNdeDpEditWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpEditWidget),
   mu32_NodeIndex(0)
{
   // Init Ui
   mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_ListsWidget->setVisible(false);

   // connect to get the selected datapool in the selector
   // must happen before SetNode
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigErrorChange,
           this, &C_SdNdeDpEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigOpenDataPoolContent,
           this, &C_SdNdeDpEditWidget::m_OpenDataPoolContent);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigNoDataPoolSelected,
           this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::Clear);

   // forwarding of the changed signal
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigChanged,
           this, &C_SdNdeDpEditWidget::SigChanged);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigChanged,
           this, &C_SdNdeDpEditWidget::SigChanged);
   // forwarding of the changed signal only if the entire Datapool is affected
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigChanged,
           this, &C_SdNdeDpEditWidget::SigDataPoolsChanged);

   //Connect error handling
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigErrorChange, this,
           &C_SdNdeDpEditWidget::m_OnErrorChange);
   //Connect size handling
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigSizeChange, this,
           &C_SdNdeDpEditWidget::m_OnSizeChange);
   //Bus enter
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigUpdateLists, this->mpc_Ui->pc_ListsWidget,
           &C_SdNdeDpListEditWidget::SetDataPool);
   //Connect save commands
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigSave, this,
           &C_SdNdeDpEditWidget::SigSave);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigSaveAs, this,
           &C_SdNdeDpEditWidget::SigSaveAs);
   //Connect navigation
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigBack, this,
           &C_SdNdeDpEditWidget::m_OnListsBack);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListEditWidget::SigNavigate, this,
           &C_SdNdeDpEditWidget::m_OnListsNavigate);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpEditWidget::~C_SdNdeDpEditWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new node

   \param[in]  oru32_NodeIndex   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::SetNode(const uint32 & oru32_NodeIndex)
{
   //Reset visibility
   this->mpc_Ui->pc_ListsWidget->setVisible(false);
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mpc_Ui->pc_WidgetDpView->SetNode(oru32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in]  os32_DataPoolIndex      Datapool index
   \param[in]  os32_ListIndex          Optional list index (if not used set to -1)
   \param[in]  os32_DataElementIndex   Optional data element index (if not used set to -1)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::OpenDetail(const sint32 os32_DataPoolIndex, const sint32 os32_ListIndex,
                                     const sint32 os32_DataElementIndex) const
{
   C_OSCNodeDataPool::E_Type e_Type;
   this->mpc_Ui->pc_WidgetDpView->SetActualDataPool(static_cast<uint32>(os32_DataPoolIndex));

   if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex, static_cast<uint32>(os32_DataPoolIndex),
                                                        e_Type) == C_NO_ERR)
   {
      this->m_OpenDataPoolContent(os32_DataPoolIndex);

      if ((os32_ListIndex >= 0) &&
          (e_Type != C_OSCNodeDataPool::eCOM) &&
          (e_Type != C_OSCNodeDataPool::eHALC) &&
          (e_Type != C_OSCNodeDataPool::eHALC_NVM))
      {
         this->mpc_Ui->pc_ListsWidget->OpenDetail(os32_ListIndex, os32_DataElementIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Shows the Datapool overview again if it is visible
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::OpenOverview(void) const
{
   if (this->mpc_Ui->pc_WidgetDpView->isVisible() == false)
   {
      this->m_OnListsBack();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On lists back
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnListsBack(void) const
{
   this->mpc_Ui->pc_ListsWidget->setVisible(false);
   this->mpc_Ui->pc_WidgetDpView->setVisible(true);
   this->mpc_Ui->pc_WidgetDpView->SetNoActualDataPool();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On lists navigate

   \param[in]  oq_Forward  true: navigate to next Datapool
                           false: navigate to previous Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnListsNavigate(const bool oq_Forwards) const
{
   this->mpc_Ui->pc_WidgetDpView->NavigateToNextDataPool(oq_Forwards);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Opens the the matching actual Datapool content widget

   \param[in]  ou32_DataPoolIndex   Index of selected datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OpenDataPoolContent(const uint32 ou32_DataPoolIndex) const
{
   C_OSCNodeDataPool::E_Type e_Type;
   if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex, ou32_DataPoolIndex, e_Type) == C_NO_ERR)
   {
      bool q_ShowInternalWidget = true;

      //Decide
      if (e_Type == C_OSCNodeDataPool::eCOM)
      {
         Q_EMIT (this->SigSwitchToCommMessages(ou32_DataPoolIndex));
         q_ShowInternalWidget = false;
      }
      // Comment this else if block out, to get the HAL Datapool in the list edit widget
      else if ((e_Type == C_OSCNodeDataPool::eHALC) ||
               (e_Type == C_OSCNodeDataPool::eHALC_NVM))
      {
         Q_EMIT (this->SigSwitchToHalc());
         q_ShowInternalWidget = false;
      }
      else
      {
         this->mpc_Ui->pc_ListsWidget->SetDefaultVisible();
         this->mpc_Ui->pc_ListsWidget->SetDataPool(this->mu32_NodeIndex, ou32_DataPoolIndex);
      }

      this->mpc_Ui->pc_ListsWidget->setVisible(q_ShowInternalWidget);
      this->mpc_Ui->pc_WidgetDpView->setVisible(!q_ShowInternalWidget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error change

   \param[in]  orq_Error   Error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnErrorChange(const bool & orq_Error) const
{
   this->mpc_Ui->pc_WidgetDpView->SetActualDataPoolConflict(orq_Error);
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle size change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnSizeChange(void) const
{
   this->mpc_Ui->pc_WidgetDpView->UpdateActualDataPool();
}
