//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Label with adaptive size (implementation)

   Label with adaptive size

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgeLabAdaptiveSize.h"

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

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabAdaptiveSize::C_OgeLabAdaptiveSize(QWidget * const opc_Parent) :
   QLabel(opc_Parent),
   mq_AllowAutomatedAdaptation(true)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reset font to last known value (necessary after stylesheet polish)

   \created     22.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::ResetFont(void)
{
   this->setFont(this->mc_LastFont);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to current size

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::AdjustFontToSize(void)
{
   C_OgeWiUtil::h_UpdateFontSize(this, this->text());
   this->mc_LastFont = this->font();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to specific size

   \param[in] orc_Size Size to adjust font to

   \created     05.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::AdjustFontToSpecificSize(const QSize & orc_Size)
{
   QFont c_Font = this->font();

   c_Font.setPointSize(C_OgeWiUtil::h_GetNextOptimalPointSize(c_Font, orc_Size, this->text(), 1.0F));
   this->mc_LastFont = c_Font;
   this->setFont(c_Font);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set allow automated font adjust flag

   \param[in] orc_Value New allow automated font adjust flag

   \created     27.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::SetAllowAutomatedAdaptation(const bool oq_Value)
{
   this->mq_AllowAutomatedAdaptation = oq_Value;
}
//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: adjust font to size

   \param[in,out] opc_Event Event identification and information

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::showEvent(QShowEvent * const opc_Event)
{
   QLabel::showEvent(opc_Event);
   if (this->mq_AllowAutomatedAdaptation == true)
   {
      AdjustFontToSize();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: Adapt font

   \param[in,out] opc_Event Event identification and information

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabAdaptiveSize::resizeEvent(QResizeEvent * const opc_Event)
{
   QLabel::resizeEvent(opc_Event);
   if (this->mq_AllowAutomatedAdaptation == true)
   {
      AdjustFontToSize();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current font metrics

   \return
   Current font metrics

   \created     22.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QFontMetrics C_OgeLabAdaptiveSize::m_GetFontMetrics(void) const
{
   return QFontMetrics(mc_LastFont);
}
