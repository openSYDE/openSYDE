//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Label for error frame label (implementation)

   Label for error frame label

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "C_OgeLabFrameError.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeLabFrameError::C_OgeLabFrameError(QWidget * const opc_Parent) :
   C_OgeLabToolTipBase(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new text content

   \param[in] orc_Text New text

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabFrameError::SetCompleteText(const QString & orc_Text)
{
   this->mc_Text = orc_Text;
   m_OnSizeChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Current size

   \return
   Current size

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_OgeLabFrameError::sizeHint(void) const
{
   const QFontMetrics c_Metrics = mh_GetFontMetrics();
   const QSize c_Retval(c_Metrics.width(this->mc_Text), c_Metrics.height());

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Minimum size

   \return
   Minimum size

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_OgeLabFrameError::minimumSizeHint(void) const
{
   const QFontMetrics c_Metrics = mh_GetFontMetrics();
   const QSize c_Retval(c_Metrics.width("..."), c_Metrics.height());

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: Handle label adaptations

   \param[in,out] opc_Event Event identification and information

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabFrameError::resizeEvent(QResizeEvent * const opc_Event)
{
   m_OnSizeChange(&opc_Event->size());
   QLabel::resizeEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Handle label adaptations

   \param[in,out] opc_Event Event identification and information

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabFrameError::showEvent(QShowEvent * const opc_Event)
{
   QLabel::showEvent(opc_Event);
   m_OnSizeChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle size change

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabFrameError::m_OnSizeChange(const QSize * const opc_SizeToUse)
{
   const QFontMetrics c_Metrics = mh_GetFontMetrics();
   QString c_DisplayedString;

   if (opc_SizeToUse != NULL)
   {
      c_DisplayedString =
         C_Uti::h_AdaptStringToSize(this->mc_Text, c_Metrics, static_cast<stw_types::float64>(opc_SizeToUse->width()));
   }
   else
   {
      c_DisplayedString =
         C_Uti::h_AdaptStringToSize(this->mc_Text, c_Metrics, static_cast<stw_types::float64>(this->width()));
   }

   this->setText(c_DisplayedString);
   if (c_DisplayedString.compare(this->mc_Text) != 0)
   {
      this->SetToolTipInformation("", this->mc_Text, stw_opensyde_gui::C_NagToolTip::eERROR);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get required font metrics

   \return
   Required font metrics

   \created     15.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QFontMetrics C_OgeLabFrameError::mh_GetFontMetrics(void)
{
   QFont c_Font("Segoe UI");

   c_Font.setPixelSize(12);
   return QFontMetrics(c_Font);
}
