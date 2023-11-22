//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom header view for table for tables used in proxy widgets
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTABLEHEADERVIEW_HPP
#define C_SYVDAITTABLEHEADERVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDpListTableHeaderView.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItTableHeaderView :
   public C_SdNdeDpListTableHeaderView
{
public:
   C_SyvDaItTableHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
   ~C_SyvDaItTableHeaderView(void) override;

   void SetCursorHandlingActive(const bool oq_Active);

protected:
   bool event(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaItTableHeaderView(const C_SyvDaItTableHeaderView &);
   C_SyvDaItTableHeaderView & operator =(const C_SyvDaItTableHeaderView &) &; //lint !e1511 //we want to hide the base

   bool mq_CursorHandlingActive;
   bool mq_CursorChanged;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
