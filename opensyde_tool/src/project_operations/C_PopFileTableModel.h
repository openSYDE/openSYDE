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
#include "stwtypes.h"
#include "C_OSCProject.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
                   const std::vector<stw_opensyde_core::C_OSCProject> & orc_RecentProjects);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);
   stw_types::sint32 ConvertRowToFile(const stw_types::sint32 & ors32_Row, QString & orc_FilePath) const;

private:
   std::vector<QString> mc_RecentFilePaths;
   std::vector<stw_opensyde_core::C_OSCProject> mc_RecentProjects;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
