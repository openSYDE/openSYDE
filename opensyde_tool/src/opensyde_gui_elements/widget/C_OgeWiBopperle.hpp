//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing points as index indicator (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIBOPPERLE_H
#define C_OGEWIBOPPERLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QColor>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiBopperle :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiBopperle(QWidget * const opc_Parent = NULL);

   void SetCountIndex(const int32_t os32_Count);
   void SetCurrentIndex(const int32_t os32_Index);
   void SetColor(const QColor & orc_ColorActive, const QColor & orc_ColorInactive);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void m_AdaptSize(void);

private:
   //Avoid call
   C_OgeWiBopperle(const C_OgeWiBopperle &);
   C_OgeWiBopperle & operator =(const C_OgeWiBopperle &) &;

   int32_t ms32_CountIndex;
   int32_t ms32_CurrentIndex;
   QColor mc_ColorActive;
   QColor mc_ColorInactive;

   static const int32_t mhs32_POINT_SIZE;
   static const int32_t mhs32_POINT_DISTANCE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
