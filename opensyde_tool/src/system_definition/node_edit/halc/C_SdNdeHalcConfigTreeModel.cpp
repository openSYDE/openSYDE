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
#include "stwerrors.h"
#include "constants.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"

#include "C_SdNdeHalcConfigTreeModel.h"
#include "C_TblTreItem.h"

#include "C_PuiSdHandler.h"
#include "C_SdNdeDpContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_opensyde_core;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeModel::C_SdNdeHalcConfigTreeModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DomainIndex(0),
   mu32_ChannelIndex(0),
   mu32_UseCaseIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor.

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeModel::~C_SdNdeHalcConfigTreeModel()
{
   delete (this->mpc_InvisibleRootItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return number of columns.

   Here: Exactly three columns: name, value, description.

   \param[in]  orc_Parent  Parent

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

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

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

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcConfigTreeModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, osn_Role);

   // first columns are handled by C_TblTreItem and parents only have first column
   if ((h_IsParentIndex(orc_Index) == false) && (orc_Index.column() > 0))
   {
      uint32 u32_ParameterIndex;
      uint32 u32_ParameterElementIndex;

      // get parameter element from model index
      if (m_GetParameterElementIndexe(orc_Index, u32_ParameterIndex, u32_ParameterElementIndex) == true)
      {
         const C_OSCHalcConfigParameterStruct * pc_Parameter =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelParameterConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                                     mu32_ChannelIndex,
                                                                                     u32_ParameterIndex);

         if ((pc_Parameter != NULL) && (u32_ParameterElementIndex < pc_Parameter->c_ParameterElements.size()))
         {
            const C_OSCHalcConfigParameter & rc_ParameterElement =
               pc_Parameter->c_ParameterElements[u32_ParameterElementIndex];

            const C_SdNdeHalcConfigTreeModel::E_Columns e_Col =
               C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(orc_Index.column());

            if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
            {
               // show data for column
               switch (e_Col)
               {
               case eNAME:
                  // name is handled by tree item name
                  break;
               case eVALUE:
                  c_Retval = C_SdNdeDpContentUtil::h_ConvertContentToGeneric(rc_ParameterElement.c_Value, 0);
                  break;
               case eDESCRIPTION:
                  c_Retval = rc_ParameterElement.c_Comment.c_str();
                  break;
               }
            }
            else if ((osn_Role == msn_USER_ROLE_INTERACTION_GENERIC_SPIN_BOX_PARAMETERS_LIST) && (e_Col == eVALUE))
            {
               // Set generic spin box parameters
               QStringList c_List;
               QString c_TmpStr;
               C_OSCNodeDataPoolContent c_TmpDpCont = rc_ParameterElement.c_Value;
               // minimum
               C_SdNdeDpContentUtil::h_InitMin(c_TmpDpCont);
               c_TmpStr = C_SdNdeDpContentUtil::h_GetAllContentAsString(c_TmpDpCont);
               c_List.push_back(c_TmpStr);
               // maximum
               C_SdNdeDpContentUtil::h_InitMax(c_TmpDpCont);
               c_TmpStr = C_SdNdeDpContentUtil::h_GetAllContentAsString(c_TmpDpCont);
               c_List.push_back(c_TmpStr);
               // factor
               c_TmpStr = QString::number(1.0);
               c_List.push_back(c_TmpStr);
               // offset
               c_TmpStr = QString::number(0.0);
               c_List.push_back(c_TmpStr);

               c_Retval = c_List;
            }
            else if (((osn_Role == msn_USER_ROLE_INTERACTION_COMBO_BOX_STRINGS_LIST) ||
                      (osn_Role == msn_USER_ROLE_INTERACTION_COMBO_BOX_VALUES_LIST)) && (e_Col == eVALUE))
            {
               QStringList c_Tmp;

               const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> & rc_EnumItems =
                  rc_ParameterElement.c_Value.GetEnumItems();

               for (std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent>::const_iterator c_It =
                       rc_EnumItems.begin();
                    c_It != rc_EnumItems.end(); ++c_It)
               {
                  c_Tmp.append(c_It->first.c_str());
                  // TODO: what about c_It->second?
               }

               c_Tmp.append("DummyItem"); // TODO remove
               c_Retval = c_Tmp;
            }
            else if ((osn_Role == msn_USER_ROLE_INTERACTION_ELEMENT_TYPE) && (e_Col == eVALUE))
            {
               // use edit field depending on complex type
               switch (rc_ParameterElement.c_Value.GetComplexType())
               {
               case C_OSCHalcDefContent::eCT_PLAIN:
                  c_Retval = static_cast<sintn>(eURIEL_GENERIC_SPIN_BOX);
                  break;
               // TODO somehow enums are not existent...
               case C_OSCHalcDefContent::eCT_ENUM:
               case C_OSCHalcDefContent::eCT_BIT_MASK:
                  c_Retval = static_cast<sintn>(eURIEL_COMBO_BOX);
                  break;
               }
            }
            else
            {
               // Use default
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data at index

   Here: Get data from editor widgets into data handling.

   \param[in]  orc_Index   Model index
   \param[in]  orc_Value   New data
   \param[in]  osn_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigTreeModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                         const sintn osn_Role)
{
   bool q_Retval = false;

   const C_SdNdeHalcConfigTreeModel::E_Columns e_Col =
      C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(orc_Index.column());

   if ((data(orc_Index, osn_Role) != orc_Value) && (e_Col == eVALUE))
   {
      uint32 u32_ParameterIndex;
      uint32 u32_ParameterElementIndex;

      // get parameter element from model index
      if (m_GetParameterElementIndexe(orc_Index, u32_ParameterIndex, u32_ParameterElementIndex) == true)
      {
         const C_OSCHalcConfigParameterStruct * pc_Parameter =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelParameterConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                                     mu32_ChannelIndex,
                                                                                     u32_ParameterIndex);

         if ((pc_Parameter != NULL) && (u32_ParameterElementIndex < pc_Parameter->c_ParameterElements.size()))
         {
            sint32 s32_Return;

            // copy and adapt value
            C_OSCHalcConfigParameterStruct c_NewParameter = *pc_Parameter;
            C_OSCHalcConfigParameter & rc_ParameterElement =
               c_NewParameter.c_ParameterElements[u32_ParameterElementIndex];
            s32_Return = C_SdNdeDpContentUtil::h_SetDataVariableFromGeneric(orc_Value, rc_ParameterElement.c_Value, 0);

            if (s32_Return == C_NO_ERR)
            {
               // update data
               s32_Return =
                  C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelParameterConfig(mu32_NodeIndex, mu32_DomainIndex,
                                                                                       mu32_ChannelIndex,
                                                                                       orc_Index.parent().row(),
                                                                                       c_NewParameter);
               tgl_assert(s32_Return == C_NO_ERR);
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item flags

   \param[in]  orc_Index   Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeHalcConfigTreeModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = C_TblTreModel::flags(orc_Index);

   const C_SdNdeHalcConfigTreeModel::E_Columns e_Col =
      C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(orc_Index.column());

   if ((h_IsParentIndex(orc_Index) == false) && (e_Col == eVALUE))
   {
      c_Retval = c_Retval | Qt::ItemIsEditable;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index.

   Create all tree children. Displayed data in columns other than the first is handled by data().

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeModel::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC channel and use case index.

   Create all tree children. Displayed data in columns other than the first is handled by data().

   \param[in]  ou32_DomainIndex     HALC Domain index
   \param[in]  ou32_ChannelIndex    HALC Channel index
   \param[in]  ou32_UseCaseIndex    HALC Use case index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeModel::SetHalcChannelUseCase(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                       const uint32 ou32_UseCaseIndex)
{
   // remember indexes
   this->mu32_DomainIndex = ou32_DomainIndex;
   this->mu32_ChannelIndex = ou32_ChannelIndex;
   this->mu32_UseCaseIndex = ou32_UseCaseIndex;

   // clear
   this->Clear();

   // insert model data
   this->beginResetModel();
   if (this->mpc_InvisibleRootItem == NULL)
   {
      this->mpc_InvisibleRootItem = new C_TblTreItem();
   }

   // get domain
   const C_OSCHalcConfigDomain * pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(mu32_NodeIndex, mu32_DomainIndex);

   if (pc_Domain != NULL)
   {
      // create tree items for parameters and its elements that are relevant for selected channel and use case
      for (uint32 u32_ParamIt = 0; u32_ParamIt < pc_Domain->c_Parameters.size(); u32_ParamIt++)
      {
         const C_OSCHalcDefStruct & rc_Parameter = pc_Domain->c_Parameters[u32_ParamIt];
         C_TblTreItem * const pc_ParameterTreeItem = new C_TblTreItem();
         pc_ParameterTreeItem->c_Name = rc_Parameter.c_Display.c_str();
         pc_ParameterTreeItem->c_ToolTipHeading = pc_ParameterTreeItem->c_Name;
         pc_ParameterTreeItem->c_ToolTipContent = rc_Parameter.c_Comment.c_str();

         // add parameter elements
         for (uint32 u32_ElementIt = 0; u32_ElementIt < rc_Parameter.c_StructElements.size(); u32_ElementIt++)
         {
            const C_OSCHalcDefElement & rc_Element = rc_Parameter.c_StructElements[u32_ElementIt];

            bool q_AvailableForUseCase = false;

            // check if element is available for selected use case
            for (std::vector<stw_types::uint32>::const_iterator c_ItAvail =
                    rc_Element.c_UseCaseAvailabilities.begin();
                 c_ItAvail != rc_Element.c_UseCaseAvailabilities.end(); ++c_ItAvail)
            {
               if (*c_ItAvail == this->mu32_UseCaseIndex)
               {
                  q_AvailableForUseCase = true;
                  break;
               }
            }

            if (q_AvailableForUseCase == true)
            {
               C_TblTreItem * const pc_Child = new C_TblTreItem();
               pc_Child->c_Name = rc_Element.c_Display.c_str();
               pc_Child->c_ToolTipHeading = pc_Child->c_Name;
               pc_Child->c_ToolTipContent = rc_Element.c_Comment.c_str();

               // add to tree
               pc_ParameterTreeItem->AddChild(pc_Child);
            }
         }

         // add to tree
         if (this->mpc_InvisibleRootItem != NULL)
         {
            this->mpc_InvisibleRootItem->AddChild(pc_ParameterTreeItem);
            // TODO: only add if at least one child is available for selected use case ?
         }
         else
         {
            delete pc_ParameterTreeItem;
         }
      }
   }

   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear all children.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeModel::Clear(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *>::const_iterator c_ItChildren;

      this->beginResetModel();

      // clear parameter elements (children of children)
      for (c_ItChildren = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItChildren != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItChildren)
      {
         if (*c_ItChildren != NULL)
         {
            (*c_ItChildren)->ClearChildren();
         }
      }

      // clear parameter (children)
      this->mpc_InvisibleRootItem->ClearChildren();

      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decode model index: parent or child?

   The model is not brutally complex, i.e. it is sufficient to decode the model index into "is it parent or not".

   \param[in]  orc_Index   Model index to decode

   \retval   true   Model index belongs to a parameter (parent)
                     paramter index = orc_Index.row()
   \retval   false  Model index belongs to a element (child)
                     paramter index = orc_Index.parent().row()
                     element index = orc_Index.row()
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigTreeModel::h_IsParentIndex(const QModelIndex & orc_Index)
{
   bool q_IsParent = true;

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

   return q_IsParent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get parameter and parameter element index corresponding to given model index

   \param[in]   orc_Index                    Model index
   \param[out]  oru32_ParameterIndex         Parameter index
   \param[out]  oru32_ParameterElementIndex  Parameter element index (only valid if return = true)

   \retval   true   Model index belongs to a parameter element
   \retval   false  Model index does not belong to any parameter element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigTreeModel::m_GetParameterElementIndexe(const QModelIndex & orc_Index,
                                                             uint32 & oru32_ParameterIndex,
                                                             uint32 & oru32_ParameterElementIndex) const
{
   bool q_IsValue = false;

   // model parent index row is parameter index
   if ((orc_Index.parent().isValid() == true) && (orc_Index.parent().row() >= 0))
   {
      oru32_ParameterIndex = orc_Index.parent().row();

      // model index row is parameter element index
      if ((orc_Index.isValid() == true) && (orc_Index.row() >= 0))
      {
         oru32_ParameterElementIndex = orc_Index.row();
         q_IsValue = true;
      }
   }

   return q_IsValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

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

   \param[in]  ore_Value   Enum value

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
