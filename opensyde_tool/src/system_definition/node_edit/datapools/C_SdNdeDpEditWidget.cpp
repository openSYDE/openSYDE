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

   \param[in,out] opc_Parent Optional pointer to parent
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
   this->mpc_Ui->pc_ComListsWidget->setVisible(false);

   // connect to get the selected datapool in the selector
   // must happen before SetNode
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigErrorChange,
           this, &C_SdNdeDpEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigDataPoolSelected,
           this, &C_SdNdeDpEditWidget::m_DataPoolSelected);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigNoDataPoolSelected,
           this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::Clear);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigNoDataPoolSelected,
           this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDpComListWidget::Clear);

   // forwarding of the changed signal
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigChanged,
           this, &C_SdNdeDpEditWidget::SigChanged);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigChanged,
           this, &C_SdNdeDpEditWidget::SigChanged);

   //Connect error handling
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigErrorChange, this,
           &C_SdNdeDpEditWidget::m_OnErrorChange);
   //Connect size handling
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigSizeChange, this,
           &C_SdNdeDpEditWidget::m_OnSizeChange);
   //Bus enter
   connect(this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDpComListWidget::SigSwitchToBus, this,
           &C_SdNdeDpEditWidget::m_OnSwitchToBus);
   // COM datapool edit
   connect(this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDpComListWidget::SigEdit, this,
           &C_SdNdeDpEditWidget::SigEdit);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDpViewWidget::SigUpdateLists, this->mpc_Ui->pc_ListsWidget,
           &C_SdNdeDpListsWidget::SetDataPool);
   connect(this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDpComListWidget::SigErrorChange, this,
           &C_SdNdeDpEditWidget::m_OnErrorChange);
   //Connect save commands
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigSave, this,
           &C_SdNdeDpEditWidget::SigSave);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigSaveAs, this,
           &C_SdNdeDpEditWidget::SigSaveAs);
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

   \param[in] oru32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::SetNode(const uint32 & oru32_NodeIndex)
{
   //Reset visibility
   this->mpc_Ui->pc_ListsWidget->setVisible(false);
   this->mpc_Ui->pc_ComListsWidget->setVisible(false);
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mpc_Ui->pc_WidgetDpView->SetNode(oru32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in] os32_DataPoolIndex      Datapool index
   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_DataElementIndex   Optional data element index (if not used set to -1)
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
      this->m_DataPoolSelected(os32_DataPoolIndex);

      if ((os32_ListIndex >= 0) && (e_Type != C_OSCNodeDataPool::eCOM))
      {
         this->mpc_Ui->pc_ListsWidget->OpenDetail(os32_ListIndex, os32_DataElementIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update communication lists
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::UpdateComLists(void) const
{
   this->mpc_Ui->pc_ComListsWidget->Update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change of actual selected datapool

   \param[in] ou32_DataPoolIndex Index of selected datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_DataPoolSelected(const uint32 ou32_DataPoolIndex) const
{
   C_OSCNodeDataPool::E_Type e_Type;
   if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex, ou32_DataPoolIndex, e_Type) == C_NO_ERR)
   {
      //Decide
      if (e_Type == C_OSCNodeDataPool::eCOM)
      {
         this->mpc_Ui->pc_ComListsWidget->setVisible(true);
         this->mpc_Ui->pc_ListsWidget->setVisible(false);
         this->mpc_Ui->pc_ComListsWidget->SetDataPool(this->mu32_NodeIndex, ou32_DataPoolIndex);
      }
      else
      {
         this->mpc_Ui->pc_ComListsWidget->setVisible(false);
         this->mpc_Ui->pc_ListsWidget->setVisible(true);
         this->mpc_Ui->pc_ListsWidget->SetDataPool(this->mu32_NodeIndex, ou32_DataPoolIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnErrorChange(const bool & orq_Error) const
{
   this->mpc_Ui->pc_WidgetDpView->SetActualDataPoolConflict(orq_Error);
   Q_EMIT this->SigErrorChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle size change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnSizeChange(void) const
{
   this->mpc_Ui->pc_WidgetDpView->UpdateActualDataPool();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in] oru32_BusIndex Bus index
   \param[in] orc_BusName  Bus name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpEditWidget::m_OnSwitchToBus(const uint32 & oru32_BusIndex, const QString & orc_BusName)
{
   Q_EMIT (this->SigSwitchToBus(oru32_BusIndex, orc_BusName));
}
