//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDESFORESETMESSAGETABLEVIEW_HPP
#define C_SDNDESFORESETMESSAGETABLEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewScroll.hpp"
#include "stwtypes.hpp"
#include "C_SdNdeSfoResetMessageTableModel.hpp"
#include "C_SdNdeSfoResetMessageTableDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeSfoResetMessageTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeSfoResetMessageTableView(QWidget * const opc_Parent = NULL);

   void SetNode(const uint32_t ou32_NodeIndex);

private:
   stw::opensyde_gui_logic::C_SdNdeSfoResetMessageTableDelegate mc_Delegate;
   stw::opensyde_gui_logic::C_SdNdeSfoResetMessageTableModel mc_Model;

   void m_InitColumns(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
