//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with auto fix to nearest raw value (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLEAUTOFIX_H
#define C_OGESPXDOUBLEAUTOFIX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxAutoFixBase.h"
#include "C_OgeSpxDoubleToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleAutoFix :
   public C_OgeSpxDoubleToolTipBase,
   public C_OgeSpxAutoFixBase
{
   Q_OBJECT

public:
   C_OgeSpxDoubleAutoFix(QWidget * const opc_Parent = NULL);

   stw_types::sintn GetLineEditWidth(void) const;

   virtual void fixup(QString & orc_String) const;
   virtual QValidator::State validate(QString & orc_Input, stw_types::sintn & orc_Pos) const override;

protected:
   virtual void m_Init(void) override;

private:
   stw_types::float64 mf64_ScaledMin;
   stw_types::float64 mf64_ScaledMax;
   stw_types::float64 mf64_StepWidth;

   QString m_PrepareSpinBoxValue(const QString & orc_Text) const;
   QString m_ExtractSpinBoxValue(const QString & orc_Text) const;
   static stw_types::sint32 mh_GetValue(const QString & orc_Input, stw_types::float64 & orf64_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
