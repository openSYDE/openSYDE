//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom header view for table for tables used in proxy widgets
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTABLEHEADERVIEW_H
#define C_SYVDAITTABLEHEADERVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDpListTableHeaderView.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItTableHeaderView :
   public C_SdNdeDpListTableHeaderView
{
public:
   C_SyvDaItTableHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
   ~C_SyvDaItTableHeaderView(void);

protected:
   virtual bool event(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaItTableHeaderView(const C_SyvDaItTableHeaderView &);
   C_SyvDaItTableHeaderView & operator =(const C_SyvDaItTableHeaderView &);

   bool mq_CursorChanged;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
