//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget for displaying settings widgets in collapsed state (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMOSSECTIONPOPUP_H
#define C_CAMMOSSECTIONPOPUP_H

/* -- Includes ------------------------------------------------------------- */
#include <QVBoxLayout>
#include "C_OgeWiOnlyBackground.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMosSectionPopup :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   C_CamMosSectionPopup(QWidget * const opc_Parent = NULL);
   ~C_CamMosSectionPopup(void);

   void Show(const QPoint & orc_Pos);
   void SetWidget(QWidget * const opc_Widget);
   void DoMove(const QPoint & orc_Pos);

private:
   QVBoxLayout * mpc_Layout;
   //Avoid call
   C_CamMosSectionPopup(const C_CamMosSectionPopup &);
   C_CamMosSectionPopup & operator =(const C_CamMosSectionPopup &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
