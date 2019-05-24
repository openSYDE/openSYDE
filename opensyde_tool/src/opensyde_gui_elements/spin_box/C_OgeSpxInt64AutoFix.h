//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box with auto fix to nearest raw value (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64AUTOFIX_H
#define C_OGESPXINT64AUTOFIX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxInt64ToolTipBase.h"
#include "C_OgeSpxAutoFixBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64AutoFix :
   public C_OgeSpxInt64ToolTipBase,
   public C_OgeSpxAutoFixBase
{
public:
   C_OgeSpxInt64AutoFix(QWidget * const opc_Parent = NULL);

   stw_types::sintn GetLineEditWidth(void) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void fixup(QString & orc_Input) const override;
   virtual QValidator::State validate(QString & orc_Input, stw_types::sintn & orc_Pos) const override;
   //lint -restore

protected:
   virtual void m_Init(void);

private:
   static stw_types::sint32 mh_GetUValue(const QString & orc_Input, stw_types::uint64 & oru64_Output);
   static stw_types::sint32 mh_GetSValue(const QString & orc_Input, stw_types::sint64 & ors64_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
