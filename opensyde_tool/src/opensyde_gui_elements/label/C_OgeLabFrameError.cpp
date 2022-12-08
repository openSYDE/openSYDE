//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for error frame label (implementation)

   Label for error frame label

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeLabFrameError.hpp"
#include "C_GtGetText.hpp"

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
C_OgeLabFrameError::C_OgeLabFrameError(QWidget * const opc_Parent) :
   C_OgeLabToolTipBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set foreground color (stylesheet color index)

   \param[in]  os32_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabFrameError::SetForegroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Foreground", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new text content

   \param[in]  orc_Text          New text
   \param[in]  orc_Tooltip       New tooltip
   \param[in]  oe_TooltipType    Tooltip type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabFrameError::SetCompleteText(const QString & orc_Text, const QString & orc_Tooltip,
                                         const stw::opensyde_gui::C_NagToolTip::E_Type oe_TooltipType)
{
   QString c_ToolTipHeading;

   if (oe_TooltipType == stw::opensyde_gui::C_NagToolTip::eERROR)
   {
      c_ToolTipHeading = C_GtGetText::h_GetText("Invalid");
   }
   else
   {
      c_ToolTipHeading = C_GtGetText::h_GetText("Note");
   }

   this->mc_Text = orc_Text;
   this->SetToolTipInformation(c_ToolTipHeading, orc_Tooltip, oe_TooltipType);
   m_OnSizeChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Current size

   \return
   Current size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_OgeLabFrameError::sizeHint(void) const
{
   const QFontMetrics c_Metrics = mh_GetFontMetrics();
   const QSize c_Retval(c_Metrics.horizontalAdvance(this->mc_Text), c_Metrics.height());

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Minimum size

   \return
   Minimum size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_OgeLabFrameError::minimumSizeHint(void) const
{
   const QFontMetrics c_Metrics = mh_GetFontMetrics();
   const QSize c_Retval(c_Metrics.horizontalAdvance("..."), c_Metrics.height());

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Handle label adaptations

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabFrameError::resizeEvent(QResizeEvent * const opc_Event)
{
   m_OnSizeChange(&opc_Event->size());
   QLabel::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Handle label adaptations

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabFrameError::showEvent(QShowEvent * const opc_Event)
{
   QLabel::showEvent(opc_Event);
   m_OnSizeChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle size change

   \param[in]  opc_SizeToUse  Size to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabFrameError::m_OnSizeChange(const QSize * const opc_SizeToUse)
{
   const QFontMetrics c_Metrics = mh_GetFontMetrics();
   QString c_DisplayedString;

   if (opc_SizeToUse != NULL)
   {
      c_DisplayedString =
         C_Uti::h_AdaptStringToSize(this->mc_Text, c_Metrics, static_cast<float64_t>(opc_SizeToUse->width()));
   }
   else
   {
      c_DisplayedString =
         C_Uti::h_AdaptStringToSize(this->mc_Text, c_Metrics, static_cast<float64_t>(this->width()));
   }

   this->setText(c_DisplayedString);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get required font metrics

   \return
   Required font metrics
*/
//----------------------------------------------------------------------------------------------------------------------
QFontMetrics C_OgeLabFrameError::mh_GetFontMetrics(void)
{
   QFont c_Font("Segoe UI");

   c_Font.setPixelSize(12);
   return QFontMetrics(c_Font);
}
