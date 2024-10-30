//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for Unused files and empty folders table (implementation)

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiProject.hpp"
#include "C_NagUnUsedProjectFilesTableModel.hpp"

#include <QFileInfo>
#include <QDir>
#include <QAbstractItemModel>
#include <set>
#include <algorithm>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
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
    Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagUnUsedProjectFilesTableModel::C_NagUnUsedProjectFilesTableModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent)
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagUnUsedProjectFilesTableModel::~C_NagUnUsedProjectFilesTableModel()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_NagUnUsedProjectFilesTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return static_cast<int32_t>(E_Columns::eCOLUMNCOUNT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get header data

   \param[in]  os32_Section      Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role         Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_NagUnUsedProjectFilesTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                                       const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractItemModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_NagUnUsedProjectFilesTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case E_Columns::ePATH:
            c_Retval = C_GtGetText::h_GetText("Path");
            break;
         case E_Columns::eEXTENSION:
            c_Retval = C_GtGetText::h_GetText("Extension");
            break;
         case E_Columns::eCOLUMNCOUNT:
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignTop);
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case E_Columns::ePATH:
            c_Retval = C_GtGetText::h_GetText("Path for the file");
            break;
         case E_Columns::eEXTENSION:
            c_Retval = C_GtGetText::h_GetText("Extension of the file");
            break;
         case E_Columns::eCOLUMNCOUNT:
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case E_Columns::ePATH:
            c_Retval = C_GtGetText::h_GetText("Path");
            break;
         case E_Columns::eEXTENSION:
            c_Retval = C_GtGetText::h_GetText("Extension");
            break;
         case E_Columns::eCOLUMNCOUNT:
            break;
         default:
            break;
         }
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role   Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_NagUnUsedProjectFilesTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if (orc_Index.column() == 0)
      {
         c_Retval = C_TblTreModel::data(orc_Index, os32_Role);
      }
      else
      {
         const C_NagUnUsedProjectFilesTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
         {
            switch (e_Col)
            {
            case E_Columns::ePATH:
               //For first column "Path with checkbox and Icon" is handled in above IF condition
               break;
            case E_Columns::eEXTENSION:
               c_Retval = this->mc_FileInfoList[orc_Index.row()].c_Extension;
               break;
            case E_Columns::eCOLUMNCOUNT:
               break;
            default:
               break;
            }
         }
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
C_NagUnUsedProjectFilesTableModel::E_Columns C_NagUnUsedProjectFilesTableModel::h_ColumnToEnum(
   const int32_t & ors32_Column)
{
   C_NagUnUsedProjectFilesTableModel::E_Columns e_Retval = E_Columns::ePATH;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = E_Columns::ePATH;
      break;
   case 1:
      e_Retval = E_Columns::eEXTENSION;
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
int32_t C_NagUnUsedProjectFilesTableModel::h_EnumToColumn(const C_NagUnUsedProjectFilesTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval = -1;

   switch (ore_Value)
   {
   case E_Columns::ePATH:
      s32_Retval = 0;
      break;
   case E_Columns::eEXTENSION:
      s32_Retval = 1;
      break;
   case E_Columns::eCOLUMNCOUNT:
      s32_Retval = -1;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convey data changes to table

   \param[in]   orc_UnUsedFilesList    List of unused files
   \param[in]   orc_EmptyFoldersList   List of Empty folders
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableModel::UpdateData(const QStringList & orc_UnUsedFilesList,
                                                   const QStringList & orc_EmptyFoldersList)
{
   beginResetModel();
   this->m_FillFileInfo(orc_UnUsedFilesList, orc_EmptyFoldersList);
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete selected files

   \param[in]  orc_Selection      Selected file items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableModel::DeleteSelectedFiles(const QItemSelection & orc_Selection)
{
   std::set<int32_t> c_UniqueRows;
   std::vector<uint32_t> c_SelectedFileIndexes;
   for (const QModelIndex & rc_Index : orc_Selection.indexes())
   {
      const uint32_t u32_Row = static_cast<uint32_t>(rc_Index.row());
      if (u32_Row < (static_cast<uint32_t>(this->mc_FileInfoList.size())))
      {
         c_UniqueRows.insert(u32_Row);
      }
   }
   c_SelectedFileIndexes.assign(c_UniqueRows.begin(), c_UniqueRows.end());
   std::sort(c_SelectedFileIndexes.begin(), c_SelectedFileIndexes.end());
   this->m_MoveFilesToTrash(c_SelectedFileIndexes);
   for (const uint32_t u32_Row : c_SelectedFileIndexes)
   {
      beginRemoveRows(QModelIndex(), u32_Row, u32_Row);
      this->mc_FileInfoList.erase(this->mc_FileInfoList.begin() + u32_Row);
      endRemoveRows();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete all files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableModel::DeleteAllFiles()
{
   std::vector<uint32_t> c_AllFileIndexes;
   c_AllFileIndexes.reserve(this->mc_FileInfoList.size());

   for (uint32_t u32_Index = 0; u32_Index < this->mc_FileInfoList.size(); ++u32_Index)
   {
      c_AllFileIndexes.push_back(u32_Index);
   }
   std::sort(c_AllFileIndexes.begin(), c_AllFileIndexes.end());
   this->m_MoveFilesToTrash(c_AllFileIndexes);
   for (const uint32_t u32_Row : c_AllFileIndexes)
   {
      beginRemoveRows(QModelIndex(), u32_Row, u32_Row);
      this->mc_FileInfoList.erase(this->mc_FileInfoList.begin() + u32_Row);
      endRemoveRows();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gives list of selected files for deletion

   \param[in]  orc_Selection      Selected file items

   \return
   QString   returns selected files path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_NagUnUsedProjectFilesTableModel::DetailsOfFilesToDelete(const QItemSelection & orc_Selection)
{
   std::set<int32_t> c_UniqueRows;
   std::vector<uint32_t> c_SelectedFileIndexes;
   for (const QModelIndex & rc_Index : orc_Selection.indexes())
   {
      const uint32_t u32_Row = static_cast<uint32_t>(rc_Index.row());
      if (u32_Row < (static_cast<uint32_t>(this->mc_FileInfoList.size())))
      {
         c_UniqueRows.insert(u32_Row);
      }
   }
   c_SelectedFileIndexes.assign(c_UniqueRows.begin(), c_UniqueRows.end());
   std::sort(c_SelectedFileIndexes.begin(), c_SelectedFileIndexes.end());
   std::ostringstream c_DeleteFiles;

   if (!c_SelectedFileIndexes.empty())
   {
      for (const uint32_t u32_FileIndex : c_SelectedFileIndexes)
      {
         c_DeleteFiles << this->mc_FileInfoList[u32_FileIndex].c_Path.toStdString() << "\n";
      }
   }
   return QString::fromStdString(c_DeleteFiles.str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fills file info

   \param[in]  orc_UnUsedFilesList   List of unused files
   \param[in]  orc_EmptyFoldersList   List of empty folders
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableModel::m_FillFileInfo(const QStringList & orc_UnUsedFilesList,
                                                       const QStringList & orc_EmptyFoldersList)
{
   //QStringList c_UnUsedFileList = orc_UnUsedFilesList; //CHALLA : Kept in case of absolute paths needed
   QStringList c_UnUsedFileList = this->m_ConvertAbsoluteToRelative(orc_UnUsedFilesList);
   QStringList c_EmptyFolderList = this->m_ConvertAbsoluteToRelative(orc_EmptyFoldersList);
   const QPixmap c_FilePixmap("://images/main_page_and_navi_bar/IconApp.svg");
   const QPixmap c_FileScaledPixmap = c_FilePixmap.scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
   const QIcon c_FileScaledIcon(c_FileScaledPixmap);

   const QPixmap c_FolderPixmap("://images/main_page_and_navi_bar/IconProjOpen.svg");
   const QPixmap c_FolderScaledPixmap = c_FolderPixmap.scaled(QSize(16,
                                                                    16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
   const QIcon c_FolderScaledIcon(c_FolderScaledPixmap);

   this->mpc_InvisibleRootItem = new C_TblTreItem();
   this->mpc_InvisibleRootItem->ReserveChildrenSpace(static_cast<uint32_t>(orc_UnUsedFilesList.size() +
                                                                           orc_EmptyFoldersList.size()));

   this->mc_FileInfoList.clear();
   this->mc_FileInfoList.reserve(static_cast<uint32_t>(orc_UnUsedFilesList.size() + orc_EmptyFoldersList.size()));

   this->beginResetModel();

   for (uint32_t u32_FileIndex = 0; u32_FileIndex < static_cast<uint32_t>(orc_UnUsedFilesList.size());
        u32_FileIndex++)
   {
      QString & rc_FilePath = c_UnUsedFileList[u32_FileIndex];

      C_FileTableData c_FileData;
      c_FileData.c_Path = rc_FilePath;
      c_FileData.c_Extension = this->m_GetFileSuffix(rc_FilePath);
      {
         C_TblTreItem * const pc_FileItem = new C_TblTreItem();

         pc_FileItem->c_Name = rc_FilePath;
         pc_FileItem->u32_Index = u32_FileIndex;
         pc_FileItem->c_Icon = c_FileScaledIcon;

         pc_FileItem->q_Selectable = true;
         pc_FileItem->q_Enabled = true;

         this->mc_FileInfoList.push_back(c_FileData);
         this->mpc_InvisibleRootItem->AddChild(pc_FileItem);
      }
   }

   for (uint32_t u32_FolderIndex = 0; u32_FolderIndex < static_cast<uint32_t>(orc_EmptyFoldersList.size());
        u32_FolderIndex++)
   {
      QString & rc_FolderPath = c_EmptyFolderList[u32_FolderIndex];

      C_FileTableData c_FolderData;
      c_FolderData.c_Path = rc_FolderPath;
      c_FolderData.c_Extension = "";
      {
         C_TblTreItem * const pc_FolderItem = new C_TblTreItem();

         pc_FolderItem->c_Name = rc_FolderPath;
         pc_FolderItem->u32_Index = u32_FolderIndex + static_cast<uint32_t>(orc_UnUsedFilesList.size());
         pc_FolderItem->c_Icon = c_FolderScaledIcon;

         pc_FolderItem->q_Selectable = true;
         pc_FolderItem->q_Enabled = true;

         this->mc_FileInfoList.push_back(c_FolderData);
         this->mpc_InvisibleRootItem->AddChild(pc_FolderItem);
      }
   }
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Converts Absolute path into Relative path

   \param[in]  orc_UnUsedFilesList   List of unused files

   \return
   QStringList   List of relative paths of unused files
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_NagUnUsedProjectFilesTableModel::m_ConvertAbsoluteToRelative(const QStringList & orc_UnUsedFilesList)
{
   QStringList c_RelativePathList;
   const QDir c_ProjectDir(C_PuiProject::h_GetInstance()->GetFolderPath());

   for (uint32_t u32_FileIndex = 0; u32_FileIndex < static_cast<uint32_t>(orc_UnUsedFilesList.size());
        u32_FileIndex++)
   {
      c_RelativePathList.append(c_ProjectDir.relativeFilePath(orc_UnUsedFilesList[u32_FileIndex]));
   }

   return c_RelativePathList;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Converts Relative path into absolute path

   \param[in]  orc_UnUsedFilePath   List of unused file

   \return
   QString   Relative path of unused file
*/
//----------------------------------------------------------------------------------------------------------------------
const QString C_NagUnUsedProjectFilesTableModel::m_ConvertRelativeToAbsolute(const QString & orc_UnUsedFilePath) const
{
   const QDir c_ProjectDir(C_PuiProject::h_GetInstance()->GetFolderPath());

   return c_ProjectDir.absoluteFilePath(orc_UnUsedFilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets file suffix
 *
   \param[in]  orc_FilePath   File path

   \return
   QString    returns file suffix
*/
//----------------------------------------------------------------------------------------------------------------------
const QString C_NagUnUsedProjectFilesTableModel::m_GetFileSuffix(const QString & orc_FilePath) const
{
   const QFileInfo c_FileInfo(orc_FilePath);

   return "." + c_FileInfo.completeSuffix();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move files to trash/recycle bin

   \param[in]  orc_FileIndexes   vector of file indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableModel::m_MoveFilesToTrash(const std::vector<uint32_t> & orc_FileIndexes)
{
   for (const uint32_t u32_FileIndex : orc_FileIndexes)
   {
      const QString c_CurrentFileAbsolutePath =
         this->m_ConvertRelativeToAbsolute(this->mc_FileInfoList[u32_FileIndex].c_Path);
      const QFileInfo c_FileInfo(c_CurrentFileAbsolutePath);
      if (c_FileInfo.exists())
      {
         QFile c_File(c_CurrentFileAbsolutePath);
         c_File.moveToTrash();
      }
   }
}
