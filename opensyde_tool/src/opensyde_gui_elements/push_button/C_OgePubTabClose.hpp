//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab close push button (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTABCLOSE_H
#define C_OGEPUBTABCLOSE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubTabClose :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubTabClose(QWidget * const opc_Parent = NULL);
   void SetDarkMode(const bool oq_Active);

protected:
   bool event(QEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;

private:
   QIcon mc_Default;
   QIcon mc_Hovered;
   QIcon mc_Clicked;
   QIcon mc_DefaultDark;
   QIcon mc_HoveredDark;
   QIcon mc_ClickedDark;
   bool mq_DarkMode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
