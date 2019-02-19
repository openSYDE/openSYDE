//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with all status information labels
 (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMETSTATUSBARWIDGET_H
#define C_CAMMETSTATUSBARWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_CamMetStatusBarWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMetStatusBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMetStatusBarWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMetStatusBarWidget(void);

   void InitStaticNames(void);

   void SetBusLoad(const stw_types::uint8 ou8_BusLoad);
   void SetTxErrors(const stw_types::uint32 ou32_TxErrors);
   void SetFilteredMessages(const stw_types::uint32 ou32_FilteredMessages);
   void SetActiveFilters(const stw_types::uint32 ou32_ActiveFilters);

private:
   Ui::C_CamMetStatusBarWidget * mpc_Ui;

   //Avoid call
   C_CamMetStatusBarWidget(const C_CamMetStatusBarWidget &);
   C_CamMetStatusBarWidget & operator =(const C_CamMetStatusBarWidget &);

   void m_UpdateFilterLabel(void) const;

   bool mq_BusLoadWarning;
   bool mq_TxError;
   stw_types::uint32 mu32_FilteredMessages;
   stw_types::uint32 mu32_ActiveFilters;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
