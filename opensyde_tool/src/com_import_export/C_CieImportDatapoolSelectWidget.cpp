//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting a datapool while DBC/EDS/DCF file import.

   Widget for selecting a datapool while DBC/EDS/DCF file import.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GtGetText.h"
#include "TGLUtils.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"

#include "C_CieImportDatapoolSelectWidget.h"
#include "ui_C_CieImportDatapoolSelectWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]    orc_Parent     Reference to parent
   \param[in]        orc_Datapools  Vector of pointers to all COMM datapools of type
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportDatapoolSelectWidget::C_CieImportDatapoolSelectWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 ou32_NodeIndex,
   const C_OSCCanProtocol::E_Type oe_ComProtocolType) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CieImportDatapoolSelectWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   me_ComProtocolType(oe_ComProtocolType)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // initialize combo box
   this->m_InitComboBox();

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_CieImportDatapoolSelectWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_CieImportDatapoolSelectWidget::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportDatapoolSelectWidget::~C_CieImportDatapoolSelectWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDatapoolSelectWidget::InitStaticNames(void) const
{
   const QString c_Description =
      QString(C_GtGetText::h_GetText("There are multiple Datapools with communication protocol type \"%1\" defined. "
                                     "Choose one Datapool for importing new messages.")).
      arg(C_PuiSdUtil::h_ConvertProtocolTypeToString(this->me_ComProtocolType));

   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Import New Messages"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Datapool Selection"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Select COMM Datapool"));
   this->mpc_Ui->pc_LabDescription->setText(c_Description);
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Import"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabDatapool->setText(C_GtGetText::h_GetText("Datapool"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return Datapool index of currently selected datapool.

   \return
   Node Datapool index

*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_CieImportDatapoolSelectWidget::GetSelectedDatapoolIndex(void) const
{
   uint32 u32_Return = 0;
   bool q_IndexFound = false;

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      // convert combo box index to datapool index
      sint32 s32_ItProtocolDatapool = 0;
      for (uint32 u32_ItDatapool = 0; u32_ItDatapool < pc_Node->c_DataPools.size(); ++u32_ItDatapool)
      {
         if (pc_Node->c_DataPools[u32_ItDatapool].e_Type == C_OSCNodeDataPool::eCOM)
         {
            if (C_PuiSdUtil::h_GetRelatedCANProtocolType(this->mu32_NodeIndex,
                                                         u32_ItDatapool) == this->me_ComProtocolType)
            {
               if (this->mpc_Ui->pc_CbxDatapools->currentIndex() == s32_ItProtocolDatapool)
               {
                  u32_Return = u32_ItDatapool;
                  q_IndexFound = true;
                  break;
               }
               ++s32_ItProtocolDatapool; // count protocol specific datapools up
            }
         }
      }
   }

   // if no matching index was found something really went wrong
   tgl_assert(q_IndexFound == true);

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDatapoolSelectWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDatapoolSelectWidget::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDatapoolSelectWidget::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize datapool combo box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDatapoolSelectWidget::m_InitComboBox(void) const
{
   const std::vector< const C_OSCNodeDataPool *> & rc_Datapools =
      C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPools(this->mu32_NodeIndex, this->me_ComProtocolType);

   for (std::vector< const C_OSCNodeDataPool *>::const_iterator c_It = rc_Datapools.begin(); c_It != rc_Datapools.end();
        ++c_It)
   {
      if (*c_It != NULL)
      {
         const C_OSCNodeDataPool & rc_Datapool = **c_It;
         this->mpc_Ui->pc_CbxDatapools->addItem(rc_Datapool.c_Name.c_str());
      }
   }
}
