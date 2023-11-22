//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELELISTHEADER_HPP
#define C_OGELELISTHEADER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMouseEvent>
#include "C_OgeLeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeListHeader :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   C_OgeLeListHeader(QWidget * const opc_Parent = NULL);

   void SetCounter(const uint32_t ou32_Counter);
   void SetName(const QString & orc_Name);
   QString GetName(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigFocus(void);

protected:
   void focusInEvent(QFocusEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   bool mq_JustGotFocus;
   uint32_t mu32_Counter;
   QString mc_Name;

   void m_UpdateText(void);
   void m_OnEditingFinished(void);
   void m_OnTextEdited(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
