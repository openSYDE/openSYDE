//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for dashboard spin boxes of all available types (implementation)

   Widget for dashboard spin boxes of all available types

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeWiDashboardSpinBoxGroup.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
C_OgeWiDashboardSpinBoxGroup::C_OgeWiDashboardSpinBoxGroup(QWidget * const opc_Parent) :
   C_OgeWiSpinBoxGroup(opc_Parent),
   mc_Unit(""),
   mq_ShowUnit(false)
{
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Type", 2);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::AdjustFontToSize(void)
{
   //Consider borders (absolute) + buttons (relative)!
   const int32_t s32_ImprovedWidth = (this->size().width() - 2) - m_GetSpinButtonWidth();
   const QSize c_Size = QSize(std::max(1, s32_ImprovedWidth), this->size().height() - 2);
   const int32_t s32_NewPointSize = C_OgeWiUtil::h_UpdateFontSize(this, this->GetText(), 1.0F, false, &c_Size);

   if (s32_NewPointSize > 0)
   {
      this->setStyleSheet(static_cast<QString>("*{font-size:%1pt;}").arg(s32_NewPointSize));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update current unit

   \param[in]  orc_Value   New unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::SetUnit(const QString & orc_Value)
{
   this->mc_Unit = orc_Value;

   if (this->mq_ShowUnit == true)
   {
      this->SetSuffix(this->mc_Unit);
   }
   else
   {
      this->SetSuffix("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates the unit

   \param[in]  oq_ShowUnit    Flag for activating unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::SetShowUnit(const bool oq_ShowUnit)
{
   this->mq_ShowUnit = oq_ShowUnit;

   if (this->mq_ShowUnit == true)
   {
      this->SetSuffix(this->mc_Unit);
   }
   else
   {
      this->SetSuffix("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set design type

   \param[in]  oe_Type  Design type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::SetDesignType(const C_PuiSvDbSpinBox::E_Type oe_Type)
{
   switch (oe_Type)
   {
   case C_PuiSvDbSpinBox::eTYPE1:
      C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Type", 1);
      break;
   case C_PuiSvDbSpinBox::eTYPE2:
      C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Type", 2);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: adjust font to size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::showEvent(QShowEvent * const opc_Event)
{
   C_OgeWiSpinBoxGroup::showEvent(opc_Event);

   AdjustFontToSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Adapt font

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeWiSpinBoxGroup::resizeEvent(opc_Event);

   AdjustFontToSize();
}
