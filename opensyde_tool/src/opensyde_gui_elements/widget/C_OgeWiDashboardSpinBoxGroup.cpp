//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for dashboard spin boxes of all available types (implementation)

   Widget for dashboard spin boxes of all available types

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgeWiDashboardSpinBoxGroup.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     21.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeWiDashboardSpinBoxGroup::C_OgeWiDashboardSpinBoxGroup(QWidget * const opc_Parent) :
   C_OgeWiSpinBoxGroup(opc_Parent),
   mc_Unit("")
{
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Type", 2);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to current size

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::AdjustFontToSize(void)
{
   //Consider borders (absolute) + buttons (relative)!
   const sintn sn_ImprovedWidth = (this->size().width() - 2) - GetSpinButtonWidth();
   const QSize c_Size = QSize(std::max(1, sn_ImprovedWidth), this->size().height() - 2);
   const sintn sn_NewPointSize = C_OgeWiUtil::h_UpdateFontSize(this, this->GetText(), 1.0F, false, &c_Size);

   if (sn_NewPointSize > 0)
   {
      this->setStyleSheet(QString("*{font-size:%1pt;}").arg(sn_NewPointSize));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update current unit

   \param[in] orc_Value New unit

   \created     12.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Activates or deactivates the unit

   \param[in]     oq_ShowUnit    Flag for activating unit

   \created     12.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set design type

   \param[in] oe_Type Design type

   \created     13.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key event slot

   Here: handle enter press

   \param[in,out] opc_Event Event identification and information

   \created     26.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::keyPressEvent(QKeyEvent * const opc_Event)
{
   C_OgeWiSpinBoxGroup::keyPressEvent(opc_Event);
   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_Event->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      Q_EMIT SigValueChanged();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: adjust font to size

   \param[in,out] opc_Event Event identification and information

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::showEvent(QShowEvent * const opc_Event)
{
   C_OgeWiSpinBoxGroup::showEvent(opc_Event);

   AdjustFontToSize();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: Adapt font

   \param[in,out] opc_Event Event identification and information

   \created     16.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiDashboardSpinBoxGroup::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeWiSpinBoxGroup::resizeEvent(opc_Event);

   AdjustFontToSize();
}
