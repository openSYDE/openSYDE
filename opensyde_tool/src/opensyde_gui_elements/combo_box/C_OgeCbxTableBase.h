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
#include "C_OSCNodeDataPoolContent.h"

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
   void InitMinMaxAndScaling(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                             const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                             const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset);
   stw_types::sint32 GetValue(stw_types::sint64 & ors64_Value, QString & orc_ErrorDescription) const;
   void SetValue(const stw_types::sint64 os64_Value);

private:
   QStringList mc_Values;
   QStringList mc_Strings;
   stw_opensyde_core::C_OSCNodeDataPoolContent mc_UnscaledMin;
   stw_opensyde_core::C_OSCNodeDataPoolContent mc_UnscaledMax;
   stw_types::float64 mf64_Factor;
   stw_types::float64 mf64_Offset;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
