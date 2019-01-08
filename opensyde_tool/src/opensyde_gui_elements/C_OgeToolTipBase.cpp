//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for all gui element classes with a tool tip (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeToolTipBase.h"

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

   \created     22.05.2017 STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeToolTipBase::C_OgeToolTipBase() :
   mpc_ToolTip(NULL),
   mc_ToolTipHeading(""),
   mc_ToolTipContent(""),
   me_ToolTipType(stw_opensyde_gui::C_NagToolTip::eDEFAULT),
   mq_ToolTipActive(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     22.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeToolTipBase::~C_OgeToolTipBase()
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the strings for the tooltip

   \param[in]     orc_Heading     String with heading
   \param[in]     orc_Content     String with content
   \param[in]     oe_Type         Optional tooltip type

   \created     18.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeToolTipBase::SetToolTipInformation(const QString & orc_Heading, const QString & orc_Content,
                                             const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type)
{
   this->mc_ToolTipHeading = orc_Heading;
   this->mc_ToolTipContent = orc_Content;
   this->me_ToolTipType = oe_Type;

   if ((this->mc_ToolTipContent != "") || (this->mc_ToolTipHeading != ""))
   {
      this->mq_ToolTipActive = true;

      this->m_CreateToolTip();

      if (this->mpc_ToolTip->isVisible() == true)
      {
         this->mpc_ToolTip->SetHeading(orc_Heading);
         this->mpc_ToolTip->SetContent(orc_Content);
         this->mpc_ToolTip->SetType(oe_Type);
      }
   }
   else
   {
      this->mq_ToolTipActive = false;
      if ((this->mpc_ToolTip != NULL) &&
          (this->mpc_ToolTip->isVisible() == true))
      {
         this->mpc_ToolTip->hide();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Creates an instance for mpc_ToolTip if not already exist

   \created     17.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeToolTipBase::m_CreateToolTip(void)
{
   if (mpc_ToolTip == NULL)
   {
      mpc_ToolTip = new stw_opensyde_gui::C_NagToolTip();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Hides the tool tip

   \created     17.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeToolTipBase::m_HideToolTip(void)
{
   if (this->mpc_ToolTip != NULL)
   {
      this->mpc_ToolTip->hide();
   }
}
