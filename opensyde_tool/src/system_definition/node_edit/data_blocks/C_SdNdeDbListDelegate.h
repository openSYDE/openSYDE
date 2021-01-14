//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Application list display delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBLISTDELEGATE_H
#define C_SDNDEDBLISTDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "stwtypes.h"
#include "C_SdNdeDbWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbListDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeDbListDelegate(QObject * const opc_Parent = NULL);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;

   void StartPaint(const stw_types::sint32 s32_Index, stw_opensyde_gui::C_SdNdeDbWidget * const opc_MovingWidget);
   void StopPaint(void);

private:
   //Avoid call
   C_SdNdeDbListDelegate(const C_SdNdeDbListDelegate &);
   C_SdNdeDbListDelegate & operator =(const C_SdNdeDbListDelegate &);

   stw_opensyde_gui::C_SdNdeDbWidget * mpc_ApplicationWidget;
   stw_types::sint32 ms32_IndexPaint;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
