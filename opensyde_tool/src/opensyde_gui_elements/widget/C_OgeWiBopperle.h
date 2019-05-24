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

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiBopperle :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiBopperle(QWidget * const opc_Parent = NULL);

   void SetCountIndex(const stw_types::sint32 os32_Count);
   void SetCurrentIndex(const stw_types::sint32 os32_Index);
   void SetColor(const QColor & orc_ColorActive, const QColor & orc_ColorInactive);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
   void m_AdaptSize(void);

private:
   //Avoid call
   C_OgeWiBopperle(const C_OgeWiBopperle &);
   C_OgeWiBopperle & operator =(const C_OgeWiBopperle &);

   stw_types::sint32 ms32_CountIndex;
   stw_types::sint32 ms32_CurrentIndex;
   QColor mc_ColorActive;
   QColor mc_ColorInactive;

   static const stw_types::sintn mhsn_PointSize;
   static const stw_types::sintn mhsn_PointDistance;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
