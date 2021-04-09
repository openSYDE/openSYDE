//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Frame with generic border and background color
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEFRAGENERIC_H
#define C_OGEFRAGENERIC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFrame>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeFraGeneric :
   public QFrame
{
   Q_OBJECT

public:
   explicit C_OgeFraGeneric(QWidget * const opc_Parent = NULL);

   void SetBorderColor(const stw_types::sintn osn_Value);
   void SetBackgroundColor(const stw_types::sintn osn_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
