//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for main page project operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBPROJACTION_H
#define C_OGEPUBPROJACTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubProjAction :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubProjAction(QWidget * const opc_Parent = NULL);

   void SetCustomIcon(const QString & orc_IconPathEnabled, const QString & orc_IconPathDisabled);
   void SetIconPaddingLeft(const stw_types::sintn osn_Value);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QIcon mc_IconEnabled;
   QIcon mc_IconDisabled;
   stw_types::sintn msn_IconPaddingLeft;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
