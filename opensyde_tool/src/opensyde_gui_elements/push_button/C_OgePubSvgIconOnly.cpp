//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       SVG icon only push button (implementation)

   SVG icon only push button

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEvent>
#include <QPainter>
#include "C_OgePubSvgIconOnly.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     10.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgePubSvgIconOnly::C_OgePubSvgIconOnly(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mq_Hovered(false),
   mpc_SvgRendererEnabled(NULL),
   mpc_SvgRendererDisabeld(NULL),
   mpc_SvgRendererHovered(NULL),
   mpc_SvgRendererCheckedEnabled(NULL),
   mpc_SvgRendererCheckedDisabeld(NULL),
   mpc_SvgRendererCheckedHovered(NULL)
{
   this->setAttribute(Qt::WA_Hover, true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     10.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgePubSvgIconOnly::~C_OgePubSvgIconOnly(void)
{
   delete (mpc_SvgRendererEnabled);
   delete (mpc_SvgRendererDisabeld);
   delete (mpc_SvgRendererHovered);
   delete (mpc_SvgRendererCheckedEnabled);
   delete (mpc_SvgRendererCheckedDisabeld);
   delete (mpc_SvgRendererCheckedHovered);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new SVG path

   \param[in] orc_PathEnabled         New SVG path for enabled state
   \param[in] orc_PathDisabled        New SVG path for disabled state
   \param[in] orc_PathHovered         New SVG path for hovered state
   \param[in] orc_PathToggledEnabled  New SVG path for enabled and checked state
   \param[in] orc_PathToggledDisabled New SVG path for disabled and checked state
   \param[in] orc_PathToggledHovered  New SVG path for hovered and checked stat

   \created     10.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgePubSvgIconOnly::SetSvg(const QString & orc_PathEnabled, const QString & orc_PathDisabled,
                                 const QString & orc_PathHovered, const QString & orc_PathCheckedEnabled,
                                 const QString & orc_PathCheckedDisabled, const QString & orc_PathCheckedHovered)
{
   delete (mpc_SvgRendererEnabled);
   this->mpc_SvgRendererEnabled = new QSvgRenderer(orc_PathEnabled);
   delete (mpc_SvgRendererDisabeld);
   if (orc_PathDisabled.compare("") != 0)
   {
      this->mpc_SvgRendererDisabeld = new QSvgRenderer(orc_PathDisabled);
   }
   else
   {
      this->mpc_SvgRendererDisabeld = NULL;
   }
   delete (mpc_SvgRendererHovered);
   if (orc_PathHovered.compare("") != 0)
   {
      this->mpc_SvgRendererHovered = new QSvgRenderer(orc_PathHovered);
   }
   else
   {
      this->mpc_SvgRendererHovered = NULL;
   }
   delete (mpc_SvgRendererCheckedEnabled);
   if (orc_PathCheckedEnabled.compare("") != 0)
   {
      this->mpc_SvgRendererCheckedEnabled = new QSvgRenderer(orc_PathCheckedEnabled);
   }
   else
   {
      this->mpc_SvgRendererCheckedEnabled = NULL;
   }
   delete (mpc_SvgRendererCheckedDisabeld);
   if (orc_PathCheckedDisabled.compare("") != 0)
   {
      this->mpc_SvgRendererCheckedDisabeld = new QSvgRenderer(orc_PathCheckedDisabled);
   }
   else
   {
      this->mpc_SvgRendererCheckedDisabeld = NULL;
   }
   delete (mpc_SvgRendererCheckedHovered);
   if (orc_PathCheckedHovered.compare("") != 0)
   {
      this->mpc_SvgRendererCheckedHovered = new QSvgRenderer(orc_PathCheckedHovered);
   }
   else
   {
      this->mpc_SvgRendererCheckedHovered = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Override paint event

   Draws the element

   \param[in,out] opc_Event  Pointer to paint event

   \created     10.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgePubSvgIconOnly::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   if (this->mpc_SvgRendererEnabled != NULL)
   {
      QPainter c_Painter(this);
      if (this->isChecked() == true)
      {
         if ((this->mpc_SvgRendererCheckedDisabeld != NULL) && (this->isEnabled() == false))
         {
            this->mpc_SvgRendererCheckedDisabeld->render(&c_Painter, this->rect());
         }
         else if ((this->mpc_SvgRendererCheckedHovered != NULL) && (this->mq_Hovered == true))
         {
            this->mpc_SvgRendererCheckedHovered->render(&c_Painter, this->rect());
         }
         else if (this->mpc_SvgRendererCheckedEnabled != NULL)
         {
            this->mpc_SvgRendererCheckedEnabled->render(&c_Painter, this->rect());
         }
         else
         {
            this->mpc_SvgRendererEnabled->render(&c_Painter, this->rect());
         }
      }
      else
      {
         if ((this->mpc_SvgRendererDisabeld != NULL) && (this->isEnabled() == false))
         {
            this->mpc_SvgRendererDisabeld->render(&c_Painter, this->rect());
         }
         else if ((this->mpc_SvgRendererHovered != NULL) && (this->mq_Hovered == true))
         {
            this->mpc_SvgRendererHovered->render(&c_Painter, this->rect());
         }
         else
         {
            this->mpc_SvgRendererEnabled->render(&c_Painter, this->rect());
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle hover

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     27.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgePubSvgIconOnly::event(QEvent * const opc_Event)
{
   bool q_Retval = C_OgePubToolTipBase::event(opc_Event);

   if (opc_Event->type() == QEvent::HoverLeave)
   {
      this->mq_Hovered = false;
   }
   else if (opc_Event->type() == QEvent::Hide)
   {
      this->mq_Hovered = false;
   }
   else if (opc_Event->type() == QEvent::Show)
   {
      this->mq_Hovered = false;
   }
   else if (opc_Event->type() == QEvent::HoverEnter)
   {
      this->mq_Hovered = true;
   }
   else
   {
      //No handling required
   }
   return q_Retval;
}
