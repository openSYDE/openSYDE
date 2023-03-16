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

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpiBase :
   public QSplitter
{
public:
   C_OgeSpiBase(QWidget * const opc_Parent = NULL);

   void SetFirstSegment(const int32_t os32_Width);
   void SetSecondSegment(const int32_t os32_Width);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
