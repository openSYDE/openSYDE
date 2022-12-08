//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"

#include "C_SdNdeHalcConfigTreeModel.hpp"
#include "C_TblTreItem.hpp"

#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

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
   mu32_UseCaseIndex(0),
   mq_ChannelCase(true)
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
int32_t C_SdNdeHalcConfigTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 3;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcConfigTreeModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                                const int32_t os32_Role) const
{
   QVariant c_Retval = C_TblTreModel::headerData(os32_Section,
                                                 oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeHalcConfigTreeModel::E_Columns e_Col = C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
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
         default:
            tgl_assert(false);
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
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
         default:
            tgl_assert(false);
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
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
         default:
            tgl_assert(false);
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
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
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcConfigTreeModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, os32_Role);

   // first columns are handled by C_TblTreItem
   if (orc_Index.column() > 0)
   {
      const C_SdNdeHalcConfigTreeModel::E_Columns e_Col =
         C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(orc_Index.column());

      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         // get parameter data (of parameter itself or of parameter element)
         const C_OscHalcConfigParameter * const pc_ParameterElement = m_GetParameterElement(orc_Index);
         if (pc_ParameterElement != NULL)
         {
            const std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> > & rc_EnumItems =
               pc_ParameterElement->c_Value.GetEnumItems();
            uint32_t u32_Counter = 0;

            // show data for column
            switch (e_Col)
            {
            case eNAME:
               // name is handled by tree item name
               break;
            case eVALUE:
               switch (pc_ParameterElement->c_Value.GetComplexType())
               {
               case C_OscHalcDefContent::eCT_PLAIN:
                  c_Retval = C_SdNdeDpContentUtil::h_ConvertContentToGeneric(pc_ParameterElement->c_Value, 0);
                  break;
               case C_OscHalcDefContent::eCT_ENUM:
                  c_Retval = C_GtGetText::h_GetText("unknown");

                  for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
                          rc_EnumItems.begin(); c_It != rc_EnumItems.end(); ++c_It)
                  {
                     if (c_It->second == pc_ParameterElement->c_Value)
                     {
                        if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
                        {
                           c_Retval = c_It->first.c_str();
                        }
                        else
                        {
                           c_Retval = static_cast<int64_t>(u32_Counter);
                        }
                        break;
                     }
                     u32_Counter++;
                  }
                  break;
               case C_OscHalcDefContent::eCT_BIT_MASK:
                  if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
                  {
                     c_Retval = mh_ConvertBitmasksToString(pc_ParameterElement->c_Value);
                  }
                  else
                  {
                     c_Retval = mh_ConvertBitmasksToBitArray(pc_ParameterElement->c_Value);
                  }
                  break;
               default:
                  tgl_assert(false);
                  break;
               }
               break;
            case eDESCRIPTION:
               c_Retval = pc_ParameterElement->c_Comment.c_str();
               break;
            default:
               tgl_assert(false);
               break;
            }
         }
      }
      else if ((os32_Role == ms32_USER_ROLE_INTERACTION_GENERIC_SPIN_BOX_PARAMETERS_LIST) && (e_Col == eVALUE))
      {
         const C_OscHalcDefElement * const pc_DefElement = this->m_GetDefParameterElement(orc_Index);
         if (pc_DefElement != NULL)
         {
            // Set generic spin box parameters
            QStringList c_List;
            QString c_TmpStr;
            // minimum
            c_TmpStr = C_SdNdeDpContentUtil::h_GetAllContentAsString(pc_DefElement->c_MinValue);
            c_List.push_back(c_TmpStr);
            // maximum
            c_TmpStr = C_SdNdeDpContentUtil::h_GetAllContentAsString(pc_DefElement->c_MaxValue);
            c_List.push_back(c_TmpStr);
            // factor
            c_TmpStr = QString::number(1.0);
            c_List.push_back(c_TmpStr);
            // offset
            c_TmpStr = QString::number(0.0);
            c_List.push_back(c_TmpStr);

            c_Retval = c_List;
         }
      }
      else if ((os32_Role == ms32_USER_ROLE_INTERACTION_MULTI_SELECT_COMBO_BOX_STRINGS_LIST) && (e_Col == eVALUE))
      {
         // get parameter data (of parameter itself or of parameter element)
         const C_OscHalcConfigParameter * const pc_ParameterElement = m_GetParameterElement(orc_Index);
         if (pc_ParameterElement != NULL)
         {
            c_Retval = mh_ConvertBitmasksToStringList(pc_ParameterElement->c_Value);
         }
      }
      else if ((os32_Role == ms32_USER_ROLE_INTERACTION_COMBO_BOX_STRINGS_LIST) && (e_Col == eVALUE))
      {
         // get parameter data (of parameter itself or of parameter element)
         const C_OscHalcConfigParameter * const pc_ParameterElement = m_GetParameterElement(orc_Index);
         if (pc_ParameterElement != NULL)
         {
            c_Retval = mh_ConvertEnumsToStringList(pc_ParameterElement->c_Value);
         }
      }
      else if ((os32_Role == ms32_USER_ROLE_INTERACTION_ELEMENT_TYPE) && (e_Col == eVALUE))
      {
         // get parameter data (of parameter itself or of parameter element)
         const C_OscHalcConfigParameter * const pc_ParameterElement = m_GetParameterElement(orc_Index);
         if (pc_ParameterElement != NULL)
         {
            // use edit field depending on complex type
            switch (pc_ParameterElement->c_Value.GetComplexType())
            {
            case C_OscHalcDefContent::eCT_PLAIN:
               c_Retval = static_cast<int32_t>(eURIEL_GENERIC_SPIN_BOX);
               break;
            case C_OscHalcDefContent::eCT_ENUM:
               c_Retval = static_cast<int32_t>(eURIEL_COMBO_BOX);
               break;
            case C_OscHalcDefContent::eCT_BIT_MASK:
               c_Retval = static_cast<int32_t>(eURIEL_MULTI_SELECT_COMBO_BOX);
               break;
            default:
               tgl_assert(false);
               break;
            }
         }
      }
      else
      {
         // Use default
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data at index

   Here: Get data from editor widgets into data handling.

   \param[in]  orc_Index   Model index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigTreeModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                         const int32_t os32_Role)
{
   bool q_Retval = false;

   const C_SdNdeHalcConfigTreeModel::E_Columns e_Col =
      C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(orc_Index.column());

   if ((data(orc_Index, os32_Role) != orc_Value) && (e_Col == eVALUE))
   {
      bool q_Tmp;
      std::vector<uint32_t> c_LinkedChannels;
      if (C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                        this->mu32_ChannelIndex, this->mq_ChannelCase,
                                                                        q_Tmp, NULL, &c_LinkedChannels) == C_NO_ERR)
      {
         // get parameter data (of parameter itself or of parameter element)
         const C_OscHalcConfigParameter * const pc_ParameterElement = m_GetParameterElement(orc_Index);
         if (pc_ParameterElement != NULL)
         {
            uint32_t u32_ParameterIndex;
            uint32_t u32_ParameterElementIndex;
            mh_GetParameterElementIndexe(orc_Index, u32_ParameterIndex, u32_ParameterElementIndex);

            // extra handling of enum values
            switch (pc_ParameterElement->c_Value.GetComplexType())
            {
            case C_OscHalcDefContent::eCT_ENUM:
               {
                  //Enums
                  const QStringList c_Tmp = mh_ConvertEnumsToStringList(pc_ParameterElement->c_Value);
                  const int32_t s32_ComboBoxIndex = static_cast<int32_t>(orc_Value.toLongLong());

                  if (s32_ComboBoxIndex < c_Tmp.size())
                  {
                     const QString c_Value = c_Tmp.at(s32_ComboBoxIndex).toStdString().c_str();
                     //Set value
                     if (C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelParameterConfigElementEnum(
                            this->mu32_NodeIndex,
                            this->mu32_DomainIndex,
                            this->mu32_ChannelIndex, u32_ParameterIndex, u32_ParameterElementIndex,
                            this->mq_ChannelCase,
                            c_Value.toStdString().c_str()) == C_NO_ERR)
                     {
                        q_Retval = true;
                        //Linked values
                        for (uint32_t u32_It = 0UL; u32_It < c_LinkedChannels.size(); ++u32_It)
                        {
                           tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelParameterConfigElementEnum(
                                         this->mu32_NodeIndex,
                                         this->mu32_DomainIndex,
                                         c_LinkedChannels[u32_It], u32_ParameterIndex, u32_ParameterElementIndex,
                                         this->mq_ChannelCase,
                                         c_Value.toStdString().c_str()) == C_NO_ERR);
                        }
                     }
                  }
               }
               break;
            case C_OscHalcDefContent::eCT_BIT_MASK:
               {
                  //Bitmasks
                  const QStringList c_Names = mh_ConvertBitmasksToStringList(pc_ParameterElement->c_Value);
                  const QBitArray c_Bits = orc_Value.toBitArray();

                  if (c_Names.size() == c_Bits.size())
                  {
                     for (int32_t s32_It = 0; s32_It < c_Names.size(); ++s32_It)
                     {
                        //Set value
                        if (C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelParameterConfigElementBitmask(
                               this->mu32_NodeIndex,
                               this->mu32_DomainIndex,
                               this->mu32_ChannelIndex, u32_ParameterIndex, u32_ParameterElementIndex,
                               this->mq_ChannelCase,
                               c_Names.at(s32_It).toStdString().c_str(), c_Bits.at(s32_It)) == C_NO_ERR)
                        {
                           q_Retval = true;
                           //Linked values
                           for (uint32_t u32_It = 0UL; u32_It < c_LinkedChannels.size(); ++u32_It)
                           {
                              tgl_assert(
                                 C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelParameterConfigElementBitmask(
                                    this->mu32_NodeIndex,
                                    this->mu32_DomainIndex,
                                    c_LinkedChannels[u32_It], u32_ParameterIndex, u32_ParameterElementIndex,
                                    this->mq_ChannelCase,
                                    c_Names.at(s32_It).toStdString().c_str(), c_Bits.at(s32_It)) == C_NO_ERR);
                           }
                        }
                     }
                  }
               }
               break;
            case C_OscHalcDefContent::eCT_PLAIN:
               {
                  //Plain values
                  // copy and adapt value
                  C_OscHalcDefContent c_NewValue = pc_ParameterElement->c_Value;
                  if (C_SdNdeDpContentUtil::h_SetDataVariableFromGeneric(orc_Value, c_NewValue, 0) == C_NO_ERR)
                  {
                     //Set value
                     if (C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelParameterConfigElementPlain(
                            this->mu32_NodeIndex,
                            this->mu32_DomainIndex,
                            this->mu32_ChannelIndex, u32_ParameterIndex, u32_ParameterElementIndex,
                            this->mq_ChannelCase,
                            c_NewValue) == C_NO_ERR)
                     {
                        q_Retval = true;
                        //Linked values
                        for (uint32_t u32_It = 0UL; u32_It < c_LinkedChannels.size(); ++u32_It)
                        {
                           tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelParameterConfigElementPlain(
                                         this->mu32_NodeIndex,
                                         this->mu32_DomainIndex,
                                         c_LinkedChannels[u32_It], u32_ParameterIndex, u32_ParameterElementIndex,
                                         this->mq_ChannelCase,
                                         c_NewValue) == C_NO_ERR);
                        }
                     }
                  }
               }
               break;
            default:
               tgl_assert(false);
               break;
            }
            Q_EMIT (this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role));
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

   if ((e_Col == eVALUE) && (this->rowCount(orc_Index) == 0) /*i.e. no children*/)
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
void C_SdNdeHalcConfigTreeModel::SetNode(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeHalcConfigTreeModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain, channel and use case index.

   Create all tree children. Displayed data in columns other than the first is handled by data().

   \param[in]  ou32_DomainIndex     HALC Domain index
   \param[in]  ou32_ChannelIndex    HALC Channel index
   \param[in]  ou32_UseCaseIndex    HALC Use case index
   \param[in]  oq_UseChannelIndex   True: manage channel- and use case specific parameters
                                    False: manage domain parameters standalone
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeModel::SetHalcChannelUseCase(const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex,
                                                       const uint32_t ou32_UseCaseIndex, const bool oq_UseChannelIndex)
{
   // remember indexes
   this->mu32_DomainIndex = ou32_DomainIndex;
   this->mu32_ChannelIndex = ou32_ChannelIndex;
   this->mu32_UseCaseIndex = ou32_UseCaseIndex;
   this->mq_ChannelCase = oq_UseChannelIndex;

   // clear
   this->Clear();

   // insert model data
   this->beginResetModel();
   if (this->mpc_InvisibleRootItem == NULL)
   {
      this->mpc_InvisibleRootItem = new C_TblTreItem();
   }

   // get domain
   const C_OscHalcConfigDomain * const pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(mu32_NodeIndex, mu32_DomainIndex);

   if (pc_Domain != NULL)
   {
      const std::vector<C_OscHalcDefStruct> & rc_Parameters =
         mq_ChannelCase ? pc_Domain->c_ChannelValues.c_Parameters : pc_Domain->c_DomainValues.c_Parameters;

      // create tree items for parameters and elements that are relevant for selected channel and use case resp. domain
      for (uint32_t u32_ParamIt = 0; u32_ParamIt < rc_Parameters.size(); u32_ParamIt++)
      {
         bool q_AddParameterToTree = false;
         const C_OscHalcDefStruct & rc_Parameter = rc_Parameters[u32_ParamIt];
         C_TblTreItem * const pc_ParameterTreeItem = new C_TblTreItem();

         pc_ParameterTreeItem->c_Name = rc_Parameter.c_Display.c_str();
         pc_ParameterTreeItem->c_ToolTipHeading = pc_ParameterTreeItem->c_Name;
         pc_ParameterTreeItem->c_ToolTipContent = rc_Parameter.c_Comment.c_str();
         pc_ParameterTreeItem->u32_Index = u32_ParamIt;

         // check availability of parameter itself
         if (this->m_CheckAvailability(rc_Parameter) == true)
         {
            // add parameter elements
            for (uint32_t u32_ElementIt = 0; u32_ElementIt < rc_Parameter.c_StructElements.size(); u32_ElementIt++)
            {
               const C_OscHalcDefElement & rc_Element = rc_Parameter.c_StructElements[u32_ElementIt];

               // add tree item if element is available for selected use case or in domain case
               if (this->m_CheckAvailability(rc_Element) == true)
               {
                  C_TblTreItem * const pc_Child = new C_TblTreItem();
                  pc_Child->c_Name = rc_Element.c_Display.c_str();
                  pc_Child->c_ToolTipHeading = pc_Child->c_Name;
                  pc_Child->c_ToolTipContent = rc_Element.c_Comment.c_str();
                  pc_Child->u32_Index = u32_ElementIt;

                  // add parameter element to tree
                  pc_ParameterTreeItem->AddChild(pc_Child);

                  // parameter can be added because at least one available parameter element found
                  q_AddParameterToTree = true;
               }
            }

            // no elements case: parameter is configurable itself (and is available too)
            if (rc_Parameter.c_StructElements.empty() == true)
            {
               q_AddParameterToTree = true;
            }
         }

         // finally add to tree
         if ((this->mpc_InvisibleRootItem != NULL) &&
             (q_AddParameterToTree == true))
         {
            this->mpc_InvisibleRootItem->AddChild(pc_ParameterTreeItem);
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
/*! \brief  Get parameter and parameter element index corresponding to given model index

   \param[in]   orc_Index                    Model index
   \param[out]  oru32_ParameterIndex         Parameter index
   \param[out]  oru32_ParameterElementIndex  Parameter element index (only valid if return = true)

   \retval   true   Model index belongs to a parameter element
   \retval   false  Model index does not belong to any parameter element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigTreeModel::mh_GetParameterElementIndexe(const QModelIndex & orc_Index,
                                                              uint32_t & oru32_ParameterIndex,
                                                              uint32_t & oru32_ParameterElementIndex)
{
   bool q_IsParent = true;

   oru32_ParameterIndex = 0U;
   oru32_ParameterElementIndex = 0U;

   if (orc_Index.isValid() == true)
   {
      // Decode model index: parent or child?
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_CurrentItem = static_cast<const C_TblTreItem *>(orc_Index.internalPointer());
      if (pc_CurrentItem != NULL)
      {
         const C_TblTreItem * const pc_Parent = dynamic_cast<const C_TblTreItem * const>(pc_CurrentItem->pc_Parent);
         if ((pc_Parent != NULL) && (pc_Parent->pc_Parent != NULL))
         {
            q_IsParent = false;
            oru32_ParameterIndex = pc_Parent->u32_Index;
            oru32_ParameterElementIndex = pc_CurrentItem->u32_Index;
            // do not use orc_Index.parent().row() because not all parameter elements are visible in tree!
         }
         else
         {
            oru32_ParameterIndex = pc_CurrentItem->u32_Index;
         }
      }
   }

   return !q_IsParent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pointer to parameter element data or parameter data.

   Decides if the provided model index belongs to a parameter element or a parameter itself.
   If it does not belong to a parameter struct, i.e. if the parameter has no elements, the data structure of
   the parameter itself is returned.

   Returns NULL if either some configuration is wrong or if the index belongs to a parameter that has elements.

   \param[in]  orc_Index   Model index

   \return
   NULL  no corresponding parameter data found
   else  corresponding parameter data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcConfigParameter * C_SdNdeHalcConfigTreeModel::m_GetParameterElement(const QModelIndex & orc_Index) const
{
   const C_OscHalcConfigParameter * pc_Return = NULL;

   uint32_t u32_ParameterIndex;
   uint32_t u32_ParameterElementIndex = 0U;
   const bool q_IsParameterElement = mh_GetParameterElementIndexe(orc_Index, u32_ParameterIndex,
                                                                  u32_ParameterElementIndex);
   const C_OscHalcConfigParameterStruct * const pc_Parameter =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelParameterConfigStructData(mu32_NodeIndex, mu32_DomainIndex,
                                                                                     mu32_ChannelIndex,
                                                                                     u32_ParameterIndex,
                                                                                     mq_ChannelCase);

   if (pc_Parameter != NULL)
   {
      if (q_IsParameterElement == true)
      {
         // case: model index belongs to parameter struct element
         if (u32_ParameterElementIndex < pc_Parameter->c_ParameterElements.size())
         {
            pc_Return = &pc_Parameter->c_ParameterElements[u32_ParameterElementIndex];
         }
      }
      else
      {
         // case: model index belongs to parameter itself
         if (pc_Parameter->c_ParameterElements.empty() == true)
         {
            pc_Return = pc_Parameter;
         }
      }
   }

   return pc_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pointer to parameter element definition data or parameter definition data.

   Decides if the provided model index belongs to a parameter element or a parameter itself.
   If it does not belong to a parameter struct, i.e. if the parameter has no elements, the data structure of
   the parameter itself is returned.

   Returns NULL if either some configuration is wrong or if the index belongs to a parameter that has elements.

   \param[in]  orc_Index   Model index

   \return
   NULL  no corresponding parameter definition data found
   else  corresponding parameter definition data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefElement * C_SdNdeHalcConfigTreeModel::m_GetDefParameterElement(const QModelIndex & orc_Index) const
{
   const C_OscHalcDefElement * pc_Return = NULL;

   uint32_t u32_ParameterIndex;
   uint32_t u32_ParameterElementIndex = 0U;
   const bool q_IsParameterElement = mh_GetParameterElementIndexe(orc_Index, u32_ParameterIndex,
                                                                  u32_ParameterElementIndex);
   const C_OscHalcDefStruct * const pc_DefElement =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileVariableData(mu32_NodeIndex, mu32_DomainIndex,
                                                                     C_OscHalcDefDomain::eVA_PARAM,
                                                                     u32_ParameterIndex);

   if (pc_DefElement != NULL)
   {
      if (q_IsParameterElement == true)
      {
         // case: model index belongs to parameter struct element
         if (u32_ParameterElementIndex < pc_DefElement->c_StructElements.size())
         {
            pc_Return = &pc_DefElement->c_StructElements[u32_ParameterElementIndex];
         }
      }
      else
      {
         if (pc_DefElement->c_StructElements.empty() == true)
         {
            // case: model index belongs to parameter itself
            pc_Return = pc_DefElement;
         }
      }
   }

   return pc_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check availability of parameter (element)

   For channels: check if parameter struct is available for current use case
   For domain: domain parameters are always available

   \param[in]  orc_Parameter  Parameter to check

   \retval   true   parameter is available
   \retval   false  parameter is not available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigTreeModel::m_CheckAvailability(const C_OscHalcDefElement & orc_Parameter) const
{
   bool q_Available = false;

   if (this->mq_ChannelCase == true)
   {
      std::vector<uint32_t>::const_iterator c_ItParamAvail;
      for (c_ItParamAvail = orc_Parameter.c_UseCaseAvailabilities.begin();
           c_ItParamAvail != orc_Parameter.c_UseCaseAvailabilities.end(); ++c_ItParamAvail)
      {
         if (*c_ItParamAvail == this->mu32_UseCaseIndex)
         {
            q_Available = true;
            break;
         }
      }
   }
   else
   {
      // domain without channels case: all parameters are available
      q_Available = true;
   }

   return q_Available;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert enum names to string list

   \param[in]  orc_Value   HALC Def Content value of type enum

   \return
   String list of enum value names.
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SdNdeHalcConfigTreeModel::mh_ConvertEnumsToStringList(const C_OscHalcDefContent & orc_Value)
{
   QStringList c_Return;

   const std::vector<std::pair<stw::scl::C_SclString,
                               C_OscNodeDataPoolContent> > & rc_EnumItems = orc_Value.GetEnumItems();

   for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
           rc_EnumItems.begin();
        c_It != rc_EnumItems.end(); ++c_It)
   {
      c_Return.append(c_It->first.c_str());
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert bitmasks to string list

   \param[in]  orc_Value   Value

   \return
   String list of bitmask value names.
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SdNdeHalcConfigTreeModel::mh_ConvertBitmasksToStringList(const C_OscHalcDefContent & orc_Value)
{
   std::vector<stw::scl::C_SclString> c_Display;
   QStringList c_Retval;
   orc_Value.GetBitmaskStatusValues(&c_Display, NULL);
   c_Retval.reserve(c_Display.size());
   for (uint32_t u32_It = 0UL; u32_It < c_Display.size(); ++u32_It)
   {
      c_Retval.push_back(c_Display[u32_It].c_str());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert bitmasks to string

   \param[in]  orc_Value   Value

   \return
   String of bitmask value names.
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeHalcConfigTreeModel::mh_ConvertBitmasksToString(const C_OscHalcDefContent & orc_Value)
{
   QString c_Display;

   std::vector<stw::scl::C_SclString> c_Displays;
   std::vector<bool> c_Values;
   orc_Value.GetBitmaskStatusValues(&c_Displays, &c_Values);
   if (c_Displays.size() == c_Values.size())
   {
      bool q_Found = false;

      // search for items are checked
      for (uint32_t u32_Counter = 0; u32_Counter < c_Displays.size(); ++u32_Counter)
      {
         if (c_Values[u32_Counter] == true)
         {
            // a checked item is found
            q_Found = true;

            // set display text
            if (!c_Display.isEmpty())
            {
               c_Display += ", ";
            }
            c_Display += c_Displays[u32_Counter].c_str();
         }
      }

      // no checked item found
      if (q_Found == false)
      {
         c_Display = C_GtGetText::h_GetText("Not Set");
      }
   }
   return c_Display;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert bitmasks to bit array

   \param[in]  orc_Value   Value

   \return
   Bit array of bitmask values.
*/
//----------------------------------------------------------------------------------------------------------------------
QBitArray C_SdNdeHalcConfigTreeModel::mh_ConvertBitmasksToBitArray(const C_OscHalcDefContent & orc_Value)
{
   std::vector<bool> c_Values;
   QBitArray c_Retval;
   orc_Value.GetBitmaskStatusValues(NULL, &c_Values);
   c_Retval.resize(c_Values.size());
   for (uint32_t u32_It = 0UL; u32_It < c_Values.size(); ++u32_It)
   {
      c_Retval.setBit(static_cast<int32_t>(u32_It), c_Values[u32_It]);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeModel::E_Columns C_SdNdeHalcConfigTreeModel::h_ColumnToEnum(const int32_t & ors32_Column)
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
int32_t C_SdNdeHalcConfigTreeModel::h_EnumToColumn(const C_SdNdeHalcConfigTreeModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

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
