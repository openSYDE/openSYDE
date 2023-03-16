//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Information widget for service mode active
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGSERVICEMODEINFO_H
#define C_NAGSERVICEMODEINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiWithToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_NagServiceModeInfo;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagServiceModeInfo :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_NagServiceModeInfo(QWidget * const opc_Parent = NULL);
   ~C_NagServiceModeInfo(void) override;

   void InitStaticNames(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_NagServiceModeInfo * mpc_Ui;

   //Avoid call
   C_NagServiceModeInfo(const C_NagServiceModeInfo &);
   C_NagServiceModeInfo & operator =(const C_NagServiceModeInfo &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
