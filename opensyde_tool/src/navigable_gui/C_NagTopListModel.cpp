//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for topology item list (implementation)

   Model for topology item list

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "stwtypes.h"
#include "C_SdUtil.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_NagToolTip.h"
#include "C_NagTopListModel.h"

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
C_NagTopListModel::C_NagTopListModel(QObject * const opc_Parent) :
   QAbstractListModel(opc_Parent),
   ms32_Selected(-1),
   mq_TypeNode(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item types are nodes flag

   \param[in] oq_IsNode Flag if items are nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopListModel::SetTypeNode(const bool oq_IsNode)
{
   this->mq_TypeNode = oq_IsNode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update selected index

   \param[in] os32_Selected Selected index (is any)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopListModel::SetSelectedIndex(const sint32 os32_Selected)
{
   this->ms32_Selected = os32_Selected;
   if (this->rowCount() > 0)
   {
      QVector<sintn> c_Roles;
      c_Roles.push_back(static_cast<sintn>(Qt::ForegroundRole));
      Q_EMIT this->dataChanged(this->index(0, 0), this->index(this->rowCount() - 1, 0), c_Roles);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list content

   \param[in] orc_Content List content names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopListModel::SetContent(const std::vector<QString> & orc_Content)
{
   this->beginResetModel();
   this->mc_Content = orc_Content;
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list errors

   \param[in] orc_Error List error states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopListModel::SetError(const std::vector<bool> & orc_Error)
{
   this->mc_Error = orc_Error;
   if (this->rowCount() > 0)
   {
      QVector<sintn> c_Roles;
      c_Roles.push_back(static_cast<sintn>(Qt::DecorationRole));
      Q_EMIT this->dataChanged(this->index(0, 0), this->index(this->rowCount() - 1, 0), c_Roles);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in] orc_Parent Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_NagTopListModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      sn_Retval = this->mc_Content.size();
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
QVariant C_NagTopListModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if ((orc_Index.isValid() == true) && (orc_Index.row() >= 0))
   {
      const uint32 u32_Row = static_cast<uint32>(orc_Index.row());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) && (u32_Row < this->mc_Content.size()))
      {
         c_Retval = this->mc_Content[u32_Row];
      }
      else if (osn_Role == static_cast<sintn>(Qt::BackgroundRole))
      {
         if (this->ms32_Selected == orc_Index.row())
         {
            c_Retval = mc_STYLE_GUIDE_COLOR_21;
         }
         else
         {
            c_Retval = mc_STYLE_GUIDE_COLOR_67;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         c_Retval = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);
      }
      else if (osn_Role == static_cast<sintn>(Qt::DecorationRole))
      {
         if ((u32_Row < this->mc_Error.size()) && (this->mc_Error[u32_Row] == true))
         {
            //Error
            c_Retval = C_SdUtil::h_InitStaticIconSvg("://images/Error_iconV2.svg", mc_ICON_SIZE_24);
         }
         else
         {
            //Reserve the space
            c_Retval = QColor(255, 0, 0, 0);
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         if ((u32_Row < this->mc_Error.size()) && (this->mc_Error[u32_Row] == true))
         {
            QString c_Content;
            if (this->mq_TypeNode == true)
            {
               bool q_Error;
               C_SdUtil::h_GetErrorToolTipNode(u32_Row, c_Content, q_Error);
            }
            else
            {
               C_SdUtil::h_GetErrorToolTipBus(u32_Row, c_Content);
            }
            c_Retval = c_Content;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         if ((u32_Row < this->mc_Error.size()) && (this->mc_Error[u32_Row] == true))
         {
            if (this->mq_TypeNode == true)
            {
               c_Retval = C_GtGetText::h_GetText("Node has invalid content");
            }
            else
            {
               c_Retval = C_GtGetText::h_GetText("Invalid Bus Content");
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_TYPE)
      {
         if ((u32_Row < this->mc_Error.size()) && (this->mc_Error[u32_Row] == true))
         {
            c_Retval = static_cast<sintn>(C_NagToolTip::eERROR);
         }
      }
      else
      {
         //Not necessary
      }
   }
   return c_Retval;
}
