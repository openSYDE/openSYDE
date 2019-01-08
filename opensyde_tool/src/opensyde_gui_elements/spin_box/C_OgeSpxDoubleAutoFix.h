//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with auto fix to nearest raw value (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLEAUTOFIX_H
#define C_OGESPXDOUBLEAUTOFIX_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeSpxAutoFixBase.h"
#include "C_OgeSpxDoubleToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxDoubleAutoFix :
   public C_OgeSpxDoubleToolTipBase,
   public C_OgeSpxAutoFixBase
{
   Q_OBJECT

public:
   C_OgeSpxDoubleAutoFix(QWidget * const opc_Parent = NULL);

   stw_types::sintn GetLineEditWidth(void) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void fixup(QString & orc_String) const;
   virtual QValidator::State validate(QString & orc_Input, stw_types::sintn & orc_Pos) const override;
   //lint -restore

protected:
   virtual void m_Init(void);

private:
   stw_types::float64 mf64_ScaledMin;
   stw_types::float64 mf64_ScaledMax;
   stw_types::float64 mf64_StepWidth;

   static stw_types::sint32 mh_GetValue(const QString & orc_Input, stw_types::float64 & orf64_Output);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
