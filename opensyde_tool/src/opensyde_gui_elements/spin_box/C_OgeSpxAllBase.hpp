//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all spin boxes (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXALLBASE_H
#define C_OGESPXALLBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QEvent>
#include "stwtypes.hpp"
#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxAllBase :
   public C_OgeToolTipBase
{
public:
   C_OgeSpxAllBase(void);

   void SetToolTipAdditionalInfo(const QString & orc_Text, const stw::opensyde_gui::C_NagToolTip::E_Type oe_Type);
   void ActivateDefaultToolTip(void);

protected:
   virtual bool m_IsEnabled(void) const = 0;
   virtual QString m_GetMinimumString(void) const;
   virtual QString m_GetMaximumString(void) const;
   virtual QString m_GetMinimumRawString(void) const = 0;
   virtual QString m_GetMaximumRawString(void) const = 0;
   virtual void m_SetMouseTracking(const bool oq_Active) = 0;
   virtual bool m_CallBaseEvent(QEvent * const opc_Event) = 0;

   bool m_HandleEvent(QEvent * const opc_Event);

   QString mc_UserMinValString;
   QString mc_UserMaxValString;

private:
   QString mc_ToolTipAdditionalContent;
   stw::opensyde_gui::C_NagToolTip::E_Type me_ToolTipAdditionalType;

   void m_ApplyToolTipInfo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
