//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for project file table (implementation)

   Model for project file table

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <QFileInfo>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"
#include "C_PopFileTableModel.hpp"
#include "C_GtGetText.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiProject.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopFileTableModel::C_PopFileTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table

   \param[in]  orc_RecentFilePaths  Recent file paths
   \param[in]  orc_RecentProjects   Recent projects
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopFileTableModel::UpdateData(const std::vector<QString> & orc_RecentFilePaths,
                                     const std::vector<C_OscProject> & orc_RecentProjects)
{
   tgl_assert(orc_RecentFilePaths.size() == orc_RecentProjects.size());
   if (orc_RecentFilePaths.size() == orc_RecentProjects.size())
   {
      beginResetModel();
      this->mc_RecentFilePaths = orc_RecentFilePaths;
      this->mc_RecentProjects = orc_RecentProjects;
      endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PopFileTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->mc_RecentFilePaths.size();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PopFileTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 3;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_PopFileTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_PopFileTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         if ((u32_Index < this->mc_RecentFilePaths.size()) && (u32_Index < this->mc_RecentProjects.size()))
         {
            const QString & rc_RecentFilePaths = this->mc_RecentFilePaths[u32_Index];
            const C_OscProject & rc_RecentProjects = this->mc_RecentProjects[u32_Index];
            QFileInfo c_ProjectFileInfo;
            QFont c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            c_Font.setPixelSize(c_Font.pointSize());

            c_ProjectFileInfo.setFile(rc_RecentFilePaths);
            switch (e_Col)
            {
            case eNAME:
               // Translation: %1 = sequential number, %2 = project name
               c_Retval = static_cast<QString>("#%1 - %2").arg(orc_Index.row() + 1).arg(
                  c_ProjectFileInfo.completeBaseName());
               break;
            case eVERSION:
               c_Retval = rc_RecentProjects.c_Version.c_str();
               break;
            case ePATH:
               c_Retval = C_Uti::h_MinimizePath(static_cast<QString>(rc_RecentFilePaths), c_Font, 535, 12);
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::FontRole))
      {
         QFont c_Font;
         //Stylesheets do not allow access of specific columns so we need to set fonts manually
         switch (e_Col)
         {
         case ePATH:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         case eVERSION:
         case eNAME:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         default:
            tgl_assert(false);
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
      {
         switch (e_Col)
         {
         case ePATH:
            c_Retval = mc_STYLE_GUIDE_COLOR_10;
            break;
         case eVERSION:
         case eNAME:
            c_Retval = mc_STYLE_GUIDE_COLOR_12;
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         if ((u32_Index < this->mc_RecentFilePaths.size()) && (u32_Index < this->mc_RecentProjects.size()))
         {
            const QString & rc_RecentFilePaths = this->mc_RecentFilePaths[u32_Index];
            QFileInfo c_ProjectFileInfo;

            c_ProjectFileInfo.setFile(rc_RecentFilePaths);
            c_Retval = c_ProjectFileInfo.completeBaseName();
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         if ((u32_Index < this->mc_RecentFilePaths.size()) && (u32_Index < this->mc_RecentProjects.size()))
         {
            const QString & rc_RecentFilePaths = this->mc_RecentFilePaths[u32_Index];
            const C_OscProject & rc_RecentProject = this->mc_RecentProjects[u32_Index];
            QFileInfo c_ProjectFileInfo;
            c_ProjectFileInfo.setFile(rc_RecentFilePaths);
            QString c_TooltipContent;
            QString c_CreationTime;
            QString c_ModificationTime;
            // if it's a syde_sp file we don't have access to creation and modification time
            if (c_ProjectFileInfo.completeSuffix().compare("syde_sp") == 0)
            {
               c_CreationTime = "N/A";
               c_ModificationTime = "N/A";
            }
            else
            {
               c_CreationTime = C_OscProject::h_GetTimeFormatted(rc_RecentProject.c_CreationTime).c_str();
               c_ModificationTime = C_OscProject::h_GetTimeFormatted(rc_RecentProject.c_ModificationTime).c_str();
            }
            c_TooltipContent =
               static_cast<QString>(C_GtGetText::h_GetText("Version: %1 \nAuthor: %2 \nCreated: %3 \n"
                                                           "Last modified: %4 (by %5) \nUsed openSYDE version: %6")).
               arg(rc_RecentProject.c_Version.c_str()).
               arg(rc_RecentProject.c_Author.c_str()).
               arg(c_CreationTime).
               arg(c_ModificationTime).
               arg(rc_RecentProject.c_Editor.c_str()).
               arg(C_Uti::h_ConvertVersionToStwStyle(rc_RecentProject.c_OpenSydeVersion.c_str()));
            if (c_ProjectFileInfo.exists() == true)
            {
               const uint64_t u64_SizeByte = C_PuiProject::h_GetProjectSize(rc_RecentFilePaths);
               const float64_t f64_SizeFloat = std::ceil(static_cast<float64_t>(u64_SizeByte) / 1024.0);

               c_TooltipContent.append(
                  static_cast<QString>(C_GtGetText::h_GetText("\nSize: %L1 kB")).arg(static_cast
                                                                                     <uint64_t>(
                                                                                        f64_SizeFloat)));
            }
            c_TooltipContent.append("\n\n" + rc_RecentFilePaths);
            c_Retval = c_TooltipContent;
         }
      }
      else
      {
         //Unknown
      }
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
C_PopFileTableModel::E_Columns C_PopFileTableModel::h_ColumnToEnum(const int32_t & ors32_Column)
{
   C_PopFileTableModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eNAME;
      break;
   case 1:
      e_Retval = eVERSION;
      break;
   case 2:
      e_Retval = ePATH;
      break;
   default:
      //Unknown
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
int32_t C_PopFileTableModel::h_EnumToColumn(const C_PopFileTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eNAME:
      s32_Retval = 0;
      break;
   case eVERSION:
      s32_Retval = 1;
      break;
   case ePATH:
      s32_Retval = 2;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert row to message

   \param[in]   ors32_Row     Model row
   \param[out]  orc_FilePath  File path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PopFileTableModel::ConvertRowToFile(const int32_t & ors32_Row, QString & orc_FilePath) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (ors32_Row >= 0)
   {
      const uint32_t u32_Row = static_cast<uint32_t>(ors32_Row);
      if (u32_Row < this->mc_RecentFilePaths.size())
      {
         orc_FilePath = this->mc_RecentFilePaths[u32_Row];
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
