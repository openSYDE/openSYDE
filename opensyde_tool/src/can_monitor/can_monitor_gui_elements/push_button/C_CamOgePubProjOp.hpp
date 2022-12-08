//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button styled for can monitor title bar (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEPUBPROJOP_H
#define C_CAMOGEPUBPROJOP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgePubProjOp :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubProjOp(QWidget * const opc_Parent = NULL);

   void SetSvg(const QString & orc_Path);
   void SetIconOnly(const bool oq_IconOnly);

protected:
   virtual void paintEvent(QPaintEvent * const opc_PaintEvent);

private:
   QString mc_SvgIcon;
   bool mq_IconOnly;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
