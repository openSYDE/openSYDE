//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for project file table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPFILETABLEMODEL_H
#define C_POPFILETABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_OscProject.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopFileTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eNAME,
      eVERSION,
      ePATH
   };

   explicit C_PopFileTableModel(QObject * const opc_Parent = NULL);
   void UpdateData(const std::vector<QString> & orc_RecentFilePaths,
                   const std::vector<stw::opensyde_core::C_OscProject> & orc_RecentProjects);

   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);
   int32_t ConvertRowToFile(const int32_t & ors32_Row, QString & orc_FilePath) const;

private:
   std::vector<QString> mc_RecentFilePaths;
   std::vector<stw::opensyde_core::C_OscProject> mc_RecentProjects;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
