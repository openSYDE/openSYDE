//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for spin boxes of all available types (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWISPINBOXGROUP_H
#define C_OGEWISPINBOXGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_NagToolTip.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeWiSpinBoxGroup;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeWiSpinBoxGroup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiSpinBoxGroup(QWidget * const opc_Parent = NULL);
   ~C_OgeWiSpinBoxGroup(void) override;

   QString GetText(void) const;
   void Init(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
             const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,  const float64_t of64_Factor = 1.0,
             const float64_t of64_Offset = 0.0, const uint32_t ou32_Index = 0);
   void SetSuffix(const QString & orc_Value) const;
   void SetValue(const QVariant & orc_Value) const;
   QVariant GetValue(void) const;
   void InterpretText(void) const;
   void SelectAll(void) const;
   void SetToolTipInformation(const QString & orc_Heading, const QString & orc_Content,
                              const stw::opensyde_gui::C_NagToolTip::E_Type oe_Type) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigValueChanged(void);

protected:
   bool event(QEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void keyReleaseEvent(QKeyEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   void focusInEvent(QFocusEvent * const opc_Event) override;
   void focusOutEvent(QFocusEvent * const opc_Event) override;

   void m_Resize(const int32_t os32_Height = 30) const;
   int32_t m_GetSpinButtonWidth(void) const;

private:
   Ui::C_OgeWiSpinBoxGroup * mpc_Ui;
   bool mq_DoubleMode;

   void m_InitDefault(void);
   void m_InitConnections(void) const;
   void m_DeactivateConnections(void) const;

   //Avoid call
   C_OgeWiSpinBoxGroup(const C_OgeWiSpinBoxGroup &);
   C_OgeWiSpinBoxGroup & operator =(const C_OgeWiSpinBoxGroup &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
