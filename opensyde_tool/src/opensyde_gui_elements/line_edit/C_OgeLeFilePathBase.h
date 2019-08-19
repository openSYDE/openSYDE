//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for line edit field with file paths (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEFILEPATHBASE_H
#define C_OGELEFILEPATHBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeFilePathBase :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeLeFilePathBase(QWidget * const opc_Parent = NULL);

   void SetPath(const QString & orc_New, const QString & orc_RelativeTo = "");
   QString GetPath(void) const;
   void InsertVariable(const QString & orc_Variable);

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual QString m_ResolveVariables(const QString & orc_Path);

private:
   QString mc_Path;
   QString mc_RelativeTo;
   stw_types::sint32 ms32_LastKnownCursorPos;

   void m_UpdateMinimizing(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
