//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button for options (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBOPTIONS_HPP
#define C_OGEPUBOPTIONS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubOptions :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubOptions(QWidget * const opc_Parent = NULL);
   void SetCustomIcon(const QString & orc_IconPathEnabled, const QString & orc_IconPathDisabled);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

   QIcon mc_IconEnabled;
   QIcon mc_IconDisabled;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
