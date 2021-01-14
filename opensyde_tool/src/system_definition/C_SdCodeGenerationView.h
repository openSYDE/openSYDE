//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view of code generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCODEGENERATIONVIEW_H
#define C_SDCODEGENERATIONVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include <QModelIndex>
#include <QVector>

#include "C_OgeTreeViewToolTipBase.h"
#include "C_SdCodeGenerationModel.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdCodeGenerationView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdCodeGenerationView(QWidget * const opc_Parent = NULL);
   ~C_SdCodeGenerationView(void);

   void Init(const std::vector<stw_types::uint32> & orc_NodeIndices);
   void GetCheckedItems(std::vector<stw_types::uint32> & orc_NodeIndices,
                        std::vector<std::vector<stw_types::uint32> > & orc_AppIndicesPerNode) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(const stw_types::sintn osn_Count);

private:
   stw_opensyde_gui_logic::C_SdCodeGenerationModel mc_Model;
   stw_opensyde_gui_elements::C_OgeTreeViewToolTipBaseDelegate mc_Delegate;

   void m_ChangedData(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
