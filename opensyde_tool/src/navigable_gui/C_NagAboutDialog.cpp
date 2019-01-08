//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for "About openSYDE" popup dialog. (implementation)

   Widget for "About openSYDE" popup dialog.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_NagAboutDialog.h"
#include "ui_C_NagAboutDialog.h"
#include "C_HeHandler.h"
#include "C_Uti.h"

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

   \param[in,out] orc_Parent Reference to parent

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_NagAboutDialog::C_NagAboutDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_NagAboutDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   // init text and logo
   InitStaticNames();
   InitDynamicNames();
   InitLogo();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_NagAboutDialog::m_OkClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_NagAboutDialog::~C_NagAboutDialog(void)
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagAboutDialog::InitStaticNames(void) const
{
   QString c_Text;

   mpc_Ui->pc_LabelDescOpenSYDEVersion->setText(C_GtGetText::h_GetText("Version"));
   this->mpc_Ui->pc_AboutCopyright->setText(C_GtGetText::h_GetText("Copyright"));

   c_Text =  C_GtGetText::h_GetText("Sensor-Technik Wiedemann GmbH. All rights reserved.\n");
   c_Text +=  C_GtGetText::h_GetText("\n");
   c_Text +=  C_GtGetText::h_GetText("openSYDE is provided under GPL V3 license.\n");
   c_Text +=  C_GtGetText::h_GetText("\n");
   c_Text += C_GtGetText::h_GetText("openSYDE uses the following open source libraries:\n");
   c_Text += C_GtGetText::h_GetText("    - Qt 5.7.0 by The Qt Company\n");
   c_Text += C_GtGetText::h_GetText("    - gettext by the Free Software Foundation\n");
   c_Text += C_GtGetText::h_GetText("    - Vector::DBC Module by Tobias Lorenz\n");
   c_Text += C_GtGetText::h_GetText("    - TinyXML-2 by Lee Thomason and others\n");
   c_Text += C_GtGetText::h_GetText("    - The MinGW Runtime\n");
   c_Text += C_GtGetText::h_GetText("    - Miniz\n");

   this->mpc_Ui->pc_LabelValCopyright->setText(c_Text);

   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("ABOUT"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("openSYDE"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize displayed dynamic names

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------

void C_NagAboutDialog::InitDynamicNames(void) const
{
   //lint -e{40} Defined by project file
   mpc_Ui->pc_LabelValOpenSYDEVersion->setText(C_Uti::h_ConvertVersionToSTWStyle(APPLICATION_VERSION));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize displayed logo

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------

void C_NagAboutDialog::InitLogo(void) const
{
   QPixmap c_AboutLogo;

   c_AboutLogo.load(":/images/LogoOpensyde_XXL.png");
   c_AboutLogo = c_AboutLogo.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   mpc_Ui->pc_LabelLogo->setPixmap(c_AboutLogo);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagAboutDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagAboutDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}
