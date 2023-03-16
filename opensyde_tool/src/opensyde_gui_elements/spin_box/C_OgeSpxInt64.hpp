//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box for int64 values (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64_H
#define C_OGESPXINT64_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QVariant>
#include <QAbstractSpinBox>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64 :
   public QAbstractSpinBox
{
   Q_OBJECT

public:
   C_OgeSpxInt64(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);
   bool GetIsUnsigned(void) const;
   void SetValue(const QVariant & orc_Value, const bool oq_AllowValueUpdate);
   void SetStepWidth(const uint64_t ou64_Step);
   void SetMinimum(const QVariant & orc_Minimum);
   void SetMaximum(const QVariant & orc_Maximum);
   void SetSuffix(const QString & orc_Value);
   QVariant GetValue(void) const;
   QVariant GetMinimum(void) const;
   QVariant GetMaximum(void) const;
   uint64_t GetStepWidth(void) const;

   void stepBy(const int32_t os32_Steps) override;
   QValidator::State validate(QString & orc_Input, int32_t & ors32_Pos) const override;

   void SetIsUnsigned(const bool & orq_Value);

   static const QChar hc_UNIT_SEPERATION_CHARACTER;
   static QString h_ExtractSpinBoxValue(const QString & orc_Text, const QString & orc_Suffix);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigValueChanged(void);
   void SigMinMaxChanged(void);

protected:
   StepEnabled stepEnabled(void) const override;

   QString m_PrepareSpinBoxValue(const QString & orc_Text) const;
   QString m_ExtractSpinBoxValue(const QString & orc_Text) const;

private:
   bool mq_IsUnsigned;
   QVariant mc_Minimum;
   QVariant mc_Maximum;
   QString mc_Suffix;
   uint64_t mu64_StepWidth;

   void m_SetSpinBoxValue(const QString & orc_Text) const;
   void m_ApplyMinMax(QVariant & orc_Value) const;
   void m_CheckMinMax(const QVariant & orc_Value, bool & orq_IsUnderMinimum, bool & orq_IsOverMaximum) const;
   void m_ResetMinMax(void);
   QVariant m_PrepareValue(const QVariant & orc_Value) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
