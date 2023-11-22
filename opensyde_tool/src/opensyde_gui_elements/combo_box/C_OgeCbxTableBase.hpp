//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with base functionality for table usage
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTABLEBASE_HPP
#define C_OGECBXTABLEBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OgeCbxResizingView.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxTableBase :
   public C_OgeCbxResizingView
{
public:
   C_OgeCbxTableBase(QWidget * const opc_Parent = NULL);

   void InitFromStringList(const QStringList & orc_Strings, const QStringList & orc_Values = QStringList());
   void InitMinMaxAndScaling(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                             const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max, const float64_t of64_Factor,
                             const float64_t of64_Offset);
   int32_t GetValue(int64_t & ors64_Value, QString & orc_ErrorDescription) const;
   void SetValue(const int64_t os64_Value);

private:
   QStringList mc_Values;
   QStringList mc_Strings;
   stw::opensyde_core::C_OscNodeDataPoolContent mc_UnscaledMin;
   stw::opensyde_core::C_OscNodeDataPoolContent mc_UnscaledMax;
   float64_t mf64_Factor;
   float64_t mf64_Offset;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
