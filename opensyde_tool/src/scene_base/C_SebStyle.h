//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Style for scene rubber band (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBSTYLE_H
#define C_SEBSTYLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QProxyStyle>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebStyle :
   public QProxyStyle
{
public:
   C_SebStyle(QStyle * const opc_Style = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual void drawControl(const ControlElement oe_Element, const QStyleOption * const opc_Option,
                            QPainter * const opc_Painter, const QWidget * const opc_Widget = NULL) const override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
