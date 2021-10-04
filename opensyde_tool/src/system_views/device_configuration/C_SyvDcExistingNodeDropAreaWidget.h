//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for a drop area (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCEXISTINGNODEDROPAREAWIDGET_H
#define C_SYVDCEXISTINGNODEDROPAREAWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OSCProtocolSerialNumber.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDcExistingNodeDropAreaWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDcExistingNodeDropAreaWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcExistingNodeDropAreaWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDcExistingNodeDropAreaWidget(void);

   void InitStaticNames(void) const;
   void SetContent(const bool oq_ValidSerialNumber,
                   const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_PureSerialNumber);
   bool IsAssigned(void) const;
   void GetContent(stw_opensyde_core::C_OSCProtocolSerialNumber & orc_PureSerialNumber) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDisconnect(const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_SyvDcExistingNodeDropAreaWidget * mpc_Ui;
   stw_opensyde_core::C_OSCProtocolSerialNumber mc_PureSerialNumber;
   bool mq_Assigned;

   void m_OnDisconnectRequest(void);

   //Avoid call
   C_SyvDcExistingNodeDropAreaWidget(const C_SyvDcExistingNodeDropAreaWidget &);
   C_SyvDcExistingNodeDropAreaWidget & operator =(const C_SyvDcExistingNodeDropAreaWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
