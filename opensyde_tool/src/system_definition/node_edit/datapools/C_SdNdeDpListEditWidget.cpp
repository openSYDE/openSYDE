//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Edit widget for any datapool list

   Edit widget for any datapool list

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SdNdeDpListEditWidget.hpp"
#include "ui_C_SdNdeDpListEditWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
C_SdNdeDpListEditWidget::C_SdNdeDpListEditWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListEditWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // button icons
   this->mpc_Ui->pc_PubBack->InitCustomIcon("://images/system_definition/NodeEdit/lists/DatapoolOverview.svg",
                                            "://images/system_definition/NodeEdit/lists/DatapoolOverview.svg");
   this->mpc_Ui->pc_PubPrevious->SetSvg("://images/system_definition/NodeEdit/lists/BackArrowEnabled.svg", "", "",
                                        "://images/system_definition/NodeEdit/lists/BackArrowDisabled.svg");
   this->mpc_Ui->pc_PubNext->SetSvg("://images/system_definition/NodeEdit/lists/NextArrowEnabled.svg", "", "",
                                    "://images/system_definition/NodeEdit/lists/NextArrowDisabled.svg");

   this->mpc_Ui->pc_ListsWidget->setVisible(false);

   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigChanged,
           this, &C_SdNdeDpListEditWidget::SigChanged);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigErrorChange,
           this, &C_SdNdeDpListEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigSizeChange,
           this, &C_SdNdeDpListEditWidget::SigSizeChange);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigSave,
           this, &C_SdNdeDpListEditWidget::SigSave);
   connect(this->mpc_Ui->pc_ListsWidget, &C_SdNdeDpListsWidget::SigSaveAs,
           this, &C_SdNdeDpListEditWidget::SigSaveAs);
   connect(this->mpc_Ui->pc_PubBack, &QPushButton::clicked, this, &C_SdNdeDpListEditWidget::m_OnBack);
   connect(this->mpc_Ui->pc_PubPrevious, &QPushButton::clicked, this, &C_SdNdeDpListEditWidget::m_OnPrevious);
   connect(this->mpc_Ui->pc_PubNext, &QPushButton::clicked, this, &C_SdNdeDpListEditWidget::m_OnNext);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListEditWidget::~C_SdNdeDpListEditWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::Clear(void) const
{
   this->mpc_Ui->pc_ListsWidget->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PubBack->setText(C_GtGetText::h_GetText("Back to Overview"));
   this->mpc_Ui->pc_PubPrevious->setText(C_GtGetText::h_GetText("Prev."));
   this->mpc_Ui->pc_PubNext->setText(C_GtGetText::h_GetText("Next"));
   this->mpc_Ui->pc_PubBack->SetToolTipInformation(C_GtGetText::h_GetText("Back to Overview"),
                                                   C_GtGetText::h_GetText("Go back to Datapool overview."));
   // tool tips of previous and next buttons are dynamic
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set default visible
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::SetDefaultVisible(void) const
{
   this->mpc_Ui->pc_ListsWidget->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open detail

   \param[in]  os32_ListIndex          List index
   \param[in]  os32_DataElementIndex   Data element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::OpenDetail(const int32_t os32_ListIndex, const int32_t os32_DataElementIndex) const
{
   this->mpc_Ui->pc_ListsWidget->OpenDetail(os32_ListIndex, os32_DataElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data pool

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::SetDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) const
{
   this->mpc_Ui->pc_ListsWidget->SetDataPool(ou32_NodeIndex, ou32_DataPoolIndex);
   this->m_UpdateButtonTooltips(ou32_NodeIndex, ou32_DataPoolIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event slot

   Here: go back to Datapool overview on escape key press

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      this->m_OnBack();
      opc_Event->accept();
   }
   else
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On back
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::m_OnBack(void)
{
   Q_EMIT (this->SigBack());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On previous
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::m_OnPrevious(void)
{
   Q_EMIT (this->SigNavigate(false));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On next
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::m_OnNext(void)
{
   Q_EMIT (this->SigNavigate(true));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update button tool tips: next and previous Datapool

   \param[in]  ou32_DatapoolIndex   Datapool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListEditWidget::m_UpdateButtonTooltips(const uint32_t ou32_NodeIndex,
                                                     const uint32_t ou32_DatapoolIndex) const
{
   const int32_t s32_Next = C_SdNdeDpUtil::h_GetNextDiagOrNvmDpIndex(ou32_NodeIndex, ou32_DatapoolIndex, true);
   const int32_t s32_Prev = C_SdNdeDpUtil::h_GetNextDiagOrNvmDpIndex(ou32_NodeIndex, ou32_DatapoolIndex, false);
   const C_OscNodeDataPool * pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(ou32_NodeIndex, s32_Next);

   // next
   if ((pc_Dp != NULL) && (s32_Next >= 0))
   {
      this->mpc_Ui->pc_PubNext->
      SetToolTipInformation(C_GtGetText::h_GetText("Go to Next Datapool"),
                            static_cast<QString>(C_GtGetText::h_GetText("#%1 %2 (%3)")).
                            arg(C_PuiSdHandler::h_GetInstance()->GetDataPoolTypeIndex(ou32_NodeIndex, s32_Next) + 1).
                            arg(pc_Dp->c_Name.c_str()).
                            arg(C_PuiSdUtil::h_ConvertDataPoolTypeToString(pc_Dp->e_Type)));
   }

   // previous
   pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(ou32_NodeIndex, s32_Prev);
   if ((pc_Dp != NULL) && (s32_Next >= 0))
   {
      this->mpc_Ui->pc_PubPrevious->
      SetToolTipInformation(C_GtGetText::h_GetText("Go to Previous Datapool"),
                            static_cast<QString>(C_GtGetText::h_GetText("#%1 %2 (%3)")).
                            arg(C_PuiSdHandler::h_GetInstance()->GetDataPoolTypeIndex(ou32_NodeIndex, s32_Prev) + 1).
                            arg(pc_Dp->c_Name.c_str()).
                            arg(C_PuiSdUtil::h_ConvertDataPoolTypeToString(pc_Dp->e_Type)));
   }
}
