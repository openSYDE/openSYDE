//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tree view item for C_OgeTreeViewNaviBar (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeTreeViewItemNaviBar.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_OgeTreeViewItemNaviBar::mhc_ChangeMark = "*";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     09.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTreeViewItemNaviBar::C_OgeTreeViewItemNaviBar(QTreeWidgetItem * const opc_Parent) :
   QTreeWidgetItem(opc_Parent),
   mq_ChangeMarkActive(false),
   mc_OriginalText("")
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     09.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTreeViewItemNaviBar::~C_OgeTreeViewItemNaviBar()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the main text for showing independent of the change mark

   \param[in]     orc_Text         Main text which will be showed always

   \created     09.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeTreeViewItemNaviBar::SetMainText(const QString & orc_Text)
{
   this->mc_OriginalText = orc_Text;

   this->m_UpdateShownText();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the main text for showing independent of the change mark

   \return
   Main text which will be showed always

   \created     09.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_OgeTreeViewItemNaviBar::GetMainText() const
{
   return this->mc_OriginalText;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates or deactivates the change mark in the text

   \param[in]     oq_Active   Flag if the change mark should be shown or not

   \created     09.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeTreeViewItemNaviBar::SetChangeMark(const bool oq_Active)
{
   this->mq_ChangeMarkActive = oq_Active;

   this->m_UpdateShownText();
}

//-----------------------------------------------------------------------------
void C_OgeTreeViewItemNaviBar::m_UpdateShownText()
{
   if (this->mq_ChangeMarkActive == true)
   {
      this->setText(0, mhc_ChangeMark + this->mc_OriginalText);
   }
   else
   {
      this->setText(0, this->mc_OriginalText);
   }
}
