//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom small default check box (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXDEFAULTSMALL_H
#define C_OGECHXDEFAULTSMALL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OgeChxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxDefaultSmall :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxDefaultSmall(QWidget * const opc_Parent = NULL);
   C_OgeChxDefaultSmall(const uint32_t ou32_Index, const uint32_t ou32_SubIndex, QWidget * const opc_Parent = NULL);

   void GetIndexes(uint32_t & oru32_Index, uint32_t & oru32_SubIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigCheckBoxToggled(const uint32_t ou32_Index, const uint32_t ou32_SubIndex, const bool oq_Checked);

private:
   void m_CheckBoxToggled(const bool oq_Checked);

   const uint32_t mu32_Index;
   const uint32_t mu32_SubIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
