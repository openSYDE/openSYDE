//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboards settings

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDaDashboardSettings.hpp"
#include "ui_C_SyvDaDashboardSettings.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDaDashboardSettings::mhs32_MIN = 10;
const int32_t C_SyvDaDashboardSettings::mhs32_MAX = 65000;
const int32_t C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN = 10;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent     Reference to parent
   \param[in]     ou32_ViewIndex View index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardSettings::C_SyvDaDashboardSettings(C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ViewIndex) :
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
   this->mpc_Ui->pc_SpinBoxFast->SetMinimumCustom(C_SyvDaDashboardSettings::mhs32_MIN);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxMedium->SetMinimumCustom(
      C_SyvDaDashboardSettings::mhs32_MIN + C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxSlow->SetMinimumCustom(
      C_SyvDaDashboardSettings::mhs32_MIN + (2 * C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN));
   //Max
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxFast->SetMaximumCustom(
      C_SyvDaDashboardSettings::mhs32_MAX - (2 * C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN));
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxMedium->SetMaximumCustom(
      C_SyvDaDashboardSettings::mhs32_MAX - C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxSlow->SetMaximumCustom(C_SyvDaDashboardSettings::mhs32_MAX);
   //Step size
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxFast->setSingleStep(C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxMedium->setSingleStep(C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_SpinBoxSlow->setSingleStep(C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);

   this->m_Load();

   //Disable enter handling
   this->mpc_Ui->pc_PushButtonCancel->setDefault(true);
   this->mpc_Ui->pc_PushButtonCancel->setAutoDefault(true);
   this->mpc_Ui->pc_PushButtonOk->setDefault(true);
   this->mpc_Ui->pc_PushButtonOk->setAutoDefault(true);

   //Connects
   connect(this->mpc_Ui->pc_PushButtonOk, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDaDashboardSettings::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDaDashboardSettings::m_CancelClicked);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxFast, static_cast<void (QSpinBox::*)(
                                                        int32_t)>(&QSpinBox::valueChanged), this,
           &C_SyvDaDashboardSettings::m_OnFastChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxMedium, static_cast<void (QSpinBox::*)(
                                                          int32_t)>(&QSpinBox::valueChanged), this,
           &C_SyvDaDashboardSettings::m_OnMediumChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSlow, static_cast<void (QSpinBox::*)(
                                                        int32_t)>(&QSpinBox::valueChanged), this,
           &C_SyvDaDashboardSettings::m_OnSlowChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardSettings::~C_SyvDaDashboardSettings(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle save to data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::Save(void) const
{
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewUpdateRateFast(this->mu32_ViewIndex,
                                                                     static_cast<uint16_t>(this->mpc_Ui->pc_SpinBoxFast
                                                                                           ->
                                                                                           value())) ==
              C_NO_ERR);
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewUpdateRateMedium(this->mu32_ViewIndex,
                                                                       static_cast<uint16_t>(this->mpc_Ui->
                                                                                             pc_SpinBoxMedium->
                                                                                             value())) ==
              C_NO_ERR);
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewUpdateRateSlow(this->mu32_ViewIndex,
                                                                     static_cast<uint16_t>(this->mpc_Ui->pc_SpinBoxSlow
                                                                                           ->
                                                                                           value())) ==
              C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle load from data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_Load(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      this->mpc_Ui->pc_SpinBoxFast->setValue(static_cast<int32_t>(pc_View->GetUpdateRateFast()));
      this->mpc_Ui->pc_SpinBoxMedium->setValue(static_cast<int32_t>(pc_View->GetUpdateRateMedium()));
      this->mpc_Ui->pc_SpinBoxSlow->setValue(static_cast<int32_t>(pc_View->GetUpdateRateSlow()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On ok clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OkClicked(void)
{
   this->mrc_Parent.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On cancel clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_CancelClicked(void)
{
   this->mrc_Parent.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle fast value change

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OnFastChanged(const int32_t os32_Value) const
{
   const int32_t s32_FastValue = os32_Value;
   const int32_t s32_MediumValue = this->mpc_Ui->pc_SpinBoxMedium->value();

   if (s32_FastValue >= (s32_MediumValue - C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN))
   {
      this->mpc_Ui->pc_SpinBoxMedium->setValue(s32_FastValue + C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle medium value change

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OnMediumChanged(const int32_t os32_Value) const
{
   const int32_t s32_FastValue = this->mpc_Ui->pc_SpinBoxFast->value();
   const int32_t s32_MediumValue = os32_Value;
   const int32_t s32_SlowValue = this->mpc_Ui->pc_SpinBoxSlow->value();

   if (s32_FastValue >= (s32_MediumValue - C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN))
   {
      this->mpc_Ui->pc_SpinBoxFast->setValue(s32_MediumValue - C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   }

   if (s32_SlowValue <= (s32_MediumValue + C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN))
   {
      this->mpc_Ui->pc_SpinBoxSlow->setValue(s32_MediumValue + C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle slow value change

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSettings::m_OnSlowChanged(const int32_t os32_Value) const
{
   const int32_t s32_MediumValue = this->mpc_Ui->pc_SpinBoxMedium->value();
   const int32_t s32_SlowValue = os32_Value;

   if (s32_SlowValue <= (s32_MediumValue + C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN))
   {
      this->mpc_Ui->pc_SpinBoxMedium->setValue(s32_SlowValue - C_SyvDaDashboardSettings::mhs32_MIN_DISTANCE_BETWEEN);
   }
}
