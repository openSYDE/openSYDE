//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with base functionality for table usage
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTABLEBASE_H
#define C_OGECBXTABLEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OgeCbxResizingView.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxTableBase :
   public C_OgeCbxResizingView
{
public:
   C_OgeCbxTableBase(QWidget * const opc_Parent = NULL);

   void InitFromStringList(const QStringList & orc_Strings, const QStringList & orc_Values = QStringList());
   stw_types::sint64 GetValueForCurrentIndex(void) const;
   void SetValue(const stw_types::sint64 os64_Value);

private:
   QStringList mc_Values;
   QStringList mc_Strings;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
