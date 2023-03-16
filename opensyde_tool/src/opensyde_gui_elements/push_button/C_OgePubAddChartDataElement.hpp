//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button with specific style for use on dashboard chart signal selector widget
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBADDCHARTDATAELEMENT_H
#define C_OGEPUBADDCHARTDATAELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubAddChartDataElement :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubAddChartDataElement(QWidget * const opc_Parent = NULL);
   void SetDarkMode(const bool oq_DarkMode);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   bool mq_DarkMode;
   QIcon mc_IconEnabledBright;
   QIcon mc_IconDisabledBright;
   QIcon mc_IconEnabledDark;
   QIcon mc_IconDisabledDark;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
