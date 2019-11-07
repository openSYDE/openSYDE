//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_SdNdeHalcConfigTreeModel.h"

#include "C_TblTreItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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
C_SdNdeHalcConfigTreeModel::C_SdNdeHalcConfigTreeModel(QObject * const opc_Parent) :
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

   pc_Item1->c_Name = "Initialization";
   pc_Item1->c_ToolTipHeading = "Tip";
   pc_Item1->c_ToolTipContent = "Some information about initialization.";
   pc_Item1->q_Selectable = true;

   pc_Child1->c_Name = "Parameter_1";
   pc_Child1->q_Selectable = false;
   pc_Child2->c_Name = "Parameter_2";
   pc_Child2->q_Selectable = false;

   pc_Item1->AddChild(pc_Child1);
   pc_Item1->AddChild(pc_Child2);

   this->mpc_InvisibleRootItem->AddChild(pc_Item1);

   pc_Item2->c_Name = "CategoryABC";
   pc_Item2->c_ToolTipHeading = "Tip";
   pc_Item2->c_ToolTipContent = "Babeldibubeldi";
   pc_Item2->q_Selectable = false;

   pc_Child3->c_Name = "Parameter_n";
   pc_Child2->q_Selectable = false;

   pc_Item2->AddChild(pc_Child3);

   this->mpc_InvisibleRootItem->AddChild(pc_Item2);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor.

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeModel::~C_SdNdeHalcConfigTreeModel()
{
   delete (this->mpc_InvisibleRootItem); // TODO GS ask ME why this is not done in base class where it lives
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return number of columns.

   Here: Exactly three columns: name, value, description.

   \return
   Number of columns, namely 3
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeHalcConfigTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 3;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in] osn_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] osn_Role       Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcConfigTreeModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                                const sintn osn_Role) const
{
   QVariant c_Retval = C_TblTreModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeHalcConfigTreeModel::E_Columns e_Col = C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eVALUE:
            c_Retval = C_GtGetText::h_GetText("Value");
            break;
         case eDESCRIPTION:
            c_Retval = C_GtGetText::h_GetText("Description");
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eVALUE:
            c_Retval = C_GtGetText::h_GetText("Value");
            break;
         case eDESCRIPTION:
            c_Retval = C_GtGetText::h_GetText("Description");
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Parameter name.");
            break;
         case eVALUE:
            c_Retval = C_GtGetText::h_GetText("Value of this parameter.");
            break;
         case eDESCRIPTION:
            c_Retval = C_GtGetText::h_GetText("Description of this parameter.");
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //No data in default case
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcConfigTreeModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, osn_Role);

   bool q_IsParent = true;

   // find out if parent or child
   if (orc_Index.isValid() == true)
   {
      //lint -e{925}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_RootItem = static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
      if (pc_RootItem != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         const C_TblTreItem * const pc_Parent = dynamic_cast<const C_TblTreItem * const>(pc_RootItem->pc_Parent);
         if ((pc_Parent != NULL) && (pc_Parent->pc_Parent != NULL))
         {
            q_IsParent = false;
         }
      }
   }

   //First column etc. is handled by C_TblTreItem and parent item does not have information in in columns despite first
   if ((q_IsParent == false) && (orc_Index.column() > 0))
   {
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         const C_SdNdeHalcConfigTreeModel::E_Columns e_Col =
            C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(orc_Index.column());
         // TODO these are only dummies
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eVALUE:
            c_Retval = C_GtGetText::h_GetText("Value");
            break;
         case eDESCRIPTION:
            c_Retval = C_GtGetText::h_GetText("Description");
            break;
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeModel::E_Columns C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(const sint32 & ors32_Column)
{
   C_SdNdeHalcConfigTreeModel::E_Columns e_Retval;

   switch (ors32_Column)
   {
   case 0:
      e_Retval = eNAME;
      break;
   case 1:
      e_Retval = eVALUE;
      break;
   case 2:
      e_Retval = eDESCRIPTION;
      break;
   default:
      e_Retval = eNAME;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enum to column conversion

   \param[in] ore_Value Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcConfigTreeModel::h_EnumToColumn(const C_SdNdeHalcConfigTreeModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

   switch (ore_Value)
   {
   case eNAME:
      s32_Retval = 0;
      break;
   case eVALUE:
      s32_Retval = 1;
      break;
   case eDESCRIPTION:
      s32_Retval = 2;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}
