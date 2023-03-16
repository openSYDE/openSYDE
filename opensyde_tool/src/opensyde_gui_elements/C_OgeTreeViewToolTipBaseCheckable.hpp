//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view of code generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEVIEWTOOLTIPBASECHECKABLE_H
#define C_OGETREEVIEWTOOLTIPBASECHECKABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include <QModelIndex>
#include <QVector>

#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_TblTreeModelCheckable.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeViewToolTipBaseCheckable :
   public C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_OgeTreeViewToolTipBaseCheckable(QWidget * const opc_Parent = NULL);
   ~C_OgeTreeViewToolTipBaseCheckable(void) override;

   void Init(stw::opensyde_gui_logic::C_TblTreeModelCheckable * const opc_Model,
             const std::vector<uint32_t> & orc_ElementIndices);
   void GetCheckedItems(const stw::opensyde_gui_logic::C_TblTreeModelCheckable * const opc_Model,
                        std::vector<uint32_t> & orc_ElementIndices,
                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(const int32_t os32_Count);

protected:
   stw::opensyde_gui_logic::C_TblTreeModelCheckable * mpc_Model;
   void m_ChangedData();
   C_OgeTreeViewToolTipBaseDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
