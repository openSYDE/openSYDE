//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common splitter functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPIBASE_H
#define C_OGESPIBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSplitter>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpiBase :
   public QSplitter
{
public:
   C_OgeSpiBase(QWidget * const opc_Parent = NULL);

   void SetFirstSegment(const stw_types::sint32 os32_Width);
   void SetSecondSegment(const stw_types::sint32 os32_Width);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
