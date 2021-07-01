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
#include "stwtypes.h"

#include <QModelIndex>
#include <QVector>

#include "C_OgeTreeViewToolTipBase.h"
#include "C_TblTreeModelCheckable.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeViewToolTipBaseCheckable :
   public C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_OgeTreeViewToolTipBaseCheckable(QWidget * const opc_Parent = NULL);
   ~C_OgeTreeViewToolTipBaseCheckable(void);

   void Init(stw_opensyde_gui_logic::C_TblTreeModelCheckable * const opc_Model,
             const std::vector<stw_types::uint32> & orc_ElementIndices);
   void GetCheckedItems(const stw_opensyde_gui_logic::C_TblTreeModelCheckable * const opc_Model,
                        std::vector<stw_types::uint32> & orc_ElementIndices,
                        std::vector<std::vector<stw_types::uint32> > & orc_ChildIndicesPerElement) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(const stw_types::sintn osn_Count);

protected:
   stw_opensyde_gui_logic::C_TblTreeModelCheckable * mpc_Model;
   void m_ChangedData();
   C_OgeTreeViewToolTipBaseDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
