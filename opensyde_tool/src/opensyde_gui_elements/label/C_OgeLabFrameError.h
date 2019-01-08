//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for error frame label (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABFRAMEERROR_H
#define C_OGELABFRAMEERROR_H

/* -- Includes ------------------------------------------------------------- */
#include <QFontMetrics>
#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabFrameError :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabFrameError(QWidget * const opc_Parent = NULL);

   void SetCompleteText(const QString & orc_Text);
   virtual QSize sizeHint(void) const override;
   virtual QSize minimumSizeHint(void) const override;

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   QString mc_Text;

   void m_OnSizeChange(const QSize * const opc_SizeToUse = NULL);
   static QFontMetrics mh_GetFontMetrics(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
