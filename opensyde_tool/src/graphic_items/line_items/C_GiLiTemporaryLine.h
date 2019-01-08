//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Temporary dashed line for connector creation (header)

   Temporary dashed line for connector creation (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.08.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILITEMPORARYLINE_H
#define C_GILITEMPORARYLINE_H

/* -- Includes ------------------------------------------------------------- */

#include <QObject>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiLiTemporaryLine :
   public QObject,
   public QGraphicsLineItem
{
   Q_OBJECT

public:
   C_GiLiTemporaryLine(QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiTemporaryLine();
   void UpdateP2(const QPointF & orc_Pos);

private:
   //Avoid call
   C_GiLiTemporaryLine(const C_GiLiTemporaryLine &);
   C_GiLiTemporaryLine & operator =(const C_GiLiTemporaryLine &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
