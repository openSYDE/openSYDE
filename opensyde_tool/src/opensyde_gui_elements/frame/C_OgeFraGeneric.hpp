//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Frame with generic border and background color
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEFRAGENERIC_HPP
#define C_OGEFRAGENERIC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFrame>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeFraGeneric :
   public QFrame
{
   Q_OBJECT

public:
   explicit C_OgeFraGeneric(QWidget * const opc_Parent = NULL);

   void SetBorderColor(const int32_t os32_Value);
   void SetBackgroundColor(const int32_t os32_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
