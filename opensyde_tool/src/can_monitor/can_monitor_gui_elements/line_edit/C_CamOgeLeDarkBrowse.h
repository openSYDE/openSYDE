//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit with right border styled for seamless transition to browse-button. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGELEDARKBROWSE_H
#define C_CAMOGELEDARKBROWSE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeLeDarkBrowse :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeLeDarkBrowse(QWidget * const opc_Parent = NULL);

   QString GetPath(void) const;
   void SetPath(const QString & orc_NewPath);

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   QString mc_CompletePath;

   void m_ShowMinimizedPath(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
