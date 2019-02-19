//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tool button for open project or showing recent projects. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGETOBRECENTPROJECTS_H
#define C_CAMOGETOBRECENTPROJECTS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeTobToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeTobRecentProjects :
   public C_OgeTobToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeTobRecentProjects(QWidget * const opc_Parent = NULL);

   void SetSvg(const QString & orc_Path);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_PaintEvent);
   //lint -restore

private:
   QString mc_SvgIcon;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
