//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeHalcOverviewTreeModel.h"

#include "C_TblTreItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]    opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcOverviewTreeModel::C_SdNdeHalcOverviewTreeModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent)
{
   // TODO GS Spielwiese
   // for examples see C_TblTreDataElementModel and C_TblTreDataElementView
   this->beginResetModel();
   this->mpc_InvisibleRootItem = new C_TblTreItem();
   C_TblTreItem * const pc_Item1 = new C_TblTreItem();
   C_TblTreItem * const pc_Item2 = new C_TblTreItem();
   C_TblTreItem * const pc_Child1 = new C_TblTreItem();
   C_TblTreItem * const pc_Child2 = new C_TblTreItem();
   C_TblTreItem * const pc_Child3 = new C_TblTreItem();

   pc_Item1->c_Name = "Inputs";
   pc_Item1->c_ToolTipHeading = "Tip";
   pc_Item1->c_ToolTipContent = "Some information about inputs.";
   pc_Item1->q_Selectable = false;

   pc_Child1->c_Name = "MyInput1 (X_IN_IDA35V1)";
   pc_Child2->c_Name = "MyInput2 (X_IN_IDA35V2)";

   pc_Item1->AddChild(pc_Child1);
   pc_Item1->AddChild(pc_Child2);

   this->mpc_InvisibleRootItem->AddChild(pc_Item1);

   pc_Item2->c_Name = "Outputs";
   pc_Item2->c_ToolTipHeading = "Tip";
   pc_Item2->c_ToolTipContent = "Babeldibubeldi";
   pc_Item2->q_Selectable = false;

   pc_Child3->c_Name = "MyOutput1 (X_OUT_01)";

   pc_Item2->AddChild(pc_Child3);

   this->mpc_InvisibleRootItem->AddChild(pc_Item2);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor.

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcOverviewTreeModel::~C_SdNdeHalcOverviewTreeModel()
{
   delete (this->mpc_InvisibleRootItem); // TODO GS ask ME why this is not done in base class where it lives
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return number of columns.

   Here: Only one column.

   \return
   Number of columns, namely 1
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeHalcOverviewTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}
