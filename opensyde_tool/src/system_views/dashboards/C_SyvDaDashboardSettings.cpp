//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for system view dashboards settings

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDaDashboardSettings.h"
#include "ui_C_SyvDaDashboardSettings.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const stw_types::sintn C_SyvDaDashboardSettings::mhsn_Min = 10;
const stw_types::sintn C_SyvDaDashboardSettings::mhsn_Max = 65000;
const stw_types::sintn C_SyvDaDashboardSettings::mhsn_MinDistanceBetween = 10;

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

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardSettings::C_SyvDaDashboardSettings(C_OgePopUpDialog & orc_Parent,
                                                   const stw_types::uint32 ou32_ViewIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaDashboardSettings),
   mrc_Parent(orc_Parent),
   mu32_ViewIndex(ou32_ViewIndex)
{
   mpc_Ui->setupUi(this);
   this->mrc_Parent.SetWidget(this);
   InitStaticNames();

   //Min
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxFast->SetMinimumCustom(C_SyvDaDashboardSettings::mhsn_Min);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxMedium->SetMinimumCustom(
      C_SyvDaDashboardSettings::mhsn_Min + C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxSlow->SetMinimumCustom(
      C_SyvDaDashboardSettings::mhsn_Min + (2 * C_SyvDaDashboardSettings::mhsn_MinDistanceBetween));
   //Max
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxFast->SetMaximumCustom(
      C_SyvDaDashboardSettings::mhsn_Max - (2 * C_SyvDaDashboardSettings::mhsn_MinDistanceBetween));
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxMedium->SetMaximumCustom(
      C_SyvDaDashboardSettings::mhsn_Max - C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxSlow->SetMaximumCustom(C_SyvDaDashboardSettings::mhsn_Max);
   //Step size
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxFast->setSingleStep(C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxMedium->setSingleStep(C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxSlow->setSingleStep(C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);

   this->m_Load();

   //Disable enter handling
   this->mpc_Ui->pc_PushButtonCancel->setDefault(true);
   this->mpc_Ui->pc_PushButtonCancel->setAutoDefault(true);
   this->mpc_Ui->pc_PushButtonOk->setDefault(true);
   this->mpc_Ui->pc_PushButtonOk->setAutoDefault(true);

   //Connects
   connect(this->mpc_Ui->pc_PushButtonOk, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDaDashboardSettings::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDaDashboardSettings::m_CancelClicked);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxFast, static_cast<void (QSpinBox::*)(
                                                        sintn)>(&QSpinBox::valueChanged), this,
           &C_SyvDaDashboardSettings::m_OnFastChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxMedium, static_cast<void (QSpinBox::*)(
                                                          sintn)>(&QSpinBox::valueChanged), this,
           &C_SyvDaDashboardSettings::m_OnMediumChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSlow, static_cast<void (QSpinBox::*)(
                                                        sintn)>(&QSpinBox::valueChanged), this,
           &C_SyvDaDashboardSettings::m_OnSlowChanged);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardSettings::~C_SyvDaDashboardSettings(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::InitStaticNames(void) const
{
   this->mrc_Parent.SetTitle(C_GtGetText::h_GetText("Data Transmission"));
   this->mrc_Parent.SetSubTitle(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Cyclic Transmission Intervals"));
   this->mpc_Ui->pc_LabelFast->setText(C_GtGetText::h_GetText("Fast"));
   this->mpc_Ui->pc_LabelMedium->setText(C_GtGetText::h_GetText("Medium"));
   this->mpc_Ui->pc_LabelSlow->setText(C_GtGetText::h_GetText("Slow"));
   this->mpc_Ui->pc_SpinBoxFast->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpinBoxMedium->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpinBoxSlow->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   //Tooltips
   this->mpc_Ui->pc_LabelFast->SetToolTipInformation(C_GtGetText::h_GetText("Fast"),
                                                     C_GtGetText::h_GetText(
                                                        "Data elements configured using the cyclic interval \"Fast\" will use this cyclic transmission time"));
   this->mpc_Ui->pc_LabelMedium->SetToolTipInformation(C_GtGetText::h_GetText("Medium"), C_GtGetText::h_GetText(
                                                          "Data elements configured using the cyclic interval \"Medium\" will use this cyclic transmission time"));
   this->mpc_Ui->pc_LabelSlow->SetToolTipInformation(C_GtGetText::h_GetText("Slow"),
                                                     C_GtGetText::h_GetText(
                                                        "Data elements configured using the cyclic interval \"Slow\" will use this cyclic transmission time"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle save to data

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::Save(void) const
{
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewUpdateRateFast(this->mu32_ViewIndex,
                                                                     static_cast<uint16>(this->mpc_Ui->pc_SpinBoxFast->
                                                                                         value())) ==
              C_NO_ERR);
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewUpdateRateMedium(this->mu32_ViewIndex,
                                                                       static_cast<uint16>(this->mpc_Ui->
                                                                                           pc_SpinBoxMedium->
                                                                                           value())) ==
              C_NO_ERR);
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewUpdateRateSlow(this->mu32_ViewIndex,
                                                                     static_cast<uint16>(this->mpc_Ui->pc_SpinBoxSlow->
                                                                                         value())) ==
              C_NO_ERR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->mrc_Parent.accept();
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
   \brief   Handle load from data

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_Load(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      this->mpc_Ui->pc_SpinBoxFast->setValue(static_cast<sintn>(pc_View->GetUpdateRateFast()));
      this->mpc_Ui->pc_SpinBoxMedium->setValue(static_cast<sintn>(pc_View->GetUpdateRateMedium()));
      this->mpc_Ui->pc_SpinBoxSlow->setValue(static_cast<sintn>(pc_View->GetUpdateRateSlow()));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   On ok clicked

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OkClicked(void)
{
   this->mrc_Parent.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On cancel clicked

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_CancelClicked(void)
{
   this->mrc_Parent.reject();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle fast value change

   \param[in] osn_Value New value

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OnFastChanged(const stw_types::sintn osn_Value) const
{
   const sintn sn_FastValue = osn_Value;
   const sintn sn_MediumValue = this->mpc_Ui->pc_SpinBoxMedium->value();

   if (sn_FastValue >= (sn_MediumValue - C_SyvDaDashboardSettings::mhsn_MinDistanceBetween))
   {
      this->mpc_Ui->pc_SpinBoxMedium->setValue(sn_FastValue + C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle medium value change

   \param[in] osn_Value New value

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OnMediumChanged(const stw_types::sintn osn_Value) const
{
   const sintn sn_FastValue = this->mpc_Ui->pc_SpinBoxFast->value();
   const sintn sn_MediumValue = osn_Value;
   const sintn sn_SlowValue = this->mpc_Ui->pc_SpinBoxSlow->value();

   if (sn_FastValue >= (sn_MediumValue - C_SyvDaDashboardSettings::mhsn_MinDistanceBetween))
   {
      this->mpc_Ui->pc_SpinBoxFast->setValue(sn_MediumValue - C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   }

   if (sn_SlowValue <= (sn_MediumValue + C_SyvDaDashboardSettings::mhsn_MinDistanceBetween))
   {
      this->mpc_Ui->pc_SpinBoxSlow->setValue(sn_MediumValue + C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle slow value change

   \param[in] osn_Value New value

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OnSlowChanged(const stw_types::sintn osn_Value) const
{
   const sintn sn_MediumValue = this->mpc_Ui->pc_SpinBoxMedium->value();
   const sintn sn_SlowValue = osn_Value;

   if (sn_SlowValue <= (sn_MediumValue + C_SyvDaDashboardSettings::mhsn_MinDistanceBetween))
   {
      this->mpc_Ui->pc_SpinBoxMedium->setValue(sn_SlowValue - C_SyvDaDashboardSettings::mhsn_MinDistanceBetween);
   }
}
