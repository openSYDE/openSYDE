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
#include "C_OgeSpxAutoFixBase.hpp"
#include "C_OgeSpxDoubleToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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

   int32_t GetLineEditWidth(void) const;

   void fixup(QString & orc_String) const override;
   QValidator::State validate(QString & orc_Input, int32_t & ors32_Pos) const override;

protected:
   void m_Init(void) override;

private:
   float64_t mf64_ScaledMin;
   float64_t mf64_ScaledMax;
   float64_t mf64_StepWidth;

   QString m_PrepareSpinBoxValue(const QString & orc_Text) const;
   QString m_ExtractSpinBoxValue(const QString & orc_Text) const;
   static int32_t mh_GetValue(const QString & orc_Input, float64_t & orf64_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
