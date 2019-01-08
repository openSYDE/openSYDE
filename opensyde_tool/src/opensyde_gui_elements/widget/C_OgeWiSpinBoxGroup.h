//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for spin boxes of all available types (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWISPINBOXGROUP_H
#define C_OGEWISPINBOXGROUP_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "C_NagToolTip.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_OgeWiSpinBoxGroup;
}

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_OgeWiSpinBoxGroup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiSpinBoxGroup(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeWiSpinBoxGroup(void);

   QString GetText(void) const;
   void Init(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
             const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,  const stw_types::float64 of64_Factor = 1.0,
             const stw_types::float64 of64_Offset = 0.0, const stw_types::uint32 ou32_Index = 0);
   void SetSuffix(const QString & orc_Value) const;
   void SetValue(const QVariant & orc_Value) const;
   QVariant GetValue(void) const;
   void InterpretText(void) const;
   void SelectAll(void) const;
   void SetToolTipInformation(const QString & orc_Heading, const QString & orc_Content,
                              const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigValueChanged(void);

protected:
   virtual bool event(QEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;

   void m_ResizeToDefault(void) const;
   stw_types::sintn GetSpinButtonWidth(void) const;

private:
   Ui::C_OgeWiSpinBoxGroup * mpc_Ui;
   bool mq_DoubleMode;

   void m_InitDefault(void);
   void m_InitConnections(void);
   void m_DeactivateConnections(void);

   //Avoid call
   C_OgeWiSpinBoxGroup(const C_OgeWiSpinBoxGroup &);
   C_OgeWiSpinBoxGroup & operator =(const C_OgeWiSpinBoxGroup &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
