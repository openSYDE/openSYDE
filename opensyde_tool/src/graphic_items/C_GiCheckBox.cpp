//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Graphics item for showing a checkbox with both states (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "constants.h"

#include "C_GiCheckBox.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;

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

   \param[in]     orc_Rect     Initial size of the item
   \param[in,out] opc_Parent   Optional pointer to parent

   \created     23.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiCheckBox::C_GiCheckBox(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   C_GiRectPixmap(orc_Rect, opc_Parent),
   mq_Checked(false)
{
   // create the conflict icon

   this->SetSvg(":/images/CheckBoxActiveWithoutBackground.svg");

   this->SetChecked(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     23.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiCheckBox::~C_GiCheckBox()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the check state of the checkbox

   \param[in]     oq_Checked      Flag for state

   \created     dd.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiCheckBox::SetChecked(const bool oq_Checked)
{
   this->mq_Checked = oq_Checked;

   if (this->mq_Checked == true)
   {
      this->SetBackgroundColor(mc_STYLE_GUIDE_COLOR_22);
   }
   else
   {
      this->SetBackgroundColor(mc_STYLE_GUIDE_COLOR_11);
   }

   this->SetDrawPixmap(this->mq_Checked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the check state

   \return
   true     is checked
   false    is not checked

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_GiCheckBox::IsChecked(void) const
{
   return this->mq_Checked;
}
