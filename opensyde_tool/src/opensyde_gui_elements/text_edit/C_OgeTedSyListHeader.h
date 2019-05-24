//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for system view list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDSYLISTHEADER_H
#define C_OGETEDSYLISTHEADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTedListHeaderBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedSyListHeader :
   public C_OgeTedListHeaderBase
{
   Q_OBJECT

public:
   C_OgeTedSyListHeader(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
