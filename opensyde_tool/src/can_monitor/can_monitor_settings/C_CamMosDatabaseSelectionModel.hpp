//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for database message selection (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASESELECTIONMODEL_H
#define C_CAMMOSDATABASESELECTIONMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <array>
#include <QMap>

#include "C_TblTreItem.hpp"
#include "C_TblTreModel.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDatabaseSelectionModel :
   public C_TblTreModel
{
public:
   C_CamMosDatabaseSelectionModel(QObject * const opc_Parent = NULL);
   ~C_CamMosDatabaseSelectionModel(void) override;

   void Init(void);
   std::vector<std::array<QString, 2> > GetDataElements(const QModelIndex & orc_Index) const;

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //Generic interface
   QModelIndex GetIndexForItem(const QString & orc_ItemText) const;
   std::vector<uint32_t> GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex) const;

private:
   static const QString mhc_ICON_DATABASE;
   static const QString mhc_ICON_MESSAGE;
   QMap<QString, QString> mc_DatabaseMap;
   QMap<QString, QString> mc_MessageMap;

   void m_Init(void);
   C_TblTreItem * m_CreateAndFillDatabaseNode(const QString & orc_File, C_TblTreSimpleItem * const opc_ParentItem);
   void m_CreateAndFillMessageNode(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                   C_TblTreSimpleItem * const opc_ParentItem);
   static void mh_CombineIndices(std::vector<std::array<QString, 2> > & orc_BigVectorToAppendTo,
                                 const std::vector<std::array<QString, 2> > & orc_SmallVectorToAdd);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
