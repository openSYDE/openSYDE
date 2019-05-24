//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for file paths with minimizing file path handling (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEFILEPATH_H
#define C_OGELEFILEPATH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeProperties.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeFilePath :
   public C_OgeLeProperties
{
   Q_OBJECT

public:
   explicit C_OgeLeFilePath(QWidget * const opc_Parent = NULL);

   void SetPath(const QString & orc_New, const QString & orc_RelativeTo = "");
   QString GetPath(void) const;

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   QString mc_Path;
   QString mc_RelativeTo;

   void m_SetToolTip(const QString & orc_AbsoluteFilePath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
