//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate component for message generator table (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.02.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMGENTABLEDELEGATE_H
#define C_CAMGENTABLEDELEGATE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_TblDelegate.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamGenTableDelegate :
   public C_TblDelegate
{
public:
   C_CamGenTableDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   //lint -restore

private:
   static const QColor mhc_DefaultBackgroundColor;
   static const QColor mhc_DefaultBorderColor;
   static const QFont mhc_DefaultFont;
   static const QColor mhc_HighlightBackgroundColor;
   static const QColor mhc_HighlightForegroundColor;
   static const QColor mhc_HighlightBorderColor;
   static const QFont mhc_HighlightFont;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
