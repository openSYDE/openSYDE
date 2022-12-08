//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label with adaptive size (implementation)

   Label with adaptive size

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeLabAdaptiveSize.hpp"

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
C_OgeLabAdaptiveSize::C_OgeLabAdaptiveSize(QWidget * const opc_Parent) :
   QLabel(opc_Parent),
   mq_AllowAutomatedAdaptation(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset font to last known value (necessary after stylesheet polish)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::ResetFont(void)
{
   this->setFont(this->mc_LastFont);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::AdjustFontToSize(void)
{
   C_OgeWiUtil::h_UpdateFontSize(this, this->text());
   this->mc_LastFont = this->font();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to specific size

   \param[in]  orc_Size    Size to adjust font to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::AdjustFontToSpecificSize(const QSize & orc_Size)
{
   QFont c_Font = this->font();

   c_Font.setPointSize(C_OgeWiUtil::h_GetNextOptimalPointSize(c_Font, orc_Size, this->text(), 1.0F));
   this->mc_LastFont = c_Font;
   this->setFont(c_Font);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set allow automated font adjust flag

   \param[in]  oq_Value    New allow automated font adjust flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::SetAllowAutomatedAdaptation(const bool oq_Value)
{
   this->mq_AllowAutomatedAdaptation = oq_Value;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: adjust font to size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::showEvent(QShowEvent * const opc_Event)
{
   QLabel::showEvent(opc_Event);
   if (this->mq_AllowAutomatedAdaptation == true)
   {
      AdjustFontToSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Adapt font

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::resizeEvent(QResizeEvent * const opc_Event)
{
   QLabel::resizeEvent(opc_Event);
   if (this->mq_AllowAutomatedAdaptation == true)
   {
      AdjustFontToSize();
   }
}
