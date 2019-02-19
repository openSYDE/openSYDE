//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Model for database message selection (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASESELECTIONMODEL_H
#define C_CAMMOSDATABASESELECTIONMODEL_H

/* -- Includes ------------------------------------------------------------- */
#include <array>
#include <QMap>

#include "C_TblTreModel.h"
#include "C_OSCCanMessage.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMosDatabaseSelectionModel :
   public C_TblTreModel
{
public:
   C_CamMosDatabaseSelectionModel(QObject * const opc_Parent = NULL);
   ~C_CamMosDatabaseSelectionModel(void);

   void Init(void);
   std::vector<std::array<QString, 2> > GetDataElements(const QModelIndex & orc_Index) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -restore

   //Generic interface
   QModelIndex GetIndexForItem(const QString & orc_ItemText) const;
   std::vector<stw_types::uint32> GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex) const;

private:
   static const QString mhc_IconDatabase;
   static const QString mhc_IconMessage;
   QMap<QString, QString> mc_DatabaseMap;
   QMap<QString, QString> mc_MessageMap;

   void m_Init(void);
   C_TblTreItem * m_CreateAndFillDatabaseNode(const QString & orc_File, C_TblTreItem * const opc_ParentItem);
   void m_CreateAndFillMessageNode(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                   C_TblTreItem * const opc_ParentItem);
   static void mh_CombineIndices(std::vector<std::array<QString, 2> > & orc_BigVectorToAppendTo,
                                 const std::vector<std::array<QString, 2> > & orc_SmallVectorToAdd);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
