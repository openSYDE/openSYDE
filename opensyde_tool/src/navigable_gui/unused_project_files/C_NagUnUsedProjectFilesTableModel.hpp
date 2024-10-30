//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for Unused files and empty folders
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGUNUSEDPROJECTFILESTABLEMODEL_H
#define C_NAGUNUSEDPROJECTFILESTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QItemSelection>

#include "C_TblTreModel.hpp"
#include "C_TblTreItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagUnUsedProjectFilesTableModel :
   public C_TblTreModel
{
   Q_OBJECT

public:
   C_NagUnUsedProjectFilesTableModel(QObject * const opc_Parent = NULL);
   ~C_NagUnUsedProjectFilesTableModel() override;
   enum E_Columns
   {
      ePATH = 0,
      eEXTENSION,
      eCOLUMNCOUNT
   };

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

   void UpdateData(const QStringList & orc_UnUsedFilesList, const QStringList & orc_EmptyFoldersList);
   void DeleteSelectedFiles(const QItemSelection & orc_Selection);
   void DeleteAllFiles();
   QString DetailsOfFilesToDelete(const QItemSelection & orc_Selection);

private:
   class C_FileTableData
   {
   public:
      QString c_Path;
      QString c_Extension;
   };

   std::vector<C_FileTableData> mc_FileInfoList;

   void m_FillFileInfo(const QStringList & orc_UnUsedFilesList, const QStringList & orc_EmptyFoldersList);
   QStringList m_ConvertAbsoluteToRelative(const QStringList & orc_UnUsedFilesList);
   const QString m_ConvertRelativeToAbsolute(const QString & orc_UnUsedFilePath) const;
   const QString m_GetFileSuffix(const QString & orc_FilePath) const;
   void m_MoveFilesToTrash(const std::vector<uint32_t> & orc_FileIndexes);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
