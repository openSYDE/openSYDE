//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom small default check box (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXDEFAULTSMALL_H
#define C_OGECHXDEFAULTSMALL_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OgeChxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxDefaultSmall :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxDefaultSmall(QWidget * const opc_Parent = NULL);
   C_OgeChxDefaultSmall(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_SubIndex,
                        QWidget * const opc_Parent = NULL);

   void GetIndexes(stw_types::uint32 & oru32_Index, stw_types::uint32 & oru32_SubIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigCheckBoxToggled(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_SubIndex,
                           const bool oq_Checked);

private:
   void m_CheckBoxToggled(const bool oq_Checked);

   const stw_types::uint32 mu32_Index;
   const stw_types::uint32 mu32_SubIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
