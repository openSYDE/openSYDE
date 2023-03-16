//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for data pool entry with delete functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBDATAPOOLENTRY_H
#define C_SDNDEDBDATAPOOLENTRY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiWithToolTip.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbDataPoolEntry;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbDataPoolEntry :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDbDataPoolEntry(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                   QWidget * const opc_Parent = NULL);
   ~C_SdNdeDbDataPoolEntry(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDeleteDataPool(C_SdNdeDbDataPoolEntry * const opc_Source, const uint32_t ou32_Index);

private:
   Ui::C_SdNdeDbDataPoolEntry * mpc_Ui;
   const uint32_t mu32_DataPoolIndex;

   void m_OnDeleteClick(void);
   void m_Init(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);

   //Avoid call
   C_SdNdeDbDataPoolEntry(const C_SdNdeDbDataPoolEntry &);
   C_SdNdeDbDataPoolEntry & operator =(const C_SdNdeDbDataPoolEntry &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
