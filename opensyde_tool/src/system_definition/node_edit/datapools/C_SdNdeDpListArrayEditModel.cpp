//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (implementation)

   Node data pool list array edit model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>
#include <QMimeData>
#include "C_SdNdeDpListArrayEditModel.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_Uti.h"
#include "C_SdNdeUnoUtil.h"
#include "C_SdNdeDpContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_errors;

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
void C_SdNdeDpListArrayEditModel::SetElement(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                             const uint32 & oru32_ListIndex, const uint32 & oru32_ElementIndex,
                                             const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                             const uint32 & oru32_DataSetIndex)
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
bool C_SdNdeDpListArrayEditModel::Equals(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                         const uint32 & oru32_ListIndex, const uint32 & oru32_ElementIndex,
                                         const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                         const uint32 & oru32_DataSetIndex) const
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

   \param[in] osn_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] osn_Role       Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListArrayEditModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                                 const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Vertical)
   {
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         c_Retval = C_GtGetText::h_GetText("Value");
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
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
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         c_Retval = QString::number(osn_Section);
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
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
sintn C_SdNdeDpListArrayEditModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 1;
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in] orc_Parent Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpListArrayEditModel::columnCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex, this->mu32_ElementIndex);
      if (pc_Element != NULL)
      {
         //For table parent should always be invalid
         sn_Retval = pc_Element->GetArraySize();
      }
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListArrayEditModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         if (orc_Index.column() >= 0)
         {
            const C_OSCNodeDataPoolListElement * const pc_OSCElement =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex,
                                                                          this->mu32_ElementIndex);
            if (pc_OSCElement != NULL)
            {
               switch (this->me_ArrayEditType)
               {
               case C_SdNdeDpUtil::eARRAY_EDIT_MIN:
                  c_Retval =
                     C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OSCElement->c_MinValue,
                                                                           pc_OSCElement->f64_Factor,
                                                                           pc_OSCElement->f64_Offset,
                                                                           static_cast<uint32>(orc_Index.column()));
                  break;
               case C_SdNdeDpUtil::eARRAY_EDIT_MAX:
                  c_Retval =
                     C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OSCElement->c_MaxValue,
                                                                           pc_OSCElement->f64_Factor,
                                                                           pc_OSCElement->f64_Offset,
                                                                           static_cast<uint32>(orc_Index.column()));
                  break;
               case C_SdNdeDpUtil::eARRAY_EDIT_DATA_SET:
                  if (this->mu32_DataSetIndex < pc_OSCElement->c_DataSetValues.size())
                  {
                     c_Retval =
                        C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                           pc_OSCElement->c_DataSetValues[this->mu32_DataSetIndex], pc_OSCElement->f64_Factor,
                           pc_OSCElement->f64_Offset, static_cast<uint32>(orc_Index.column()));
                  }
                  break;
               default:
                  break;
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         //Default
         c_Retval = mc_STYLE_GUIDE_COLOR_6;
         switch (this->me_ArrayEditType)
         {
         case C_SdNdeDpUtil::eARRAY_EDIT_DATA_SET:
            //Check error
            if (orc_Index.column() >= 0)
            {
               bool q_ValueBelowMin = false;
               const C_OSCNodeDataPoolList * const pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                      this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex);
               if (pc_OSCElement != NULL)
               {
                  const stw_types::uint32 u32_ArrayIndex = static_cast<stw_types::uint32>(orc_Index.column());
                  bool q_ValueOverMax = false;
                  pc_OSCElement->CheckErrorDataSetValue(this->mu32_ElementIndex,
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
            //Check error
            if (orc_Index.column() >= 0)
            {
               bool q_MinMaxError = false;
               const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             this->mu32_ListIndex,
                                                                             this->mu32_ElementIndex);
               if (pc_OSCElement != NULL)
               {
                  const stw_types::uint32 u32_ArrayIndex = static_cast<stw_types::uint32>(orc_Index.column());

                  q_MinMaxError = pc_OSCElement->c_MinValue.CompareArrayGreater(pc_OSCElement->c_MaxValue,
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
   \param[in] osn_Role  Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListArrayEditModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                          const sintn osn_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, osn_Role) != orc_Value)
   {
      if (osn_Role == static_cast<sintn>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.column());
            Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                       this->mu32_ListIndex, this->mu32_ElementIndex, this->me_ArrayEditType,
                                       this->mu32_DataSetIndex,
                                       u32_Index, orc_Value);

            //lint -e{1793} Qt example
            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<stw_types::sintn>() << osn_Role);
            Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                                this->mu32_ElementIndex, this->me_ArrayEditType,
                                                this->mu32_DataSetIndex);
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
void C_SdNdeDpListArrayEditModel::HandleDataChange(const uint32 & oru32_Column)
{
   Q_EMIT this->dataChanged(this->index(0, oru32_Column), this->index(0, oru32_Column));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type of current data

   \return
   Type of current data
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_SdNdeDpListArrayEditModel::GetType(void) const
{
   C_OSCNodeDataPoolContent::E_Type e_Retval = C_OSCNodeDataPoolContent::eUINT8;
   const C_OSCNodeDataPoolListElement * const pc_OSCElement =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                 this->mu32_DataPoolIndex,
                                                                 this->mu32_ListIndex,
                                                                 this->mu32_ElementIndex);
   if (pc_OSCElement != NULL)
   {
      e_Retval = pc_OSCElement->GetType();
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
const C_OSCNodeDataPoolListElement * C_SdNdeDpListArrayEditModel::GetOSCElement(void) const
{
   return C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
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
