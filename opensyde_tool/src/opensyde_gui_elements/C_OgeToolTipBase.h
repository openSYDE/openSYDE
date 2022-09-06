//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all gui element classes with a tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETOOLTIPBASE_H
#define C_OGETOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "C_NagToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeToolTipBase
{
public:
   C_OgeToolTipBase();
   virtual ~C_OgeToolTipBase();

   void SetToolTipInformation(const QString & orc_Heading, const QString & orc_Content, const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type =
                                 stw_opensyde_gui::C_NagToolTip::eDEFAULT);
   void ShowToolTipWhenDisabled(const bool oq_ShowToolTip);

protected:
   virtual void m_HideToolTip(void);
   stw_opensyde_gui::C_NagToolTip * m_GetToolTip(void);

   QString mc_ToolTipHeading;
   QString mc_ToolTipContent;
   stw_opensyde_gui::C_NagToolTip::E_Type me_ToolTipType;
   bool mq_ToolTipActive;
   bool mq_ShowToolTipWhenDisabled;

private:
   stw_opensyde_gui::C_NagToolTip * mpc_ToolTip;

   void m_CreateToolTip(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
