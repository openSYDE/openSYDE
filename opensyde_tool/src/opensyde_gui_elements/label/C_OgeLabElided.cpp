//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label that automatically elides text on resize event

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "constants.h"
#include "C_OgeLabElided.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

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
C_OgeLabElided::C_OgeLabElided(QWidget * const opc_Parent) :
   C_OgeLabGenericNoPaddingNoMargins(opc_Parent),
   mc_CompleteText(""),
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   mc_Font(mc_STYLE_GUIDE_FONT_SEMIBOLD_14)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten resize event slot

   Here: elide text

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabElided::resizeEvent(QResizeEvent * const opc_Event)
{
   this->m_UpdateElidedText();
   C_OgeLabGenericNoPaddingNoMargins::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update elided text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabElided::m_UpdateElidedText(void)
{
   const QFontMetrics c_Metric = QFontMetrics(C_Uti::h_GetFontPixel(this->mc_Font));

   this->setText(c_Metric.elidedText(this->mc_CompleteText, Qt::ElideRight, this->width()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set text of label.

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabElided::SetCompleteText(const QString & orc_Text)
{
   this->mc_CompleteText = orc_Text;
   this->m_UpdateElidedText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get complete text of label.

   \return
   Current label text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLabElided::GetCompleteText(void) const
{
   return this->mc_CompleteText;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set font for eliding.

   This is not optimal as font has to be set twice. Feel free to improve if you read this.

   \param[in]  orc_Font    Font
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabElided::SetFontForEliding(const QFont & orc_Font)
{
   this->mc_Font = orc_Font;
}
