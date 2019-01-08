//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for system definition update

   detailed description

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_SdNdeDataPoolEditWidget.h"
#include "ui_C_SdNdeDataPoolEditWidget.h"
#include "C_HeHandler.h"
#include "C_SdNdeDataPoolListHeaderWidget.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

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

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolEditWidget::C_SdNdeDataPoolEditWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDataPoolEditWidget),
   mu32_NodeIndex(0)
{
   // Init Ui
   mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_ListsWidget->setVisible(false);
   this->mpc_Ui->pc_ComListsWidget->setVisible(false);

   // connect to get the selected datapool in the selector
   // must happen before SetNode
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDataPoolViewWidget::SigErrorChange,
           this, &C_SdNdeDataPoolEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDataPoolViewWidget::SigDataPoolSelected,
           this, &C_SdNdeDataPoolEditWidget::m_DataPoolSelected);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDataPoolViewWidget::SigNoDataPoolSelected,
           this->mpc_Ui->pc_ListsWidget, &C_SdNdeDataPoolListsWidget::Clear);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDataPoolViewWidget::SigNoDataPoolSelected,
           this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDataPoolComListWidget::Clear);

   // forwarding of the changed signal
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDataPoolViewWidget::SigChanged,
           this, &C_SdNdeDataPoolEditWidget::SigChanged);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDataPoolListsWidget::SigChanged,
           this, &C_SdNdeDataPoolEditWidget::SigChanged);

   //Connect error handling
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDataPoolListsWidget::SigErrorChange, this,
           &C_SdNdeDataPoolEditWidget::m_OnErrorChange);
   //Connect size handling
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDataPoolListsWidget::SigSizeChange, this,
           &C_SdNdeDataPoolEditWidget::m_OnSizeChange);
   //Bus enter
   connect(this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDataPoolComListWidget::SigSwitchToBus, this,
           &C_SdNdeDataPoolEditWidget::m_OnSwitchToBus);
   // COM datapool edit
   connect(this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDataPoolComListWidget::SigEdit, this,
           &C_SdNdeDataPoolEditWidget::SigEdit);
   connect(this->mpc_Ui->pc_WidgetDpView, &C_SdNdeDataPoolViewWidget::SigUpdateLists, this->mpc_Ui->pc_ListsWidget,
           &C_SdNdeDataPoolListsWidget::SetDataPool);
   connect(this->mpc_Ui->pc_ComListsWidget, &C_SdNdeDataPoolComListWidget::SigErrorChange, this,
           &C_SdNdeDataPoolEditWidget::m_OnErrorChange);
   //Connect save commands
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDataPoolListsWidget::SigSave, this,
           &C_SdNdeDataPoolEditWidget::SigSave);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDataPoolListsWidget::SigSaveAs, this,
           &C_SdNdeDataPoolEditWidget::SigSaveAs);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolEditWidget::~C_SdNdeDataPoolEditWidget()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new node

   \param[in] oru32_NodeIndex Node index

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::SetNode(const uint32 & oru32_NodeIndex)
{
   //Reset visibility
   this->mpc_Ui->pc_ListsWidget->setVisible(false);
   this->mpc_Ui->pc_ComListsWidget->setVisible(false);
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mpc_Ui->pc_WidgetDpView->SetNode(oru32_NodeIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger check of data pool interaction availability

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::CheckDataPoolInteraction(void) const
{
   this->mpc_Ui->pc_WidgetDpView->CheckDataPoolInteraction();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in] os32_DataPoolIndex      Datapool index
   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_DataElementIndex   Optional data element index (if not used set to -1)

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::OpenDetail(const sint32 os32_DataPoolIndex, const sint32 os32_ListIndex,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Update communication lists

   \created     12.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::UpdateComLists(void) const
{
   this->mpc_Ui->pc_ComListsWidget->Update();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change of actual selected datapool

   \param[in] ou32_DataPoolIndex Index of selected datapool

   \created     22.02.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::m_DataPoolSelected(const uint32 ou32_DataPoolIndex) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle error change

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::m_OnErrorChange(const bool & orq_Error) const
{
   this->mpc_Ui->pc_WidgetDpView->SetActualDataPoolConflict(orq_Error);
   Q_EMIT SigErrorChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle size change

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::m_OnSizeChange(void) const
{
   this->mpc_Ui->pc_WidgetDpView->UpdateActualDataPool();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] oru32_BusIndex Bus index
   \param[in] orc_BusName  Bus name

   \created     23.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolEditWidget::m_OnSwitchToBus(const uint32 & oru32_BusIndex, const QString & orc_BusName)
{
   Q_EMIT this->SigSwitchToBus(oru32_BusIndex, orc_BusName);
}
