//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for system view dashboard data element update mode (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_GtGetText.h"
#include "C_SyvDaDashboardSettings.h"
#include "C_SyvDaPeUpdateModeConfiguration.h"
#include "ui_C_SyvDaPeUpdateModeConfiguration.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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

   \param[in,out] orc_Parent     Reference to parent
   \param[in]     ou32_ViewIndex View index

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeConfiguration::C_SyvDaPeUpdateModeConfiguration(C_OgePopUpDialog & orc_Parent,
                                                                   const stw_types::uint32 ou32_ViewIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeUpdateModeConfiguration),
   mu32_ViewIndex(ou32_ViewIndex),
   mrc_ParentDialog(orc_Parent)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_TreeWidget->Init(ou32_ViewIndex);

   //Handle no elements
   if (this->mpc_Ui->pc_TreeWidget->topLevelItemCount() > 0)
   {
      this->mpc_Ui->pc_TreeWidget->setVisible(true);
      this->mpc_Ui->pc_GroupBoxNoElements->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_TreeWidget->setVisible(false);
      this->mpc_Ui->pc_GroupBoxNoElements->setVisible(true);
   }

   //Remove debug string
   this->mpc_Ui->pc_GroupBoxNoElements->setTitle("");

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Dashboards"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Configuration"));
   InitStaticNames();

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SyvDaPeUpdateModeConfiguration::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SyvDaPeUpdateModeConfiguration::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonConfigure, &QPushButton::clicked, this,
           &C_SyvDaPeUpdateModeConfiguration::m_ConfigureClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeConfiguration::~C_SyvDaPeUpdateModeConfiguration(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeConfiguration::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelDataElament->setText(C_GtGetText::h_GetText("Diagnostic Data Elements"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_PushButtonConfigure->setText(C_GtGetText::h_GetText("Configure Cyclic transmission intervals"));
   this->mpc_Ui->pc_LabelNoElements->setText(C_GtGetText::h_GetText("No Diagnostic Data Elements declared."));
   //Tooltips
   this->mpc_Ui->pc_PushButtonConfigure->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                  "Configure Cyclic transmission intervals"),
                                                               C_GtGetText::h_GetText(
                                                                  "Configure the transmission intervals used by data elements in transmission mode \"Cyclic\""));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select specified ID

   \param[in] orc_Id ID to select

   \created     19.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeConfiguration::Select(const C_PuiSvDbNodeDataPoolListElementId & orc_Id) const
{
   this->mpc_Ui->pc_TreeWidget->Select(orc_Id);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     19.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeConfiguration::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->mrc_ParentDialog.accept();
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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeConfiguration::m_OkClicked(void)
{
   //Apply internal data
   this->mpc_Ui->pc_TreeWidget->ApplyData();
   this->mrc_ParentDialog.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Cancel button

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeConfiguration::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Configure button

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeConfiguration::m_ConfigureClicked(void) const
{
   //Set parent for better hierarchy handling via window manager
   QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());
   C_SyvDaDashboardSettings * pc_Dialog = new C_SyvDaDashboardSettings(*c_New, this->mu32_ViewIndex);

   //Resize
   c_New->SetSize(QSize(600, 430));

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      pc_Dialog->Save();
   }

   //Hide overlay after dialog is not relevant anymore
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent pc_Dialog and the Qt memory management
}
