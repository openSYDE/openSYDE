//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit with right border styled for seamless transition to browse-button. (implementation)

   Line edit with right border styled for seamless transition to browse-button.
   Mostly this class exist to apply a stylesheet, but it also handles path minimizing.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDir>

#include "C_CamOgeLeDarkBrowse.h"
#include "C_Uti.h"
#include "constants.h"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeLeDarkBrowse::C_CamOgeLeDarkBrowse(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent),
   mc_CompletePath("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get complete path (pendant to text() of usual line edit)

   \return
   complete path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamOgeLeDarkBrowse::GetPath() const
{
   return this->mc_CompletePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new path, show minimized text and update tooltip.

   \param[in]     orc_NewPath    new path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeLeDarkBrowse::SetPath(const QString & orc_NewPath)
{
   this->mc_CompletePath = QDir::cleanPath(orc_NewPath);

   this->m_ShowMinimizedPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus out event slot to minimize path.

   \param[in,out]    opc_Event    Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeLeDarkBrowse::focusInEvent(QFocusEvent * const opc_Event)
{
   C_OgeLeToolTipBase::focusInEvent(opc_Event);
   this->setText(this->mc_CompletePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus out event slot to minimize path.

   \param[in,out]    opc_Event    Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeLeDarkBrowse::focusOutEvent(QFocusEvent * const opc_Event)
{
   this->mc_CompletePath = QDir::cleanPath(this->text());
   C_OgeLeToolTipBase::focusOutEvent(opc_Event);
   this->m_ShowMinimizedPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event.

   Here: Recalculate minimized path on resize.

   \param[in]     opc_Event      event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeLeDarkBrowse::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeLeToolTipBase::resizeEvent(opc_Event);
   this->m_ShowMinimizedPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Set new path on enter or return key press.
   This is analogue to normal line edit but often not needed because of following focus loose.

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeLeDarkBrowse::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if ((opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Return)))
   {
      this->mc_CompletePath = QDir::cleanPath(this->text());
   }

   // after update of complete path
   C_OgeLeToolTipBase::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate minimized path and set text to this.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeLeDarkBrowse::m_ShowMinimizedPath()
{
   const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
   const QString c_MinimizedPath = C_Uti::h_MinimizePath(this->mc_CompletePath, c_Font, this->width(), 16);

   this->setText(c_MinimizedPath);
}
