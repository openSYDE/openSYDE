//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with all status information labels
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETSTATUSBARWIDGET_H
#define C_CAMMETSTATUSBARWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMetStatusBarWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetStatusBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMetStatusBarWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMetStatusBarWidget(void) override;

   void InitStaticNames(void);

   void SetBusLoad(const uint8_t ou8_BusLoad, const int32_t os32_CanBitrate);
   void SetTxErrors(const uint32_t ou32_TxErrors);
   void SetTransmittedMessages(const uint32_t ou32_NumTxMessages);
   void SetFilteredMessages(const uint32_t ou32_FilteredMessages);
   void SetActiveFilters(const uint32_t ou32_ActiveFilters);

private:
   Ui::C_CamMetStatusBarWidget * mpc_Ui;

   //Avoid call
   C_CamMetStatusBarWidget(const C_CamMetStatusBarWidget &);
   C_CamMetStatusBarWidget & operator =(const C_CamMetStatusBarWidget &) &;

   void m_UpdateFilterLabel(void) const;
   void m_UpdateTxLabel(void);

   bool mq_BusLoadWarning;
   bool mq_TxError;
   uint32_t mu32_TxErrors;
   uint32_t mu32_TransmittedMessages;
   uint32_t mu32_FilteredMessages;
   uint32_t mu32_ActiveFilters;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
