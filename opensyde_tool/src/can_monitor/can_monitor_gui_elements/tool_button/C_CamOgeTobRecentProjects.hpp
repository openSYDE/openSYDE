//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tool button for open project or showing recent projects. (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGETOBRECENTPROJECTS_HPP
#define C_CAMOGETOBRECENTPROJECTS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTobToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeTobRecentProjects :
   public C_OgeTobToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeTobRecentProjects(QWidget * const opc_Parent = NULL);

   void SetSvg(const QString & orc_Path);

protected:
   virtual void paintEvent(QPaintEvent * const opc_PaintEvent);

private:
   QString mc_SvgIcon;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
