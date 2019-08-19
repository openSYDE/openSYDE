//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model base class (implementation)

   Tree model base class

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "stwtypes.h"
#include "constants.h"
#include "C_TblTreItem.h"
#include "C_TblTreModel.h"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreModel::C_TblTreModel(QObject * const opc_Parent) :
   C_TblTreSimpleModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_TblTreModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if ((orc_Index.isValid() == true) && (orc_Index.column() == 0))
   {
      if (osn_Role == static_cast<stw_types::sintn>(Qt::DisplayRole))
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_Name;
         }
      }
      else if  (osn_Role == static_cast<stw_types::sintn>(Qt::DecorationRole))
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_Icon;
         }
      }
      else if  (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_ToolTipHeading;
         }
      }
      else if  (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_ToolTipContent;
         }
      }
      else
      {
         //Not necessary
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item flags

   \param[in] orc_Index Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_TblTreModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = Qt::NoItemFlags;
   if (orc_Index.isValid() == true)
   {
      //lint -e{925}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem =
         static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->q_Enabled == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEnabled;
         }
         if (pc_TreeItem->q_Selectable == true)
         {
            c_Retval = c_Retval | Qt::ItemIsSelectable;
         }
      }
   }
   return c_Retval;
}
