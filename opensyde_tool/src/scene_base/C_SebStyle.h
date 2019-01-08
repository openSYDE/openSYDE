//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Style for scene rubber band (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBSTYLE_H
#define C_SEBSTYLE_H

/* -- Includes ------------------------------------------------------------- */
#include <QProxyStyle>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebStyle :
   public QProxyStyle
{
public:
   C_SebStyle(QStyle * const opc_Style = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void drawControl(const ControlElement oe_Element, const QStyleOption * const opc_Option,
                            QPainter * const opc_Painter, const QWidget * const opc_Widget = NULL) const override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
