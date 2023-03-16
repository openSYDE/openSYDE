//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple line with style utility functions (header)

   Simple line with style utility functions (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILISIMPLELINE_H
#define C_GILISIMPLELINE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsLineItem>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiSimpleLine :
   public QGraphicsLineItem
{
public:
   C_GiLiSimpleLine(QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiSimpleLine() override;

   int32_t type() const override;

   virtual void SetWidth(const int32_t & ors32_Width);
   virtual void SetColor(const QColor & orc_Color);
   int32_t GetWidth(void) const;
   QColor GetColor(void) const;

   virtual QPointF GetPos(void) const;
   QPointF GetP1(void) const;
   QPointF GetP2(void) const;

private:
   //Avoid call
   C_GiLiSimpleLine(const C_GiLiSimpleLine &);
   C_GiLiSimpleLine & operator =(const C_GiLiSimpleLine &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
