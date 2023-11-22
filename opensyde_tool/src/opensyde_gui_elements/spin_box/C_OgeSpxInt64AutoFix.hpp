//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box with auto fix to nearest raw value (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64AUTOFIX_HPP
#define C_OGESPXINT64AUTOFIX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxInt64ToolTipBase.hpp"
#include "C_OgeSpxAutoFixBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64AutoFix :
   public C_OgeSpxInt64ToolTipBase,
   public C_OgeSpxAutoFixBase
{
public:
   C_OgeSpxInt64AutoFix(QWidget * const opc_Parent = NULL);

   int32_t GetLineEditWidth(void) const;

   void fixup(QString & orc_Input) const override;
   QValidator::State validate(QString & orc_Input, int32_t & ors32_Pos) const override;

protected:
   void m_Init(void) override;

private:
   static int32_t mh_GetUnsignedValue(const QString & orc_Input, uint64_t & oru64_Output);
   static int32_t mh_GetSignedValue(const QString & orc_Input, int64_t & ors64_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
