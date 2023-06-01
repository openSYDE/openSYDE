//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       This class is used for Creating context menu with custom stylings
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABGENERICWITHCONTEXTMENU_H
#define C_OGELABGENERICWITHCONTEXTMENU_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabGenericWithContextMenu :
   public C_OgeLabContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeLabGenericWithContextMenu(QWidget * const opc_Parent = NULL);
   void SetBackgroundColor(const int32_t os32_Value);
   void SetForegroundColor(const int32_t os32_Value);
   void SetMarginsPadding(const int32_t os32_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
