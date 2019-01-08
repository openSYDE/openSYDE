//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all spin boxes (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXALLBASE_H
#define C_OGESPXALLBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QEvent>
#include "stwtypes.h"
#include "C_OgeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxAllBase :
   public C_OgeToolTipBase
{
public:
   C_OgeSpxAllBase(void);

   void SetToolTipAdditionalInfo(const QString & orc_Text, const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type);
   void ActivateDefaultToolTip(void);

protected:
   virtual bool m_IsEnabled(void) const = 0;
   virtual QString m_GetMinimum(void) const = 0;
   virtual QString m_GetMaximum(void) const = 0;
   virtual void m_SetMouseTracking(const bool oq_Active) = 0;
   virtual bool m_CallBaseEvent(QEvent * const opc_Event) = 0;

   bool m_HandleEvent(QEvent * const opc_Event);

private:
   QString mc_ToolTipAdditionalContent;
   stw_opensyde_gui::C_NagToolTip::E_Type me_ToolTipAdditionalType;

   void m_ApplyToolTipInfo(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
