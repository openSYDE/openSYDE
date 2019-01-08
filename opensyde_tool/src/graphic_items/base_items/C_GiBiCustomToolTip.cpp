//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for changeable tool tip on hover over (implementation)

   Base class for changeable tool tip on hover over

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_GiBiCustomToolTip.h"

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

   \created     22.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiBiCustomToolTip::C_GiBiCustomToolTip() :
   me_DefaultToolTipType(C_NagToolTip::eDEFAULT),
   me_CurrentToolTipType(C_NagToolTip::eDEFAULT)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiBiCustomToolTip::~C_GiBiCustomToolTip()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Restore default tool tip

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::RestoreDefaultToolTip()
{
   this->mc_CurrentToolTipHeading = this->mc_DefaultToolTipHeading;
   this->mc_CurrentToolTipContent = this->mc_DefaultToolTipContent;
   this->me_CurrentToolTipType = this->me_DefaultToolTipType;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set default tool tip heading

   \param[in] orc_Text New text

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::SetDefaultToolTipHeading(const QString & orc_Text)
{
   this->mc_DefaultToolTipHeading = orc_Text;
   this->mc_CurrentToolTipHeading = orc_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set default tool tip content

   \param[in] orc_Text New text

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::SetDefaultToolTipContent(const QString & orc_Text)
{
   this->mc_DefaultToolTipContent = orc_Text;
   this->mc_CurrentToolTipContent = orc_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set default tool tip type

   \param[in] oe_Type Tool tip type

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::SetDefaultToolTipType(const C_NagToolTip::E_Type oe_Type)
{
   this->me_CurrentToolTipType = oe_Type;
   this->me_DefaultToolTipType = oe_Type;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set temporary tool tip heading

   \param[in] orc_TemporaryToolTip New text

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::SetTemporaryToolTipHeading(const QString & orc_TemporaryToolTip)
{
   this->mc_CurrentToolTipHeading = orc_TemporaryToolTip;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set temporary tool tip content

   \param[in] orc_TemporaryToolTip New text

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::SetTemporaryToolTipContent(const QString & orc_TemporaryToolTip)
{
   this->mc_CurrentToolTipContent = orc_TemporaryToolTip;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current tool tip heading

   \return
   Current heading

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_GiBiCustomToolTip::GetCurrentToolTipHeading(void) const
{
   return this->mc_CurrentToolTipHeading;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current tool tip content

   \return
   Current content

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_GiBiCustomToolTip::GetCurrentToolTipContent(void) const
{
   return this->mc_CurrentToolTipContent;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current tool tip type

   \return
   Current tool tip type

   \created     16.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagToolTip::E_Type C_GiBiCustomToolTip::GetCurrentToolTipType(void) const
{
   return this->me_CurrentToolTipType;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if tool tip not empty

   \return
   true:  Tool tip not empty
   false: Tool tip empty

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiBiCustomToolTip::NotEmpty() const
{
   return (this->mc_CurrentToolTipHeading.compare("") != 0) || (this->mc_CurrentToolTipContent.compare("") != 0);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Deactivate tool tip temporarily

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiCustomToolTip::DeactivateToolTipTemporarily(void)
{
   this->mc_CurrentToolTipContent = "";
   this->mc_CurrentToolTipHeading = "";
}
