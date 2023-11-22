//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Temporary dashed line for connector creation (header)

   Temporary dashed line for connector creation (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILITEMPORARYLINE_HPP
#define C_GILITEMPORARYLINE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiTemporaryLine :
   public QObject,
   public QGraphicsLineItem
{
   Q_OBJECT

public:
   C_GiLiTemporaryLine(QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiTemporaryLine() override;
   void UpdateP2(const QPointF & orc_Pos);

private:
   //Avoid call
   C_GiLiTemporaryLine(const C_GiLiTemporaryLine &);
   C_GiLiTemporaryLine & operator =(const C_GiLiTemporaryLine &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
