//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for line edit with placeholder variables support (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEPLACEHOLDERVAR_HPP
#define C_OGELEPLACEHOLDERVAR_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLePlaceholderVar :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeLePlaceholderVar(QWidget * const opc_Parent = NULL);

   void InsertVariable(const QString & orc_Variable);
   void SetDbProjectPath(const QString & orc_DbPath);

protected:
   void focusOutEvent(QFocusEvent * const opc_Event) override;

private:
   int32_t ms32_LastKnownCursorPos;
   QString mc_DbProjectPath;

   void m_UpdateTooltip(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
