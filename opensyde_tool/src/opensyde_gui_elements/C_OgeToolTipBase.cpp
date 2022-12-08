//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all gui element classes with a tool tip (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeToolTipBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeToolTipBase::C_OgeToolTipBase() :
   mc_ToolTipHeading(""),
   mc_ToolTipContent(""),
   me_ToolTipType(stw::opensyde_gui::C_NagToolTip::eDEFAULT),
   mq_ToolTipActive(false),
   mq_ShowToolTipWhenDisabled(false),
   mpc_ToolTip(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeToolTipBase::~C_OgeToolTipBase()
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the strings for the tooltip

   \param[in]     orc_Heading     String with heading
   \param[in]     orc_Content     String with content
   \param[in]     oe_Type         Optional tooltip type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeToolTipBase::SetToolTipInformation(const QString & orc_Heading, const QString & orc_Content,
                                             const stw::opensyde_gui::C_NagToolTip::E_Type oe_Type)
{
   this->mc_ToolTipHeading = orc_Heading;
   this->mc_ToolTipContent = orc_Content;
   this->me_ToolTipType = oe_Type;

   if ((this->mc_ToolTipContent != "") || (this->mc_ToolTipHeading != ""))
   {
      this->mq_ToolTipActive = true;
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for showing the tooltip even when the element is disabled

   \param[in]       oq_ShowToolTip     Flag for showing tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeToolTipBase::ShowToolTipWhenDisabled(const bool oq_ShowToolTip)
{
   this->mq_ShowToolTipWhenDisabled = oq_ShowToolTip;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hides the tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeToolTipBase::m_HideToolTip(void)
{
   if (this->mpc_ToolTip != NULL)
   {
      this->mpc_ToolTip->hide();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the current tool tip

   \return
   Current tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui::C_NagToolTip * C_OgeToolTipBase::m_GetToolTip(void)
{
   m_CreateToolTip();
   return this->mpc_ToolTip;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates an instance for mpc_ToolTip if not already exist
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeToolTipBase::m_CreateToolTip(void)
{
   if (mpc_ToolTip == NULL)
   {
      mpc_ToolTip = new stw::opensyde_gui::C_NagToolTip();
   }
}
