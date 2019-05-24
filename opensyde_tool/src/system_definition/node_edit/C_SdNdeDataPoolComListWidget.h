//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display data pool list com entries (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLCOMLISTWIDGET_H
#define C_SDNDEDATAPOOLCOMLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDataPoolComListWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolComListWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolComListWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolComListWidget(void);
   void SetDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex);
   void Clear(void);
   void Update(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigEdit(const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex);
   void SigSwitchToBus(const stw_types::uint32 & oru32_BusIndex, const QString & orc_BusName);
   void SigErrorChange(const bool oq_Error);

private:
   Ui::C_SdNdeDataPoolComListWidget * const mpc_Ui;
   std::vector<QWidget *> mc_LastWidgets;

   void m_OnErrorChange(void);
   void m_OnEdit(const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex);
   void m_OnSwitchToBus(const stw_types::uint32 & oru32_BusIndex, const QString & orc_BusName);

   //Avoid call
   C_SdNdeDataPoolComListWidget(const C_SdNdeDataPoolComListWidget &);
   C_SdNdeDataPoolComListWidget & operator =(const C_SdNdeDataPoolComListWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
