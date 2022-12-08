//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab close push button (implementation)

   Tab close push button.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QEvent>
#include "C_OgePubTabClose.hpp"

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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubTabClose::C_OgePubTabClose(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mc_Default("://images/system_views/IconTabClose.svg"),
   mc_Hovered("://images/system_views/IconTabCloseHover.svg"),
   mc_Clicked("://images/system_views/IconTabCloseClicked.svg"),
   mc_DefaultDark("://images/system_views/IconTabCloseDark.svg"),
   mc_HoveredDark("://images/system_views/IconTabCloseHoverDark.svg"),
   mc_ClickedDark("://images/system_views/IconTabCloseClickedDark.svg"),
   mq_DarkMode(false)
{
   this->setIcon(this->mc_Default);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark mode active

   \param[in] oq_Active Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubTabClose::SetDarkMode(const bool oq_Active)
{
   this->mq_DarkMode = oq_Active;
   if (oq_Active == true)
   {
      this->setIcon(this->mc_DefaultDark);
   }
   else
   {
      this->setIcon(this->mc_Default);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten event slot

   Here: Set icon depending on state

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgePubTabClose::event(QEvent * const opc_Event)
{
   //Hovered
   if (opc_Event->type() == QEvent::Enter)
   {
      if (this->mq_DarkMode == true)
      {
         this->setIcon(this->mc_HoveredDark);
      }
      else
      {
         this->setIcon(this->mc_Hovered);
      }
   }

   //Default
   if (opc_Event->type() == QEvent::Leave)
   {
      if (this->mq_DarkMode == true)
      {
         this->setIcon(this->mc_DefaultDark);
      }
      else
      {
         this->setIcon(this->mc_Default);
      }
   }
   return C_OgePubToolTipBase::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Manual icon handling

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubTabClose::mousePressEvent(QMouseEvent * const opc_Event)
{
   C_OgePubToolTipBase::mousePressEvent(opc_Event);
   if (this->mq_DarkMode == true)
   {
      this->setIcon(this->mc_ClickedDark);
   }
   else
   {
      this->setIcon(this->mc_Clicked);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: Manual icon handling

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubTabClose::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   if (this->mq_DarkMode == true)
   {
      this->setIcon(this->mc_DefaultDark);
   }
   else
   {
      this->setIcon(this->mc_Default);
   }
   C_OgePubToolTipBase::mouseReleaseEvent(opc_Event);
}
