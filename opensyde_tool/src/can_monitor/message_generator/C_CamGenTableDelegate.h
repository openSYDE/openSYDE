//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate component for message generator table (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENTABLEDELEGATE_H
#define C_CAMGENTABLEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_CamTblDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenTableDelegate :
   public C_CamTblDelegate
{
public:
   C_CamGenTableDelegate(QObject * const opc_Parent = NULL);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;

private:
   static const QColor mhc_DEFAULT_BACKGROUND_COLOR;
   static const QColor mhc_DEFAULT_BORDER_COLOR;
   static const QFont mhc_DEFAULT_FONT;
   static const QColor mhc_HIGHLIGHT_BACKGROUND_COLOR;
   static const QColor mhc_HIGHLIGHT_FOREGROUND_COLOR;
   static const QColor mhc_HIGHLIGHT_BORDER_COLOR;
   static const QFont mhc_HIGHLIGHT_FONT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
