//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic line edit for table edit (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLEDITLINEEDITBASE_H
#define C_TBLEDITLINEEDITBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVariant>
#include "C_OgeLeToolTipBase.h"

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblEditLineEditBase :
   public stw_opensyde_gui_elements::C_OgeLeToolTipBase
{
public:
   C_TblEditLineEditBase(QWidget * const opc_Parent = NULL);

   void SetFromVariant(const QVariant & orc_DisplayValue, const QVariant & orc_EditValue);
   void SetMinFromVariant(const QVariant & orc_Value);
   void SetMaxFromVariant(const QVariant & orc_Value);
   stw_types::sint32 GetValueAsVariant(QVariant & orc_Value, QString & orc_ErrorDescription) const;

private:
   QVariant::Type me_Type;
   QString mc_MinValue;
   QString mc_MaxValue;

   void m_UpdateToolTip(void);
   static stw_types::float64 mh_GetStringAsFloat(const QString & orc_Value, bool & orq_Worked);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
