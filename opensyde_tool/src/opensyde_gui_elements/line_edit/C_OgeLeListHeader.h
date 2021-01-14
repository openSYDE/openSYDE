//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELELISTHEADER_H
#define C_OGELELISTHEADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMouseEvent>
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeListHeader :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   C_OgeLeListHeader(QWidget * const opc_Parent = NULL);

   void SetCounter(const stw_types::uint32 ou32_Counter);
   void SetName(const QString & orc_Name);
   QString GetName(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigFocus(void);

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   bool mq_JustGotFocus;
   stw_types::uint32 mu32_Counter;
   QString mc_Name;

   void m_UpdateText(void);
   void m_OnEditingFinished(void);
   void m_OnTextEdited(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
