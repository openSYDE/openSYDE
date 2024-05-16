//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (implementation)

   Node data pool list array edit model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>
#include <QMimeData>
#include "C_SdNdeDpListArrayEditModel.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_SdNdeUnoUtil.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::errors;

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
C_SdNdeDpListArrayEditModel::C_SdNdeDpListArrayEditModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0),
   mu32_ElementIndex(0),
   me_ArrayEditType(C_SdNdeDpUtil::eARRAY_EDIT_MIN),
   mu32_DataSetIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated list

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
   \param[in] oru32_ElementIndex  Node data pool list element index
   \param[in] ore_ArrayEditType   Enum for node data pool list element variable
   \param[in] oru32_DataSetIndex  If min or max use 0
                                  Else use data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditModel::SetElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                             const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                             const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                             const uint32_t & oru32_DataSetIndex)
{
   beginResetModel();
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   this->mu32_ElementIndex = oru32_ElementIndex;
   this->me_ArrayEditType = ore_ArrayEditType;
   this->mu32_DataSetIndex = oru32_DataSetIndex;
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if equal

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
   \param[in] oru32_ElementIndex  Node data pool list element index
   \param[in] ore_ArrayEditType   Enum for node data pool list element variable
   \param[in] oru32_DataSetIndex  If min or max use 0
                                  Else use data set index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListArrayEditModel::Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                         const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                         const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                         const uint32_t & oru32_DataSetIndex) const
{
   bool q_Retval;

   if ((((((this->mu32_NodeIndex == oru32_NodeIndex) && (this->mu32_DataPoolIndex == oru32_DataPoolIndex)) &&
          (this->mu32_ListIndex == oru32_ListIndex)) && (this->mu32_ElementIndex == oru32_ElementIndex)) &&
        (this->me_ArrayEditType == ore_ArrayEditType)) && (this->mu32_DataSetIndex == oru32_DataSetIndex))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in] os32_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] os32_Role       Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListArrayEditModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                                 const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Vertical)
   {
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         c_Retval = C_GtGetText::h_GetText("Value");
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   else
   {
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         c_Retval = QString::number(os32_Section);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in] orc_Parent Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpListArrayEditModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 1;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in] orc_Parent Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpListArrayEditModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex, this->mu32_ElementIndex);
      if (pc_Element != NULL)
      {
         //For table parent should always be invalid
         s32_Retval = pc_Element->GetArraySize();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] os32_Role  Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListArrayEditModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const C_OscNodeDataPoolListElement * const pc_OscElement =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex,
                                                                       this->mu32_DataPoolIndex,
                                                                       this->mu32_ListIndex,
                                                                       this->mu32_ElementIndex);
         if (pc_OscElement != NULL)
         {
            switch (this->me_ArrayEditType)
            {
            case C_SdNdeDpUtil::eARRAY_EDIT_MIN:
               c_Retval =
                  C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OscElement->c_MinValue,
                                                                        pc_OscElement->f64_Factor,
                                                                        pc_OscElement->f64_Offset,
                                                                        static_cast<uint32_t>(orc_Index.column()));
               break;
            case C_SdNdeDpUtil::eARRAY_EDIT_MAX:
               c_Retval =
                  C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OscElement->c_MaxValue,
                                                                        pc_OscElement->f64_Factor,
                                                                        pc_OscElement->f64_Offset,
                                                                        static_cast<uint32_t>(orc_Index.column()));
               break;
            case C_SdNdeDpUtil::eARRAY_EDIT_DATA_SET:
               if (this->mu32_DataSetIndex < pc_OscElement->c_DataSetValues.size())
               {
                  c_Retval =
                     C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                        pc_OscElement->c_DataSetValues[this->mu32_DataSetIndex], pc_OscElement->f64_Factor,
                        pc_OscElement->f64_Offset, static_cast<uint32_t>(orc_Index.column()));
               }
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
      {
         //Default
         c_Retval = mc_STYLE_GUIDE_COLOR_6;
         switch (this->me_ArrayEditType)
         {
         case C_SdNdeDpUtil::eARRAY_EDIT_DATA_SET:
            {
               bool q_ValueBelowMin = false;
               const C_OscNodeDataPoolList * const pc_OscElement =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(this->mu32_NodeIndex,
                                                                      this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex);
               if (pc_OscElement != NULL)
               {
                  const uint32_t u32_ArrayIndex = static_cast<uint32_t>(orc_Index.column());
                  bool q_ValueOverMax = false;
                  pc_OscElement->CheckErrorDataSetValue(this->mu32_ElementIndex,
                                                        this->mu32_DataSetIndex, &q_ValueBelowMin,
                                                        &q_ValueOverMax, &u32_ArrayIndex);
                  if (q_ValueOverMax == true)
                  {
                     q_ValueBelowMin = true;
                  }
               }
               if (q_ValueBelowMin == true)
               {
                  c_Retval = mc_STYLE_GUIDE_COLOR_18;
               }
            }
            break;
         case C_SdNdeDpUtil::eARRAY_EDIT_MIN:
         case C_SdNdeDpUtil::eARRAY_EDIT_MAX:
            {
               bool q_MinMaxError = false;
               const C_OscNodeDataPoolListElement * const pc_OscElement =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             this->mu32_ListIndex,
                                                                             this->mu32_ElementIndex);
               if (pc_OscElement != NULL)
               {
                  const uint32_t u32_ArrayIndex = static_cast<uint32_t>(orc_Index.column());

                  q_MinMaxError = pc_OscElement->c_MinValue.CompareArrayGreater(pc_OscElement->c_MaxValue,
                                                                                u32_ArrayIndex);
               }
               if (q_MinMaxError == true)
               {
                  c_Retval = mc_STYLE_GUIDE_COLOR_18;
               }
            }
            break;
         default:
            //Default already set
            break;
         }
      }
      else
      {
         //No handling necessary
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in] orc_Index Index
   \param[in] orc_Value New data
   \param[in] os32_Role  Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListArrayEditModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                          const int32_t os32_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if (os32_Role == static_cast<int32_t>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.column());
            Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                       this->mu32_ListIndex, this->mu32_ElementIndex, this->me_ArrayEditType,
                                       this->mu32_DataSetIndex,
                                       u32_Index, orc_Value);

            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role);
            Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                                this->mu32_ElementIndex, this->me_ArrayEditType,
                                                this->mu32_DataSetIndex);
            q_Retval = true;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in] orc_Index Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeDpListArrayEditModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   }
   else
   {
      c_Retval = Qt::NoItemFlags;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal model update

   \param[in] oru32_Column   Changed column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditModel::HandleDataChange(const uint32_t & oru32_Column)
{
   Q_EMIT this->dataChanged(this->index(0, oru32_Column), this->index(0, oru32_Column));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type of current data

   \return
   Type of current data
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_SdNdeDpListArrayEditModel::GetType(void) const
{
   C_OscNodeDataPoolContent::E_Type e_Retval = C_OscNodeDataPoolContent::eUINT8;
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex,
                                                                 this->mu32_DataPoolIndex,
                                                                 this->mu32_ListIndex,
                                                                 this->mu32_ElementIndex);
   if (pc_OscElement != NULL)
   {
      e_Retval = pc_OscElement->GetType();
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get osc node data pool list element

   \return
   OSC node data pool list element
   Else NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_SdNdeDpListArrayEditModel::GetOscElement(void) const
{
   return C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex,
                                                                     this->mu32_DataPoolIndex,
                                                                     this->mu32_ListIndex,
                                                                     this->mu32_ElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array edit type

   \return
   Current array edit type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpUtil::E_ArrayEditType C_SdNdeDpListArrayEditModel::GetArrayEditType() const
{
   return this->me_ArrayEditType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model data reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditModel::Reset(void)
{
   this->beginResetModel();
   this->endResetModel();
}
