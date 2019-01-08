//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Progress bar part of dashboard progress bar item (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEDASHBOARDPROGRESSBAR_H
#define C_OGEDASHBOARDPROGRESSBAR_H

/* -- Includes ------------------------------------------------------------- */
#include <QProgressBar>
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeDashboardProgressBar :
   public QProgressBar
{
   Q_OBJECT

public:
   C_OgeDashboardProgressBar(QWidget * const opc_Parent = NULL);

   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event);
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style me_Style;
   QColor mc_BackgroundColor;
   QColor mc_ProgressColor;
   stw_types::sintn msn_Offset;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
