//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for spin boxes of all available types

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QKeyEvent>
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OgeWiSpinBoxGroup.h"
#include "ui_C_OgeWiSpinBoxGroup.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiSpinBoxGroup::C_OgeWiSpinBoxGroup(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiSpinBoxGroup),
   mq_DoubleMode(true)
{
   mpc_Ui->setupUi(this);

   m_InitDefault();

   //Necessary for table edit focus
   this->setFocusPolicy(Qt::StrongFocus);
   this->mpc_Ui->pc_SpinBox64->setFocusProxy(this);
   this->mpc_Ui->pc_DoubleSpinBox->setFocusProxy(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiSpinBoxGroup::~C_OgeWiSpinBoxGroup(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current text

   \return
   Current text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeWiSpinBoxGroup::GetText(void) const
{
   QString c_Retval;

   if (this->mq_DoubleMode == false)
   {
      c_Retval = this->mpc_Ui->pc_SpinBox64->text();
   }
   else
   {
      c_Retval = this->mpc_Ui->pc_DoubleSpinBox->text();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize widget with valid values

   \param[in] orc_Min     Unscaled minimum value
   \param[in] orc_Max     Unscaled maximum value
   \param[in] of64_Factor Scaling factor
   \param[in] of64_Offset Scaling offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::Init(const C_OSCNodeDataPoolContent & orc_Min, const C_OSCNodeDataPoolContent & orc_Max,
                               const float64 of64_Factor, const float64 of64_Offset, const uint32 ou32_Index)
{
   std::vector<C_OSCNodeDataPoolContent::E_Type> c_Types;
   m_DeactivateConnections();
   if (C_SdNdeDataPoolContentUtil::h_GetMinimalTypeAfterScaling(orc_Min, orc_Max, of64_Factor, of64_Offset,
                                                                c_Types) == C_NO_ERR)
   {
      if (ou32_Index < c_Types.size())
      {
         switch (c_Types[ou32_Index])
         {
         case C_OSCNodeDataPoolContent::eSINT8:
         case C_OSCNodeDataPoolContent::eSINT16:
         case C_OSCNodeDataPoolContent::eSINT32:
         case C_OSCNodeDataPoolContent::eSINT64:
            this->mpc_Ui->pc_SpinBox64->setVisible(true);
            this->mpc_Ui->pc_DoubleSpinBox->setVisible(false);
            this->mq_DoubleMode = false;

            this->mpc_Ui->pc_SpinBox64->SetIsUnsigned(false);

            this->mpc_Ui->pc_SpinBox64->Init(orc_Min, orc_Max, of64_Factor, of64_Offset, ou32_Index);
            break;
         case C_OSCNodeDataPoolContent::eUINT8:
         case C_OSCNodeDataPoolContent::eUINT16:
         case C_OSCNodeDataPoolContent::eUINT32:
         case C_OSCNodeDataPoolContent::eUINT64:
            this->mpc_Ui->pc_SpinBox64->setVisible(true);
            this->mpc_Ui->pc_DoubleSpinBox->setVisible(false);
            this->mq_DoubleMode = false;

            this->mpc_Ui->pc_SpinBox64->SetIsUnsigned(true);

            this->mpc_Ui->pc_SpinBox64->Init(orc_Min, orc_Max, of64_Factor, of64_Offset, ou32_Index);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
         case C_OSCNodeDataPoolContent::eFLOAT64:
            this->mpc_Ui->pc_SpinBox64->setVisible(false);
            this->mpc_Ui->pc_DoubleSpinBox->setVisible(true);
            this->mq_DoubleMode = true;

            this->mpc_Ui->pc_DoubleSpinBox->Init(orc_Min, orc_Max, of64_Factor, of64_Offset, ou32_Index);
            break;
         }
      }
      else
      {
         m_InitDefault();
      }
   }
   else
   {
      m_InitDefault();
   }
   //Afterwards (Requires valid spin box set)
   m_InitConnections();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update suffix

   \param[in] orc_Value New suffix
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::SetSuffix(const QString & orc_Value) const
{
   this->mpc_Ui->pc_DoubleSpinBox->setSuffix(orc_Value);
   this->mpc_Ui->pc_SpinBox64->SetSuffix(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Assign new value to spin box

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::SetValue(const QVariant & orc_Value) const
{
   if (this->mq_DoubleMode == false)
   {
      this->mpc_Ui->pc_SpinBox64->SetValue(orc_Value, false);
   }
   else
   {
      float64 f64_Value;
      switch (orc_Value.type())
      {
      case QVariant::Int:
         f64_Value = static_cast<float64>(orc_Value.toInt());
         break;
      case QVariant::UInt:
         f64_Value = static_cast<float64>(orc_Value.toUInt());
         break;
      case QVariant::LongLong:
         f64_Value = static_cast<float64>(orc_Value.toLongLong());
         break;
      case QVariant::ULongLong:
         f64_Value = static_cast<float64>(orc_Value.toULongLong());
         break;
      case QVariant::Double:
         f64_Value = orc_Value.toDouble();
         break;
      default:
         f64_Value = 0.0;
         break;
      }
      this->mpc_Ui->pc_DoubleSpinBox->setValue(f64_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value

   \return
   Current value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_OgeWiSpinBoxGroup::GetValue(void) const
{
   QVariant c_Retval;

   if (this->mq_DoubleMode == false)
   {
      c_Retval = this->mpc_Ui->pc_SpinBox64->GetValue();
   }
   else
   {
      c_Retval = this->mpc_Ui->pc_DoubleSpinBox->value();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Force spin box validation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::InterpretText(void) const
{
   if (this->mq_DoubleMode == false)
   {
      this->mpc_Ui->pc_SpinBox64->interpretText();
   }
   else
   {
      this->mpc_Ui->pc_DoubleSpinBox->interpretText();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select all content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::SelectAll(void) const
{
   if (this->mq_DoubleMode == false)
   {
      this->mpc_Ui->pc_SpinBox64->selectAll();
   }
   else
   {
      this->mpc_Ui->pc_DoubleSpinBox->selectAll();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the strings for the tooltip

   \param[in]     orc_Heading     String with heading
   \param[in]     orc_Content     String with content
   \param[in]     oe_Type         Optional tooltip type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::SetToolTipInformation(const QString & orc_Heading, const QString & orc_Content,
                                                const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type) const
{
   if (this->mq_DoubleMode == false)
   {
      this->mpc_Ui->pc_SpinBox64->SetToolTipInformation(orc_Heading, orc_Content, oe_Type);
   }
   else
   {
      this->mpc_Ui->pc_DoubleSpinBox->SetToolTipInformation(orc_Heading, orc_Content, oe_Type);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten event slot

   Here: Forward event to spin box (Necessary for table edit handling)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeWiSpinBoxGroup::event(QEvent * const opc_Event)
{
   switch (opc_Event->type())
   {
   case QEvent::ShortcutOverride:
      if (this->mq_DoubleMode == true)
      {
         this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
      }
      else
      {
         this->mpc_Ui->pc_SpinBox64->event(opc_Event);
      }
      break;
   case QEvent::InputMethod:
      if (this->mq_DoubleMode == true)
      {
         this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
      }
      else
      {
         this->mpc_Ui->pc_SpinBox64->event(opc_Event);
      }
      break;
   default:
      break;
   }

   return QWidget::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Forward event to spin box (Necessary for table edit handling)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (this->mq_DoubleMode == true)
   {
      this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
   }
   else
   {
      this->mpc_Ui->pc_SpinBox64->event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key release event slot

   Here: Forward event to spin box (Necessary to properly register key release)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::keyReleaseEvent(QKeyEvent * const opc_Event)
{
   if (this->mq_DoubleMode == true)
   {
      this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
   }
   else
   {
      this->mpc_Ui->pc_SpinBox64->event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Forward event to spin box (Necessary for table edit handling)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (this->mq_DoubleMode == true)
   {
      this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
   }
   else
   {
      this->mpc_Ui->pc_SpinBox64->event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: Forward event to spin box (Necessary for table edit handling)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   if (this->mq_DoubleMode == true)
   {
      this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
   }
   else
   {
      this->mpc_Ui->pc_SpinBox64->event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus in event slot

   Here: Forward event to spin box (Necessary for table edit handling)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::focusInEvent(QFocusEvent * const opc_Event)
{
   if (this->mq_DoubleMode == true)
   {
      this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
   }
   else
   {
      this->mpc_Ui->pc_SpinBox64->event(opc_Event);
   }
   QWidget::focusInEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus out event slot

   Here: Forward event to spin box (Necessary for table edit handling)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::focusOutEvent(QFocusEvent * const opc_Event)
{
   if (this->mq_DoubleMode == true)
   {
      this->mpc_Ui->pc_DoubleSpinBox->event(opc_Event);
   }
   else
   {
      this->mpc_Ui->pc_SpinBox64->event(opc_Event);
   }
   QWidget::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resize to default height (30)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::m_ResizeToDefault(const stw_types::sintn osn_Height) const
{
   this->mpc_Ui->pc_SpinBox64->setMinimumHeight(osn_Height);
   this->mpc_Ui->pc_DoubleSpinBox->setMinimumHeight(osn_Height);
   this->mpc_Ui->pc_SpinBox64->setMaximumHeight(osn_Height);
   this->mpc_Ui->pc_DoubleSpinBox->setMaximumHeight(osn_Height);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current spin button width

   \return
   Current spin button width
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_OgeWiSpinBoxGroup::GetSpinButtonWidth(void) const
{
   sintn sn_Retval;
   sintn sn_LineEditWidth;

   if (this->mq_DoubleMode == true)
   {
      sn_LineEditWidth = this->mpc_Ui->pc_DoubleSpinBox->GetLineEditWidth();
   }
   else
   {
      sn_LineEditWidth = this->mpc_Ui->pc_SpinBox64->GetLineEditWidth();
   }
   sn_Retval = (this->width() - 2) - sn_LineEditWidth;
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize default state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::m_InitDefault(void)
{
   //ME: do not explicitly set this visible as this will break the stylesheet for the double spin box
   //this->mpc_Ui->pc_DoubleSpinBox->setVisible(true);
   this->mpc_Ui->pc_SpinBox64->setVisible(false);
   this->mq_DoubleMode = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init changed connections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::m_InitConnections(void)
{
   if (this->mq_DoubleMode == true)
   {
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      connect(this->mpc_Ui->pc_DoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(
                                                             stw_types::float64)>(&QDoubleSpinBox::valueChanged), this,
              &C_OgeWiSpinBoxGroup::SigValueChanged);
   }
   else
   {
      connect(this->mpc_Ui->pc_SpinBox64, &C_OgeSpxInt64AutoFix::SigValueChanged, this,
              &C_OgeWiSpinBoxGroup::SigValueChanged);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Deactivate any left over connections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiSpinBoxGroup::m_DeactivateConnections(void)
{
   if (this->mq_DoubleMode == true)
   {
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      disconnect(this->mpc_Ui->pc_DoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(
                                                                stw_types::float64)>(&QDoubleSpinBox::valueChanged), this,
                 &C_OgeWiSpinBoxGroup::SigValueChanged);
   }
   else
   {
      disconnect(this->mpc_Ui->pc_SpinBox64, &C_OgeSpxInt64AutoFix::SigValueChanged, this,
                 &C_OgeWiSpinBoxGroup::SigValueChanged);
   }
}
