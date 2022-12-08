//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update PieChart (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPIECHART_H
#define C_SYVUPPIECHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QMovie>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPieChart :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPieChart(QWidget * const opc_Parent = NULL);
   ~C_SyvUpPieChart(void) override;

   void SetProgress100(const uint16_t ou16_Progress100, const bool oq_Finished = false);

   void SetActiveColor(const QColor & orc_Value);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   static const QString mhc_FILE_PATH;
   uint16_t mu16_ValueInPercent;
   QColor mc_ActiveColor;
   bool mq_FinishedAnimation;
   QMovie * mpc_Movie;

   void m_OnFrameChanged(void);
   //Avoid call
   C_SyvUpPieChart(const C_SyvUpPieChart &);
   C_SyvUpPieChart & operator =(const C_SyvUpPieChart &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
